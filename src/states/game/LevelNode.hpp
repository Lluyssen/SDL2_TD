#pragma once

#include "raylib.h"
#include <ui/AnimatedSprite.hpp>
#include <math.h>

class GameContext;
class LevelNode
{
private:
    int _id;
    Vector2 _normalizedPos;

    bool _hover = false;

    static constexpr float RADIUS = 30.f;

public:
    LevelNode(int id, Vector2 normalizedPos) : _id(id), _normalizedPos(normalizedPos) {}

    int id(void) const
    {
        return _id;
    }

    Vector2 getScreenPos(int screenW, int screenH) const
    {
        return {
            _normalizedPos.x * screenW,
            _normalizedPos.y * screenH};
    }

    bool isHovered(void) const
    {
        return _hover;
    }

    bool update(Vector2 mouse, Vector2 screenPos, bool playable)
    {
        _hover = CheckCollisionPointCircle(mouse, screenPos, RADIUS);
        return _hover && playable && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }
};