#pragma once
#include "../../ecs/ECS.hpp"
#include "raylib.h"
#include <vector>
#include <math.h>

// Composant pour une étoile
struct StarData
{
    float size;
    float phase;
    float speed;
};

struct Position
{
    float x = 0.f, y = 0.f;
};
struct Velocity
{
    float vx = 0.f, vy = 0.f;
};

// Alias pour faciliter les systèmes
using StarComponents = TypeList<Position, Velocity, StarData>;

// Système pour mettre à jour et dessiner les étoiles
class StarfieldSystem : public SystemTypeList<StarComponents>
{
private:
    int screenWidth;
    int screenHeight;

public:
    StarfieldSystem(int w, int h) : screenWidth(w), screenHeight(h) {}

    void update(double dt, Registry<StarComponents> &reg) override
    {
        // Mise à jour de position et phase
        reg.template forEachEntityWith<StarComponents>([&](Entity, Position &pos, Velocity &vel, StarData &s) {
            s.phase += dt * 2.0f;
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;

            if (pos.x < 0) pos.x += screenWidth;
            if (pos.x > screenWidth) pos.x -= screenWidth;
            if (pos.y < 0) pos.y += screenHeight;
            if (pos.y > screenHeight) pos.y -= screenHeight; });
    }

    void draw(Registry<StarComponents> &reg)
    {
        Vector2 mouse = GetMousePosition();
        float nx = (mouse.x / screenWidth) - 0.5f;
        float ny = (mouse.y / screenHeight) - 0.5f;

        reg.template forEachEntityWith<StarComponents>([&](Entity, Position &pos, Velocity &vel, StarData &s) {
            float twinkle = (sinf(s.phase * 3.0f) + 1.0f) * 0.5f;
            float brightness = 160 + twinkle * 80;

            Color c{(unsigned char)brightness, (unsigned char)brightness, (unsigned char)(brightness + 20), 255};
            float size = s.size + twinkle * 0.4f;

            DrawCircleV({pos.x + nx * 30, pos.y + ny * 30}, size, c); });
    }

    const char *name() const override { return "StarfieldSystem"; }
};

// Fonction utilitaire pour créer un champ d’étoiles
inline void spawnStarfield(Registry<StarComponents> &reg, int count, int w, int h, float sizeMin, float sizeMax)
{
    for (int i = 0; i < count; i++)
    {
        Entity e = reg.create();
        reg.add<Position>(e, {(float)GetRandomValue(0, w), (float)GetRandomValue(0, h)});
        reg.add<Velocity>(e, {(float)GetRandomValue(-20, 20), (float)GetRandomValue(-20, 20)});
        reg.add<StarData>(e, {GetRandomValue((int)(sizeMin * 10), (int)(sizeMax * 10)) / 10.0f,
                              GetRandomValue(0, 628) / 100.0f,
                              0.0f});
    }
}