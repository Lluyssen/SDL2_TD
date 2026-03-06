#pragma once

#include "../utils/StateManager.hpp"
#include "raylib.h"
#include "game/LevelState.hpp"
#include "game/LevelNode.hpp"
#include "../ui/petitMenu/PetitMenu.hpp"

class GameState : public IGameState
{
private:
    Texture2D _mapTexture{};
    AnimatedSprite _lockAnim;

    std::vector<LevelNode> _levels;

    Vector2 _bgOffset{0, 0};

    PetitMenu _tooltip;
    Texture2D _tooltipTexture;

    float _zoom = 1.05f;

    // conversion map -> écran
    Vector2 worldToScreen(Vector2 world, int w, int h, float drawW, float drawH)
    {
        float scaleX = drawW / (float)_mapTexture.width;
        float scaleY = drawH / (float)_mapTexture.height;

        return {
            world.x * scaleX + (w - drawW) * 0.5f + _bgOffset.x,
            world.y * scaleY + (h - drawH) * 0.5f + _bgOffset.y};
    }

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        _mapTexture = LoadTexture("../assets/ui/map.png");

        _lockAnim.loadSheet(ctx, "../assets/ui/flag/Graysprite.png", 6, 0.2f);

        _tooltipTexture = LoadTexture("../assets/ui/papyrus.png");
        _tooltip.init(_tooltipTexture);

        _levels =
            {
                {0, {160, 120}},
                {1, {700, 650}},
                {2, {1100, 250}},
                {3, {1020, 740}},
                {4, {300, 700}}};

        for (auto &n : _levels)
            n.init(ctx);
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();

        Vector2 mouse = GetMousePosition();

        int unlocked = ctx.gethighestUnlockedLevel();

        ctx.updateMusic();

        int w = ctx.getWidth();
        int h = ctx.getHeight();
        float drawW = w * _zoom;
        float drawH = h * _zoom;

        bool tooltipVisible = false;

        for (auto &node : _levels)
        {
            bool unlockedNode = node.id() <= unlocked;

            Vector2 pos = worldToScreen(node.position(), w, h, drawW, drawH);

            if (node.update(dt, mouse, pos, unlockedNode))
                sm.changeState<LevelState>(node.id());

            if (!tooltipVisible && CheckCollisionPointCircle(mouse, pos, 30))
            {
                std::vector<std::string> lines =
                    {
                        TextFormat("Level %d", node.id() + 1),
                        "Difficulty: Easy",
                        "Reward: 200 gold"};

                _tooltip.show({pos.x, pos.y - 40}, lines);

                tooltipVisible = true;
            }
        }

        if (!tooltipVisible)
            _tooltip.hide();

        // parallaxe souris
        float nx = (mouse.x / w) - 0.5f;
        float ny = (mouse.y / h) - 0.5f;

        _bgOffset.x += (nx * 40.f - _bgOffset.x) * 4.f * dt;
        _bgOffset.y += (ny * 40.f - _bgOffset.y) * 4.f * dt;

        _lockAnim.update(dt);
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        float zoom = 1.05f;

        float drawW = w * zoom;
        float drawH = h * zoom;

        ClearBackground(BLACK);

        Rectangle src{
            0, 0,
            (float)_mapTexture.width,
            (float)_mapTexture.height};

        Rectangle dst{
            (w - drawW) * 0.5f + _bgOffset.x,
            (h - drawH) * 0.5f + _bgOffset.y,
            drawW,
            drawH};

        DrawTexturePro(_mapTexture, src, dst, {0, 0}, 0, WHITE);

        int unlocked = ctx.gethighestUnlockedLevel();

        for (auto &node : _levels)
        {
            bool unlockedNode = node.id() <= unlocked;

            Vector2 pos = worldToScreen(node.position(), w, h, drawW, drawH);

            node.draw(pos, _lockAnim, unlockedNode);
        }

        _tooltip.draw(w, h);
    }

    void onExit(StateManager &) override
    {
        UnloadTexture(_mapTexture);
        UnloadTexture(_tooltipTexture);
    }
};