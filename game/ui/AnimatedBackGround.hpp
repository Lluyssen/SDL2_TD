#pragma once
#include <SDL2/SDL.h>
#include <vector>

class AnimatedBackground
{
private:
    std::vector<SDL_Texture *> frames;
    float timer = 0.f;
    int frame = 0;
    float frameDuration = 0.3f;

public:
    void addFrame(SDL_Texture *tex)
    {
        frames.push_back(tex);
    }

    void update(float dt)
    {
        timer += dt;

        if (timer >= frameDuration)
        {
            timer -= frameDuration;
            frame = (frame + 1) % frames.size();
        }
    }

    void render(SDL_Renderer *renderer, const SDL_Rect &dst)
    {
        if (frames.empty())
            return;

        SDL_RenderCopy(renderer, frames[frame], nullptr, &dst);
    }
};