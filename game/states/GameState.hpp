#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "PauseState.hpp"

class GameState : public IGameState
{
private:
    using ScenePV = Scene<TypeList<Position, Velocity>>;
    using SceneHM = Scene<TypeList<Heal, Mana>>;

    GameManager _manager;

    ScenePV *_scenePosVel = nullptr;
    SceneHM *_sceneHealMana = nullptr;

    void setupMovementScene()
    {
        auto &reg = _scenePosVel->getRegistry();

        Entity e = reg.create();
        reg.add<Position>(e, {100.f, 100.f});
        reg.add<Velocity>(e, {50.f, 30.f});

        _scenePosVel->addSystem(new MovementSystem);
    }

    void setupStatsScene()
    {
        auto &reg = _sceneHealMana->getRegistry();

        createStatsEntity(reg, 20, 15);
        createStatsEntity(reg, 10, 25);

        _sceneHealMana->addSystem(new RegenSystem, 10);
        _sceneHealMana->addSystem(new PrintSystem, 50);
    }

    template <typename Registry>
    void createStatsEntity(Registry &reg, int hp, int mp)
    {
        Entity e = reg.create();
        reg.template add<Heal>(e, {hp});
        reg.template add<Mana>(e, {mp});
    }

    void renderMovement(SDL_Renderer *renderer)
    {
        auto &reg = _scenePosVel->getRegistry();

        SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);

        for (Entity e : reg.getAliveEntities())
        {
            if (auto *pos = reg.getIf<Position>(e))
            {
                SDL_Rect rect{
                    static_cast<int>(pos->x),
                    static_cast<int>(pos->y),
                    20, 20};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    void renderStats(SDL_Renderer *renderer)
    {
        auto &reg = _sceneHealMana->getRegistry();

        constexpr int barWidth = 100;
        constexpr int barHeight = 10;
        constexpr int spacing = 5;

        int yOffset = 400;

        for (Entity e : reg.getAliveEntities())
        {
            auto *heal = reg.getIf<Heal>(e);
            auto *mana = reg.getIf<Mana>(e);

            if (!heal || !mana)
                continue;

            int hpWidth = std::min(heal->hp * 5, barWidth);
            int mpWidth = std::min(mana->mp * 5, barWidth);

            SDL_Rect hpBar{50, yOffset, hpWidth, barHeight};
            SDL_Rect mpBar{50, yOffset + barHeight + spacing, mpWidth, barHeight};

            SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
            SDL_RenderFillRect(renderer, &hpBar);

            SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
            SDL_RenderFillRect(renderer, &mpBar);

            yOffset += 2 * (barHeight + spacing);
        }
    }

public:
    bool allowUpdateBelow() const override { return false; }
    bool allowRenderBelow() const override { return true; }

    void onEnter(StateManager &) override
    {
        std::cout << "[GameState] Enter\n";

        _scenePosVel = &_manager.createScene<TypeList<Position, Velocity>>("PosVelScene", true);
        _sceneHealMana = &_manager.createScene<TypeList<Heal, Mana>>("HealManaScene", true);

        setupMovementScene();
        setupStatsScene();
    }

    void onExit(StateManager &) override
    {
        std::cout << "[GameState] Exit\n";
    }

    void handleEvent(StateManager &sm, const SDL_Event &e) override
    {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            sm.pushState<PauseState>();
    }

    void update(StateManager &, float dt) override
    {
        _manager.update(dt);
    }

    void render(StateManager &sm) override
    {
        auto *renderer = sm.getContext().renderer;

        renderMovement(renderer);
        renderStats(renderer);
    }
};