#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "raylib.h"
#include "MusicManager.hpp"

/*
    Contexte global partagé par tous les states.
    Permet d'accéder aux ressources du jeu.
*/

class GameContext
{
private:
    int _highestUnlockedLevel = 0;
    int _selectedLevel = -1;
    int _screenWidth = 1280;
    int _screenHeight = 720;

    // Cache de textures
    std::unordered_map<std::string, Texture2D> _textures;

    MusicManager _music;
    bool _musicLoaded = false;

public:
    GameContext(void) = default;

    ~GameContext()
    {
        unloadAllTextures();
    }

    // -------- Screen --------

    int getWidth(void) const { return _screenWidth; }
    int getHeight(void) const { return _screenHeight; }
    void setSelectedLevel(int id) { _selectedLevel = id; }
    int getSelectedLevel(void) const { return _selectedLevel; }

    void setResolution(int w, int h)
    {
        _screenWidth = w;
        _screenHeight = h;
    }

    // -------- Progression --------

    int getHighestUnlockedLevel(void) const
    {
        return _highestUnlockedLevel;
    }

    void setHighestUnlockedLevel(int level)
    {
        _highestUnlockedLevel = level;
    }

    // -------- Texture Management --------

    Texture2D &loadTexture(const std::string &path)
    {
        if (!IsWindowReady())
        {
            throw std::runtime_error(
                "Attempted to LoadTexture before InitWindow()");
        }

        auto it = _textures.find(path);

        if (it != _textures.end())
            return it->second;

        Texture2D tex = LoadTexture(path.c_str());
        auto [iter, inserted] = _textures.emplace(path, tex);

        return iter->second;
    }

    Texture2D &getTexture(const std::string &path)
    {
        return _textures.at(path);
    }

    void unloadAllTextures()
    {
        for (auto &[path, tex] : _textures)
            UnloadTexture(tex);

        _textures.clear();
    }

    // -------- Music --------

    void initMusic(const std::string &path)
    {
        if (!_musicLoaded)
        {
            _music.load(path);
            _musicLoaded = true;
        }
    }

    void updateMusic()
    {
        if (_musicLoaded)
            _music.update();
    }
};