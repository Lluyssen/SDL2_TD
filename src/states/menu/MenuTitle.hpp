#pragma once

#include "raylib.h"

// Affiche le titre du menu avec un effet de glow et d’ombre.
class MenuTitle
{
public:
    // Dessine le titre centré avec superposition de couleurs et transparence.
    void draw(int w, int h)
    {
        const char *title = "NERVOUS GAME";
        int ts = 80; // taille du texte
        int tw = MeasureText(title, ts);
        int x = (w - tw) / 2; // centrage horizontal
        int y = h / 4;        // position verticale

        // Effet de glow en superposant plusieurs textes semi-transparents
        for (int i = 8; i > 0; i--)
        {
            DrawText(
                title,
                x,
                y,
                ts,
                Color{255, 255, 255, (unsigned char)(10 * i)});
        }

        // Ombre du titre
        DrawText(title, x + 6, y + 6, ts, Color{0, 0, 0, 150});

        // Texte final coloré
        DrawText(title, x, y, ts, Color{255, 220, 120, 255});
    }
};