#pragma once

#include <vector>
#include <memory>

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

public:
    explicit StateManager(GameContext &ctx) : _context(ctx) {}

    virtual ~StateManager() = default;

    // Remplace tous les états par un nouveau
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

    // Ajoute un état au-dessus du stack
    template <typename T, typename... Args>
    void pushState(Args &&...args)
    {
        auto state = std::make_unique<T>(std::forward<Args>(args)...);
        state->onEnter(*this);
        _stack.push_back(std::move(state));
    }

    // Supprime l'état courant
    void popState(void)
    {
        if (_stack.empty())
            return;

        _stack.back()->onExit(*this);
        _stack.pop_back();
    }

    // Update du stack (du haut vers le bas)
    void update(float dt)
    {
        for (int i = (int)_stack.size() - 1; i >= 0; --i)
        {
            _stack[i]->update(*this, dt);

            if (!_stack[i]->allowUpdateBelow())
                break;
        }
    }

    // Render du stack (du bas vers le haut)
    void render(void)
    {
        for (size_t i = 0; i < _stack.size(); ++i)
        {
            _stack[i]->render(*this);

            if (!_stack[i]->allowRenderBelow())
                break;
        }
    }

    // Accès au contexte global du jeu
    GameContext &getContext(void)
    {
        return _context;
    }

    // Récupérer un état spécifique dans le stack
    template <typename T>
    T *getState(void)
    {
        for (auto &s : _stack)
        {
            if (auto ptr = dynamic_cast<T *>(s.get()))
                return ptr;
        }
        return nullptr;
    }

    // Accès rapide à l'état courant
    IGameState *top(void)
    {
        if (_stack.empty())
            return nullptr;
        return _stack.back().get();
    }

    // Vide entièrement le stack
    void clear(void)
    {
        while (!_stack.empty())
        {
            _stack.back()->onExit(*this);
            _stack.pop_back();
        }
    }
};