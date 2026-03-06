#pragma once

#include "../../utils/StateManager.hpp"
#include "../MenuState.hpp"
#include "raylib.h"
#include <vector>


class LevelState : public IGameState
{
    public:
    virtual ~LevelState(void) = default; 
    LevelState(int){} 
    void onEnter(StateManager &) override {}
    void onExit(StateManager &) override {}
    void update(StateManager &, float) override {}
    void render(StateManager &) override {}
};
