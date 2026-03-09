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
    std::vector<UIButton> _buttons;
    const char *title = "PAUSED";
    int fs = 60;

public:
    bool allowRenderBelow(void) const override
    {
        return true; // on voit le jeu derrière
    }

    bool allowUpdateBelow(void) const override
    {
        return false;
    }

    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int bw = 260;
        int bh = 60;

        int startY = h / 2 - 80;

        _buttons.clear();

        _buttons.emplace_back(
            "Resume",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY), (float)bw, (float)bh});

        _buttons.emplace_back(
            "Restart",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY + 80), (float)bw, (float)bh});

        _buttons.emplace_back(
            "Quit to Menu",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY + 160), (float)bw, (float)bh});

        for (auto &b : _buttons)
        {
            b.setEnterAnimation(std::make_unique<PixelRevealAnimation>());
            b.setHoverAnimation(std::make_unique<ScaleHoverAnimation>());
        }
    }

    void update(StateManager &sm, float dt) override
    {
        Vector2 mouse = GetMousePosition();

        for (size_t i = 0; i < _buttons.size(); ++i)
        {
            if (_buttons[i].update(dt, false))
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

        // overlay sombre
        DrawRectangle(0, 0, w, h, Color{0, 0, 0, 150});
        int tw = MeasureText(title, fs);

        DrawText(title, w / 2 - tw / 2, h / 2 - 180, fs, Color{255, 220, 120, 255});

        for (auto &b : _buttons)
            b.draw();
    }

    void activate(StateManager &sm, int id)
    {
        switch (id)
        {
        case 0: // Resume
            sm.popState();
            break;

        case 1: // Restart
            sm.changeState<GameState>();
            break;

        case 2: // Quit menu
            sm.changeState<MenuState>();
            break;
        }
    }
};