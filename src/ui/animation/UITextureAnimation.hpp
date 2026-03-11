#pragma once

#include "raylib.h"

class UITextureAnimation
{
public:

    virtual ~UITextureAnimation(void) = default;

    virtual void reset(void) {}

    virtual void update(float) {}

    virtual void draw(Texture2D&, Rectangle, Rectangle) = 0;

    virtual bool finished(void) const { return true; }
};