#pragma once

#include "raylib.h"
#include <string>
#include <unordered_map>

/*
    Contexte global partagé par tous les states.
    Permet d'accéder aux ressources du jeu.
*/
class GameContext
{
private:
    int _screenWidth = 1280;
    int _screenHeight = 720;

    // Cache simple de textures
    std::unordered_map<std::string, Texture2D> _textures;

public:
    GameContext() = default;

    ~GameContext(void)
    {
        unloadAllTextures();
    }

    int getWidth() const { return _screenWidth; }
    int getHeight() const { return _screenHeight; }

    void setResolution(int w, int h)
    {
        _screenWidth = w;
        _screenHeight = h;
    }

    // Charge une texture avec cache
    Texture2D &loadTexture(const std::string &path)
    {
        auto it = _textures.find(path);

        if (it != _textures.end())
            return it->second;

        Texture2D tex = LoadTexture(path.c_str());
        _textures[path] = tex;

        return _textures[path];
    }

    // Récupère une texture déjà chargée
    Texture2D &getTexture(const std::string &path)
    {
        return _textures.at(path);
    }

    // Décharge toutes les textures
    void unloadAllTextures(void)
    {
        for (auto &[k, tex] : _textures)
            UnloadTexture(tex);
        _textures.clear();
    }
};