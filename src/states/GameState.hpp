#pragma once

#include <core/StateManager.hpp>
#include "raylib.h"
#include <states/game/LevelNode.hpp>
#include <ui/petitMenu/PetitMenu.hpp>
#include <states/PauseMenuState.hpp>
#include <states/MapState.hpp>
#include <ui/animation/PixelRevealTextureAnimation.hpp>

// Etat principal de la sélection de niveau.
// Affiche la carte, les nodes de niveaux et un tooltip animé.
class GameState : public IGameState
{
private:
    // différentes textures de carte selon la progression
    std::vector<Texture2D> _maps;

    // texture utilisée par la bannière du tooltip
    Texture2D _bannerTexture{};

    // sprites animés des nodes (niveau disponible / verrouillé)
    AnimatedSprite _whiteIdle;
    AnimatedSprite _redIdle;

    // liste des nodes de niveaux sur la carte
    std::vector<LevelNode> _levels;

    // tooltip affiché lorsqu'on survole un niveau
    PetitMenu _tooltip;

    // son joué lors de l'ouverture du tooltip
    Sound _tooltipOpenSound{};

    // animation appliquée à la texture du tooltip
    PixelRevealTextureAnimation reveal;

    // timer utilisé pour l'effet de pulsation du niveau actif
    float _pulseTimer = 0.f;

    // permet de cliquer pour afficher les coordonnées normalisées
    bool _debugNodePlacement = true;

    // résolution de référence utilisée pour le scaling UI
    static constexpr float REF_WIDTH = 1920.f;
    static constexpr float REF_HEIGHT = 1080.f;

    float _uiScale = 1.f;

    // lignes de texte affichées dans le tooltip
    std::vector<std::string> _tooltipLines;

public:
    // appelé lors de l'entrée dans cet état
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        // calcul du scale UI basé sur la résolution actuelle
        float scaleX = ctx.getWidth() / REF_WIDTH;
        float scaleY = ctx.getHeight() / REF_HEIGHT;
        _uiScale = std::min(scaleX, scaleY);

        _tooltipLines.reserve(4);

        // chargement du son du tooltip
        _tooltipOpenSound = LoadSound("../assets/audio/banner_open.wav");

        // configuration du tooltip
        _tooltip.setTextureScale(1.2f);
        _tooltip.setAnimation(&reveal);

        // libère les anciennes textures de carte
        for (auto &t : _maps)
        {
            if (t.id != 0)
                UnloadTexture(t);
        }

        _maps.clear();

        // charge la carte principale
        _maps.push_back(LoadTexture("../assets/ui/levelSelect/map/map.png"));

        // charge les variantes de carte débloquées progressivement
        for (int i = 1; i < 6; ++i)
            _maps.push_back(LoadTexture(TextFormat("../assets/ui/levelSelect/map/map%d.png", i)));

        // initialisation des sprites animés des nodes
        _whiteIdle.setScale(.75f * _uiScale);
        _whiteIdle.loadAtlas(ctx, "../assets/ui/levelSelect/wIdle.png", "../assets/ui/levelSelect/wIdle.json", 0.15f);

        _redIdle.setScale(.75f * _uiScale);
        _redIdle.loadAtlas(ctx, "../assets/ui/levelSelect/redIdle.png", "../assets/ui/levelSelect/redIdle.json", 0.15f);

        // chargement de la bannière du tooltip
        if (_bannerTexture.id != 0)
            UnloadTexture(_bannerTexture);

        _bannerTexture = LoadTexture("../assets/ui/levelSelect/banner.png");

        _tooltip.init(_bannerTexture, _tooltipOpenSound);

        // positions normalisées des niveaux sur la carte
        _levels = {
            {0, {0.139f, 0.215f}},
            {1, {0.173f, 0.720f}},
            {2, {0.467f, 0.610f}},
            {3, {0.696f, 0.230f}},
            {4, {0.671f, 0.730f}}};

        _pulseTimer = 0.f;
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();

        Vector2 mouse = GetMousePosition();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        // niveau le plus élevé débloqué
        int unlocked = ctx.getHighestUnlockedLevel();

        bool changeStateRequested = false;

        LevelNode *hovered = nullptr;

        // mode debug pour récupérer les positions normalisées des nodes
        if (_debugNodePlacement && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            float nx = mouse.x / w;
            float ny = mouse.y / h;

            TraceLog(LOG_INFO, "Node position normalized: {%.3ff, %.3ff}", nx, ny);
        }

        // mise à jour des nodes
        for (auto &node : _levels)
        {
            Vector2 pos = node.getScreenPos(w, h);

            bool playable = node.id() == unlocked;

            // si on clique sur un niveau jouable → changement d'état
            if (node.update(mouse, pos, playable))
            {
                ctx.setSelectedLevel(node.id());
                changeStateRequested = true;
            }

            // détection du node survolé
            if (node.isHovered())
                hovered = &node;
        }

        if (changeStateRequested)
        {
            sm.changeState<MapState>();
            return;
        }

        // gestion du tooltip
        if (hovered)
        {
            _tooltipLines.clear();

            _tooltipLines.push_back(TextFormat("Level %d", hovered->id() + 1));
            _tooltipLines.emplace_back("Difficulty: Easy");
            _tooltipLines.emplace_back("Reward: 200 gold");

            Vector2 pos = hovered->getScreenPos(w, h);
            pos.y -= 60.f;

            // affiche ou met à jour la position du tooltip
            if (!_tooltip.isVisible())
                _tooltip.show(pos, _tooltipLines, w, h);
            else
                _tooltip.setPosition(pos);
        }
        else
            _tooltip.hide();

        _pulseTimer += dt;

        // mise à jour des animations
        _whiteIdle.update(dt);
        _redIdle.update(dt);
        _tooltip.update(dt);

        ctx.updateMusic();
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int unlocked = ctx.getHighestUnlockedLevel();

        // sélection de la carte correspondant au niveau débloqué
        Texture2D &map = _maps[std::min(unlocked, static_cast<int>(_maps.size()) - 1)];

        ClearBackground(BLACK);

        // dessin de la carte en plein écran
        Rectangle src{0.f, 0.f, static_cast<float>(map.width), static_cast<float>(map.height)};
        Rectangle dst{0.f, 0.f, static_cast<float>(w), static_cast<float>(h)};

        DrawTexturePro(map, src, dst, {0.f, 0.f}, 0.f, WHITE);

        // animation de pulsation du niveau jouable
        float pulse = 1.0f + sinf(_pulseTimer * 3.f) * 0.08f;

        for (auto &node : _levels)
        {
            Vector2 pos = node.getScreenPos(w, h);

            if (node.id() == unlocked)
            {
                _whiteIdle.setScale(.75f * pulse * _uiScale);
                _whiteIdle.draw(pos);
                _whiteIdle.setScale(.75f * _uiScale);
            }
            else
                _redIdle.draw(pos);
        }

        // dessin du tooltip
        _tooltip.draw(w, h);
    }

    void onExit(StateManager &) override
    {
        // libération des textures
        for (auto &t : _maps)
        {
            if (t.id != 0)
                UnloadTexture(t);
        }

        if (_bannerTexture.id != 0)
            UnloadTexture(_bannerTexture);

        UnloadSound(_tooltipOpenSound);
    }
};