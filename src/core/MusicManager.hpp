#pragma once

#include "raylib.h"
#include <string>

// Gère le chargement, la lecture et la libération de la musique du menu.
class MusicManager
{
private:

    Music _music;

public:
    // Charge la musique, règle le volume et démarre la lecture.
    void load(const std::string& path)
    {
        _music = LoadMusicStream(path.c_str());
        SetMusicVolume(_music, 0.5f);
        PlayMusicStream(_music);
    }

    // Met à jour le stream audio (doit être appelé chaque frame).
    void update(void)
    {
        UpdateMusicStream(_music);
    }

    // Stoppe la lecture et libère les ressources audio.
    void unload(void)
    {
        StopMusicStream(_music);
        UnloadMusicStream(_music);
    }
};