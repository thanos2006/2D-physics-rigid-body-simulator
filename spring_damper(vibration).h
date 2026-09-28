#pragma once
#include "Main_classes(physics core).h"

class SpringDamper
{
public:
    vector2 anchor;
    int bodyId;
    float restLength;
    float k;
    float c;

    SpringDamper(vector2 a, int id, float l, float _k, float _c) : anchor(a), bodyId(id), restLength(l), k(_k), c(_c) {}
    void Apply(vector<RigidBody>& bodies)
    {
        RigidBody& b = bodies[bodyId];

        // Κρατάμε το κουτί αυστηρά στον κατακόρυφο άξονα (1-DOF Ταλάντωση)
        b.position.x = anchor.x - b.width / 2.0f;
        b.velocity.x = 0.0f;

        float centerY = b.position.y + b.height / 2.0f;
        float currentLength = centerY - anchor.y;

        float stretch = currentLength - restLength;
        float springForceY = -k * stretch;
        float damperForceY = -c * b.velocity.y;

        b.ApplyForce(vector2(0.0f, springForceY + damperForceY));
    }

    void Draw(const vector<RigidBody>& bodies)
    {
        const RigidBody& b = bodies[bodyId];
        float topY = b.position.y;

        DrawLineEx({ anchor.x - 25, anchor.y }, { anchor.x + 25, anchor.y }, 4.0f, BLACK);

        // Ελατήριο (Αριστερά)
        int coils = 8;
        float springX = anchor.x - 10.0f;
        float stepY = (topY - anchor.y) / coils;
        for (int i = 0; i < coils; i++)
        {
            float y1 = anchor.y + i * stepY;
            float y2 = anchor.y + (i + 0.5f) * stepY;
            float y3 = anchor.y + (i + 1.0f) * stepY;
            DrawLine(springX, y1, springX - 6, y2, DARKGRAY);
            DrawLine(springX - 6, y2, springX, y3, DARKGRAY);
        }

        // Αποσβεστήρας (Δεξιά)
        float damperX = anchor.x + 10.0f;
        float midY = (anchor.y + topY) / 2.0f;
        DrawLineEx({ damperX, anchor.y }, { damperX, midY }, 2.0f, BLACK);
        DrawRectangleLines(damperX - 6, midY - 15, 12, 30, BLACK);
        DrawLineEx({ damperX - 4, midY }, { damperX + 4, midY }, 3.0f, DARKGRAY);
        DrawLineEx({ damperX, midY }, { damperX, topY }, 2.0f, DARKGRAY);
    }
};