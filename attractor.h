#pragma once
#include "Main_classes(physics core).h"

class Attractor
{
public:
    vector2 position;
    float strength;

    Attractor(vector2 p = vector2(0.0f, 0.0f), float s = 60000000.0f) : position(p), strength(s) {}

    void Apply(vector<RigidBody>& bodies, int draggedBoxIndex)
    {
        for (int i = 0; i < bodies.size(); i++)
        {
            if (i == draggedBoxIndex) continue;

            RigidBody& b = bodies[i];
            vector2 center(b.position.x + b.width / 2.0f, b.position.y + b.height / 2.0f);

            vector2 delta = position - center;
            float dist = delta.length();
            if (dist < 1.0f) continue;

            vector2 dir = delta / dist;

            // Νόμος Παγκόσμιας Έλξης: F = (G * M * m) / r^2
            float forceMag = (strength * b.mass) / ((dist * dist) + 3000.0f);
            b.ApplyForce(dir * forceMag);
        }
    }

    void Draw()
    {
        DrawCircleLines(position.x, position.y, 35, Fade(PURPLE, 0.4f));
        DrawCircleLines(position.x, position.y, 22, Fade(VIOLET, 0.6f));
        DrawCircle(position.x, position.y, 12, BLACK);
        DrawCircle(position.x, position.y, 4, PURPLE);
    }
};