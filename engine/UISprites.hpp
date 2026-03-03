#pragma once

#include <SDL2/SDL.h>

struct SpriteDef 
{
    SDL_Rect src;
};

namespace UISprites 
{
    inline const SpriteDef Play     {{ 210,  45, 117, 45 }};
    inline const SpriteDef Options {{ 179, 266, 186, 45 }};
    inline const SpriteDef Exit {{ 212, 322, 118, 43 }};
}