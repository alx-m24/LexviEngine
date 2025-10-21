#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Lexvi {
    class Input;

    struct CameraPlane {
        glm::vec3 normal;
        float distance;

        // Returns positive if point is in front of the plane
        float getSignedDistanceToPlane(const glm::vec3& point) const;
    };

    struct CameraFrustum {
        CameraPlane planes[6]; // left, right, top, bottom, near, far
    };

    struct CameraAABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    struct CameraData {
        glm::mat4 view;
        glm::mat4 projection;
    };

    bool isInFrustum(const CameraFrustum& frustum, const CameraAABB& aabb);

    bool IntersectPlanes(const CameraPlane& p1, const CameraPlane& p2, const CameraPlane& p3, glm::vec3& outPoint);

    // Return 8 corners of frustum in world space
    std::vector<glm::vec3> GetFrustumCorners(const CameraFrustum& frustum);

    void GetFrustumPlanesVec4(const CameraFrustum& frustum, glm::vec4 outPlanes[6]);

    void updateFrustum(CameraFrustum& frustum, const glm::mat4& projView);

    class Camera {

    protected:
        glm::vec3 position{ 0.0f };
        glm::vec3 front{ 0.0f, 0.0f, -1.0f };
        glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };
        glm::vec3 right{ 1.0f, 0.0f, 0.0f };

        float yaw = -90.0f;
        float pitch = 0.0f;
        float fov = 75.0f;

        float mouseSensitivity = 0.08f;
        float aspectRatio = 1.0f;
        float zNear = 0.1f;
        float zFar = 100.0f;

        glm::mat4 viewMatrix{ 1.0f };
        glm::mat4 projectionMatrix{ 1.0f };

        CameraFrustum frustum{};
        CameraData cameraData{};

        std::shared_ptr<Input> inputSystem = nullptr;

    protected:
        virtual void updateMatricesAndFrustum();
        virtual void updateVectors();

    public:
        Camera() = default;
        virtual ~Camera() = default;

        // Called by engine
        virtual void update(float dt) {
            // default does nothing
        }

        // Getters
        float getAspectRatio() const;
        float getFOV() const { return fov; };
        glm::vec3 getPosition() const;
        glm::vec3 getFront() const;
        glm::vec3 getUp() const;
        glm::vec3 getRight() const;
        glm::vec2 getZNearAndZFar() const;
        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix() const;
        CameraFrustum getFrustum() const;

        const CameraData& getCameraData() const{ return cameraData; };

    public:
        void SetInputSystem(std::shared_ptr<Input> inputSys);
    };
}