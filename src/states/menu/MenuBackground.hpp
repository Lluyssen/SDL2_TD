#pragma once

#include "raylib.h"
#include "../../ui/AnimatedSprite.hpp"

// Gère le fond animé du menu avec un effet de parallaxe basé sur la souris.
class MenuBackground
{
private:
    // Sprite animé utilisé comme background.
    AnimatedSprite _background;

    // Décalage interpolé appliqué pour créer l'effet de parallaxe.
    Vector2 _offset{0, 0};

public:
    // Initialise et charge la séquence d'animation du fond.
    void init(GameContext &ctx)
    {
        _background.load(ctx, "../assets/ui/bg/frame", 41, 0.1f);
    }

    // Met à jour l'animation et calcule le décalage selon la position de la souris.
    void update(float dt)
    {
        _background.update(dt);

        Vector2 mouse = GetMousePosition();

        float nx = (mouse.x / GetScreenWidth()) - 0.5f;
        float ny = (mouse.y / GetScreenHeight()) - 0.5f;

        float strength = 30.0f;

        _offset.x += (nx * strength - _offset.x) * 5.0f * dt;
        _offset.y += (ny * strength - _offset.y) * 5.0f * dt;
    }

    // Dessine le background en plein écran avec le décalage calculé.
    void draw(int w, int h)
    {
        _background.drawFullscreen(w, h, _offset.x, _offset.y);
    }
};