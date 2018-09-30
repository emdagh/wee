#pragma once

#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <Box2D/Box2D.h>
#include <SDL.h>
#include <util/logstream.hpp>

#ifndef PTM_RATIO
#define PTM_RATIO   (40.f)
#endif

#define WORLD_TO_SCREEN(x)  ((x) * (PTM_RATIO))
#define SCREEN_TO_WORLD(x)  ((x) / (PTM_RATIO))
float randf(float scale_ = 1.0f) {
    return scale_ * (static_cast<float>(rand()) / RAND_MAX);
}
