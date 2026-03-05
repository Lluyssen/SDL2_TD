#pragma once

#include "../utils/StateManager.hpp"
#include "raylib.h"
#include "GameState.hpp"
#include "../ui/Starfield.hpp"
#include "menu/MenuBackground.hpp"
#include "menu/MenuButtons.hpp"
#include "menu/MenuNPC.hpp"
#include "menu/MenuTitle.hpp"
#include "menu/MenuMusic.hpp"

// État représentant le menu principal avec fond animé, boutons, PNJ et musique.
#pragma once

#include "../utils/StateManager.hpp"
#include "raylib.h"
#include "GameState.hpp"
#include "../ui/Starfield.hpp"
#include "menu/MenuBackground.hpp"
#include "menu/MenuButtons.hpp"
#include "menu/MenuNPC.hpp"
#include "menu/MenuTitle.hpp"
#include "menu/MenuMusic.hpp"

// État du menu principal : orchestre fond, étoiles, UI, PNJ et musique.
class MenuState : public IGameState
{
private:
    // Phases d'initialisation et d'interaction du menu
    enum class Phase
    {
        Loading,   // court délai avant l'apparition de l'UI
        UIReveal,  // animation d'entrée des boutons
        Idle       // menu interactif
    };

    Phase _phase = Phase::Loading;
    float _timer = 0;

    // Sous-composants du menu
    MenuBackground _background;
    MenuButtons _buttons;
    MenuTitle _title;
    MenuNPC _npc;
    MenuMusic _music;

    // ECS utilisé pour le champ d'étoiles
    Registry<StarComponents> _starRegistry;
    std::unique_ptr<StarfieldSystem> _farStars;
    std::unique_ptr<StarfieldSystem> _midStars;
    std::unique_ptr<StarfieldSystem> _nearStars;

public:
    MenuState(void) = default;

    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();
        int w = ctx.getWidth(), h = ctx.getHeight();

        _background.init(ctx);
        _buttons.init(ctx);
        _npc.init();
        _music.load();

        // Initialisation des systèmes d'étoiles (3 couches de profondeur)
        _starRegistry = Registry<StarComponents>();
        _farStars = std::make_unique<StarfieldSystem>(w, h);
        _midStars = std::make_unique<StarfieldSystem>(w, h);
        _nearStars = std::make_unique<StarfieldSystem>(w, h);

        spawnStarfield(_starRegistry, 80, w, h, 1.f, 2.f);
        spawnStarfield(_starRegistry, 40, w, h, 2.f, 3.f);
        spawnStarfield(_starRegistry, 20, w, h, 3.f, 5.f);

        _phase = Phase::Loading;
        _timer = 0;
    }

    void update(StateManager &sm, float dt) override
    {
        dt = std::min(dt, 0.05f); // limite les gros dt (pause/debug)

        _background.update(dt);
        _music.update();
        _npc.update(dt);

        // Mise à jour du champ d'étoiles
        _farStars->update(dt, _starRegistry);
        _midStars->update(dt, _starRegistry);
        _nearStars->update(dt, _starRegistry);

        switch (_phase)
        {
        case Phase::Loading:
            _timer += dt;
            if (_timer > 0.2f)
            {
                _buttons.resetAnimations();
                _phase = Phase::UIReveal;
            }
            break;

        case Phase::UIReveal:
            _buttons.update(dt);
            if (_buttons.enterFinished())
                _phase = Phase::Idle;
            break;

        case Phase::Idle:
        {
            int action = _buttons.update(dt);
            if (action >= 0)
                activate(sm, action);
        }
        break;
        }
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();
        int w = ctx.getWidth(), h = ctx.getHeight();

        _background.draw(w, h);

        // Dessin des étoiles (3 couches)
        _farStars->draw(_starRegistry);
        _midStars->draw(_starRegistry);
        _nearStars->draw(_starRegistry);

        _title.draw(w, h);

        if (_phase == Phase::UIReveal || _phase == Phase::Idle)
            _buttons.draw();

        _npc.draw(h);
    }

    // Réagit au clic sur un bouton du menu
    void activate(StateManager &sm, int id)
    {
        switch (id)
        {
        case 0:
            sm.changeState<GameState>();
            break;

        case 2:
            CloseWindow();
            break;
        }
    }

    void onExit(StateManager &) override
    {
        _music.unload();
    }
};