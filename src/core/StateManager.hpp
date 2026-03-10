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
    explicit StateManager(GameContext &ctx)
        : _context(ctx)
    {
        _stack.reserve(8);
        _pending.reserve(8);
    }

    GameContext &getContext()
    {
        return _context;
    }

    template <typename T, typename... Args>
    void pushState(Args &&...args)
    {
        // Capture the arguments by value in a tuple
        auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

        _pending.emplace_back(
            [this, argsTuple = std::move(argsTuple)]() mutable
            {
                // Apply the tuple to construct the state
                auto state = std::apply(
                    [](auto &&...unpackedArgs)
                    {
                        return std::make_unique<T>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
                    },
                    std::move(argsTuple));

                state->onEnter(*this);
                _stack.push_back(std::move(state));
            });
    }

    void popState()
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
        auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

        _pending.emplace_back(
            [this, argsTuple = std::move(argsTuple)]() mutable
            {
                while (!_stack.empty())
                {
                    _stack.back()->onExit(*this);
                    _stack.pop_back();
                }

                auto state = std::apply(
                    [](auto &&...unpackedArgs)
                    {
                        return std::make_unique<T>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
                    },
                    std::move(argsTuple));

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

    void render()
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

    IGameState *top()
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

    void clear()
    {
        _pending.emplace_back(
            [this]()
            {
                while (!_stack.empty())
                {
                    _stack.back()->onExit(*this);
                    _stack.pop_back();
                }
            });
    }

private:
    void applyPending()
    {
        for (auto &cmd : _pending)
            cmd();

        _pending.clear();
    }
};