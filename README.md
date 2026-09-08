#Ricochet Game

<img width="1276" height="721" alt="image" src="https://github.com/user-attachments/assets/ffe265ff-d74a-4352-a458-543b5c60cdc1" />


This implementation plays a 2D physics-based arcade and puzzle game where you shoot projectiles that bounce off surfaces to destroy targets. You must calculate reflection angles, timing, and trajectory to clear each level using a limited number of shots.

Rules
The rules are pretty simple.

1. You start each level with a set number of projectiles and a layout of targets and obstacles.
2. You aim using your mouse and fire a projectile towards your target.
3. The projectile bounces off solid walls and surfaces based on the angle of incidence.
4. At this point, depending on the objects hit, several outcomes can happen:
   - If a projectile hits a target, the target is destroyed.
   - If a projectile hits a destructible block or absorber, it reduces the block's durability or gets absorbed.
   - If a projectile hits a reflector mirror, it changes trajectory according to the mirror's angle.
5. You must destroy all targets on the screen to win the level and advance to the next one.
6. If you run out of projectiles before destroying all targets, you lose the level and must restart.

How to Build and Run

Prerequisites
- C++ Compiler (C++17 or higher)
- CMake (version 3.12 or higher)
- SFML library

Installation

1. Clone the repository:
   git clone https://github.com/Esmocca/RichochetGame.git
   cd RichochetGame

2. Build using CMake:
   mkdir build
   cd build
   cmake ..
   make

3. Run the game:
   ./RicochetGame

Controls
- Mouse Movement: Aim
- Left Mouse Button: Shoot
- R Key: Reset current level
- Esc / P Key: Pause game
