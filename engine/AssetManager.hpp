#pragma once

#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class AssetManager
{

private:
    SDL_Renderer *renderer;
    std::unordered_map<std::string, SDL_Texture *> textures;

public:
    explicit AssetManager(SDL_Renderer *r) : renderer(r) {}

    ~AssetManager()
    {
        for (auto &[_, tex] : textures)
            SDL_DestroyTexture(tex);
    }

    SDL_Texture *loadTexture(const std::string &id, const std::string &path)
    {
        if (textures.count(id))
            return textures[id];

        SDL_Surface *surf = IMG_Load(path.c_str());
        if (!surf)
        {
            SDL_Log("IMG_Load failed: %s", IMG_GetError());
            return nullptr;
        }

        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);

        textures[id] = tex;
        return tex;
    }

    SDL_Texture *getTexture(const std::string &id) const
    {
        auto it = textures.find(id);
        return it != textures.end() ? it->second : nullptr;
    }
};