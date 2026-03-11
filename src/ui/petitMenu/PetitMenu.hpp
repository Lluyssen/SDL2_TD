#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <ui/animation/UITextureAnimation.hpp>

// Petit menu UI affichant une bannière avec texte.
// L'animation de la texture est externalisée via UITextureAnimation.
class PetitMenu
{
private:
    Texture2D *_bg = nullptr; // texture de fond de la bannière
    Vector2 _pos{0, 0};       // position cible du menu

    std::vector<std::string> _lines;
    std::vector<int> _lineWidths; // cache des largeurs de texte (évite MeasureText chaque frame)

    Sound *_openSound = nullptr;

    UITextureAnimation *_animation = nullptr; // animation appliquée à la texture

    bool _visible = false;
    bool _soundPlayed = false;

    float _textureScale = 1.3f;
    float _textScale = 1.f;

    // animation de scale lors de l'ouverture du menu
    float _animScale = 0.3f;
    float _targetScale = 1.f;

    // données de layout calculées dans show()
    int _cachedTextWidth = 0;
    int _cachedTextHeight = 0;
    int _cachedFontSize = 0;
    int _cachedPadding = 0;

public:
    void init(Texture2D &background, Sound &openSound)
    {
        _bg = &background;
        _openSound = &openSound;

        _lines.reserve(8);
        _lineWidths.reserve(8);
    }

    // injecte une animation externe (PixelReveal, Fade, etc.)
    void setAnimation(UITextureAnimation *anim)
    {
        _animation = anim;
    }

    void update(float dt)
    {
        if (_animation)
            _animation->update(dt);

        if (_soundPlayed)
        {
            if (_openSound)
                PlaySound(*_openSound);
            _soundPlayed = false;
        }

        if (!_visible)
            return;

        // interpolation vers la taille finale
        float speed = 10.f;
        _animScale += (_targetScale - _animScale) * speed * dt;

        if (std::fabs(_targetScale - _animScale) < 0.001f)
            _animScale = _targetScale;
    }

    void setTextureScale(float f)
    {
        _textureScale = f;
    }

    void setTextScale(float f)
    {
        _textScale = f;
    }

    // affiche le menu et prépare le layout du texte
    void show(Vector2 pos, const std::vector<std::string> &lines, int screenW, int screenH)
    {
        if (!_visible)
        {
            _animScale = 0.6f;
            _soundPlayed = true;

            if (_animation)
                _animation->reset();
        }

        _pos = pos;
        _lines = lines;
        _visible = true;

        float uiScale = std::min(screenW / 1920.f, screenH / 1080.f);

        _cachedFontSize = static_cast<int>(22 * uiScale * _textScale);
        _cachedPadding = static_cast<int>(28 * uiScale * _textScale);

        _cachedTextWidth = 0;
        _lineWidths.clear();

        // calcul du layout texte une seule fois
        for (const auto &line : _lines)
        {
            int w = MeasureText(line.c_str(), _cachedFontSize);
            _lineWidths.push_back(w);
            _cachedTextWidth = std::max(_cachedTextWidth, w);
        }

        _cachedTextHeight = _lines.size() * (_cachedFontSize + 4);
    }

    void hide(void)
    {
        _visible = false;
        _animScale = 0.3f;
        _soundPlayed = false;
    }

    bool isVisible(void) const
    {
        return _visible;
    }

    void setPosition(Vector2 pos)
    {
        _pos = pos;
    }

    void draw(int screenW, int screenH)
    {
        // Si le menu est caché ou si la texture n'est pas valide, on ne dessine rien
        if (!_visible || !_bg)
            return;

        // Calcul d'un facteur de scale basé sur une résolution de référence (1920x1080)
        // Permet d'adapter l'UI à toutes les résolutions
        float uiScale = std::min(screenW / 1920.f, screenH / 1080.f);

        // Largeur de base de la texture avec scaling UI
        float baseWidth = _bg->width * uiScale * _textureScale;

        // Largeur minimale nécessaire pour contenir le texte avec padding
        float textWidthNeeded = _cachedTextWidth + _cachedPadding * 2;

        // Largeur finale du menu : on prend la plus grande valeur
        // entre la texture et le texte
        // _animScale sert à l'animation d'apparition
        float w = std::max(baseWidth * 1.8f, textWidthNeeded) * _animScale;

        // Hauteur du menu
        float h = _bg->height * uiScale * _textureScale;

        // Position centrale du menu
        float cx = _pos.x;

        // Décalage vertical pour afficher la bannière au-dessus du point ciblé
        float cy = _pos.y - h * 0.6f;

        float halfW = w * 0.5f;
        float halfH = h * 0.5f;

        // Clamp pour éviter que la bannière sorte de l'écran
        cx = std::clamp(cx, halfW, screenW - halfW);
        cy = std::clamp(cy, halfH, screenH - halfH);

        // Rectangle source : la texture entière
        Rectangle src{
            0,
            0,
            (float)_bg->width,
            (float)_bg->height};

        // Rectangle destination : position et taille à l'écran
        Rectangle dst{
            cx - halfW,
            cy - halfH,
            w,
            h};

        // Si une animation est active et pas terminée,
        // on lui délègue le rendu de la texture
        if (_animation && !_animation->finished())
            _animation->draw(*_bg, src, dst);
        else
            // Sinon on dessine simplement la texture
            DrawTexturePro(*_bg, src, {cx, cy, w, h}, {halfW, halfH}, 0, WHITE);

        // Position verticale de départ pour centrer le texte
        float textY = cy - _cachedTextHeight * 0.5f;

        // Dessin de chaque ligne de texte centrée horizontalement
        for (size_t i = 0; i < _lines.size(); ++i)
        {
            float textX = cx - _lineWidths[i] * 0.5f;

            DrawText(
                _lines[i].c_str(),
                textX,
                textY,
                _cachedFontSize,
                BLACK);

            // Décalage pour la ligne suivante
            textY += _cachedFontSize + 4;
        }
    }
};