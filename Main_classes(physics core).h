#pragma once
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include "raylib.h"

using namespace std;

const int CELL_SIZE = 80;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 8;

class vector2
{
public:
    float x;
    float y;

    vector2(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}

    vector2 operator+(const vector2& yeee) const { return vector2(x + yeee.x, y + yeee.y); }
    vector2 operator*(float scalar) const { return vector2(x * scalar, y * scalar); }
    vector2 operator-(const vector2& allo) const { return vector2(x - allo.x, y - allo.y); }
    vector2 operator/(float ari8mos) const { return vector2(x / ari8mos, y / ari8mos); }

    float length() const { return sqrt((x * x) + (y * y)); }
};

class RigidBody
{
public:
    vector2 position;
    vector2 velocity;
    vector2 force;
    float mass = 1600.0f;
    float e = 0.5f;
    float q = 0.9f;
    float width = 40.0f;
    float height = 40.0f;
    Color color;

    RigidBody()
    {
        color = { (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), 255 };
    }

    RigidBody(float size, float startX, float startY)
    {
        width = size;
        height = size;
        mass = size * size;
        position = vector2(startX, startY);
        velocity = vector2(0.0f, 0.0f);
        force = vector2(0.0f, 0.0f);
        color = { (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), 255 };
    }

    void ApplyForce(vector2 f)
    {
        force = force + f;
    }

    void gr(float dt)
    {
        vector2 acceleration = force / mass;
        velocity = velocity + (acceleration * dt);
        position = position + (velocity * dt);
        force = vector2(0.0f, 0.0f);

        // Όρια πατώματος και τοίχων
        float groundy = 500.0f;
        if (position.y + height >= groundy)
        {
            position.y = groundy - height;
            if (abs(velocity.y) < 120.0f) velocity.y = 0.0f;
            else velocity.y = -velocity.y * 0.1f;
            velocity.x = velocity.x * q;
        }
        if (position.x + width >= 800.0f)
        {
            position.x = 800.0f - width;
            velocity.x = -velocity.x * e;
        }
        else if (position.x <= 0.0f)
        {
            position.x = 0.0f;
            velocity.x = -velocity.x * e;
        }
    }
};

// Όλος ο έλεγχος συγκρούσεων σε συνάρτηση
/* inline void : Όταν βάζεις inline μπροστά από μια συνάρτηση, λες στον compiler: 
«Μην κάνεις άλμα στη μνήμη! Πάρε τον κώδικα της συνάρτησης και κάνε τον απευθείας copy-paste μέσα στη main()
στο σημείο που την καλώ»..*/

inline void ResolveCollisions(vector<RigidBody>& bodies)
{
    vector<int> grid[GRID_WIDTH][GRID_HEIGHT];

    for (int iter = 0; iter < 4; iter++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
            for (int y = 0; y < GRID_HEIGHT; y++)
                grid[x][y].clear();

        for (int i = 0; i < bodies.size(); i++)
        {
            int startX = max(0, (int)floor(bodies[i].position.x / CELL_SIZE));
            int endX = min(GRID_WIDTH - 1, (int)floor((bodies[i].position.x + bodies[i].width) / CELL_SIZE));
            int startY = max(0, (int)floor(bodies[i].position.y / CELL_SIZE));
            int endY = min(GRID_HEIGHT - 1, (int)floor((bodies[i].position.y + bodies[i].height) / CELL_SIZE));

            for (int cx = startX; cx <= endX; cx++)
                for (int cy = startY; cy <= endY; cy++)
                    grid[cx][cy].push_back(i);
        }

        for (int x = 0; x < GRID_WIDTH; x++)
        {
            for (int y = 0; y < GRID_HEIGHT; y++)
            {
                int boxesInCell = grid[x][y].size();
                for (int a = 0; a < boxesInCell; a++)
                {
                    for (int b = a + 1; b < boxesInCell; b++)
                    {
                        int idA = grid[x][y][a];
                        int idB = grid[x][y][b];
                        if (idA == idB) continue;

                        RigidBody& boxA = bodies[idA];
                        RigidBody& boxB = bodies[idB];

                        bool overlapX = (boxA.position.x < boxB.position.x + boxB.width) && (boxA.position.x + boxA.width > boxB.position.x);
                        bool overlapY = (boxA.position.y < boxB.position.y + boxB.height) && (boxA.position.y + boxB.height > boxB.position.y);

                        if (overlapX && overlapY)
                        {
                            float overX = min(boxA.position.x + boxA.width, boxB.position.x + boxB.width) - max(boxA.position.x, boxB.position.x);
                            float overY = min(boxA.position.y + boxA.height, boxB.position.y + boxB.height) - max(boxA.position.y, boxB.position.y);
                            float totalMass = boxA.mass + boxB.mass;

                            if (overX < overY)
                            {
                                RigidBody& left = (boxA.position.x < boxB.position.x) ? boxA : boxB;
                                RigidBody& right = (boxA.position.x < boxB.position.x) ? boxB : boxA;

                                left.position.x -= overX * (right.mass / totalMass);
                                right.position.x += overX * (left.mass / totalMass);

                                float relVx = left.velocity.x - right.velocity.x;
                                if (relVx > 0)
                                {
                                    float e = 0.2f;
                                    float vLeft = (left.mass * left.velocity.x + right.mass * right.velocity.x - right.mass * e * relVx) / totalMass;
                                    float vRight = (left.mass * left.velocity.x + right.mass * right.velocity.x + left.mass * e * relVx) / totalMass;
                                    left.velocity.x = vLeft;
                                    right.velocity.x = vRight;
                                }
                            }
                            else
                            {
                                RigidBody& top = (boxA.position.y < boxB.position.y) ? boxA : boxB;
                                RigidBody& bottom = (boxA.position.y < boxB.position.y) ? boxB : boxA;

                                if (bottom.position.y + bottom.height >= 499.5f)
                                    top.position.y -= overY;
                                else
                                {
                                    top.position.y -= overY * (bottom.mass / totalMass);
                                    bottom.position.y += overY * (top.mass / totalMass);
                                }

                                float relVy = top.velocity.y - bottom.velocity.y;
                                if (relVy > 0)
                                {
                                    float commonVy = (top.mass * top.velocity.y + bottom.mass * bottom.velocity.y) / totalMass;
                                    top.velocity.y = commonVy;
                                    bottom.velocity.y = commonVy;

                                    float diffVx = top.velocity.x - bottom.velocity.x;
                                    top.velocity.x -= diffVx * 0.3f * (bottom.mass / totalMass);
                                    bottom.velocity.x += diffVx * 0.3f * (top.mass / totalMass);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < bodies.size(); i++)
        {
            if (bodies[i].position.y + bodies[i].height > 500.0f)
            {
                bodies[i].position.y = 500.0f - bodies[i].height;
                if (bodies[i].velocity.y > 0) bodies[i].velocity.y = 0;
            }
        }
    }
}