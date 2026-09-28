# 2D-physics-rigid-body-simulator in C++ 

  I created a pure object-oriented program in C++ with Raylib for graphics that works as an interactive 2D rigid body simulator. 
  It works as a full little sandbox where everything interacts with everything else in real time.


<img width="792" height="624" alt="image" src="https://github.com/user-attachments/assets/25a9501e-51d7-4781-876c-808c42cae749" />


## What's Inside 

* **1. Rigid Body Boxes:** Spawn boxes of different masses, stack them, or grab and throw them around with your mouse. Includes a custom grid collision system for faster computing.
* **2. Spring-Damper Oscillator:** A 1-DOF vertical spring and damper system. Pull the box down and watch the oscillation die out.
* **3. Pendulum:** A rigid-rod pendulum that swings freely and collides with other objects in the scene.
* **4. Gravity Attractor (Black Hole):** Drops a gravity well that pulls every box, spring, and pendulum on the screen towards it.
* **Live Stats & Hover Inspector:** Tracks total Kinetic Energy and Momentum of the whole scene. If you hover your mouse over any box, a live tooltip shows its individual mass, speed, energy, and momentum.

## Controls 

* **Top Buttons (1 - 4):** Choose what to spawn (`Box`, `Spring-Damper`, `Pendulum`, `Attractor`)
* **Right Click:** Spawn the selected object at your mouse position
* **Left Click (Hold & Drag):** Grab & throw any box
* **Mouse Wheel (or Up / Down arrows):** Change the size & mass of the next box you spawn
* **Mouse Hover:** Point at any box to inspect its live physics stats
* **Clear All:** Wipe the screen and start over

## Files 

* `physics_simulator.cpp` — Main window, mouse controls, UI buttons, and stats
* `PhysicsCore.h` — My custom `vector2` math class, `RigidBody` physics, and grid collisions
* `SpringDamper.h` — Spring & damper math and drawing
* `Pendulum.h` — Pendulum rod constraint
* `Attractor.h` — Gravity attractor field

## How to Run 
1. Download **`physics_simulator.exe`** 
2. Double-click `physics_simulator.exe`.

