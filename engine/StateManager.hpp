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
    virtual ~StateManager(void) = default;    

    // Reset
    template <typename T, typename... Args>
    void changeState(Args &&...args)
    {
        while (!_stack.empty())
        {
            _stack.back()->onExit(*this);
            _stack.pop_back();
        }
        pushState<T>(std::forward<Args>(args)...);
    }

    // AJOUT
    template <typename T, typename... Args>
    void pushState(Args &&...args)
    {
        auto state = std::make_unique<T>(std::forward<Args>(args)...);
        state->onEnter(*this);
        _stack.push_back(std::move(state));
    }

    // Retour
    void popState()
    {
        if (_stack.empty())
            return;
        _stack.back()->onExit(*this);
        _stack.pop_back();
    }

    void handleEvent(const SDL_Event &e)
    {
        if (!_stack.empty())
            _stack.back()->handleEvent(*this, e);
    }

    void update(float dt)
    {
        for (int i = _stack.size() - 1; i >= 0; --i)
        {
            _stack[i]->update(*this, dt);
            if (!_stack[i]->allowUpdateBelow())
                break;
        }
    }

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
};