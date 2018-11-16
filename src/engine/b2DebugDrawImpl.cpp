#include <engine/b2DebugDrawImpl.hpp>
#include <core/mat4.hpp>
#include <core/vec3.hpp>
#include <gfx/SDL_RendererEXT.hpp>

using namespace wee;

b2Vec2 transform_mat4(const b2Vec2& in, const mat4& m) {
    vec3 temp = {
        in.x, in.y, 0.0f
    };
    vec3 temp2 = vec3::transform(temp, m);
    return b2Vec2(temp2.x, temp2.y);
}

void b2DebugDrawImpl::SetCameraTransform(const mat4& tx) { 
    _transform = tx; 
}

void b2DebugDrawImpl::SetRenderer(SDL_Renderer* renderer) {
    _renderer = renderer;
    //int w, h;
    //SDL_RenderGetLogicalSize(renderer, &w, &h);
    
}

void b2DebugDrawImpl::DrawParticles(const b2Vec2 *, float32 , const b2ParticleColor *, int32 ) {
}

void b2DebugDrawImpl::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
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

void b2DebugDrawImpl::DrawSolidPolygon(const b2Vec2* positions, int32 vertexCount, const b2Color& color) {
    //SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
    DrawPolygon(positions, vertexCount, color);
}

void b2DebugDrawImpl::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {

    int iradius = WORLD_TO_SCREEN(radius);
    b2Vec2 PositionSS = transform_mat4(WORLD_TO_SCREEN(center), _transform);
    int x = static_cast<int>(PositionSS.x + 0.5f);
    int y = static_cast<int>(PositionSS.y + 0.5f);
    //int cx = -_rect.x + (_rect.w >> 1);
    //int cy = -_rect.y + (_rect.h >> 1);

    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawCircleEXT(_renderer, x, y, iradius);

    
}

void b2DebugDrawImpl::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2&, const b2Color& color) {
    //SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
    DrawCircle(center, radius, color);
}

void b2DebugDrawImpl::DrawSegment(const b2Vec2& _p1, const b2Vec2& _p2, const b2Color& color) {

    auto p1 = transform_mat4(WORLD_TO_SCREEN(_p1), _transform);
    auto p2 = transform_mat4(WORLD_TO_SCREEN(_p2), _transform);

    int x0 = static_cast<int>(p1.x + 0.5f);
    int y0 = static_cast<int>(p1.y + 0.5f);
    int x1 = static_cast<int>(p2.x + 0.5f);
    int y1 = static_cast<int>(p2.y + 0.5f);

    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawLine(_renderer, x0, y0, x1, y1); 
}

void b2DebugDrawImpl::DrawTransform(const b2Transform& ) {
}
