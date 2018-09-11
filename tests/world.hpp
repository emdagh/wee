#pragma once


#ifndef PTM_RATIO
#define PTM_RATIO   (40.f)
#endif

#define WORLD_TO_SCREEN(x)  ((x) * (PTM_RATIO))
#define SCREEN_TO_WORLD(x)  ((x) / (PTM_RATIO))
