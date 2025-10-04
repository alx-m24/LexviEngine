#include "pch.h"

#include "Camera/Camera.hpp"

using namespace Lexvi;

float CameraPlane::getSignedDistanceToPlane(const glm::vec3& point) const {
    return glm::dot(normal, point) + distance;
}

bool Lexvi::isInFrustum(const CameraFrustum& frustum, const CameraAABB& aabb) {
    // For each plane, check if all 8 corners are outside
    for (int i = 0; i < 6; ++i) {
        const CameraPlane& plane = frustum.planes[i];

        // Compute positive vertex for plane normal
        glm::vec3 p = aabb.min;
        if (plane.normal.x >= 0) p.x = aabb.max.x;
        if (plane.normal.y >= 0) p.y = aabb.max.y;
        if (plane.normal.z >= 0) p.z = aabb.max.z;

        if (plane.getSignedDistanceToPlane(p) < 0) {
            // All points outside this plane -> AABB is outside frustum
            return false;
        }
    }
    return true;
}

bool Lexvi::IntersectPlanes(const CameraPlane& p1, const CameraPlane& p2, const CameraPlane& p3, glm::vec3& outPoint) {
    glm::vec3 n1 = p1.normal;
    glm::vec3 n2 = p2.normal;
    glm::vec3 n3 = p3.normal;

    float denom = glm::dot(n1, glm::cross(n2, n3));
    if (fabs(denom) < 1e-6f) return false; // planes are nearly parallel

    outPoint = (
        -p1.distance * glm::cross(n2, n3) -
        p2.distance * glm::cross(n3, n1) -
        p3.distance * glm::cross(n1, n2)
        ) / denom;

    return true;
}

std::vector<glm::vec3> Lexvi::GetFrustumCorners(const CameraFrustum& frustum) {
    std::vector<glm::vec3> corners;
    corners.reserve(8);

    // Plane indices: 0=left, 1=right, 2=top, 3=bottom, 4=near, 5=far
    int combinations[8][3] = {
        {0, 2, 4}, // left, top, near
        {1, 2, 4}, // right, top, near
        {0, 3, 4}, // left, bottom, near
        {1, 3, 4}, // right, bottom, near
        {0, 2, 5}, // left, top, far
        {1, 2, 5}, // right, top, far
        {0, 3, 5}, // left, bottom, far
        {1, 3, 5}  // right, bottom, far
    };

    for (int i = 0; i < 8; ++i) {
        glm::vec3 corner;
        if (IntersectPlanes(frustum.planes[combinations[i][0]],
            frustum.planes[combinations[i][1]],
            frustum.planes[combinations[i][2]],
            corner)) {
            corners.push_back(corner);
        }
    }

    return corners;
}

void Lexvi::GetFrustumPlanesVec4(const CameraFrustum& frustum, glm::vec4 outPlanes[6]) {
    for (int i = 0; i < 6; ++i) {
        const CameraPlane& p = frustum.planes[i];
        outPlanes[i] = glm::vec4(p.normal, p.distance);
    }
}

void Lexvi::updateFrustum(CameraFrustum& frustum, const glm::mat4& projView) {
    glm::mat4 m = projView;

    // Left
    frustum.planes[0].normal.x = m[0][3] + m[0][0];
    frustum.planes[0].normal.y = m[1][3] + m[1][0];
    frustum.planes[0].normal.z = m[2][3] + m[2][0];
    frustum.planes[0].distance = m[3][3] + m[3][0];

    // Right
    frustum.planes[1].normal.x = m[0][3] - m[0][0];
    frustum.planes[1].normal.y = m[1][3] - m[1][0];
    frustum.planes[1].normal.z = m[2][3] - m[2][0];
    frustum.planes[1].distance = m[3][3] - m[3][0];

    // Bottom
    frustum.planes[2].normal.x = m[0][3] + m[0][1];
    frustum.planes[2].normal.y = m[1][3] + m[1][1];
    frustum.planes[2].normal.z = m[2][3] + m[2][1];
    frustum.planes[2].distance = m[3][3] + m[3][1];

    // Top
    frustum.planes[3].normal.x = m[0][3] - m[0][1];
    frustum.planes[3].normal.y = m[1][3] - m[1][1];
    frustum.planes[3].normal.z = m[2][3] - m[2][1];
    frustum.planes[3].distance = m[3][3] - m[3][1];

    // Near
    frustum.planes[4].normal.x = m[0][3] + m[0][2];
    frustum.planes[4].normal.y = m[1][3] + m[1][2];
    frustum.planes[4].normal.z = m[2][3] + m[2][2];
    frustum.planes[4].distance = m[3][3] + m[3][2];

    // Far
    frustum.planes[5].normal.x = m[0][3] - m[0][2];
    frustum.planes[5].normal.y = m[1][3] - m[1][2];
    frustum.planes[5].normal.z = m[2][3] - m[2][2];
    frustum.planes[5].distance = m[3][3] - m[3][2];

    // Normalize planes
    for (int i = 0; i < 6; ++i) {
        float length = glm::length(frustum.planes[i].normal);
        frustum.planes[i].normal /= length;
        frustum.planes[i].distance /= length;
    }
}

CameraFrustum Lexvi::Camera::getFrustum() const
{
    return frustum;
}

void Lexvi::Camera::SetInputSystem(std::shared_ptr<Input> inputSys)
{
    inputSystem = std::move(inputSys);;
}

float Lexvi::Camera::getAspectRatio() const
{
    return aspectRatio;
}

glm::vec3 Lexvi::Camera::getPosition() const
{
    return position;
}

glm::vec3 Lexvi::Camera::getFront() const
{
    return front;
}

glm::vec3 Lexvi::Camera::getUp() const
{
    return worldUp;
}

glm::vec2 Lexvi::Camera::getZNearAndZFar() const
{
    return glm::vec2(zNear, zFar);
}

glm::mat4 Lexvi::Camera::getViewMatrix() const
{
    return viewMatrix;
}

glm::mat4 Lexvi::Camera::getProjectionMatrix() const
{
    return projectionMatrix;
}

void Lexvi::Camera::updateVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);

    right = glm::normalize(glm::cross(front, worldUp));
}

void Lexvi::Camera::updateMatricesAndFrustum()
{
    viewMatrix = glm::lookAt(position, position + front, worldUp);
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
    updateFrustum(frustum, projectionMatrix * viewMatrix);
}
