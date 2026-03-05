#pragma once

#include "raylib.h"
#include <string>
#include <memory>
#include "animation/UIAnimation.hpp"
#include <math.h>

// Représente un bouton interactif avec animations d’entrée et de survol.
class UIButton
{
private:
    // Rectangle de base et rectangle actuel utilisé pour le dessin.
    Rectangle _baseRect;
    Rectangle _drawRect;

    // Texte affiché sur le bouton.
    std::string _text;

    // État interne pour hover, entrée, scale et profondeur.
    bool _hover = false;
    bool _entered = false;
    float _scale = 1.0f;
    float _targetScale = 1.0f;
    float _depth = 0.0f;
    float _targetDepth = 0.0f;
    float _breathTime = 0.0f;

    // Animations optionnelles pour l’entrée et le survol.
    std::unique_ptr<UIAnimation> _enterAnim;
    std::unique_ptr<UIAnimation> _hoverAnim;

public:
    // Constructeur avec texte et rectangle.
    UIButton(const std::string &text, Rectangle rect)
        : _baseRect(rect), _drawRect(rect), _text(text)
    {
    }

    // Assigne l’animation d’entrée.
    void setEnterAnimation(std::unique_ptr<UIAnimation> anim)
    {
        _enterAnim = std::move(anim);
    }

    // Assigne l’animation de survol.
    void setHoverAnimation(std::unique_ptr<UIAnimation> anim)
    {
        _hoverAnim = std::move(anim);
    }

    // Réinitialise l’animation d’entrée.
    void resetAnimations(void)    {
        _entered = false;

        if (_enterAnim)
            _enterAnim->reset();
    }

    // Vérifie si l’animation d’entrée est terminée.
    bool enterFinished() const
    {
        return _entered;
    }

    // Marque l’animation d’entrée comme terminée.
    bool finishEnter(void)    {
        _entered = true;
        return true;
    }

    // Met à jour l’état du bouton, la scale, la profondeur et détecte le clic.
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
            // Respiration du bouton pour effet subtil.
            float breath = sinf(_breathTime * 2.0f) * 0.02f;

            _targetScale = 1.0f + breath;
            _targetDepth = 0.0f;
        }
        _depth += (_targetDepth - _depth) * 10.0f * dt;

        // Interpolation douce pour scale.
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

    // Dessine le bouton en utilisant l’animation ou le rendu par défaut.
    void draw(void)    {
        if (!_entered && _enterAnim)
            _enterAnim->draw(*this);
        else
            drawDefault();
    }

    // Dessin par défaut du bouton avec bordure, ombre et texte centré.
    void drawDefault(void)    {
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

    // Retourne le rectangle de dessin actuel.
    Rectangle &rect(void)    {
        return _drawRect;
    }

    // Retourne le rectangle de base.
    const Rectangle &baseRect() const
    {
        return _baseRect;
    }

    // Retourne le texte du bouton.
    const std::string &text() const
    {
        return _text;
    }

    // Indique si la souris survole le bouton.
    bool hover() const
    {
        return _hover;
    }
};