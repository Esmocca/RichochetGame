@echo off
echo Building Ricochet Game in %CD%...

g++ -std=c++17 src/main.cpp src/Game.cpp src/Window.cpp -o RicochetGame.exe ^
    -I SFML/include ^
    -L SFML/lib ^
    -static -static-libgcc -static-libstdc++ ^
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Running game...
    RicochetGame.exe
) else (
    echo Build failed!
)