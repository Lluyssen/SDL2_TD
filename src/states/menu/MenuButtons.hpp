#pragma once

#include "raylib.h"
#include "../../ui/UIButton.hpp"
#include <vector>
#include "../../utils/GameContext.hpp"
#include "../../ui/animation/ScaleHoverAnimation.hpp"
#include "../../ui/animation/PixelRevealAnimation.hpp"

// Gère la création, l’animation et l’interaction des boutons du menu principal.
class MenuButtons
{
private:
    // Liste des boutons affichés dans le menu.
    std::vector<UIButton> _buttons;

public:
    // Initialise les boutons, leurs positions et leurs animations.
    void init(GameContext &ctx)
    {
        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int bw = 320;
        int bh = 70;
        int startY = h / 2;

        _buttons.clear();

        _buttons.emplace_back(
            "Game",
            Rectangle{(float)(w / 2 - bw / 2), (float)startY, (float)bw, (float)bh});

        _buttons.emplace_back(
            "Settings",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY + 90), (float)bw, (float)bh});

        _buttons.emplace_back(
            "Quit",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY + 180), (float)bw, (float)bh});

        for (auto &b : _buttons)
        {
            b.setEnterAnimation(std::make_unique<PixelRevealAnimation>());
            b.setHoverAnimation(std::make_unique<ScaleHoverAnimation>());
        }
    }

    // Réinitialise toutes les animations des boutons.
    void resetAnimations(void)
    {
        for (auto &b : _buttons)
            b.resetAnimations();
    }

    // Vérifie si toutes les animations d’entrée sont terminées.
    bool enterFinished(void)
    {
        for (auto &b : _buttons)
            if (!b.enterFinished())
                return false;
        return true;
    }

    // Met à jour les boutons et retourne l’index du bouton activé ou -1.
    int update(float dt)
    {
        Vector2 mouse = GetMousePosition();

        bool hover = false;

        for (auto &b : _buttons)
        {
            if (CheckCollisionPointRec(mouse, b.baseRect()))
            {
                hover = true;
                break;
            }
        }

        for (size_t i = 0; i < _buttons.size(); i++)
        {
            if (_buttons[i].update(dt, hover))
                return i;
        }
        return -1;
    }

    // Dessine tous les boutons à l’écran.
    void draw(void)
    {
        for (auto &b : _buttons)
            b.draw();
    }
};