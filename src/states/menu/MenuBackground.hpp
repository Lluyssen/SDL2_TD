#pragma once

#include "raylib.h"
#include <ui/AnimatedSprite.hpp>

// Gère le fond animé du menu avec un effet de parallaxe basé sur la souris.
class MenuBackground
{
private:
    AnimatedSprite _background;

    Vector2 _offset{0, 0};

    bool _ready = false;

public:
    // Charge une frame du background (utilisé pendant le loading)
    void loadFrame(GameContext &ctx, const std::string &basePath, int index)
    {
        _background.loadFrame(ctx, basePath, index);
    }

    // Appelé une fois toutes les frames chargées
    void finalize()
    {
        _background.finalize(0.1f);
        _ready = true;
    }

    // Mise à jour du background
    void update(float dt)
    {
        if (!_ready)
            return;

        _background.update(dt);

        Vector2 mouse = GetMousePosition();

        float nx = (mouse.x / GetScreenWidth()) - 0.5f;
        float ny = (mouse.y / GetScreenHeight()) - 0.5f;

        float strength = 30.0f;

        _offset.x += (nx * strength - _offset.x) * 5.0f * dt;
        _offset.y += (ny * strength - _offset.y) * 5.0f * dt;
    }

    // Dessine le background
    void draw(int w, int h)
    {
        if (!_ready)
            return;

        _background.drawFullscreen(w, h, _offset.x, _offset.y);
    }
};