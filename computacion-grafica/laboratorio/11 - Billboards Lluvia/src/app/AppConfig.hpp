#pragma once

#include <cstddef>

#include <glm/vec3.hpp>

namespace appconfig
{
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr const char *WINDOW_TITLE = "Billboards - Rain Scene";
    constexpr std::size_t RAIN_DROP_COUNT = 350U;
    constexpr float MAX_FRAME_DELTA = 0.033f;
    constexpr glm::vec3 CLEAR_COLOR(0.07f, 0.10f, 0.16f);
    constexpr glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

    namespace camera
    {
        constexpr float ROTATION_SPEED = 65.0f;
        constexpr float MIN_PITCH = -75.0f;
        constexpr float MAX_PITCH = 15.0f;
        constexpr float DISTANCE = 14.0f;
        constexpr float FIELD_OF_VIEW_DEGREES = 50.0f;
        constexpr float NEAR_PLANE = 0.1f;
        constexpr float FAR_PLANE = 60.0f;
        constexpr float INITIAL_YAW_DEGREES = -25.0f;
        constexpr float INITIAL_PITCH_DEGREES = -20.0f;
        constexpr glm::vec3 LOOK_TARGET(0.0f, 1.4f, 0.0f);
    }

    namespace rain
    {
        constexpr float SPAWN_RADIUS = 9.0f;
        constexpr float SPAWN_TOP = 8.5f;
        constexpr float SPAWN_MIN_HEIGHT = 0.4f;
        constexpr float GROUND_HEIGHT = 0.0f;
        constexpr float MIN_SPEED = 6.5f;
        constexpr float MAX_SPEED = 10.5f;
        constexpr float MIN_HALF_WIDTH = 0.025f;
        constexpr float MAX_HALF_WIDTH = 0.04f;
        constexpr float MIN_HALF_HEIGHT = 0.22f;
        constexpr float MAX_HALF_HEIGHT = 0.36f;
        constexpr float MIN_ALPHA = 0.05f;
        constexpr float ALPHA_SCALE = 0.88f;
        constexpr glm::vec3 TINT(0.68f, 0.82f, 1.0f);
        constexpr unsigned char TEXTURE_RED = 220;
        constexpr unsigned char TEXTURE_GREEN = 235;
        constexpr unsigned char TEXTURE_BLUE = 255;
        constexpr int TEXTURE_WIDTH = 32;
        constexpr int TEXTURE_HEIGHT = 64;
        constexpr float TEXTURE_BASE_WIDTH = 0.16f;
        constexpr float TEXTURE_WIDTH_GAIN = 0.38f;
        constexpr float TEXTURE_VERTICAL_FADE_POWER = 1.35f;
        constexpr float TEXTURE_ALPHA_POWER = 1.8f;
        constexpr float BILLBOARD_FORWARD_FALLBACK_DISTANCE_SQUARED = 0.0001f;
    }

    namespace ground
    {
        constexpr float HALF_EXTENT = 12.0f;
    }
}
