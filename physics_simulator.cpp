#include "Main_classes(Physics core).h"
#include "Spring_damper(vibration).h"
#include "Pendulum.h"
#include "attractor.h"

bool DrawButton(float x, float y, float w, float h, const char* text, bool isSelected, Color baseColor)
{
    Vector2 m = GetMousePosition();
    bool hovered = (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);

    Color drawCol = isSelected ? SKYBLUE : (hovered ? Fade(baseColor, 0.7f) : baseColor);
    DrawRectangle(x, y, w, h, drawCol);
    DrawRectangleLines(x, y, w, h, isSelected ? BLUE : BLACK);

    int textWidth = MeasureText(text, 16);
    DrawText(text, x + (w - textWidth) / 2, y + (h - 16) / 2, 16, BLACK);

    return (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

int main()
{
    InitWindow(800, 600, "2D Physics Engine - Raylib");
    SetTargetFPS(60);

    vector<RigidBody> bodies;
    vector<SpringDamper> springs;
    vector<Pendulum> pendulums;
    vector<Attractor> attractors;

    int currentTool = 0; // 0 = Box, 1 = Spring-Damper, 2 = Pendulum, 3 = attractor
    int draggedBoxIndex = -1;
    vector2 prevMousePos(0, 0);
    float currentSpawnSize = 40.0f;
    int hoveredBoxIndex = -1;
    while (!WindowShouldClose())
    {
        hoveredBoxIndex = -1;
       
        // --- 1. INPUT & ΠΟΝΤΙΚΙ ---
        if (IsKeyDown(KEY_UP)) currentSpawnSize += 1.0f;
        if (IsKeyDown(KEY_DOWN)) currentSpawnSize -= 1.0f;
        currentSpawnSize += GetMouseWheelMove() * 5.0f;
        currentSpawnSize = max(10.0f, min(300.0f, currentSpawnSize));

        vector2 mousePos(GetMouseX(), GetMouseY());
        float dt = min(GetFrameTime(), 0.1f);
        if (mousePos.y > 55)
        {
            for (int i = (int)bodies.size() - 1; i >= 0; i--)
            {
                if (mousePos.x >= bodies[i].position.x && mousePos.x <= bodies[i].position.x + bodies[i].width &&
                    mousePos.y >= bodies[i].position.y && mousePos.y <= bodies[i].position.y + bodies[i].height)
                {
                    hoveredBoxIndex = i;
                    break;
                }
            }
        }

        // Αριστερό κλικ: Grab (μόνο κάτω από την μπάρα των κουμπιών y > 55)
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mousePos.y > 55)
        {
            draggedBoxIndex = -1;
            for (int i = (int)bodies.size() - 1; i >= 0; i--)
            {
                if (mousePos.x >= bodies[i].position.x && mousePos.x <= bodies[i].position.x + bodies[i].width &&
                    mousePos.y >= bodies[i].position.y && mousePos.y <= bodies[i].position.y + bodies[i].height)
                {
                    draggedBoxIndex = i;
                    break;
                }
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && draggedBoxIndex != -1)
        {
            bodies[draggedBoxIndex].position.x = mousePos.x - bodies[draggedBoxIndex].width / 2.0f;
            bodies[draggedBoxIndex].position.y = mousePos.y - bodies[draggedBoxIndex].height / 2.0f;
            bodies[draggedBoxIndex].velocity = vector2(0, 0);
            bodies[draggedBoxIndex].force = vector2(0, 0);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && draggedBoxIndex != -1)
        {
            if (dt > 0)
            {
                bodies[draggedBoxIndex].velocity = (mousePos - prevMousePos) / dt;
            }
            draggedBoxIndex = -1;
        }

        // Δεξί κλικ: Spawn ανάλογα με το επιλεγμένο κουμπί!
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && mousePos.y > 55)
        {
            if (currentTool == 0) // Κουτί
            {
                bodies.push_back(RigidBody(currentSpawnSize, mousePos.x - currentSpawnSize / 2, mousePos.y - currentSpawnSize / 2));
            }
            else if (currentTool == 1) // Spring-Damper
            {
                RigidBody newBox(currentSpawnSize, mousePos.x - currentSpawnSize / 2, mousePos.y + 140.0f - currentSpawnSize / 2);
                bodies.push_back(newBox);
                springs.push_back({ mousePos, (int)bodies.size() - 1, 110.0f, newBox.mass * 25.0f, newBox.mass * 1.2f });
            }
            else if (currentTool == 2) // Εκκρεμές 
            {
                float rodLength = 140.0f;
                RigidBody newBox(currentSpawnSize, mousePos.x + 100.0f - currentSpawnSize / 2, mousePos.y + 100.0f - currentSpawnSize / 2);
                bodies.push_back(newBox);
                pendulums.push_back({ mousePos, (int)bodies.size() - 1, rodLength });
            }
            else if (currentTool == 3) // 4. Attractor 
            {
                attractors.push_back(Attractor(mousePos, 60000000.0f));
            }
        }

        prevMousePos = mousePos;

        // --- 2. ΦΥΣΙΚΗ & ΣΥΓΚΡΟΥΣΕΙΣ ---

        for (int i = 0; i < attractors.size(); i++)
            attractors[i].Apply(bodies, draggedBoxIndex);

        for (int i = 0; i < springs.size(); i++)
            if (springs[i].bodyId != draggedBoxIndex)
                springs[i].Apply(bodies);

        for (int i = 0; i < bodies.size(); i++)
        {
            if (i != draggedBoxIndex)
            {
                bodies[i].ApplyForce(vector2(0, bodies[i].mass * 980.0f));
                bodies[i].gr(dt);
            }
        }

        for (int i = 0; i < pendulums.size(); i++)
            if (pendulums[i].bodyId != draggedBoxIndex)
                pendulums[i].ApplyConstraint(bodies);

        ResolveCollisions(bodies);

        float totalKineticEnergy = 0.0f;
        float totalMomentum = 0.0f;
        for (int i = 0; i < bodies.size(); i++)
        {
            float v_squared = pow(bodies[i].velocity.length(), 2);
            totalKineticEnergy += 0.5f * bodies[i].mass * v_squared / 10000.0f;
            totalMomentum += (bodies[i].mass * bodies[i].velocity.length()) / 1000.0f;
        }

        // --- 3. ΣΧΕΔΙΑΣΗ (RENDERING) ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 500, 800, 100, LIGHTGRAY);
        for (int x = 0; x < 800; x += CELL_SIZE) DrawLine(x, 0, x, 600, Fade(LIGHTGRAY, 0.5f));
        for (int y = 0; y < 600; y += CELL_SIZE) DrawLine(0, y, 800, y, Fade(LIGHTGRAY, 0.5f));
        for (int i = 0; i < attractors.size(); i++) attractors[i].Draw();
        for (int i = 0; i < springs.size(); i++)   springs[i].Draw(bodies);
        for (int i = 0; i < pendulums.size(); i++) pendulums[i].Draw(bodies);

        for (int i = 0; i < bodies.size(); i++)
        {
            DrawRectangle(bodies[i].position.x, bodies[i].position.y, bodies[i].width, bodies[i].height, bodies[i].color);
            DrawRectangleLines(bodies[i].position.x, bodies[i].position.y, bodies[i].width, bodies[i].height, BLACK);

            float cx = bodies[i].position.x + bodies[i].width / 2.0f;
            float cy = bodies[i].position.y + bodies[i].height / 2.0f;
            DrawLine(cx, cy, cx + bodies[i].velocity.x * 0.1f, cy + bodies[i].velocity.y * 0.1f, RED);
        }

        // Toolbar στην κορυφή
        DrawRectangle(0, 0, 800, 55, Fade(LIGHTGRAY, 0.85f));
        DrawLine(0, 55, 800, 55, GRAY);

        if (DrawButton(10, 10, 110, 35, "1. Box", currentTool == 0, LIGHTGRAY)) currentTool = 0;
        if (DrawButton(130, 10, 150, 35, "2. Spring-Damper", currentTool == 1, LIGHTGRAY)) currentTool = 1;
        if (DrawButton(290, 10, 130, 35, "3. Pendulum", currentTool == 2, LIGHTGRAY)) currentTool = 2;
        if (DrawButton(430, 10, 130, 35, "4. Attractror", currentTool == 3, LIGHTGRAY)) currentTool = 3;

        if (DrawButton(680, 10, 110, 35, "Clear All", false, MAROON))
        {
            bodies.clear();
            springs.clear();
            pendulums.clear();
            attractors.clear();
            draggedBoxIndex = -1;
        }

        DrawText(TextFormat("Boxes: %d | FPS: %d", (int)bodies.size(), GetFPS()), 10, 65, 20, DARKGRAY);
        DrawText(TextFormat("Spawn Mass (WHEEL): %.0f", currentSpawnSize * currentSpawnSize), 10, 90, 20, RED);
        DrawText(TextFormat("Total Kinetic Energy (J): %.1f k", totalKineticEnergy), 10, 115, 20, RED); DrawText(TextFormat("Total Momentum (p): %.1f kNs", totalMomentum), 10, 140, 20, PURPLE);

        if (hoveredBoxIndex != -1 && hoveredBoxIndex < bodies.size())
        {
            RigidBody& b = bodies[hoveredBoxIndex];
            float speed = b.velocity.length();
            float boxKE = 0.5f * b.mass * (speed * speed) / 10000.0f;
            float boxMomentum = (b.mass * speed) / 1000.0f;

            // Κίτρινο περίγραμμα στο κουτί που κοιτάμε
            DrawRectangleLinesEx({ b.position.x, b.position.y, b.width, b.height }, 3.0f, GOLD);

            // Μαύρο καρτελάκι στοιχείων δίπλα στο ποντίκι
            DrawRectangle(mousePos.x + 15, mousePos.y + 15, 210, 95, Fade(BLACK, 0.8f));
            DrawRectangleLines(mousePos.x + 15, mousePos.y + 15, 210, 95, GOLD);

            DrawText(TextFormat("Box #%d (m: %.0f)", hoveredBoxIndex, b.mass), mousePos.x + 23, mousePos.y + 23, 15, GOLD);
            DrawText(TextFormat("Speed: %.1f px/s", speed), mousePos.x + 23, mousePos.y + 43, 15, RAYWHITE);
            DrawText(TextFormat("Kinetic E: %.2f kJ", boxKE), mousePos.x + 23, mousePos.y + 63, 15, SKYBLUE);
            DrawText(TextFormat("Momentum: %.2f kNs", boxMomentum), mousePos.x + 23, mousePos.y + 83, 15, GREEN);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}