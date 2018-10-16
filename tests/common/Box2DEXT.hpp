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

    const b2Vec2& bPos = WORLD_TO_SCREEN(body->GetPosition());
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

    int cx = -camera.x + (camera.w >> 1);
    int cy = -camera.y + (camera.h >> 1);

    for(b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
        if(!body->IsActive()) {
            continue;
        }

        b2DrawBody(renderer, body, camera);
        
        

        for(b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        }
    }
}
