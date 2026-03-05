#pragma once

#include "UIAnimation.hpp"
#include "../UIButton.hpp"

class ScaleHoverAnimation : public UIAnimation
{
public:

    void update(UIButton& btn, float) override
    {
        Rectangle& r = btn.rect();

        float scale = btn.hover() ? 1.05f : 1.0f;

        float cx = btn.baseRect().x + btn.baseRect().width/2;
        float cy = btn.baseRect().y + btn.baseRect().height/2;

        r.width  = btn.baseRect().width  * scale;
        r.height = btn.baseRect().height * scale;

        r.x = cx - r.width/2;
        r.y = cy - r.height/2;
    }
};