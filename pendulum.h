#pragma once
#include "Main_classes(physics core).h"

class Pendulum
{
public:
    vector2 pivot;   // Σημείο εξάρτησης
    int bodyId;      // Ποιο κουτί είναι το βαρίδι
    float length;    // Μήκος άκαμπτης ράβδου (L)

    Pendulum(vector2 p, int id, float l) : pivot(p), bodyId(id), length(l) {}

    void ApplyConstraint(vector<RigidBody>& bodies)
    {
        RigidBody& b = bodies[bodyId];

        vector2 center(b.position.x + b.width / 2.0f, b.position.y + b.height / 2.0f);
        vector2 delta = center - pivot;
        float currentDist = delta.length();
        if (currentDist < 0.001f) return;

        vector2 dir = delta / currentDist; // Μοναδιαίο διάνυσμα ράβδου

        // θέση σε απόσταση L 
        vector2 lockedCenter = pivot + (dir * length);
        b.position.x = lockedCenter.x - b.width / 2.0f;
        b.position.y = lockedCenter.y - b.height / 2.0f;

        // Αφαιρούμε την ταχύτητα κατά μήκος της ράβδου, κρατάμε την εφαπτομενική
        float radialVel = (b.velocity.x * dir.x) + (b.velocity.y * dir.y);
        b.velocity.x -= radialVel * dir.x;
        b.velocity.y -= radialVel * dir.y;

        // Ελάχιστη αντίσταση αέρα
        b.velocity = b.velocity * 0.999f;
    }

    void Draw(const vector<RigidBody>& bodies)
    {
        const RigidBody& b = bodies[bodyId];
        vector2 center(b.position.x + b.width / 2.0f, b.position.y + b.height / 2.0f);

        DrawLineEx({ pivot.x, pivot.y }, { center.x, center.y }, 3.0f, DARKBLUE);
        DrawCircle(pivot.x, pivot.y, 6, BLACK);
    }
};