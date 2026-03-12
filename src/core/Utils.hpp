#pragma once

#include "raylib.h"
#include <iostream>

namespace utils
{

    // retourne position souris normalisée [0..1]
    inline Vector2 getMouseNormalized(int width, int height)
    {
        Vector2 m = GetMousePosition();
        return {m.x / (float)width, m.y / (float)height};
    }

    // imprime la position normalisée dans la console
    /*
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            utils::printMouseNormalized(w, h);
    */
    inline void printMouseNormalized(int width, int height)
    {
        Vector2 n = getMouseNormalized(width, height);
        std::cout << "{" << n.x << "f, " << n.y << "f}" << std::endl;
    }

    inline void printMouseNormalized(int width, int height, int id)
    {
        Vector2 n = getMouseNormalized(width, height);
        std::cout << "{" << id << ", " << n.x << "f, " << n.y << "f}" << std::endl;
    }

    // imprime un node complet (pratique pour LEVELS)
    inline void printLevelNode(int width, int height, int id)
    {
        Vector2 n = getMouseNormalized(width, height);
        std::cout << "{" << id << ", {" << n.x << "f, " << n.y << "f}},\n";
    }

    // affiche les coordonnées en debug à l'écran
    inline void drawMouseNormalized(int width, int height)
    {
        Vector2 n = getMouseNormalized(width, height);
        DrawText(TextFormat("Mouse: %.3f %.3f", n.x, n.y), 20, 20, 20, WHITE);
    }

    // sert à convertir une position normalisée (0 -> 1) en position écran réelle en pixels
    inline Vector2 normalizedToScreen(Vector2 n, int w, int h)
    {
        return {n.x * w, n.y * h};
    }

    inline Vector2 screenToNormalized(Vector2 p, int w, int h)
    {
        return {p.x / w, p.y / h};
    }

}
