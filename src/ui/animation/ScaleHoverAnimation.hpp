#pragma once

#include "UIAnimation.hpp"
#include <ui/UIButton.hpp>

// Animation qui agrandit légèrement un bouton lorsque la souris le survole.
class ScaleHoverAnimation : public UIAnimation
{
public:
    // Met à jour la taille du rectangle du bouton selon l’état hover.
    void update(UIButton& btn, float) override
    {
        Rectangle& r = btn.rect();

        float scale = btn.hover() ? 1.05f : 1.0f;

        // Centre du rectangle original pour conserver l’alignement
        float cx = btn.baseRect().x + btn.baseRect().width/2;
        float cy = btn.baseRect().y + btn.baseRect().height/2;

        r.width  = btn.baseRect().width  * scale;
        r.height = btn.baseRect().height * scale;

        // Repositionne le rectangle pour rester centré
        r.x = cx - r.width/2;
        r.y = cy - r.height/2;
    }
};