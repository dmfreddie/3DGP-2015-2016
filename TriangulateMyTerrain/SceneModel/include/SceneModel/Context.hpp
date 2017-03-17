#pragma once

#include "Camera.hpp"
#include "Array2d.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <chrono>
#include <memory>

namespace SceneModel
{

class FirstPersonMovement;

class Context
{
public:
    Context();

    // Bezier patches
    const std::vector<Array2d<glm::vec3, 4, 4>>& getTerrainPatches() const;

    std::string getTerrainDisplacementMapName() const;

    // X and -Z are on the ground, Y is up
    float getTerrainSizeX() const;
    float getTerrainSizeY() const;
    float getTerrainSizeZ() const;

    // Positions to be used as world space X and -Z
    const std::vector<glm::vec2>& getAllShapePositions() const;

    void update();

    float getTimeInSeconds() const;

    const Camera& getCamera() const;
    Camera& getCamera();

private:
    std::chrono::system_clock::time_point start_time_;
    float time_seconds_;

    std::shared_ptr<FirstPersonMovement> camera_movement_;
    std::unique_ptr<Camera> camera_;

    std::vector<glm::vec2> shape_positions_;

    std::vector<Array2d<glm::vec3, 4, 4>> _patches;
};

} // end namespace SceneModel
