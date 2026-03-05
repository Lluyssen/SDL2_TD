#pragma once

#include "raylib.h"
#include <string>
#include <memory>
#include "animation/UIAnimation.hpp"
#include <math.h>

class UIButton
{
private:
    Rectangle _baseRect;
    Rectangle _drawRect;

    std::string _text;

    bool _hover = false;
    bool _entered = false;
    float _scale = 1.0f;
    float _targetScale = 1.0f;
    float _depth = 0.0f;
    float _targetDepth = 0.0f;
    float _breathTime = 0.0f;

    std::unique_ptr<UIAnimation> _enterAnim;
    std::unique_ptr<UIAnimation> _hoverAnim;

public:
    UIButton(const std::string &text, Rectangle rect)
        : _baseRect(rect), _drawRect(rect), _text(text)
    {
    }

    void setEnterAnimation(std::unique_ptr<UIAnimation> anim)
    {
        _enterAnim = std::move(anim);
    }

    void setHoverAnimation(std::unique_ptr<UIAnimation> anim)
    {
        _hoverAnim = std::move(anim);
    }

    void resetAnimations()
    {
        _entered = false;

        if (_enterAnim)
            _enterAnim->reset();
    }

    bool enterFinished() const
    {
        return _entered;
    }

    bool finishEnter()
    {
        _entered = true;
        return true;
    }

    bool update(float dt, bool someoneHover)
    {
        Vector2 mouse = GetMousePosition();

        _hover = CheckCollisionPointRec(mouse, _baseRect);
        _breathTime += dt;

        if (_hover)
        {
            _targetScale = 1.10f;
            _targetDepth = -8.0f;
        }
        else if (someoneHover)
        {
            _targetScale = 0.92f;
            _targetDepth = 6.0f;
        }
        else
        {
            // respiration
            float breath = sinf(_breathTime * 2.0f) * 0.02f;

            _targetScale = 1.0f + breath;
            _targetDepth = 0.0f;
        }
        _depth += (_targetDepth - _depth) * 10.0f * dt;

        // interpolation douce
        _scale += (_targetScale - _scale) * 10.0f * dt;

        float cx = _baseRect.x + _baseRect.width * 0.5f;
        float cy = _baseRect.y + _baseRect.height * 0.5f;

        _drawRect.width = _baseRect.width * _scale;
        _drawRect.height = _baseRect.height * _scale;

        _drawRect.x = cx - _drawRect.width * 0.5f;
        _drawRect.y = cy - _drawRect.height * 0.5f + _depth;

        if (!_entered && _enterAnim)
            _enterAnim->update(*this, dt);

        if (_hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            return true;

        return false;
    }

    void draw()
    {
        if (!_entered && _enterAnim)
            _enterAnim->draw(*this);
        else
            drawDefault();
    }

    void drawDefault()
    {
        DrawRectangleRounded(
            Rectangle{
                _drawRect.x + 4,
                _drawRect.y + 6,
                _drawRect.width,
                _drawRect.height},
            0.3f,
            8,
            Color{0, 0, 0, 80});
        DrawRectangleRoundedLines(
            _drawRect,
            0.3f,
            8,
            2,
            _hover ? Color{255,220,120,255} : GRAY);

        int fs = 28;
        int tw = MeasureText(_text.c_str(), fs);

        DrawText(
            _text.c_str(),
            _drawRect.x + (_drawRect.width - tw) / 2,
            _drawRect.y + (_drawRect.height - fs) / 2,
            fs,
            _hover ? YELLOW : WHITE);
    }

    Rectangle &rect()
    {
        return _drawRect;
    }

    const Rectangle &baseRect() const
    {
        return _baseRect;
    }

    const std::string &text() const
    {
        return _text;
    }

    bool hover() const
    {
        return _hover;
    }
};