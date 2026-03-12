#pragma once

#include <core/StateManager.hpp>
#include "raylib.h"
#include <states/game/LevelNode.hpp>
#include <ui/petitMenu/PetitMenu.hpp>
#include <states/PauseMenuState.hpp>
#include <states/MapState.hpp>
#include <ui/animation/PixelRevealTextureAnimation.hpp>
#include <ui/animation/Particles.hpp>
#include <core/Utils.hpp>
#include <ui/animation/BulleAnimation.hpp>

// Niveaux définis en coordonnées normalisées (0..1)
// Cela permet de rendre la map indépendante de la résolution écran.
static const std::array<LevelNode, 5> LEVELS = {{{0, {0.139f, 0.215f}},
                                                 {1, {0.173f, 0.720f}},
                                                 {2, {0.467f, 0.610f}},
                                                 {3, {0.696f, 0.230f}},
                                                 {4, {0.671f, 0.730f}}}};

class LevelSelectionState : public IGameState
{
private:
    // Tableau fixe car le nombre de maps est connu à la compilation
    std::array<Texture2D, 6> _maps{};

    Texture2D _ruinTexture{};
    Texture2D _bannerTexture{};
    Texture2D _fogTexture{};
    Texture2D _haloTexture{};

    AnimatedSprite _whiteIdle;
    AnimatedSprite _redIdle;

    // Sprites animés décoratifs (liquide magique)
    std::array<AnimatedSprite, 4> _liquid;

    // Effet de bulles associé à chaque liquide
    std::array<BubbleAnimation, 4> _bubbles;

    // Positions des liquides en coordonnées normalisées
    std::array<Vector2, 4> _liquidPos{{{0.658854f, 0.918519f},
                                       {0.747396f, 0.791667f},
                                       {0.732292f, 0.600926f},
                                       {0.784375f, 0.728704f}}};

    // Cache des positions converties en pixels écran
    // évite de recalculer normalizedToScreen chaque frame
    std::array<Vector2, 4> _liquidScreenPos{};

    std::array<float, 4> _liquidScale{1.f, .8f, .6f, .8f};

    // Copie locale des nodes pour permettre modifications runtime
    std::vector<LevelNode> _levels;

    PetitMenu _tooltip;
    Sound _tooltipOpenSound{};

    PixelRevealTextureAnimation reveal;

    std::vector<std::string> _tooltipLines;

    float _pulseTimer = 0.f;
    float _uiScale = 1.f;

    // Offset utilisé pour créer l'effet de déplacement du fog
    float _fogOffsetX = 0.f;
    float _fogOffsetY = 0.f;

    std::vector<Particle> _particles;
    std::vector<Particle> _redParticles;

    // Résolution de référence utilisée pour le scaling UI
    static constexpr float REF_WIDTH = 1920.f;
    static constexpr float REF_HEIGHT = 1080.f;

private:
    // Sécurise la libération d'une texture GPU
    // évite double free si la texture n'est pas chargée
    void safeUnload(Texture2D &tex)
    {
        if (tex.id)
        {
            UnloadTexture(tex);
            tex = {};
        }
    }

    // Initialise les pools de particules
    // reserve évite les reallocations runtime
    void initParticles(std::vector<Particle> &p)
    {
        p.clear();
        p.reserve(128);
    }

    void updateFog(float dt)
    {
        // déplacement lent pour simuler un brouillard dynamique
        _fogOffsetX += dt * 10.f;
        _fogOffsetY += dt * 4.f;
    }

    void drawFog(int w, int h)
    {
        if (!_fogTexture.id)
            return;

        constexpr float scale = 2.f;

        float wTex = _fogTexture.width * scale;
        float hTex = _fogTexture.height * scale;

        // modulo pour créer un effet de scrolling infini
        float offsetX = fmod(_fogOffsetX, wTex);
        float offsetY = fmod(_fogOffsetY, hTex);

        for (float x = -wTex; x < w; x += wTex)
        {
            for (float y = -hTex; y < h; y += hTex)
            {
                DrawTextureEx(
                    _fogTexture,
                    {x + offsetX, y + offsetY},
                    0,
                    scale,
                    Fade(WHITE, 0.35f));
            }
        }
    }

    void drawLiquid()
    {
        // Utilise les positions écran déjà calculées
        for (size_t i = 0; i < _liquid.size(); i++)
            _liquid[i].draw(_liquidScreenPos[i]);
    }

    void drawMap(Texture2D &map, int w, int h)
    {
        if (!map.id)
            return;

        Rectangle src{0, 0, (float)map.width, (float)map.height};
        Rectangle dst{0, 0, (float)w, (float)h};

        // étire la texture pour couvrir toute la fenêtre
        DrawTexturePro(map, src, dst, {0, 0}, 0, WHITE);
    }

    void drawActiveNode(Vector2 pos, float pulse)
    {
        // halo pulsant autour du niveau actif
        float haloScale = (1.2f + sinf(_pulseTimer * 2.f) * 0.1f) * _uiScale * 0.15f;

        float halfW = (_haloTexture.width * haloScale) * 0.5f;
        float halfH = (_haloTexture.height * haloScale) * 0.5f;

        DrawTextureEx(_haloTexture, {pos.x - halfW, pos.y - halfH}, 0, haloScale, Fade(GOLD, 0.7f));

        DrawTextureEx(_haloTexture, {pos.x - halfW * 1.4f, pos.y - halfH * 1.4f}, 0, haloScale * 1.4f, Fade(YELLOW, 0.25f));

        _whiteIdle.setScale(.75f * pulse * _uiScale);
        _whiteIdle.draw(pos);

        // on restaure l'échelle pour éviter qu'elle reste modifiée
        _whiteIdle.setScale(.75f * _uiScale);
    }

    void drawLockedNode(Vector2 pos, float scale)
    {
        // ruine affichée pour les niveaux verrouillés
        DrawTextureEx(_ruinTexture, {pos.x - (_ruinTexture.width * scale) * 0.5f, pos.y - (_ruinTexture.height * scale) * 0.5f - 50.f * _uiScale}, 0, scale, WHITE);

        _redIdle.draw(pos);
    }

    void drawNodes(int w, int h, int unlocked)
    {
        float pulse = 1.f + sinf(_pulseTimer * 3.f) * 0.08f;
        float scale = .75f * _uiScale;

        for (auto &node : _levels)
        {
            Vector2 pos = node.getScreenPos(w, h);

            if (node.id() == unlocked)
                drawActiveNode(pos, pulse);
            else
                drawLockedNode(pos, scale);
        }
    }

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        // calcul du scale UI basé sur la résolution réelle
        _uiScale = std::min((float)w / REF_WIDTH, (float)h / REF_HEIGHT);

        initParticles(_particles);
        initParticles(_redParticles);

        for (auto &b : _bubbles)
            b.reserve(128);

        _tooltipLines.reserve(4);

        _tooltipOpenSound = LoadSound("../assets/audio/banner_open.wav");

        _tooltip.setTextureScale(1.2f);
        _tooltip.setAnimation(&reveal);

        // chargement des différentes maps
        _maps[0] = LoadTexture("../assets/ui/levelSelect/map/map.png");

        for (int i = 1; i < 6; i++)
            _maps[i] = LoadTexture(TextFormat("../assets/ui/levelSelect/map/map%d.png", i));

        _whiteIdle.setScale(.75f * _uiScale);
        _whiteIdle.loadAtlas(ctx, "../assets/ui/levelSelect/wIdle.png", "../assets/ui/levelSelect/wIdle.json", 0.15f);

        _redIdle.setScale(.75f * _uiScale);
        _redIdle.loadAtlas(ctx, "../assets/ui/levelSelect/redIdle.png", "../assets/ui/levelSelect/redIdle.json", 0.15f);

        for (size_t i = 0; i < _liquid.size(); i++)
        {
            _liquid[i].setScale(.75f * _liquidScale[i] * _uiScale);
            _liquid[i].loadAtlas(ctx, "../assets/ui/levelSelect/liquidSprite.png", "../assets/ui/levelSelect/liquidSprite.json", 0.15f);
            _liquid[i].setPos(_liquidPos[i]);
            _liquidScreenPos[i] = utils::normalizedToScreen(_liquidPos[i], w, h); // conversion normalisé -> écran calculée une seule fois
        }

        safeUnload(_ruinTexture);
        _ruinTexture = LoadTexture("../assets/ui/levelSelect/ruineTexture.png");
        _bannerTexture = LoadTexture("../assets/ui/levelSelect/banner.png");
        _fogTexture = LoadTexture("../assets/ui/levelSelect/fog.png");
        _haloTexture = LoadTexture("../assets/ui/levelSelect/halo.png");

        _tooltip.init(_bannerTexture, _tooltipOpenSound);

        // copie des niveaux statiques vers structure dynamique
        _levels.assign(LEVELS.begin(), LEVELS.end());
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        Vector2 mouse = GetMousePosition();

        int unlocked = ctx.getHighestUnlockedLevel();

        bool changeStateRequested = false;

        // pointeur utilisé pour mémoriser le node hover durant l'itération
        LevelNode *hovered = nullptr;

        for (auto &node : _levels)
        {
            // conversion normalisé → écran (évite de stocker les positions pixels)
            Vector2 pos = node.getScreenPos(w, h);

            bool playable = node.id() == unlocked;

            // update retourne true si le node est activé (click)
            if (node.update(mouse, pos, playable))
            {
                ctx.setSelectedLevel(node.id());
                changeStateRequested = true;
            }

            // pas de break volontaire ici pour laisser les autres nodes générer des particules
            if (node.isHovered())
                hovered = &node;

            int id = node.id();

            // génération aléatoire de particules autour du niveau actif
            if (id == unlocked && GetRandomValue(0, 100) < 10)
                spawnParticle(pos, _particles, _uiScale);

            // particules différentes pour les niveaux verrouillés
            else if (id > unlocked && GetRandomValue(0, 100) < 6)
                spawnParticle(pos, _redParticles, _uiScale);
        }

        // changement d'état différé pour éviter de continuer l'update de l'écran actuel
        if (changeStateRequested)
        {
            sm.changeState<MapState>();
            return;
        }

        // --- TOOLTIP ---
        if (hovered)
        {
            std::string title = "Level " + std::to_string(hovered->id() + 1);

            // reconstruction du texte chaque frame (simple mais non optimal)
            _tooltipLines = {title, "Difficulty: Easy", "Reward: 200 gold"};

            Vector2 pos = hovered->getScreenPos(w, h);
            pos.y -= 60.f; // offset vertical UI

            if (!_tooltip.isVisible())
                _tooltip.show(pos, _tooltipLines, w, h);
            else
                _tooltip.setPosition(pos);
        }
        else
            _tooltip.hide();

        // update systèmes de particules
        updateParticles(_particles, dt);
        updateParticles(_redParticles, dt);

        updateFog(dt);

        // timer utilisé pour les animations sin/cos pulsantes
        _pulseTimer = fmod(_pulseTimer + dt, 1000.f);

        _whiteIdle.update(dt);
        _redIdle.update(dt);

        for (auto &l : _liquid)
            l.update(dt);

        for (size_t i = 0; i < _bubbles.size(); i++)
        {
            // position synchronisée avec le liquide correspondant
            _bubbles[i].setPos(_liquidScreenPos[i]);

            // spawn probabiliste (~5%) pour éviter une génération continue
            if (GetRandomValue(0, 20) == 0)
                _bubbles[i].spawn();

            _bubbles[i].update(dt);
        }

        _tooltip.update(dt);

        // gestion musique centralisée dans le contexte
        ctx.updateMusic();
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int unlocked = ctx.getHighestUnlockedLevel();
        int index = std::min(unlocked, 5);

        Texture2D &map = _maps[index];

        ClearBackground(BLACK);

        drawMap(map, w, h);
        drawFog(w, h);
        drawNodes(w, h, unlocked);
        drawLiquid();

        for (auto &b : _bubbles)
            b.draw(GREEN, LIME);

        drawParticles(_particles, GOLD, YELLOW, WHITE);
        drawParticles(_redParticles, MAROON, RED, ORANGE);

        _tooltip.draw(w, h);
    }

    void onExit(StateManager &) override
    {
        for (auto &t : _maps)
            safeUnload(t);

        safeUnload(_bannerTexture);
        safeUnload(_ruinTexture);
        safeUnload(_fogTexture);
        safeUnload(_haloTexture);

        UnloadSound(_tooltipOpenSound);
    }
};