#pragma once
#include "raylib.h"

class UIElement
{
protected:
    Vector2 _pos{0, 0};
    bool _visible = true;

public:
    virtual ~UIElement() = default;

    virtual void update(float dt) {}
    virtual void draw() {}

    void setPosition(Vector2 p)
    {
        _pos = p;
    }

    void setVisible(bool v)
    {
        _visible = v;
    }

    bool visible() const
    {
        return _visible;
    }
};