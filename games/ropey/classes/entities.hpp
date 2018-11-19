#pragma once

#include <Box2D/Box2D.h>
#include <classes/common.hpp>
#include <core/vec2.hpp>

class b2World;
struct SDL_Point;


entity_type create_player(b2World* world, const wee::vec2f& at);
entity_type create_block(b2World* world, const SDL_Rect& r) ;
entity_type create_rope(b2World* world, entity_type a, entity_type b, const b2Vec2& bPosWS); 
entity_type create_sensor(b2World* world, entity_type parent, const wee::vec2f& offset, float radius, const wee::collision_callback&);
entity_type create_tile(SDL_Texture* texture, const SDL_Point& dst, const SDL_Rect& src, const SDL_RendererFlip& flip, float radians);
//entity_type create_pickup(b2World*, const wee::vec2f& at, int value);
