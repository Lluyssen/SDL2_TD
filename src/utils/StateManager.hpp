#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "GameContext.hpp"
class StateManager;

// Interface de base pour tous les états du jeu
class IGameState
{
public:
    virtual ~IGameState() = default;

    // Lifecycle
    virtual void onEnter(StateManager &) {}
    virtual void onExit(StateManager &) {}

    // Update logique
    virtual void update(StateManager &, float) {}

    // Render graphique
    virtual void render(StateManager &) {}

    // Permet de laisser les états en dessous s'update
    virtual bool allowUpdateBelow() const { return false; }

    // Permet de laisser les états en dessous se rendre
    virtual bool allowRenderBelow() const { return false; }
};

// Gestionnaire de stack d'états (Menu, Game, Pause...)
class StateManager
{
private:
    GameContext &_context;

    std::vector<std::unique_ptr<IGameState>> _stack;
    std::vector<std::function<void()>> _pending;

public:
    explicit StateManager(GameContext &ctx) : _context(ctx)
    {
        _stack.reserve(8);
        _pending.reserve(8);
    }

    ~StateManager(void) = default;

    GameContext &getContext()
    {
        return _context;
    }

    template <typename T, typename... Args>
    void pushState(Args &&...args)
    {
        _pending.emplace_back(
            [this, args...]()
            {
                auto state = std::make_unique<T>(args...);
                state->onEnter(*this);
                _stack.push_back(std::move(state));
            });
    }

    void popState(void)
    {
        _pending.emplace_back(
            [this]()
            {
                if (_stack.empty())
                    return;

                _stack.back()->onExit(*this);
                _stack.pop_back();
            });
    }

    template <typename T, typename... Args>
    void changeState(Args &&...args)
    {
        _pending.emplace_back(
            [this, args...]()
            {
                while (!_stack.empty())
                {
                    _stack.back()->onExit(*this);
                    _stack.pop_back();
                }

                auto state = std::make_unique<T>(args...);
                state->onEnter(*this);
                _stack.push_back(std::move(state));
            });
    }

    void update(float dt)
    {
        for (int i = (int)_stack.size() - 1; i >= 0; --i)
        {
            _stack[i]->update(*this, dt);

            if (!_stack[i]->allowUpdateBelow())
                break;
        }

        applyPending();
    }

    void render(void)
    {
        int start = 0;

        for (int i = (int)_stack.size() - 1; i >= 0; --i)
        {
            if (!_stack[i]->allowRenderBelow())
            {
                start = i;
                break;
            }
        }

        for (size_t i = start; i < _stack.size(); ++i)
            _stack[i]->render(*this);
    }

    IGameState *top(void)
    {
        if (_stack.empty())
            return nullptr;
        return _stack.back().get();
    }

    template <typename T>
    T *getState()
    {
        for (auto &s : _stack)
        {
            if (auto ptr = dynamic_cast<T *>(s.get()))
                return ptr;
        }

        return nullptr;
    }

    void clear(void)
    {
        _pending.emplace_back( [this]() {
                while (!_stack.empty())
                {
                    _stack.back()->onExit(*this);
                    _stack.pop_back();
                }
            });
    }

private:
    void applyPending(void)
    {
        for (auto &cmd : _pending)
            cmd();
        _pending.clear();
    }
};