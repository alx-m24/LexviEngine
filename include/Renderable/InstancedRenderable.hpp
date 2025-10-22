#pragma once

#ifndef INSTANCESYSTEM_HPP
#define INSTANCESYSTEM_HPP

#ifndef GLAD_ALREADY_INCLUDED
#define GLAD_ALREADY_INCLUDED
#include <glad/glad.h>
#endif

#include "IRenderable/IRenderable.hpp"
#include "Utils/IndirectBuffer.hpp"
#include "Shader/ComputeShader.hpp"
#include "Shader/Shader.hpp"
#include "Camera/Camera.hpp"
#include "Utils/UBO.hpp"
#include "Utils/SSBO.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <functional>


namespace Lexvi {
	struct IEntity {
		virtual std::vector<IEntity*> getChildren() = 0;
		virtual glm::mat4 getModel() const = 0;
		virtual glm::vec2 getExtraData() const = 0;
		virtual ~IEntity() = default;
	};

	struct IOwner {
		virtual IEntity* getRoot() = 0;
		virtual ~IOwner() = default;
		virtual glm::vec3 getPosition() = 0;
	};

	template<class MeshType>
	class InstanceSystem : public IRenderable {
	private:
		struct SubInstanceDataCPU {
			glm::mat4 model;
			glm::vec2 extraData;
			bool updated;
			bool active;
		};

		struct SubInstanceDataGPU {
			glm::mat4 model;
			glm::vec4 extraFlags;
		};

		SubInstanceDataGPU packSubInstance(const SubInstanceDataCPU& subInstance) {
			return SubInstanceDataGPU{
				subInstance.model,
				glm::vec4(subInstance.extraData, (subInstance.updated) ? 1.0f : 0.0f, (subInstance.active) ? 1.0f : 0.0f)
			};
		}

		std::vector<SubInstanceDataGPU> allSubInstances;
		std::vector<size_t> freeIndices;
		std::vector<size_t> pendingUpdates;

		uint32_t nextEntityID = 0;
		std::unordered_map<uint32_t, std::vector<size_t>> entityMap; // [entityID, subInstance indices]

	private:
		/* --- GPU Culling / Drawing --- */
		SSBO allSubInstancesSSBO;
		SSBO visibleSubInstancesSSBO;
		SSBO indirectBuffer;
		UBO frustumUBO;

		std::shared_ptr<ComputeShader> cullShader;
		std::shared_ptr<Camera> camera;

	private:
		MeshType baseMesh;
		std::function<void(MeshType&)> generateMeshFunc;

		const uint32_t MAX_UPDATE_PER_FRAME = 10000;
		const size_t DEFAULT_SUBINSTANCE_COUNT = 1'000'000;
		GLint maxX, maxY, maxZ;
		size_t MAX_SUBINSTANCE_COUNT = DEFAULT_SUBINSTANCE_COUNT;

		DrawElementsIndirectCommand drawCmd = {};

	public:
		InstanceSystem(std::function<void(MeshType&)> genMesh, std::shared_ptr<ComputeShader> cullShader)
			: generateMeshFunc(genMesh), cullShader(cullShader)
		{
			InitSystem();
		}

		inline void SetCurrentCamera(std::shared_ptr<Camera> cam) {
			camera = cam;
		}
	private:

		inline void InitSystem() {
			// Generate user-supplied mesh
			generateMeshFunc(baseMesh);

			drawCmd = {
			.count = static_cast<GLsizei>(baseMesh.indices.size()),
			.instanceCount = 0,
			.firstIndex = 0,
			.baseVertex = 0,
			.baseInstance = 0
			};

			CreateSSBO(allSubInstancesSSBO, MAX_SUBINSTANCE_COUNT * sizeof(SubInstanceDataGPU), 0);
			CreateSSBO(visibleSubInstancesSSBO, MAX_SUBINSTANCE_COUNT * sizeof(SubInstanceDataGPU), 1);

			ResizeSSBO(allSubInstancesSSBO, MAX_SUBINSTANCE_COUNT * sizeof(SubInstanceDataGPU));
			ResizeSSBO(visibleSubInstancesSSBO, MAX_SUBINSTANCE_COUNT * sizeof(SubInstanceDataGPU));

			// manually set indirectbuffer because of "GL_DYNAMIC_STORAGE_BIT"
			indirectBuffer = {
				.id = 0,
				.bindingPoint = 2,
				.size = sizeof(DrawElementsIndirectCommand)
			};
			glCreateBuffers(1, &indirectBuffer.id);
			glNamedBufferStorage(indirectBuffer.id, indirectBuffer.size, &drawCmd, GL_DYNAMIC_STORAGE_BIT);
			BindSSBO(indirectBuffer);

			// Allocate 6 vec4s worth of space (each vec4 = 16 bytes, so 6 * 16 = 96 bytes)
			CreateUBO(frustumUBO, sizeof(glm::vec4) * 6, 2);

			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxX);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxY);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxZ);

			ResizeSSBOs();
		}

	private:
		inline void setActive(const size_t& index, bool active) {
			glm::vec4 v = allSubInstances[index].extraFlags;
			v.z = active ? 1.0f : 0.0f;
			v.w = active ? 1.0f : 0.0f;
			allSubInstances[index].extraFlags = v;
		}

		inline void AllocateSubInstance(const uint32_t& entityID, const SubInstanceDataCPU& subInstance) {
			size_t index;
			if (freeIndices.empty()) {
				index = allSubInstances.size();
				allSubInstances.push_back(packSubInstance(subInstance));
			}
			else {
				index = freeIndices.back();
				allSubInstances[index] = packSubInstance(subInstance);
				freeIndices.pop_back();
			}

			entityMap[entityID].push_back(index);
			pendingUpdates.push_back(index);
		}

		inline void FreeSubInstance(const size_t& index) {
			freeIndices.push_back(index);
			pendingUpdates.push_back(index);
			setActive(index, false);
		}

		inline void FreeEntity(const uint32_t& entityID) {
			for (auto& index : entityMap[entityID])
				FreeSubInstance(index);
			entityMap.erase(entityID);
		}

		inline void UpdateSSBOs() {
			UpdateSSBO(indirectBuffer, &drawCmd, sizeof(DrawElementsIndirectCommand), 0);

			if (MAX_SUBINSTANCE_COUNT < allSubInstances.size())
				ResizeSSBOs();

			if (pendingUpdates.empty()) return;

			std::sort(pendingUpdates.begin(), pendingUpdates.end());
			pendingUpdates.erase(std::unique(pendingUpdates.begin(), pendingUpdates.end()), pendingUpdates.end());

			size_t updated = 0;
			while (updated < pendingUpdates.size() && updated < MAX_UPDATE_PER_FRAME) {
				size_t start = updated;
				size_t count = 1;

				while (start + count < pendingUpdates.size() &&
					pendingUpdates[start + count] == pendingUpdates[start + count - 1] + 1 &&
					(count + updated) <= MAX_UPDATE_PER_FRAME)
				{
					count++;
				}

				size_t ssboIndex = pendingUpdates[start];

				UpdateSSBO(allSubInstancesSSBO, &allSubInstances[ssboIndex], count * sizeof(SubInstanceDataGPU), ssboIndex * sizeof(SubInstanceDataGPU));

				updated += count;
			}

			pendingUpdates.erase(pendingUpdates.begin(), pendingUpdates.begin() + updated);
		}

		inline void ResizeSSBOs() {
			MAX_SUBINSTANCE_COUNT = std::max(DEFAULT_SUBINSTANCE_COUNT, allSubInstances.size() * 2);

			ResizeSSBO(allSubInstancesSSBO, MAX_SUBINSTANCE_COUNT * sizeof(SubInstanceDataGPU));
			ResizeSSBO(visibleSubInstancesSSBO, MAX_SUBINSTANCE_COUNT * sizeof(SubInstanceDataGPU));

			// Clear the old pending updates
			pendingUpdates.clear();

			// Fill pendingUpdates with all indices that are currently active
			for (size_t i = 0; i < allSubInstances.size(); ++i) {
				// Skip indices that are in freeIndices
				if (std::find(freeIndices.begin(), freeIndices.end(), i) != freeIndices.end())
					continue;

				pendingUpdates.push_back(i);
			}

		}

		inline void SendAllData() {
			if (MAX_SUBINSTANCE_COUNT < allSubInstances.size())
				ResizeSSBOs();

			UpdateSSBO(allSubInstancesSSBO, allSubInstances.data(), allSubInstances.size() * sizeof(SubInstanceDataGPU), 0);
			pendingUpdates.clear();
		}

	private:
		inline void RecursiveAddEntity(uint32_t entityID, IEntity* entity, const glm::vec3& origin = glm::vec3(0.0f)) {
			if (!entity) return;

			// Apply the origin offset as a translation
			glm::mat4 model = glm::translate(glm::mat4(1.0f), origin) * entity->getModel();

			SubInstanceDataCPU data{ model, entity->getExtraData(), true, true };
			AllocateSubInstance(entityID, data);

			for (auto child : entity->getChildren()) {
				RecursiveAddEntity(entityID, child, origin);
			}
		}

	public:
		inline uint32_t AddEntity(IOwner& owner) {
			uint32_t entityID = nextEntityID++;
			RecursiveAddEntity(entityID, owner.getRoot(), owner.getPosition());
			return entityID;
		}

		inline uint32_t Add_NONTREE_Entity(IOwner& owner) {
			uint32_t entityID = nextEntityID++;
			for (auto child : owner.getRoot()->getChildren()) {
				RecursiveAddEntity(entityID, child, owner.getPosition());
			}
			return entityID;
		}

		inline std::vector<uint32_t> AddEntities(const std::vector<IOwner*>& owners) {
			std::vector<uint32_t> ids;
			ids.reserve(owners.size());
			for (IOwner* owner : owners) {
				uint32_t id = nextEntityID++;
				RecursiveAddEntity(id, owner->getRoot(), owner->getPosition());
				ids.push_back(id);
			}
			SendAllData();
			return ids;
		}

		inline void UpdateEntity(uint32_t& entityID, IOwner& entity) {
			FreeEntity(entityID);
			entityID = AddEntity(entity);
		}

		inline void Draw(const Shader* shader) override {
			UpdateSSBOs();

			BindSSBO(allSubInstancesSSBO);
			BindSSBO(visibleSubInstancesSSBO);
			BindSSBO(indirectBuffer);

			glm::vec4 frustumPlanes[6];
			GetFrustumPlanesVec4(camera->getFrustum(), frustumPlanes);

			cullShader->use();
			cullShader->setUint("InstanceCount", static_cast<uint32_t>(allSubInstances.size()));
			cullShader->setVec3("cameraPos", camera->getPosition());
			cullShader->setFloat("maxDistance", camera->getZNearAndZFar().y);

			UpdateUBO(frustumUBO, frustumPlanes, sizeof(glm::vec4) * 6, 0);

			uint32_t threadsPerGroupX = 32; // match shader local_size_x
			uint32_t threadsPerGroupY = 32; // match shader local_size_y
			uint32_t threadsPerGroupZ = 1;  // match shader local_size_z

			uint32_t totalThreads = static_cast<uint32_t>(allSubInstances.size());

			if (totalThreads == 0) return;

			// Total groups needed to cover all threads
			uint32_t totalGroups = (totalThreads + threadsPerGroupX * threadsPerGroupY * threadsPerGroupZ - 1)
				/ (threadsPerGroupX * threadsPerGroupY * threadsPerGroupZ);

			// Try to divide groups into 3D grid
			uint32_t groupsX = std::min(static_cast<uint32_t>(maxX), totalGroups);
			uint32_t groupsY = std::min(static_cast<uint32_t>(maxY), (totalGroups + groupsX - 1) / groupsX);
			uint32_t groupsZ = std::min(static_cast<uint32_t>(maxZ), (totalGroups + groupsX * groupsY - 1) / (groupsX * groupsY));

			glDispatchCompute(groupsX, groupsY, groupsZ);
			glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

			shader->use();
			glBindVertexArray(baseMesh.VAO);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer.id);
			glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);
		}
	};
}
#endif // !INSTANCESYSTEM_HPP