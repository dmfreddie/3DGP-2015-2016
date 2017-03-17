#include "FirstPersonMovement.hpp"
#include <SceneModel/Context.hpp>
#include <random>

using namespace SceneModel;

Context::Context() : _patches(2)
{
    start_time_ = std::chrono::system_clock::now();
    time_seconds_ = 0.f;

    camera_movement_ = std::make_shared<FirstPersonMovement>();
    camera_movement_->init(glm::vec3(0, 16, 0), -0.785f, -0.25f);

    camera_ = std::make_unique<Camera>();
    camera_->setVerticalFieldOfViewInDegrees(50.f);
    camera_->setNearPlaneDistance(1);
    camera_->setFarPlaneDistance(12000);

    std::default_random_engine rand(0);
    std::uniform_real_distribution<float> x_dis(0, 1);
    std::uniform_real_distribution<float> z_dis(0, 1);
    const size_t shape_count = 10000;
    shape_positions_.resize(shape_count);
    for (size_t i = 0; i < shape_count; ++i)
    {
        const float x = x_dis(rand);
        const float z = z_dis(rand);
        const glm::vec2 pos{ x * x * x * getTerrainSizeX(),
                             z * z * z * getTerrainSizeZ() };
        shape_positions_[i] = pos;
    }

    _patches[0].at(0, 0) = glm::vec3(0, 0, 0);
    _patches[0].at(0, 1) = glm::vec3(0, 0, -2731);
    _patches[0].at(0, 2) = glm::vec3(0, 0, -5461);
    _patches[0].at(0, 3) = glm::vec3(0, 538, -8192);
    _patches[0].at(1, 0) = glm::vec3(1365, 0, 0);
    _patches[0].at(1, 1) = glm::vec3(2156, 1333, -2731);
    _patches[0].at(1, 2) = glm::vec3(2156, 2666, -5461);
    _patches[0].at(1, 3) = glm::vec3(1365, 0, -8192);
    _patches[0].at(2, 0) = glm::vec3(1051, -1611, 182);
    _patches[0].at(2, 1) = glm::vec3(1051, -1241, -2574);
    _patches[0].at(2, 2) = glm::vec3(1051, -871, -5329);
    _patches[0].at(2, 3) = glm::vec3(1051, -501, -8085);
    _patches[0].at(3, 0) = glm::vec3(4091, -556, 38);
    _patches[0].at(3, 1) = glm::vec3(4091, -186, -2718);
    _patches[0].at(3, 2) = glm::vec3(4091, 184, -5474);
    _patches[0].at(3, 3) = glm::vec3(4091, 554, -8192);

    _patches[1].at(0, 0) = glm::vec3(4091, -556, 38);
    _patches[1].at(0, 1) = glm::vec3(4091, -186, -2718);
    _patches[1].at(0, 2) = glm::vec3(4091, 184, -5474);
    _patches[1].at(0, 3) = glm::vec3(4091, 554, -8192);
    _patches[1].at(1, 0) = glm::vec3(7137, 1437, -106);
    _patches[1].at(1, 1) = glm::vec3(7141, 867, -2862);
    _patches[1].at(1, 2) = glm::vec3(7136, 1238, -5618);
    _patches[1].at(1, 3) = glm::vec3(7131, 1609, -8192);
    _patches[1].at(2, 0) = glm::vec3(6827, 939, 0);
    _patches[1].at(2, 1) = glm::vec3(6827, 0, -2731);
    _patches[1].at(2, 2) = glm::vec3(6827, 0, -5461);
    _patches[1].at(2, 3) = glm::vec3(6827, 0, -8192);
    _patches[1].at(3, 0) = glm::vec3(8192, 939, 0);
    _patches[1].at(3, 1) = glm::vec3(8192, 939, -2731);
    _patches[1].at(3, 2) = glm::vec3(8192, 0, -5461);
    _patches[1].at(3, 3) = glm::vec3(8192, 0, -8192);
}

const std::vector<Array2d<glm::vec3, 4, 4>>& Context::getTerrainPatches() const
{
    return _patches;
}

std::string Context::getTerrainDisplacementMapName() const
{
    return "terrain.png";
}

float Context::getTerrainSizeX() const
{
    return 8192;
}

float Context::getTerrainSizeY() const
{
    return 1000;
}

float Context::getTerrainSizeZ() const
{
    return 8192;
}

const std::vector<glm::vec2>& Context::getAllShapePositions() const
{
    return shape_positions_;
}

void Context::update()
{
    const auto clock_time = std::chrono::system_clock::now() - start_time_;
    const auto clock_millisecs
        = std::chrono::duration_cast<std::chrono::milliseconds>(clock_time);
    const float prev_time = time_seconds_;
    time_seconds_ = 0.001f * clock_millisecs.count();
    const float dt = time_seconds_ - prev_time;

    auto camera_translation_speed = getCamera().getLinearVelocity();
    auto camera_rotation_speed = getCamera().getRotationalVelocity();
    camera_movement_->moveForward(camera_translation_speed.z * dt);
    camera_movement_->moveRight(camera_translation_speed.x * dt);
    camera_movement_->spinHorizontal(camera_rotation_speed.x * dt);
    camera_movement_->spinVertical(camera_rotation_speed.y * dt);
    camera_->setPosition(camera_movement_->position());
    camera_->setDirection(camera_movement_->direction());
}

float Context::getTimeInSeconds() const
{
    return time_seconds_;
}

const Camera& Context::getCamera() const
{
    return *camera_;
}

Camera& Context::getCamera()
{
    return *camera_;
}
