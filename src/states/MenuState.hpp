#pragma once

#include "../utils/StateManager.hpp"
#include "raylib.h"
#include "GameState.hpp"
#include "../ui/AnimatedSprite.hpp"
#include "../ui/UIButton.hpp"
#include "../ui/animation/ScaleHoverAnimation.hpp"
#include "../ui/animation/PixelRevealAnimation.hpp"
#include <vector>
#include <string>
#include <iostream>
#include "../ui/Starfield.hpp"

class GameState;

class MenuState : public IGameState
{
private:
    enum class Phase
    {
        Loading,
        BackgroundIntro,
        UIReveal,
        Idle
    };

    Phase _phase = Phase::Loading;

    AnimatedSprite _background;

    std::vector<UIButton> _buttons;

    float _timer = 0.0f;

    Vector2 _bgOffset{0, 0};
    Starfield _stars;
    Music _music;

    // personnage
    float _idleTimer = 0.0f;
    bool _npcVisible = false;
    Texture2D _npc;
    std::string _npcText = "La chine c'etait mieux";

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        _music = LoadMusicStream("../assets/audio/menu_music.mp3");
        SetMusicVolume(_music, 0.5f); // volume 0 → 1
        PlayMusicStream(_music);

        _phase = Phase::Loading;
        _timer = 0.0f;

        // background animé
        _background.load(ctx, "../assets/ui/bg/frame", 41, 0.1f);

        _npc = LoadTexture("../assets/ui/guigui.png");

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int bw = 320;
        int bh = 70;

        int startY = h / 2;

        _buttons.clear();

        _buttons.emplace_back(
            "Game",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY), (float)bw, (float)bh});

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
        _stars.init(ctx.getWidth(), ctx.getHeight());
    }

    void update(StateManager &sm, float dt) override
    {
        // éviter les dt trop grands
        dt = std::min(dt, 0.05f);

        _background.update(dt);
        UpdateMusicStream(_music);

        // block calcul curseur qui fait bouger le fond
        Vector2 mouse = GetMousePosition();
        float nx = (mouse.x / GetScreenWidth()) - 0.5f;
        float ny = (mouse.y / GetScreenHeight()) - 0.5f;
        float strength = 30.0f;
        _bgOffset.x += (nx * strength - _bgOffset.x) * 5.0f * dt;
        _bgOffset.y += (ny * strength - _bgOffset.y) * 5.0f * dt;

        switch (_phase)
        {

        case Phase::Loading:
        {
            _timer += dt;
            // laisser le temps au background de démarrer
            if (_timer > 0.2f)
            {
                _timer = 0.0f;
                _phase = Phase::BackgroundIntro;
            }
            break;
        }

        case Phase::BackgroundIntro:
        {
            for (auto &b : _buttons)
                b.resetAnimations();

            _phase = Phase::UIReveal;
            break;
        }

        case Phase::UIReveal:
        {
            bool finished = true;

            Vector2 mouse = GetMousePosition();

            bool someoneHover = false;

            for (auto &b : _buttons)
            {
                if (CheckCollisionPointRec(mouse, b.baseRect()))
                {
                    someoneHover = true;
                    break;
                }
            }

            for (auto &b : _buttons)
            {
                b.update(dt, someoneHover);

                if (!b.enterFinished())
                    finished = false;
            }

            if (finished)
                _phase = Phase::Idle;

            break;
        }

        case Phase::Idle:
        {
            Vector2 mouse = GetMousePosition();

            bool someoneHover = false;

            for (auto &b : _buttons)
            {
                if (CheckCollisionPointRec(mouse, b.baseRect()))
                {
                    someoneHover = true;
                    break;
                }
            }

            for (unsigned int i = 0; i < _buttons.size(); i++)
            {
                if (_buttons[i].update(dt, someoneHover))
                    activate(sm, i);
            }

            break;
        }
        }

        auto &ctx = sm.getContext();
        _stars.update(dt, ctx.getWidth(), ctx.getHeight());

        // personnage
        _idleTimer += dt;
        if (!_npcVisible && _idleTimer > 15.0f)
            _npcVisible = true;

        if (IsKeyPressed(KEY_ESCAPE))
            CloseWindow();
    }

    void activate(StateManager &sm, int id)
    {
        switch (id)
        {
        case 0:
            sm.changeState<GameState>();
            break;

        case 1:
            // futur SettingsState
            break;

        case 2:
            CloseWindow();
            break;
        }
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        _background.drawFullscreen(
            w,
            h,
            _bgOffset.x,
            _bgOffset.y);
        _stars.draw(w, h);

        const char *title = "NERVOUS GAME";

        int ts = 80;
        int tw = MeasureText(title, ts);
        int x = (w - tw) / 2;
        int y = h / 4;

        // glow
        for (int i = 8; i > 0; i--)
        {
            DrawText(
                title,
                x,
                y,
                ts,
                Color{255, 255, 255, (unsigned char)(10 * i)});
        }

        // ombre
        DrawText(title, x + 6, y + 6, ts, Color{0, 0, 0, 150});

        // texte principal
        DrawText(title, x, y, ts, Color{255, 220, 120, 255});

        if (_phase == Phase::UIReveal || _phase == Phase::Idle)
        {
            for (auto &b : _buttons)
                b.draw();
        }

        //personnage
        if (_npcVisible)
        {
            int px = 80;
            int py = h - _npc.height - 20;

            DrawTexture(_npc, px, py, WHITE);

            // bulle
            Rectangle bubble{
                (float)px + 80,
                (float)py - 80,
                300,
                70};

            DrawRectangleRounded(bubble, 0.4f, 8, Color{255, 255, 255, 230});
            DrawRectangleRoundedLines(bubble, 0.4f, 8, 2, BLACK);

            DrawText(_npcText.c_str(), bubble.x + 10, bubble.y + 20, 20, BLACK);
        }
    }

    void onExit(StateManager &sm) override
    {
        StopMusicStream(_music);
        UnloadMusicStream(_music);
    }
};