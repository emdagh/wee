#pragma once

#include "world.hpp"
#include <SDL.h>
#include <Box2D/Box2D.h>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>

void b2DebugDrawEXT(b2World* world, SDL_Renderer* renderer, const SDL_Rect& camera) {

    int cx = -camera.x + (camera.w >> 1);
    int cy = -camera.y + (camera.h >> 1);

    for(b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
        for(b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            b2Shape::Type shapeType = fixture->GetType();

            if ( shapeType == b2Shape::e_circle ){
                b2CircleShape* circleShape = (b2CircleShape*)fixture->GetShape();
                const b2Vec2& wPos = WORLD_TO_SCREEN(body->GetPosition());//circleShape->m_p);
                float radius = WORLD_TO_SCREEN(circleShape->m_radius);
                SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::IndianRed);
                SDL_RenderDrawCircleEXT(renderer, 
                        cx + (int)(wPos.x + 0.5f), 
                        cy + (int)(wPos.y + 0.5f), 
                        radius
                );

            } else if ( shapeType == b2Shape::e_polygon ) {
                b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
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
}
