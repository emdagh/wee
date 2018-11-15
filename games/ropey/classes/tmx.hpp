#pragma once


#include <string>

class b2World;
struct SDL_Point;
typedef int gid;

void parse_tmx(b2World* world, const std::string& pt, SDL_Point* spawnPoint);
