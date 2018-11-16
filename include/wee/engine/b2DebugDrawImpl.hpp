#pragma once

#include <Box2D/Box2D.h>
#include <core/mat4.hpp>
#include <engine/ecs.hpp>

namespace wee {
    class b2DebugDrawImpl : public b2Draw {
        SDL_Renderer* _renderer;
        mat4 _transform;
    public:
        void SetCameraTransform(const mat4& tx);
        void SetRenderer(SDL_Renderer* renderer); 
    public:
        void DrawParticles(const b2Vec2 *, float32 , const b2ParticleColor *, int32 );
        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
        void DrawSolidPolygon(const b2Vec2* positions, int32 vertexCount, const b2Color& color); 
        void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
        void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2&, const b2Color& color);
        void DrawSegment(const b2Vec2& _p1, const b2Vec2& _p2, const b2Color& color);
        void DrawTransform(const b2Transform&);
    };
}
