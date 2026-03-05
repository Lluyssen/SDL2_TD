#pragma once

#include "raylib.h"
#include <string>

// Gère l'affichage d'un PNJ avec texte après un certain temps d'inactivité.
class MenuNPC
{
private:
    // Texture représentant le PNJ.
    Texture2D _npc;

    // Compteur de temps écoulé pour déclencher l'apparition.
    float _idleTimer = 0.0f;

    // Indique si le PNJ est visible à l'écran.
    bool _visible = false;

    // Texte affiché dans la bulle du PNJ.
    std::string _text = "La Chine c'était mieux";

public:
    // Charge la texture du PNJ.
    void init(void)
    {
        _npc = LoadTexture("../assets/ui/guigui.png");
    }

    // Met à jour le timer et rend le PNJ visible après 15 secondes.
    void update(float dt)
    {
        _idleTimer += dt;

        if (!_visible && _idleTimer > 15.0f)
            _visible = true;
    }

    // Dessine le PNJ et sa bulle de texte si visible.
    void draw(int h)
    {
        if (!_visible)
            return;

        int px = 80;
        int py = h - _npc.height - 20;

        DrawTexture(_npc, px, py, WHITE);

        Rectangle bubble{
            (float)px + 80,
            (float)py - 80,
            300,
            70};

        DrawRectangleRounded(bubble, 0.4f, 8, Color{255, 255, 255, 230});
        DrawRectangleRoundedLines(bubble, 0.4f, 8, 2, BLACK);
        DrawText(_text.c_str(), bubble.x + 10, bubble.y + 20, 20, BLACK);
    }
};