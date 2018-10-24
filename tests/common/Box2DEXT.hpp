#pragma once

#include "world.hpp"
#include <SDL.h>
#include <Box2D/Box2D.h>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <core/mat4.hpp>
#include <core/vec3.hpp>

using wee::mat4;
using wee::vec3;

std::ostream& operator << (std::ostream& os, const b2Vec2& v) {
    os << v.x << ", " << v.y;
    return os;
}

b2Vec2 transform_mat4(const b2Vec2& in, const mat4& m) {
    vec3 temp = {
        in.x, in.y, 0.0f
    };
    vec3 temp2 = vec3::transform(temp, m);
    return b2Vec2(temp2.x, temp2.y);
}

class b2DebugDrawImpl : public b2Draw {
    SDL_Renderer* _renderer;
    mat4 _transform;
public:
    void SetCameraTransform(const mat4& tx) { _transform = tx; }
    void SetRenderer(SDL_Renderer* renderer) {
        _renderer = renderer;
        //int w, h;
        //SDL_RenderGetLogicalSize(renderer, &w, &h);
        
    }


public:
	void DrawParticles(const b2Vec2 *, float32 , const b2ParticleColor *, int32 ) {}
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
        assert(_renderer);

        //int cx = -_rect.x + (_rect.w >> 1);
        //int cy = -_rect.y + (_rect.h >> 1);

        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        
        for(int32 i=0; i < vertexCount; i++) {
            const b2Vec2& a_WS = vertices[i];
            const b2Vec2& b_WS = vertices[(i + 1) % vertexCount];

            b2Vec2 a_SS = transform_mat4(WORLD_TO_SCREEN(a_WS), _transform);
            b2Vec2 b_SS = transform_mat4(WORLD_TO_SCREEN(b_WS), _transform);

            int x0 = static_cast<int>(a_SS.x + 0.5f);
            int y0 = static_cast<int>(a_SS.y + 0.5f);
            int x1 = static_cast<int>(b_SS.x + 0.5f);
            int y1 = static_cast<int>(b_SS.y + 0.5f);
            
            SDL_RenderDrawLine(_renderer, x0, y0, x1, y1);
        }
    }
    void DrawSolidPolygon(const b2Vec2* positions, int32 vertexCount, const b2Color& color) {
        //SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        DrawPolygon(positions, vertexCount, color);
    }
    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {

        int iradius = WORLD_TO_SCREEN(radius);
        b2Vec2 PositionSS = transform_mat4(WORLD_TO_SCREEN(center), _transform);
        int x = static_cast<int>(PositionSS.x + 0.5f);
        int y = static_cast<int>(PositionSS.y + 0.5f);
        //int cx = -_rect.x + (_rect.w >> 1);
        //int cy = -_rect.y + (_rect.h >> 1);

        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawCircleEXT(_renderer, x, y, iradius);

        
    }
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2&, const b2Color& color) {
        //SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        DrawCircle(center, radius, color);
    }
    void DrawSegment(const b2Vec2& _p1, const b2Vec2& _p2, const b2Color& color) {

        auto p1 = transform_mat4(WORLD_TO_SCREEN(_p1), _transform);
        auto p2 = transform_mat4(WORLD_TO_SCREEN(_p2), _transform);

        int x0 = static_cast<int>(p1.x + 0.5f);
        int y0 = static_cast<int>(p1.y + 0.5f);
        int x1 = static_cast<int>(p2.x + 0.5f);
        int y1 = static_cast<int>(p2.y + 0.5f);

        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawLine(_renderer, x0, y0, x1, y1); 
    }
    void DrawTransform(const b2Transform& ) {
    }
};
