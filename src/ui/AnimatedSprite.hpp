#pragma once

#include "raylib.h"
#include <vector>
#include <string>

class GameContext;

// Gère un sprite animé constitué d’une séquence de textures pour un rendu fluide.
class AnimatedSprite
{
private:
    // Liste des textures représentant chaque frame de l’animation.
    std::vector<Texture2D *> _frames;

    // Index de la frame actuelle.
    int _currentFrame = 0;

    // Timer interne pour contrôler la vitesse d’animation.
    float _timer = 0.f;

    // Durée d’affichage d’une frame.
    float _frameTime = 0.05f;

public:
    AnimatedSprite() = default;

    // Charge les textures à partir d’un chemin de base et nombre de frames.
    void load(GameContext &ctx, const std::string &basePath, int frameCount, float frameTime = 0.05f)
    {
        _frames.clear();
        _frameTime = frameTime;

        for (int i = 0; i < frameCount; i++)
        {
            std::string path = basePath + std::to_string(i) + ".png";
            Texture2D &tex = ctx.loadTexture(path);
            _frames.push_back(&tex);
        }
    }

    // Met à jour l’animation en passant à la frame suivante si nécessaire.
    void update(float dt)
    {
        _timer += dt;
        if (_timer >= _frameTime)
        {
            _currentFrame++;
            if (_currentFrame >= (int)_frames.size())
                _currentFrame = 0;
            _timer = 0.f;
        }
    }

    // Dessine la frame actuelle en plein écran sans décalage.
    void drawFullscreen(int width, int height)
    {
        if (_frames.empty())
            return;

        Texture2D &tex = *_frames[_currentFrame];
        DrawTexturePro(
            tex,
            {0, 0, (float)tex.width, (float)tex.height},
            {0, 0, (float)width, (float)height},
            {0, 0},
            0,
            WHITE);
    }

    // Dessine la frame actuelle en plein écran avec un décalage pour effet de parallaxe.
    void drawFullscreen(int w, int h, float offsetX, float offsetY)
    {
        Texture2D &tex = *_frames[_currentFrame];

        float zoom = 1.1f; // 10% plus grand que l'écran

        float drawW = w * zoom;
        float drawH = h * zoom;

        Rectangle src{0, 0, (float)tex.width, (float)tex.height};

        Rectangle dst{
            (w - drawW) * 0.5f + offsetX,
            (h - drawH) * 0.5f + offsetY,
            drawW,
            drawH};

        DrawTexturePro(tex, src, dst, {0, 0}, 0.0f, WHITE);
    }
};