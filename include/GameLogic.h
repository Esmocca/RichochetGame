#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>
#include <vector>

enum class GameState { Menu, Options, Playing, StageClear, GameOver };

struct Brick {
  sf::ConvexShape shape;
  sf::Vector2f targetPos;
  bool isDestroyed = false;
  int points = 100;
};

struct ResolutionOption {
  std::string label;
  unsigned int width;
  unsigned int height;
};

struct PaddleColorOption {
  std::string label;
  sf::Color color;
};

class GameWindow {
public:
  GameWindow(const std::string &title, unsigned int width, unsigned int height);
  ~GameWindow();

  void clear();
  void display();
  bool isOpen() const;
  std::optional<sf::Event> pollEvent();
  sf::RenderWindow &getRenderWindow() { return window; }

  void handleResize(unsigned int newWidth, unsigned int newHeight);
  void setResolution(unsigned int width, unsigned int height, bool fullscreen);
  void toggleFullscreen();
  bool getIsFullscreen() const { return isFullscreen; }

  void setGameView();
  void setUIView();

  sf::Vector2u getWindowSize() const { return window.getSize(); }

private:
  sf::RenderWindow window;
  std::string windowTitle;
  sf::Vector2u virtualSize;
  bool isFullscreen;

  sf::View gameView;
  sf::View uiView;
};

class Game {
public:
  Game();
  void run();

private:
  void processEvents();
  void update(sf::Time deltaTime);
  void render();

  void initBricks();
  void resetBallAndPaddle();
  bool isAllBricksCleared() const;

  void updateUIPositions();
  void updateMenuColors();
  void updateOptionColors();
  std::string keyToString(sf::Keyboard::Key key) const;

  GameWindow gameWindow;
  GameState currentState;

  float playAreaWidth = 250.f; // Sisi kiri untuk arena bermain
  float sidebarX = 250.f;      // Sisi kanan untuk HUD Sidebar

  sf::ConvexShape paddle;
  sf::CircleShape ball;
  sf::Vector2f ballVelocity;
  float baseSpeed = 130.f;
  float currentSpeed = 130.f;
  float maxSpeed = 350.f;
  float speedMultiplier = 1.0f;
  std::vector<Brick> bricks;

  int score = 0;
  float elapsedTime = 0.f;
  float brickSpawnTimer = 0.f;
  float paddleBounceTimer = 0.3f;

  // Controls
  sf::Keyboard::Key keyLeft;
  sf::Keyboard::Key keyRight;
  bool rebindingLeft;
  bool rebindingRight;

  // Options State
  std::vector<ResolutionOption> resolutions;
  size_t selectedResIndex;
  std::vector<PaddleColorOption> paddleColors;
  size_t selectedColorIndex;

  // Navigation
  int menuSelectedIndex;
  int optionSelectedIndex;

  // Font & BGM Audio
  sf::Font font;
  sf::Music bgmMusic;
  sf::SoundBuffer brickSoundBuffer;
  sf::Sound brickSound;
  void createBrickSoundBuffer();

  // Main Menu UI
  sf::Text titleText;
  sf::Text titleShadowText;
  sf::Text subtitleText;
  sf::Text playText;
  sf::Text optionText;
  sf::Text exitText;
  sf::RectangleShape menuBox;
  sf::ConvexShape menuPanel;    // Beveled sci-fi control panel
  sf::ConvexShape cursorPaddle; // Glowing mini-paddle selection indicator
  sf::Text techLabel0;
  sf::Text techLabel1;
  sf::Text techLabel2;
  sf::Text techLabel3;
  std::vector<Brick> menuDecorBricks; // Pulsing decorative bricks in a crescent
                                      // background pattern
  float menuAnimTimer =
      0.f; // Animation clock for retro grid scrolling & pulses

  // Options Menu UI
  sf::Text optTitleText;
  sf::Text resLabelText;
  sf::Text resValueText;
  sf::Text displayLabelText;
  sf::Text displayValueText;
  sf::Text bindLeftLabelText;
  sf::Text bindLeftValueText;
  sf::Text bindRightLabelText;
  sf::Text bindRightValueText;
  sf::Text padColorLabelText;
  sf::Text padColorValueText;
  sf::Text backText;
  sf::Text helpText;
  sf::RectangleShape optBox;

  // Gameplay Sidebar HUD UI
  sf::RectangleShape sidebarBox;
  sf::RectangleShape sidebarDivider;
  sf::Text hudTitleText;
  sf::Text hudTimeLabel;
  sf::Text hudTimeValue;
  sf::Text hudScoreLabel;
  sf::Text hudScoreValue;
  sf::Text hudSpeedLabel;
  sf::Text hudSpeedValue;

  // Retro LED HUD Elements
  sf::RectangleShape hudBackplateTime;
  sf::RectangleShape hudBackplateScore;
  sf::RectangleShape hudBackplateSpeed;
  sf::Text hudTimeBackValue;  // displays dim "88:88"
  sf::Text hudScoreBackValue; // displays dim "888888"
  sf::Text hudSpeedBackValue; // displays dim "8.8x"

  // Ball Energy Trail
  struct BallTrail {
    sf::Vector2f position;
    float lifetime = 0.f;
    float maxLifetime = 0.25f;
    sf::Color color;
  };
  std::vector<BallTrail> ballTrails;

  // Stage Clear & Game Over Overlay UI
  sf::Text stageClearText;
  sf::Text stageClearSubText;
  sf::Text gameOverText;
  sf::Text gameOverSubText;
  sf::Text continueText;
  sf::ConvexShape overlayPanel;

  // Rain Effect Elements
  struct RainDrop {
    sf::Vector2f position;
    float speed;
    float length;
    float slant;
    sf::Color color;
  };
  std::vector<RainDrop> rainDrops;
  void initRain();
  void updateRain(sf::Time deltaTime);
  void drawRain();

  // Particle System Elements
  struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime = 0.f;
    float maxLifetime = 0.5f;
  };
  std::vector<Particle> particles;
  void spawnParticles(sf::Vector2f position, sf::Color color);
  void updateParticles(float dt);
  void drawParticles();
};

#endif