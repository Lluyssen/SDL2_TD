#pragma once

#include "raylib.h"
#include "../../ui/AnimatedSprite.hpp"
#include <math.h>

class GameContext;

class LevelNode
{
private:
    int _id;
    Vector2 _position;

    bool _completed = false;
    bool _hover = false;
    float _pulseTimer = 0.f;
    AnimatedSprite _idleAnim;
    AnimatedSprite _completedAnim;

public:
    LevelNode(int id, Vector2 pos, bool completed = false)
        : _id(id), _position(pos), _completed(completed)
    {
    }

    int id() const
    {
        return _id;
    }

    Vector2 position() const
    {
        return _position;
    }

    void init(GameContext &ctx)
    {
        _idleAnim.loadSheet(ctx, "../assets/ui/flag/Redsprite.png", 6, 0.18f);
        _completedAnim.loadSheet(ctx, "../assets/ui/flag/Greensprite.png", 6, 0.12f);
    }

    void setCompleted(bool v)
    {
        _completed = v;
    }

    bool update(float dt, Vector2 mouse, Vector2 pos, bool unlocked)
    {
        float radius = 30;
        _pulseTimer += dt;
        _hover = CheckCollisionPointCircle(mouse, pos, radius);

        if (unlocked)
        {
            if (_completed)
                _completedAnim.update(dt);
            else
                _idleAnim.update(dt);
        }

        if (!_hover)
            return false;

        if (!unlocked)
            return false;

        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    void draw(Vector2 pos, AnimatedSprite &lockAnim, bool unlocked)
    {
        float scale = 2;

        if (!unlocked)
        {
            lockAnim.draw(pos, scale);
            return;
        }
        if (unlocked && !_completed)
            scale += sinf(_pulseTimer * 3.0f) * 0.08f;

        if (_completed)
            _completedAnim.draw(pos, scale);
        else
            _idleAnim.draw(pos, scale);

        DrawText(
            TextFormat("%d", _id + 1),
            pos.x - 6,
            pos.y - 10,
            20,
            WHITE);
    }
};