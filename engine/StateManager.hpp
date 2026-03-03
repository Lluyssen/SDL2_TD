#pragma once

#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include "../game/GameContext.h"
#include "State.hpp"

class IGameState;

class StateManager
{
private:
    GameContext &_context;
    std::vector<std::unique_ptr<IGameState>> _stack;

public:
    explicit StateManager(GameContext &ctx) : _context(ctx) {}
    virtual ~StateManager() = default;

    // Remplace l'état courant, optionnellement en préservant certains états
    template <typename T, typename... Args>
    void changeState(Args&&... args)
    {
        while (!_stack.empty())
        {
            _stack.back()->onExit(*this);
            _stack.pop_back(); // détruit l'état sauf si on fait du caching
        }
        pushState<T>(std::forward<Args>(args)...);
    }

    // Ajoute un état au-dessus
    template <typename T, typename... Args>
    void pushState(Args&&... args)
    {
        auto state = std::make_unique<T>(std::forward<Args>(args)...);
        state->onEnter(*this);
        _stack.push_back(std::move(state));
    }

    // Retire l'état courant et revient à l'état précédent
    void popState()
    {
        if (_stack.empty())
            return;

        _stack.back()->onExit(*this);
        _stack.pop_back();
    }

    // Gestion des événements : seulement l'état au top
    void handleEvent(const SDL_Event &e)
    {
        if (!_stack.empty())
            _stack.back()->handleEvent(*this, e);
    }

    // Update du stack : on remonte du top vers le bas, en s'arrêtant si allowUpdateBelow() == false
    void update(float dt)
    {
        for (int i = (int)_stack.size() - 1; i >= 0; --i)
        {
            _stack[i]->update(*this, dt);
            if (!_stack[i]->allowUpdateBelow())
                break;
        }
    }

    // Render du stack : du bas vers le haut, stop si allowRenderBelow() == false
    void render()
    {
        for (int i = 0; i < (int)_stack.size(); ++i)
        {
            _stack[i]->render(*this);
            if (!_stack[i]->allowRenderBelow())
                break;
        }
    }

    GameContext &getContext() { return _context; }

    // **Nouvelle méthode : récupérer un état déjà existant (ex: MenuState)**
    template <typename T>
    T* getState()
    {
        for (auto &s : _stack)
        {
            if (auto ptr = dynamic_cast<T*>(s.get()))
                return ptr;
        }
        return nullptr;
    }
};