#pragma once

#include "raylib.h"
#include <string>
#include <unordered_map>
#include "MusicManager.hpp"

/*
    Contexte global partagé par tous les states.
    Permet d'accéder aux ressources du jeu.
*/

class GameContext
{
private:
    int _highestUnlockedLevel = 0;
    int _screenWidth = 1280;
    int _screenHeight = 720;

    // Cache simple de textures
    std::unordered_map<std::string, Texture2D> _textures;
    MusicManager _music;
    bool _musicLoaded = false;

public:
    GameContext(void) = default;

    ~GameContext(void)
    {
        unloadAllTextures();
    }

    int getWidth() const { return _screenWidth; }
    int getHeight() const { return _screenHeight; }
    int gethighestUnlockedLevel() const { return _highestUnlockedLevel; }

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

    void initMusic(const std::string &path)
    {
        if (!_musicLoaded)
        {
            _music.load(path); // "../assets/audio/menu_music.mp3"
            _musicLoaded = true;
        }
    }

    void updateMusic()
    {
        if (_musicLoaded)
            _music.update();
    }
};