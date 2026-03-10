#pragma once

#include "raylib.h"
#include <ui/UIButton.hpp>
#include <vector>
#include <utils/GameContext.hpp>
#include <ui/animation/ScaleHoverAnimation.hpp>
#include <ui/animation/PixelRevealAnimation.hpp>
#include <ui/PixelButton.hpp>

// Gère la création, l’animation et l’interaction des boutons du menu principal.
class MenuButtons
{
private:
    // Liste des boutons du menu (polymorphisme possible : PixelButton, etc.)
    std::vector<std::unique_ptr<UIButton>> _buttons;
    int spacing = 150;

public:
    // Initialise les boutons et leurs animations
    void init(GameContext &ctx)
    {
        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int bw = 320;
        int bh = 70;
        int startY = h / 2;

        _buttons.clear();
        _buttons.reserve(3);

        _buttons.push_back(
            std::make_unique<PixelButton>(
                "Game",
                Rectangle{(float)(w / 2 - bw / 2), (float)startY, (float)bw, (float)bh}));

        _buttons.push_back(
            std::make_unique<PixelButton>(
                "Settings",
                Rectangle{(float)(w / 2 - bw / 2), (float)(startY + spacing), (float)bw, (float)bh}));

        _buttons.push_back(
            std::make_unique<PixelButton>(
                "Quit",
                Rectangle{(float)(w / 2 - bw / 2), (float)(startY + spacing * 2), (float)bw, (float)bh}));

        for (auto &b : _buttons)
        {
            b->setEnterAnimation(std::make_unique<PixelRevealAnimation>());
            b->setHoverAnimation(std::make_unique<ScaleHoverAnimation>());
        }
    }

    // Reset toutes les animations d'entrée
    void resetAnimations()
    {
        for (auto &b : _buttons)
            b->resetAnimations();
    }

    // Vérifie si toutes les animations d'entrée sont terminées
    bool enterFinished()
    {
        for (auto &b : _buttons)
            if (!b->enterFinished())
                return false;

        return true;
    }

    // Mise à jour des boutons
    // Retourne l'index du bouton cliqué ou -1
    int update(float dt)
    {
        Vector2 mouse = GetMousePosition();

        int hovered = -1;

        // Détecte quel bouton est survolé
        for (size_t i = 0; i < _buttons.size(); ++i)
        {
            if (CheckCollisionPointRec(mouse, _buttons[i]->baseRect()))
            {
                hovered = (int)i;
                break;
            }
        }

        // Met à jour les boutons
        for (size_t i = 0; i < _buttons.size(); ++i)
        {
            bool someoneHover = (hovered != -1 && hovered != (int)i);

            if (_buttons[i]->update(dt, someoneHover))
                return (int)i;
        }

        return -1;
    }

    // Dessin des boutons
    void draw(void)
    {
        for (auto &b : _buttons)
            b->draw();
    }

    // Accès direct (optionnel)
    UIButton &operator[](size_t i)
    {
        return *_buttons[i];
    }

    size_t size() const
    {
        return _buttons.size();
    }
};