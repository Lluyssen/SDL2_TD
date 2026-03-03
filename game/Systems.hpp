#pragma once
#include "../ecs/ECS.hpp"
#include "Components.hpp"

class MovementSystem : public SystemTypeList<TypeList<Position, Velocity>>
{
public:
    void update(double dt, Registry<Signature> &reg) override
    {
        reg.template forEachEntityWith<Signature>([&](Entity, Position &pos, Velocity &vel) {
                pos.x += vel.vx * dt;
                pos.y += vel.vy * dt;
                std::cout << "  PX : " << pos.x << " PY : " << pos.y << std::endl; });
    }

    const char *name(void) const override
    {
        return "MovementSystem";
    }
};

class RegenSystem : public SystemTypeList<TypeList<Heal, Mana>>
{
public:
    void update(double, Registry<Signature> &reg) override
    {
        reg.template forEachEntityWith<Signature>([](Entity, Heal &h, Mana &m) {
                h.hp += 1;
                m.mp += 2; });
    }

    const char *name(void) const override
    {
        return "RegenSystem";
    }
};

class PrintSystem : public SystemTypeList<TypeList<Heal, Mana>>
{
public:
    void update(double, Registry<Signature> &reg) override
    {
        reg.template forEachEntityWith<Signature>([](Entity &e, Heal &h, Mana &m) { 
            std::cout << " Entity " << e.id << " :  HP : " << h.hp << " MP: " << m.mp << " | "; });
        std::cout << "\n";
    }

    const char *name(void) const override
    {
        return "PrintSystem";
    }
};