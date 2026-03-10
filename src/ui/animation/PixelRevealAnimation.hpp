#pragma once

#include "UIAnimation.hpp"
#include <ui/UIButton.hpp>
#include <math.h>
#include "raymath.h"

// Animation d’entrée de bouton avec effet de pixels révélés progressivement.
class PixelRevealAnimation : public UIAnimation
{
private:
    // Timer interne pour gérer la progression de l’animation.
    float _timer = 0.0f;

    // Durée totale de l’animation en secondes.
    float _duration = 1.5f;

    // Taille d’un bloc de pixels à dessiner.
    int _blockSize = 12;

public:
    // Réinitialise l’animation pour recommencer.
    void reset() override
    {
        _timer = 0.0f;
    }

    // Met à jour le timer de l’animation.
    void update(UIButton &, float dt) override
    {
        _timer += dt;
    }

    // Vérifie si un point est à l’intérieur d’un rectangle arrondi.
    bool insideRounded(Rectangle r, float x, float y, float radius)
    {
        float left = r.x + radius;
        float right = r.x + r.width - radius;
        float top = r.y + radius;
        float bottom = r.y + r.height - radius;

        if (x >= left && x <= right)
            return true;

        if (y >= top && y <= bottom)
            return true;

        float dx = (x < left) ? left - x : x - right;
        float dy = (y < top) ? top - y : y - bottom;

        return (dx * dx + dy * dy) <= radius * radius;
    }

    // Dessine l’animation pixelisée du bouton avec texte centré.
    void draw(UIButton &btn) override
    {
        Rectangle r = btn.rect();

        float progress = _timer / _duration;

        int cols = r.width / _blockSize;
        int rows = r.height / _blockSize;

        Vector2 center =
            {
                r.x + r.width * 0.5f,
                r.y + r.height * 0.5f};

        float maxDist =
            sqrtf((r.width * 0.5f) * (r.width * 0.5f) +
                  (r.height * 0.5f) * (r.height * 0.5f));

        float radius = r.height * 0.3f;

        // Parcourt chaque bloc et dessine ceux révélés selon la progression.
        for (int y = 0; y < rows; y++)
        {
            for (int x = 0; x < cols; x++)
            {
                float px = r.x + x * _blockSize;
                float py = r.y + y * _blockSize;

                float cx = px + _blockSize * 0.5f;
                float cy = py + _blockSize * 0.5f;

                float dx = cx - center.x;
                float dy = cy - center.y;

                float dist = (dx * dx + dy * dy) / (maxDist * maxDist);

                float noise = fmodf(sinf(x * 12.9898f + y * 78.233f) * 43758.5453f, 1.0f);

                if (dist + noise * 0.25f <= progress && insideRounded(r, cx, cy, radius))
                    DrawRectangle(px, py, _blockSize, _blockSize, Color{0, 0, 0, 120});
            }
        }

        // Dessine le texte du bouton centré et coloré selon hover.
        int fs = 28;
        int tw = MeasureText(btn.text().c_str(), fs);

        DrawText(btn.text().c_str(), r.x + (r.width - tw) / 2, r.y + (r.height - fs) / 2, fs, btn.hover() ? YELLOW : WHITE);

        // Marque le bouton comme terminé lorsque l’animation est finie.
        if (progress >= 1.0f)
            btn.finishEnter();
    }
};