#pragma once

#include "../utils/StateManager.hpp"
#include "../ui/UIButton.hpp"
#include "GameState.hpp"
#include "MenuState.hpp"
#include "../ui/animation/ScaleHoverAnimation.hpp"
#include "../ui/animation/PixelRevealAnimation.hpp"

class GameState;
class MenuState;

class PauseMenuState : public IGameState
{
private:
    std::vector<std::unique_ptr<UIButton>> _buttons;
    static constexpr const char *TITLE = "PAUSED";
    static constexpr int FONT_SIZE = 60;
    static constexpr int BUTTON_WIDTH = 260;
    static constexpr int BUTTON_HEIGHT = 60;
    static constexpr int BUTTON_SPACING = 80;

public:
    bool allowRenderBelow() const override { return true; } // On voit le jeu derrière
    bool allowUpdateBelow() const override { return false; }

    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();
        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int startY = h / 2 - BUTTON_SPACING;

        _buttons.clear();
        _buttons.reserve(3);

        // Crée les boutons avec animations PixelReveal et ScaleHover
        std::vector<std::string> labels = {"Resume", "Restart", "Quit to Menu"};
        for (size_t i = 0; i < labels.size(); ++i)
        {
            Rectangle rect{
                (float)(w / 2 - BUTTON_WIDTH / 2),
                (float)(startY + i * BUTTON_SPACING),
                (float)BUTTON_WIDTH,
                (float)BUTTON_HEIGHT};

            _buttons.push_back(std::make_unique<UIButton>(labels[i], rect));
            _buttons.back()->setEnterAnimation(std::make_unique<PixelRevealAnimation>());
            _buttons.back()->setHoverAnimation(std::make_unique<ScaleHoverAnimation>());
        }
    }

    void update(StateManager &sm, float dt) override
    {
        dt = std::min(dt, 0.05f); // Clamp dt pour stabilité

        Vector2 mouse = GetMousePosition();

        for (size_t i = 0; i < _buttons.size(); ++i)
        {
            // update renvoie true si cliqué
            if (_buttons[i]->update(dt, false))
                activate(sm, (int)i);
        }

        if (IsKeyPressed(KEY_ESCAPE))
            sm.popState();
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();
        int w = ctx.getWidth();
        int h = ctx.getHeight();

        // Overlay sombre semi-transparent
        DrawRectangle(0, 0, w, h, Color{0, 0, 0, 150});

        int tw = MeasureText(TITLE, FONT_SIZE);
        DrawText(TITLE, w / 2 - tw / 2, h / 2 - 180, FONT_SIZE, Color{255, 220, 120, 255});

        for (auto &b : _buttons)
            b->draw();
    }

    void activate(StateManager &sm, int id)
    {
        switch (id)
        {
        case 0:
            sm.popState();
            break; // Resume
        case 1:
            sm.changeState<GameState>();
            break; // Restart
        case 2:
            sm.changeState<MenuState>();
            break; // Quit menu
        default:
            break;
        }
    }
};