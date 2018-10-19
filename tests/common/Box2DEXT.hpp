#pragma once

#include "world.hpp"
#include <SDL.h>
#include <Box2D/Box2D.h>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>

void b2DrawJoints(SDL_Renderer* renderer, b2Body* body, const SDL_Rect& camera) {
    SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Red);

    int cx = -camera.x + (camera.w >> 1);
    int cy = -camera.y + (camera.h >> 1);

    for(auto* ptr = body->GetJointList(); ptr; ptr = ptr->next) {
        auto* joint = ptr->joint;
        const b2Vec2& a = WORLD_TO_SCREEN(joint->GetAnchorA()); // world coordinates of anchor point.
        const b2Vec2& b = WORLD_TO_SCREEN(joint->GetAnchorB());

        SDL_RenderDrawLine(renderer, 
            (int)a.x + cx,
			(int)a.y + cy,
			(int)b.x + cx,
			(int)b.y + cy
        );


    }
}

void b2DrawFixtures(SDL_Renderer* renderer, b2Body* body, const SDL_Rect& camera) {
    SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Lime);

    int cx = -camera.x + (camera.w >> 1);
    int cy = -camera.y + (camera.h >> 1);
    
    const b2Vec2& bPos = WORLD_TO_SCREEN(body->GetPosition());
    for(auto* fixture= body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        b2Shape::Type shapeType = fixture->GetType();

        if ( shapeType == b2Shape::e_circle ){
            b2CircleShape* circleShape = (b2CircleShape*)fixture->GetShape();
            float radius = WORLD_TO_SCREEN(circleShape->m_radius);
            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);
            SDL_RenderDrawCircleEXT(renderer, 
                    cx + (int)(bPos.x + 0.5f), 
                    cy + (int)(bPos.y + 0.5f), 
                    (int)radius
            );

        } else if ( shapeType == b2Shape::e_chain ) {
            b2ChainShape* chainShape = (b2ChainShape*)fixture->GetShape();

            for(int i=0; i < chainShape->m_count - 1; i++) {
                const b2Vec2& a = WORLD_TO_SCREEN(chainShape->m_vertices[i]);
                const b2Vec2& b = WORLD_TO_SCREEN(chainShape->m_vertices[i + 1]);
                SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);

                SDL_RenderDrawLine(renderer, 
                    cx + (int)(bPos.x + a.x + 0.5f),
                    cy + (int)(bPos.y + a.y + 0.5f),
                    cx + (int)(bPos.x + b.x + 0.5f),
                    cy + (int)(bPos.y + b.y + 0.5f)
                );

            }
        } else if ( shapeType == b2Shape::e_polygon ) {
            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);
            b2PolygonShape* shape = (b2PolygonShape*)fixture->GetShape();

            for(int i=0; i < shape->m_count - 1; i++) {
                const b2Vec2& a = bPos + WORLD_TO_SCREEN(shape->m_vertices[i]);
                const b2Vec2& b = bPos + WORLD_TO_SCREEN(shape->m_vertices[i + 1]);

                SDL_RenderDrawLine(renderer,
                    cx + (int)(a.x + .5f),
                    cy + (int)(a.y + .5f),
                    cx + (int)(b.x + .5f),
                    cy + (int)(b.y + .5f)
                );
            }


            const b2Vec2& a = bPos + WORLD_TO_SCREEN(shape->m_vertices[0]);
            const b2Vec2& b = bPos + WORLD_TO_SCREEN(shape->m_vertices[shape->m_count-1]);

            SDL_RenderDrawLine(renderer,
                cx + (int)(a.x + .5f),
                cy + (int)(a.y + .5f),
                cx + (int)(b.x + .5f),
                cy + (int)(b.y + .5f)
            );
           
        } else if(shapeType == b2Shape::e_edge) {
            b2EdgeShape* edgeShape = static_cast<b2EdgeShape*>(fixture->GetShape());
            const b2Vec2& a = WORLD_TO_SCREEN(edgeShape->m_vertex1);
            const b2Vec2& b = WORLD_TO_SCREEN(edgeShape->m_vertex2);

            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);

            SDL_RenderDrawLine(renderer, 
                cx + (int)(a.x + 0.5f), 
                cy + (int)(a.y + 0.5f), 
                cx + (int)(b.x + 0.5f), 
                cy + (int)(b.y + 0.5f)
            );
        }
    }
}

void b2DrawBody(SDL_Renderer* renderer, b2Body* body, const SDL_Rect& camera) {
    b2DrawFixtures(renderer, body, camera);
    b2DrawJoints(renderer, body, camera);
}

void b2DebugDrawEXT(b2World* world, SDL_Renderer* renderer, const SDL_Rect& camera) {

    //int cx = -camera.x + (camera.w >> 1);
    //int cy = -camera.y + (camera.h >> 1);

    for(b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
        if(!body->IsActive()) {
            continue;
        }

        b2DrawBody(renderer, body, camera);
        
        

        for(b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        }
    }
}

class b2DebugDrawImpl : public b2Draw {
    SDL_Renderer* _renderer;
    SDL_Rect _rect = { 0, 0, 0, 0 };
public:
    void SetRenderer(SDL_Renderer* renderer) {
        _renderer = renderer;
        SDL_RenderGetLogicalSize(renderer, &_rect.w, &_rect.h);
    }

    void SetCameraPosition(int x, int y) {
        _rect.x = x;
        _rect.y = y;
    }

public:
	void DrawParticles(const b2Vec2 *, float32 , const b2ParticleColor *, int32 ) {}
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
        assert(_renderer);

        int cx = -_rect.x + (_rect.w >> 1);
        int cy = -_rect.y + (_rect.h >> 1);

        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        
        for(int32 i=0; i < vertexCount; i++) {
            const b2Vec2& a_WS = vertices[i];
            const b2Vec2& b_WS = vertices[(i + 1) % vertexCount];

            b2Vec2 a_SS = WORLD_TO_SCREEN(a_WS);
            b2Vec2 b_SS = WORLD_TO_SCREEN(b_WS);

            int x0 = cx + static_cast<int>(a_SS.x + 0.5f);
            int y0 = cy + static_cast<int>(a_SS.y + 0.5f);
            int x1 = cx + static_cast<int>(b_SS.x + 0.5f);
            int y1 = cy + static_cast<int>(b_SS.y + 0.5f);
            
            SDL_RenderDrawLine(_renderer, x0, y0, x1, y1);
        }
    }
    void DrawSolidPolygon(const b2Vec2* positions, int32 vertexCount, const b2Color& color) {
        //SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        DrawPolygon(positions, vertexCount, color);
    }
    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {

        int iradius = WORLD_TO_SCREEN(radius);
        b2Vec2 PositionSS = WORLD_TO_SCREEN(center);
        int x = static_cast<int>(PositionSS.x + 0.5f);
        int y = static_cast<int>(PositionSS.y + 0.5f);
        int cx = -_rect.x + (_rect.w >> 1);
        int cy = -_rect.y + (_rect.h >> 1);

        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawCircleEXT(_renderer, cx + x, cy + y, iradius);

        
    }
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2&, const b2Color& color) {
        //SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        DrawCircle(center, radius, color);
    }
    void DrawSegment(const b2Vec2& _p1, const b2Vec2& _p2, const b2Color& color) {
        int cx = -_rect.x + (_rect.w >> 1);
        int cy = -_rect.y + (_rect.h >> 1);

        auto p1 = WORLD_TO_SCREEN(_p1);
        auto p2 = WORLD_TO_SCREEN(_p2);

        int x0 = cx + static_cast<int>(p1.x + 0.5f);
        int y0 = cy + static_cast<int>(p1.y + 0.5f);
        int x1 = cx + static_cast<int>(p2.x + 0.5f);
        int y1 = cy + static_cast<int>(p2.y + 0.5f);

        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawLine(_renderer, x0, y0, x1, y1); 
    }
    void DrawTransform(const b2Transform& ) {
    }
};
