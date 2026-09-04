// Game Version 1.0.0 Beta
// Game By : Esmoocca
#include "../include/GameLogic.h"
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

static sf::ConvexShape createRoundedRect(float width, float height,
                                         float radius,
                                         unsigned int pointsPerCorner = 4) {
  sf::ConvexShape shape;
  if (radius > width / 2.f)
    radius = width / 2.f;
  if (radius > height / 2.f)
    radius = height / 2.f;

  unsigned int pointCount = pointsPerCorner * 4;
  shape.setPointCount(pointCount);

  const float pi = 3.14159265f;
  unsigned int index = 0;

  for (unsigned int i = 0; i < pointsPerCorner; ++i) {
    float angle = -pi / 2.f +
                  (pi / 2.f) * (static_cast<float>(i) / (pointsPerCorner - 1));
    float x = width - radius + radius * std::cos(angle);
    float y = radius + radius * std::sin(angle);
    shape.setPoint(index++, {x, y});
  }

  // Bottom-right corner
  for (unsigned int i = 0; i < pointsPerCorner; ++i) {
    float angle = (pi / 2.f) * (static_cast<float>(i) / (pointsPerCorner - 1));
    float x = width - radius + radius * std::cos(angle);
    float y = height - radius + radius * std::sin(angle);
    shape.setPoint(index++, {x, y});
  }

  // Bottom-left corner
  for (unsigned int i = 0; i < pointsPerCorner; ++i) {
    float angle =
        pi / 2.f + (pi / 2.f) * (static_cast<float>(i) / (pointsPerCorner - 1));
    float x = radius + radius * std::cos(angle);
    float y = height - radius + radius * std::sin(angle);
    shape.setPoint(index++, {x, y});
  }

  // Top-left corner
  for (unsigned int i = 0; i < pointsPerCorner; ++i) {
    float angle =
        pi + (pi / 2.f) * (static_cast<float>(i) / (pointsPerCorner - 1));
    float x = radius + radius * std::cos(angle);
    float y = radius + radius * std::sin(angle);
    shape.setPoint(index++, {x, y});
  }

  return shape;
}

static sf::ConvexShape createBeveledRect(float width, float height,
                                         float bevel) {
  sf::ConvexShape shape;
  if (bevel > width / 2.f)
    bevel = width / 2.f;
  if (bevel > height / 2.f)
    bevel = height / 2.f;

  shape.setPointCount(8);
  shape.setPoint(0, {bevel, 0.f});
  shape.setPoint(1, {width - bevel, 0.f});
  shape.setPoint(2, {width, bevel});
  shape.setPoint(3, {width, height - bevel});
  shape.setPoint(4, {width - bevel, height});
  shape.setPoint(5, {bevel, height});
  shape.setPoint(6, {0.f, height - bevel});
  shape.setPoint(7, {0.f, bevel});

  return shape;
}

Game::Game()
    : gameWindow("Ricochet Arcade HD", 320, 240), currentState(GameState::Menu),
      keyLeft(sf::Keyboard::Key::A), keyRight(sf::Keyboard::Key::D),
      rebindingLeft(false), rebindingRight(false), selectedResIndex(2),
      menuSelectedIndex(0), optionSelectedIndex(0), titleText(font),
      titleShadowText(font), subtitleText(font), playText(font),
      optionText(font), exitText(font), techLabel0(font), techLabel1(font),
      techLabel2(font), techLabel3(font), optTitleText(font),
      resLabelText(font), resValueText(font), displayLabelText(font),
      displayValueText(font), bindLeftLabelText(font), bindLeftValueText(font),
      bindRightLabelText(font), bindRightValueText(font),
      padColorLabelText(font), padColorValueText(font), backText(font),
      helpText(font), hudTitleText(font), hudTimeLabel(font),
      hudTimeValue(font), hudScoreLabel(font), hudScoreValue(font),
      hudSpeedLabel(font), hudSpeedValue(font), hudTimeBackValue(font),
      hudScoreBackValue(font), hudSpeedBackValue(font), stageClearText(font),
      stageClearSubText(font), gameOverText(font), gameOverSubText(font),
      continueText(font), brickSound(brickSoundBuffer) {

  if (!font.openFromFile("assets/fonts/pixel.ttf")) {
    if (!font.openFromFile("C:\\Windows\\Fonts\\consola.ttf")) {
      (void)font.openFromFile("C:\\Windows\\Fonts\\arial.ttf");
    }
  }

  menuPanel.setFillColor(sf::Color(10, 15, 28, 220));
  menuPanel.setOutlineColor(sf::Color(0, 200, 255));
  menuPanel.setOutlineThickness(2.f);

  // Initialize mini cursor paddle shape
  cursorPaddle = createBeveledRect(16.f, 5.f, 1.5f);
  cursorPaddle.setOrigin({8.f, 2.5f});
  cursorPaddle.setFillColor(sf::Color(255, 140, 0)); // bright sci-fi orange
  cursorPaddle.setOutlineColor(sf::Color(255, 220, 0));
  cursorPaddle.setOutlineThickness(1.f);

  // Initialize decorative crescent bricks for main menu background
  menuDecorBricks.clear();
  float centerX = 320.f / 2.f; // Virtual game view center X
  float centerY = 240.f / 2.f + 10.f;
  float radiusX = 110.f;
  float radiusY = 85.f;
  const int brickCount = 18;
  for (int i = 0; i < brickCount; ++i) {
    float angleDeg = 140.f + (260.f * i / (brickCount - 1));
    float angleRad = angleDeg * 3.14159265f / 180.f;
    float bx = centerX + radiusX * std::cos(angleRad);
    float by = centerY - radiusY * std::sin(angleRad);

    Brick b;
    b.shape = createRoundedRect(14.f, 5.f, 1.2f);
    b.shape.setOrigin({7.f, 2.5f});
    b.targetPos = {bx, by};
    b.shape.setPosition(b.targetPos);

    // Alternating glowing sci-fi colors: cyan, orange, gold, purple
    sf::Color c;
    if (i % 4 == 0)
      c = sf::Color(0, 220, 255); // Cyan
    else if (i % 4 == 1)
      c = sf::Color(255, 120, 0); // Orange
    else if (i % 4 == 2)
      c = sf::Color(255, 215, 0); // Gold
    else
      c = sf::Color(200, 50, 255); // Purple

    b.shape.setFillColor(c);
    menuDecorBricks.push_back(b);
  }

  if (bgmMusic.openFromFile("assets/audio/bgm.ogg")) {
    bgmMusic.setLooping(true);
    bgmMusic.setVolume(50.f);
    bgmMusic.play();
  } else {
    std::cout << "[INFO] File BGM (assets/audio/bgm.ogg) tidak ditemukan."
              << std::endl;
  }

  resolutions = {{"1920 x 1080", 1920, 1080},
                 {"1280 x 720", 1280, 720},
                 {"854 x 480", 854, 480}};

  // Customizable Paddle Colors options list
  paddleColors = {{"BLUE", sf::Color(0, 180, 255)},
                  {"ORANGE", sf::Color(255, 110, 0)},
                  {"RED", sf::Color(255, 60, 60)},
                  {"PURPLE", sf::Color(200, 60, 255)},
                  {"WHITE", sf::Color(255, 255, 255)}};
  selectedColorIndex = 0; // Default Blue

  // Rounded-rectangle paddle geometry matching the bricks' curved sides
  paddle = createRoundedRect(36.f, 6.f, 2.0f);
  paddle.setOrigin({18.f, 3.f});
  paddle.setFillColor(paddleColors[selectedColorIndex].color);
  paddle.setOutlineColor(sf::Color(0, 255, 255)); // Glowing cyan energy border
  paddle.setOutlineThickness(1.f);

  ball.setRadius(3.5f);
  ball.setFillColor(sf::Color(255, 255, 200));       // Ion sphere core
  ball.setOutlineColor(sf::Color(0, 220, 255, 200)); // Faint glow halo
  ball.setOutlineThickness(1.f);

  // Initialize retro dim LED texts for back segment illusion
  hudTimeBackValue.setString("88:88");
  hudScoreBackValue.setString("888888");
  hudSpeedBackValue.setString("8.8x");

  sf::Color dimLEDColor(10, 50, 70, 70); // Faint cyber cyan segment backdrop
  hudTimeBackValue.setFillColor(dimLEDColor);
  hudScoreBackValue.setFillColor(dimLEDColor);
  hudSpeedBackValue.setFillColor(dimLEDColor);

  resetBallAndPaddle();
  initBricks();
  updateUIPositions();

  // Seed and initialize rain
  srand(static_cast<unsigned int>(time(nullptr)));
  initRain();

  // Create retro brick sound
  createBrickSoundBuffer();
  brickSound.setBuffer(brickSoundBuffer);
  brickSound.setVolume(40.f);
}

void Game::updateUIPositions() {
  sf::Vector2u wSize = gameWindow.getWindowSize();
  float w = static_cast<float>(wSize.x);
  float h = static_cast<float>(wSize.y);

  // --- MAIN MENU ---
  titleText.setString("RICOCHET");
  titleText.setCharacterSize(static_cast<unsigned int>(h * 0.085f));
  titleText.setFillColor(sf::Color(255, 215, 0));
  sf::FloatRect tBounds = titleText.getLocalBounds();
  titleText.setPosition({(w - tBounds.size.x) / 2.f, h * 0.12f});

  // Layered glowing shadow
  titleShadowText.setString("RICOCHET");
  titleShadowText.setCharacterSize(static_cast<unsigned int>(h * 0.085f));
  titleShadowText.setFillColor(
      sf::Color(0, 160, 255, 130)); // glowing neon blue shadow
  titleShadowText.setPosition({titleText.getPosition().x + 2.f * (w / 854.f),
                               titleText.getPosition().y + 2.f * (h / 480.f)});

  // Game Subtitle
  subtitleText.setString("ARCADE BLOCK SIMULATOR");
  subtitleText.setCharacterSize(static_cast<unsigned int>(h * 0.024f));
  subtitleText.setFillColor(sf::Color(0, 255, 180, 200));
  sf::FloatRect subBounds = subtitleText.getLocalBounds();
  subtitleText.setPosition({(w - subBounds.size.x) / 2.f, h * 0.23f});

  // Sci-fi Beveled octagonal Panel
  menuPanel = createBeveledRect(w * 0.38f, h * 0.45f, 15.f);
  menuPanel.setFillColor(sf::Color(10, 15, 28, 220));
  menuPanel.setOutlineColor(sf::Color(0, 200, 255));
  menuPanel.setOutlineThickness(2.f);
  menuPanel.setPosition({(w - w * 0.38f) / 2.f, h * 0.32f});

  menuBox.setSize({w * 0.38f, h * 0.45f});
  menuBox.setPosition({(w - menuBox.getSize().x) / 2.f, h * 0.32f});

  // Four corner tech diagnostics labels
  unsigned int techFontSize = static_cast<unsigned int>(h * 0.022f);
  sf::Color techColor(80, 120, 160, 140);

  // techLabel0.setString("SYS_STATUS: ACTIVE");
  // techLabel0.setCharacterSize(techFontSize);
  // techLabel0.setFillColor(techColor);
  // techLabel0.setPosition({w * 0.02f, h * 0.02f});

  // techLabel1.setString("GRID_SHIELD: 100%");
  // techLabel1.setCharacterSize(techFontSize);
  // techLabel1.setFillColor(techColor);
  // sf::FloatRect tl1B = techLabel1.getLocalBounds();
  // techLabel1.setPosition({w - w * 0.02f - tl1B.size.x, h * 0.02f});

  techLabel2.setString("https://trakteer.id/esmoocca");
  techLabel2.setCharacterSize(techFontSize);
  techLabel2.setFillColor(techColor);
  techLabel2.setPosition({w * 0.02f, h - h * 0.045f});

  techLabel3.setString("GAME VERSION: V1.0.0");
  techLabel3.setCharacterSize(techFontSize);
  techLabel3.setFillColor(techColor);
  sf::FloatRect tl3B = techLabel3.getLocalBounds();
  techLabel3.setPosition({w - w * 0.02f - tl3B.size.x, h - h * 0.045f});

  unsigned int menuFontSize = static_cast<unsigned int>(h * 0.038f);
  playText.setCharacterSize(menuFontSize);
  optionText.setCharacterSize(menuFontSize);
  exitText.setCharacterSize(menuFontSize);
  updateMenuColors();

  // --- OPTIONS MENU ---
  optTitleText.setString("- OPTIONS -");
  optTitleText.setCharacterSize(static_cast<unsigned int>(h * 0.055f));
  optTitleText.setFillColor(sf::Color(0, 220, 255));
  sf::FloatRect optBounds = optTitleText.getLocalBounds();
  optTitleText.setPosition({(w - optBounds.size.x) / 2.f, h * 0.08f});

  optBox.setSize({w * 0.65f, h * 0.68f});
  optBox.setFillColor(sf::Color(15, 20, 35, 235));
  optBox.setOutlineColor(sf::Color(0, 200, 255));
  optBox.setOutlineThickness(2.f);
  optBox.setPosition({(w - optBox.getSize().x) / 2.f, h * 0.18f});

  float boxX = optBox.getPosition().x;
  float boxY = optBox.getPosition().y;
  float boxW = optBox.getSize().x;
  unsigned int optFontSize = static_cast<unsigned int>(h * 0.032f);

  resLabelText.setString("Resolution:");
  resLabelText.setCharacterSize(optFontSize);
  resLabelText.setPosition({boxX + boxW * 0.08f, boxY + h * 0.08f});
  resValueText.setCharacterSize(optFontSize);
  resValueText.setPosition({boxX + boxW * 0.55f, boxY + h * 0.08f});

  displayLabelText.setString("Display Mode:");
  displayLabelText.setCharacterSize(optFontSize);
  displayLabelText.setPosition({boxX + boxW * 0.08f, boxY + h * 0.18f});
  displayValueText.setCharacterSize(optFontSize);
  displayValueText.setPosition({boxX + boxW * 0.55f, boxY + h * 0.18f});

  bindLeftLabelText.setString("Move Left:");
  bindLeftLabelText.setCharacterSize(optFontSize);
  bindLeftLabelText.setPosition({boxX + boxW * 0.08f, boxY + h * 0.28f});
  bindLeftValueText.setCharacterSize(optFontSize);
  bindLeftValueText.setPosition({boxX + boxW * 0.55f, boxY + h * 0.28f});

  bindRightLabelText.setString("Move Right:");
  bindRightLabelText.setCharacterSize(optFontSize);
  bindRightLabelText.setPosition({boxX + boxW * 0.08f, boxY + h * 0.38f});
  bindRightValueText.setCharacterSize(optFontSize);
  bindRightValueText.setPosition({boxX + boxW * 0.55f, boxY + h * 0.38f});

  padColorLabelText.setString("Paddle Color:");
  padColorLabelText.setCharacterSize(optFontSize);
  padColorLabelText.setPosition({boxX + boxW * 0.08f, boxY + h * 0.48f});
  padColorValueText.setCharacterSize(optFontSize);
  padColorValueText.setPosition({boxX + boxW * 0.55f, boxY + h * 0.48f});

  backText.setCharacterSize(static_cast<unsigned int>(h * 0.036f));

  helpText.setString("[Navigasi: Panah | Pilih: Enter / Space]");
  helpText.setCharacterSize(static_cast<unsigned int>(h * 0.024f));
  helpText.setFillColor(sf::Color(180, 180, 180));
  sf::FloatRect helpB = helpText.getLocalBounds();
  helpText.setPosition({(w - helpB.size.x) / 2.f, h * 0.90f});
  updateOptionColors();

  // --- HUD SIDEBAR ---
  sidebarBox.setSize({w * 0.22f, h});
  sidebarBox.setPosition({w * 0.78f, 0.f});
  sidebarBox.setFillColor(
      sf::Color(8, 10, 16, 170)); // transparent metallic panel

  sidebarDivider.setSize({4.f, h});
  sidebarDivider.setPosition({w * 0.78f - 4.f, 0.f});
  sidebarDivider.setFillColor(
      sf::Color(0, 180, 255, 180)); // transparent cyan light divider

  float sbX = w * 0.80f;
  unsigned int hudHeaderSize = static_cast<unsigned int>(h * 0.028f);
  unsigned int hudValueSize = static_cast<unsigned int>(h * 0.035f);

  // Logo text in Top Panel (now displaying game title "RICOCHET")
  hudTitleText.setString("RICOCHET");
  hudTitleText.setCharacterSize(static_cast<unsigned int>(h * 0.038f));
  hudTitleText.setFillColor(sf::Color(255, 220, 0)); // Golden yellow
  sf::FloatRect htB = hudTitleText.getLocalBounds();
  hudTitleText.setPosition(
      {w * 0.78f + (w * 0.22f - htB.size.x) / 2.f, h * 0.10f});

  // Time Label & Value Slots (Middle Panel: h * 0.25f to h * 0.75f)
  hudTimeLabel.setString("TIME");
  hudTimeLabel.setCharacterSize(hudHeaderSize);
  hudTimeLabel.setFillColor(sf::Color(255, 220, 0)); // Reflexive Gold Label
  hudTimeLabel.setPosition({sbX, h * 0.27f});

  // Score Label & Value Slots
  hudScoreLabel.setString("SCORE");
  hudScoreLabel.setCharacterSize(hudHeaderSize);
  hudScoreLabel.setFillColor(sf::Color(255, 220, 0)); // Reflexive Gold Label
  hudScoreLabel.setPosition({sbX, h * 0.43f});

  // Speed Label & Value Slots
  hudSpeedLabel.setString("SPEED");
  hudSpeedLabel.setCharacterSize(hudHeaderSize);
  hudSpeedLabel.setFillColor(sf::Color(255, 220, 0)); // Reflexive Gold Label
  hudSpeedLabel.setPosition({sbX, h * 0.59f});

  // Backplates sizes and offsets
  float bpW = w * 0.18f;
  float bpH = hudValueSize * 1.5f;
  float bpX = w * 0.80f;
  sf::Color bpFill(8, 12, 24); // Dark blue LED pocket
  sf::Color bpOutline(0, 180, 255, 100);

  hudBackplateTime.setSize({bpW, bpH});
  hudBackplateTime.setFillColor(bpFill);
  hudBackplateTime.setOutlineColor(bpOutline);
  hudBackplateTime.setOutlineThickness(1.5f);
  hudBackplateTime.setPosition({bpX, h * 0.32f});

  hudBackplateScore.setSize({bpW, bpH});
  hudBackplateScore.setFillColor(bpFill);
  hudBackplateScore.setOutlineColor(bpOutline);
  hudBackplateScore.setOutlineThickness(1.5f);
  hudBackplateScore.setPosition({bpX, h * 0.48f});

  hudBackplateSpeed.setSize({bpW, bpH});
  hudBackplateSpeed.setFillColor(bpFill);
  hudBackplateSpeed.setOutlineColor(bpOutline);
  hudBackplateSpeed.setOutlineThickness(1.5f);
  hudBackplateSpeed.setPosition({bpX, h * 0.64f});

  // Character Sizes and Colors
  hudTimeValue.setCharacterSize(hudValueSize);
  hudTimeValue.setFillColor(sf::Color(0, 240, 255)); // Bright neon blue values
  hudTimeBackValue.setCharacterSize(hudValueSize);

  hudScoreValue.setCharacterSize(hudValueSize);
  hudScoreValue.setFillColor(sf::Color(0, 240, 255)); // Bright neon blue values
  hudScoreBackValue.setCharacterSize(hudValueSize);

  hudSpeedValue.setCharacterSize(hudValueSize);
  hudSpeedValue.setFillColor(sf::Color(0, 240, 255)); // Bright neon blue values
  hudSpeedBackValue.setCharacterSize(hudValueSize);

  // Position overlaid texts inside backplate slots
  float txOffset = 10.f * (w / 854.f);
  float tyOffset = 3.f * (h / 480.f);

  hudTimeBackValue.setPosition({bpX + txOffset, h * 0.32f + tyOffset});
  hudTimeValue.setPosition({bpX + txOffset, h * 0.32f + tyOffset});

  hudScoreBackValue.setPosition({bpX + txOffset, h * 0.48f + tyOffset});
  hudScoreValue.setPosition({bpX + txOffset, h * 0.48f + tyOffset});

  hudSpeedBackValue.setPosition({bpX + txOffset, h * 0.64f + tyOffset});
  hudSpeedValue.setPosition({bpX + txOffset, h * 0.64f + tyOffset});

  overlayPanel = createBeveledRect(w * 0.55f, h * 0.40f, 15.f);
  overlayPanel.setFillColor(sf::Color(10, 15, 28, 230));
  overlayPanel.setPosition({(w - w * 0.55f) / 2.f, h * 0.28f});

  stageClearText.setString("STAGE CLEAR!");
  stageClearText.setCharacterSize(static_cast<unsigned int>(h * 0.075f));
  stageClearText.setFillColor(sf::Color(0, 255, 140)); // Sci-fi neon green
  sf::FloatRect scB = stageClearText.getLocalBounds();
  stageClearText.setPosition({(w - scB.size.x) / 2.f, h * 0.33f});

  stageClearSubText.setString("MISSION ACCOMPLISHED");
  stageClearSubText.setCharacterSize(static_cast<unsigned int>(h * 0.026f));
  stageClearSubText.setFillColor(sf::Color(0, 220, 255)); // Neon cyan subtitle
  sf::FloatRect scsB = stageClearSubText.getLocalBounds();
  stageClearSubText.setPosition({(w - scsB.size.x) / 2.f, h * 0.44f});

  // --- GAME OVER OVERLAY UI ---
  gameOverText.setString("GAME OVER");
  gameOverText.setCharacterSize(static_cast<unsigned int>(h * 0.08f));
  gameOverText.setFillColor(sf::Color(255, 50, 60)); // Crimson danger red
  sf::FloatRect goB = gameOverText.getLocalBounds();
  gameOverText.setPosition({(w - goB.size.x) / 2.f, h * 0.33f});

  gameOverSubText.setString("MISSION FAILED");
  gameOverSubText.setCharacterSize(static_cast<unsigned int>(h * 0.026f));
  gameOverSubText.setFillColor(sf::Color(255, 140, 0)); // Warning orange
  sf::FloatRect gosB = gameOverSubText.getLocalBounds();
  gameOverSubText.setPosition({(w - gosB.size.x) / 2.f, h * 0.44f});

  // --- COMMON CONTINUE PROMPT ---
  continueText.setString("Tekan ENTER / SPACE untuk ke Menu");
  continueText.setCharacterSize(static_cast<unsigned int>(h * 0.030f));
  continueText.setFillColor(sf::Color(255, 235, 100)); // Glowing yellow prompt
  sf::FloatRect ctB = continueText.getLocalBounds();
  continueText.setPosition({(w - ctB.size.x) / 2.f, h * 0.56f});
}

void Game::initBricks() {
  bricks.clear();

  const int heartPattern[8][9] = {
      {0, 1, 1, 0, 0, 0, 1, 1, 0}, {1, 2, 2, 1, 0, 1, 2, 2, 1},
      {1, 2, 2, 2, 1, 2, 2, 2, 1}, {1, 2, 2, 2, 2, 2, 2, 2, 1},
      {0, 1, 2, 2, 2, 2, 2, 1, 0}, {0, 0, 1, 2, 2, 2, 1, 0, 0},
      {0, 0, 0, 1, 2, 1, 0, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0, 0}};

  float brickWidth = 22.f;
  float brickHeight = 8.f;
  float spacingX = 2.f;
  float spacingY = 2.f;

  float totalWidth = 9 * (brickWidth + spacingX) - spacingX;
  float startX = (playAreaWidth - totalWidth) / 2.f;
  float startY = 20.f;

  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 9; ++col) {
      int cell = heartPattern[row][col];
      if (cell > 0) {
        Brick b;
        b.shape = createRoundedRect(brickWidth, brickHeight, 2.f);
        b.shape.setOrigin({brickWidth / 2.f, brickHeight / 2.f});
        b.targetPos = {
            startX + col * (brickWidth + spacingX) + brickWidth / 2.f,
            startY + row * (brickHeight + spacingY) + brickHeight / 2.f};
        b.shape.setPosition(b.targetPos);

        if (cell == 1) {
          b.shape.setFillColor(sf::Color(255, 40, 80));
          b.points = 150;
        } else {
          b.shape.setFillColor(sf::Color(255, 105, 180));
          b.points = 100;
        }
        bricks.push_back(b);
      }
    }
  }
}

void Game::resetBallAndPaddle() {
  paddle.setPosition({playAreaWidth / 2.f, 223.f});
  ball.setPosition({playAreaWidth / 2.f - 3.5f, 208.f});

  currentSpeed = baseSpeed;
  speedMultiplier = 1.0f;
  ballVelocity = sf::Vector2f(80.f, -currentSpeed);

  score = 0;
  elapsedTime = 0.f;
  brickSpawnTimer = 0.f;
  paddleBounceTimer = 0.3f;
  particles.clear();
  ballTrails.clear();
}

std::string Game::keyToString(sf::Keyboard::Key key) const {
  if (key >= sf::Keyboard::Key::A && key <= sf::Keyboard::Key::Z) {
    return std::string(
        1, static_cast<char>('A' + static_cast<int>(key) -
                             static_cast<int>(sf::Keyboard::Key::A)));
  }
  if (key == sf::Keyboard::Key::Left)
    return "LEFT ARROW";
  if (key == sf::Keyboard::Key::Right)
    return "RIGHT ARROW";
  if (key == sf::Keyboard::Key::Space)
    return "SPACE";
  return "KEY " + std::to_string(static_cast<int>(key));
}

void Game::updateMenuColors() {
  sf::Vector2u wSize = gameWindow.getWindowSize();
  float w = static_cast<float>(wSize.x);
  float boxY = menuBox.getPosition().y;
  float boxH = menuBox.getSize().y;

  playText.setString("PLAY");
  playText.setFillColor(menuSelectedIndex == 0 ? sf::Color::Yellow
                                               : sf::Color::White);
  sf::FloatRect pB = playText.getLocalBounds();
  playText.setPosition({(w - pB.size.x) / 2.f, boxY + boxH * 0.18f});

  optionText.setString("OPTIONS");
  optionText.setFillColor(menuSelectedIndex == 1 ? sf::Color::Yellow
                                                 : sf::Color::White);
  sf::FloatRect oB = optionText.getLocalBounds();
  optionText.setPosition({(w - oB.size.x) / 2.f, boxY + boxH * 0.45f});

  exitText.setString("EXIT");
  exitText.setFillColor(menuSelectedIndex == 2 ? sf::Color::Yellow
                                               : sf::Color::White);
  sf::FloatRect eB = exitText.getLocalBounds();
  exitText.setPosition({(w - eB.size.x) / 2.f, boxY + boxH * 0.72f});
}

void Game::updateOptionColors() {
  sf::Color normColor(200, 200, 200);
  sf::Color activeColor = sf::Color::Yellow;

  resLabelText.setFillColor(optionSelectedIndex == 0 ? activeColor : normColor);
  resValueText.setFillColor(optionSelectedIndex == 0 ? activeColor : normColor);
  resValueText.setString((optionSelectedIndex == 0 ? "< " : "  ") +
                         resolutions[selectedResIndex].label +
                         (optionSelectedIndex == 0 ? " >" : "  "));

  displayLabelText.setFillColor(optionSelectedIndex == 1 ? activeColor
                                                         : normColor);
  displayValueText.setFillColor(optionSelectedIndex == 1 ? activeColor
                                                         : normColor);
  std::string dispStr =
      gameWindow.getIsFullscreen() ? "Fullscreen" : "Windowed";
  displayValueText.setString((optionSelectedIndex == 1 ? "< " : "  ") +
                             dispStr +
                             (optionSelectedIndex == 1 ? " >" : "  "));

  bindLeftLabelText.setFillColor(optionSelectedIndex == 2 ? activeColor
                                                          : normColor);
  bindLeftValueText.setFillColor(optionSelectedIndex == 2 ? activeColor
                                                          : normColor);
  bindLeftValueText.setString(rebindingLeft
                                  ? "[ Press Key... ]"
                                  : (optionSelectedIndex == 2
                                         ? "< " + keyToString(keyLeft) + " >"
                                         : keyToString(keyLeft)));

  bindRightLabelText.setFillColor(optionSelectedIndex == 3 ? activeColor
                                                           : normColor);
  bindRightValueText.setFillColor(optionSelectedIndex == 3 ? activeColor
                                                           : normColor);
  bindRightValueText.setString(rebindingRight
                                   ? "[ Press Key... ]"
                                   : (optionSelectedIndex == 3
                                          ? "< " + keyToString(keyRight) + " >"
                                          : keyToString(keyRight)));

  // Paddle Color Option
  padColorLabelText.setFillColor(optionSelectedIndex == 4 ? activeColor
                                                          : normColor);
  padColorValueText.setFillColor(optionSelectedIndex == 4 ? activeColor
                                                          : normColor);
  padColorValueText.setString((optionSelectedIndex == 4 ? "< " : "  ") +
                              paddleColors[selectedColorIndex].label +
                              (optionSelectedIndex == 4 ? " >" : "  "));

  backText.setFillColor(optionSelectedIndex == 5 ? activeColor : normColor);
  backText.setString(optionSelectedIndex == 5 ? "> BACK TO MENU <"
                                              : "BACK TO MENU");

  sf::Vector2u wSize = gameWindow.getWindowSize();
  float w = static_cast<float>(wSize.x);
  float boxY = optBox.getPosition().y;
  float boxH = optBox.getSize().y;

  sf::FloatRect bB = backText.getLocalBounds();
  backText.setPosition({(w - bB.size.x) / 2.f, boxY + boxH * 0.85f});
}

bool Game::isAllBricksCleared() const {
  for (const auto &brick : bricks) {
    if (!brick.isDestroyed)
      return false;
  }
  return true;
}

void Game::run() {
  sf::Clock clock;
  while (gameWindow.isOpen()) {
    sf::Time deltaTime = clock.restart();
    processEvents();
    update(deltaTime);
    render();
  }
}

void Game::processEvents() {
  while (const auto event = gameWindow.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      gameWindow.getRenderWindow().close();
    }

    if (const auto *resized = event->getIf<sf::Event::Resized>()) {
      gameWindow.handleResize(resized->size.x, resized->size.y);
      updateUIPositions();
    }

    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {

      if (currentState == GameState::Options &&
          (rebindingLeft || rebindingRight)) {
        if (rebindingLeft) {
          keyLeft = keyPressed->code;
          rebindingLeft = false;
        } else if (rebindingRight) {
          keyRight = keyPressed->code;
          rebindingRight = false;
        }
        updateOptionColors();
        continue;
      }

      if (currentState == GameState::Menu) {
        if (keyPressed->code == sf::Keyboard::Key::Up) {
          menuSelectedIndex = (menuSelectedIndex - 1 + 3) % 3;
          brickSound.setPitch(1.3f); // UI shift sound pitch
          brickSound.play();
          updateMenuColors();
        }
        if (keyPressed->code == sf::Keyboard::Key::Down) {
          menuSelectedIndex = (menuSelectedIndex + 1) % 3;
          brickSound.setPitch(1.3f);
          brickSound.play();
          updateMenuColors();
        }
        if (keyPressed->code == sf::Keyboard::Key::Enter ||
            keyPressed->code == sf::Keyboard::Key::Space) {
          brickSound.setPitch(0.9f); // Select confirm pitch
          brickSound.play();
          if (menuSelectedIndex == 0) {
            resetBallAndPaddle();
            initBricks();
            currentState = GameState::Playing;
          } else if (menuSelectedIndex == 1) {
            currentState = GameState::Options;
          } else if (menuSelectedIndex == 2) {
            gameWindow.getRenderWindow().close();
          }
        }
      } else if (currentState == GameState::Options) {
        if (keyPressed->code == sf::Keyboard::Key::Up) {
          optionSelectedIndex = (optionSelectedIndex - 1 + 6) % 6;
          brickSound.setPitch(1.3f);
          brickSound.play();
          updateOptionColors();
        }
        if (keyPressed->code == sf::Keyboard::Key::Down) {
          optionSelectedIndex = (optionSelectedIndex + 1) % 6;
          brickSound.setPitch(1.3f);
          brickSound.play();
          updateOptionColors();
        }

        if (keyPressed->code == sf::Keyboard::Key::Left) {
          brickSound.setPitch(1.1f);
          brickSound.play();
          if (optionSelectedIndex == 0) {
            selectedResIndex = (selectedResIndex - 1 + resolutions.size()) %
                               resolutions.size();
            gameWindow.setResolution(resolutions[selectedResIndex].width,
                                     resolutions[selectedResIndex].height,
                                     gameWindow.getIsFullscreen());
            updateUIPositions();
          } else if (optionSelectedIndex == 1) {
            gameWindow.toggleFullscreen();
            updateUIPositions();
          } else if (optionSelectedIndex == 4) {
            selectedColorIndex =
                (selectedColorIndex - 1 + paddleColors.size()) %
                paddleColors.size();
            paddle.setFillColor(paddleColors[selectedColorIndex].color);
          }
          updateOptionColors();
        }
        if (keyPressed->code == sf::Keyboard::Key::Right) {
          brickSound.setPitch(1.1f);
          brickSound.play();
          if (optionSelectedIndex == 0) {
            selectedResIndex = (selectedResIndex + 1) % resolutions.size();
            gameWindow.setResolution(resolutions[selectedResIndex].width,
                                     resolutions[selectedResIndex].height,
                                     gameWindow.getIsFullscreen());
            updateUIPositions();
          } else if (optionSelectedIndex == 1) {
            gameWindow.toggleFullscreen();
            updateUIPositions();
          } else if (optionSelectedIndex == 4) {
            selectedColorIndex = (selectedColorIndex + 1) % paddleColors.size();
            paddle.setFillColor(paddleColors[selectedColorIndex].color);
          }
          updateOptionColors();
        }

        if (keyPressed->code == sf::Keyboard::Key::Enter ||
            keyPressed->code == sf::Keyboard::Key::Space) {
          brickSound.setPitch(0.9f);
          brickSound.play();
          if (optionSelectedIndex == 2)
            rebindingLeft = true;
          if (optionSelectedIndex == 3)
            rebindingRight = true;
          if (optionSelectedIndex == 5)
            currentState = GameState::Menu;
          updateOptionColors();
        }

        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          brickSound.setPitch(0.6f);
          brickSound.play();
          currentState = GameState::Menu;
        }
      } else if (currentState == GameState::Playing) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          brickSound.setPitch(0.6f);
          brickSound.play();
          currentState = GameState::Menu;
        }
      } else if (currentState == GameState::StageClear ||
                 currentState == GameState::GameOver) {
        if (keyPressed->code == sf::Keyboard::Key::Enter ||
            keyPressed->code == sf::Keyboard::Key::Space ||
            keyPressed->code == sf::Keyboard::Key::Escape) {
          brickSound.setPitch(0.9f);
          brickSound.play();
          currentState = GameState::Menu;
        }
      }
    }
  }
}

void Game::update(sf::Time deltaTime) {
  updateRain(deltaTime);
  updateParticles(deltaTime.asSeconds());

  menuAnimTimer += deltaTime.asSeconds();

  if (currentState == GameState::Menu) {

    // Pulse the alpha of the title text glowing shadow!
    float shadowPulse = (std::sin(menuAnimTimer * 3.f) + 1.f) * 0.5f; // 0 to 1
    std::uint8_t shadowAlpha =
        static_cast<std::uint8_t>(100 + shadowPulse * 100);
    titleShadowText.setFillColor(sf::Color(0, 160, 255, shadowAlpha));

    // Slowly pulse scale of decorative bricks in a wave pattern
    for (size_t i = 0; i < menuDecorBricks.size(); ++i) {
      auto &b = menuDecorBricks[i];
      float pulse = std::sin(menuAnimTimer * 2.5f + i * 0.3f);
      float scale = 0.9f + pulse * 0.15f;
      b.shape.setScale({scale, scale});

      // Shift position slightly for dynamic floating effect
      float visualOffset = std::cos(menuAnimTimer * 1.5f + i * 0.4f) * 1.5f;
      b.shape.setPosition({b.targetPos.x, b.targetPos.y + visualOffset});
    }
  }

  if (currentState != GameState::Playing)
    return;

  elapsedTime += deltaTime.asSeconds();

  // Pergerakan Paddle
  if (sf::Keyboard::isKeyPressed(keyLeft) && paddle.getPosition().x > 18.f) {
    paddle.move({-190.f * deltaTime.asSeconds(), 0.f});
  }
  if (sf::Keyboard::isKeyPressed(keyRight) &&
      paddle.getPosition().x < playAreaWidth - 18.f) {
    paddle.move({190.f * deltaTime.asSeconds(), 0.f});
  }

  // Update Paddle Bounce Animation Timer
  if (paddleBounceTimer < 0.3f) {
    paddleBounceTimer += deltaTime.asSeconds();
  }

  // Handle Brick Spawn Animation
  if (brickSpawnTimer < 0.7f) {
    brickSpawnTimer += deltaTime.asSeconds();

    float centerX = playAreaWidth / 2.f;
    float centerY = 20.f + 4.f * (8.f + 2.f); // center of heart pattern
    sf::Vector2f centerPos(centerX, centerY);

    for (auto &brick : bricks) {
      float dx = brick.targetPos.x - centerPos.x;
      float dy = brick.targetPos.y - centerPos.y;
      float distance = std::sqrt(dx * dx + dy * dy);

      // Delay based on distance from center (explosion effect outward)
      float delay = (distance / 120.f) * 0.2f;

      float t = brickSpawnTimer - delay;
      if (t < 0.f) {
        brick.shape.setScale({0.f, 0.f});
        brick.shape.setPosition(centerPos);
      } else {
        float animDuration = 0.3f;
        float progress = t / animDuration;
        if (progress > 1.f)
          progress = 1.f;

        // Easing: easeOutBack (overshoots and settles back to normal size)
        float x = progress - 1.f;
        float ease = 1.f + 2.70158f * x * x * x + 1.70158f * x * x;
        if (ease < 0.f)
          ease = 0.f;

        brick.shape.setScale({ease, ease});
        sf::Vector2f currentPos =
            centerPos + (brick.targetPos - centerPos) * progress;
        brick.shape.setPosition(currentPos);
      }
    }

    // Lock ball on paddle during brick spawn animation
    ball.setPosition({paddle.getPosition().x - 3.5f, 208.f});
  } else {
    // Make sure all bricks are perfectly placed and scaled after animation
    for (auto &brick : bricks) {
      brick.shape.setScale({1.f, 1.f});
      brick.shape.setPosition(brick.targetPos);
    }

    // Pergerakan Bola
    ball.move(ballVelocity * deltaTime.asSeconds());

    // Spawn energy trail particle at current ball center
    BallTrail trail;
    trail.position =
        ball.getPosition() + sf::Vector2f(ball.getRadius(), ball.getRadius());
    trail.lifetime = 0.f;
    trail.maxLifetime = 0.22f;
    trail.color = sf::Color(0, 220, 255, 180); // Cyan glow trail
    ballTrails.push_back(trail);

    // Update ball trail particles
    for (auto it = ballTrails.begin(); it != ballTrails.end();) {
      it->lifetime += deltaTime.asSeconds();
      if (it->lifetime >= it->maxLifetime) {
        it = ballTrails.erase(it);
      } else {
        float progress = it->lifetime / it->maxLifetime;
        it->color.a = static_cast<std::uint8_t>(180.f * (1.f - progress));
        ++it;
      }
    }

    // Pantulan Dinding Arena (Kiri / Kanan Arena Bermain)
    if (ball.getPosition().x <= 0 ||
        ball.getPosition().x >= playAreaWidth - ball.getRadius() * 2) {
      ballVelocity.x = -ballVelocity.x;
    }
    // Pantulan Atas
    if (ball.getPosition().y <= 0) {
      ballVelocity.y = -ballVelocity.y;
    }

    // Game Over jika jatuh ke bawah
    if (ball.getPosition().y >= 240) {
      currentState = GameState::GameOver;
      brickSound.setPitch(0.4f); // Low death sound
      brickSound.play();
    }

    // Incremental speed logic
    auto increaseSpeed = [&]() {
      if (currentSpeed < maxSpeed) {
        currentSpeed *= 1.025f;
        speedMultiplier = currentSpeed / baseSpeed;

        float currentMag = std::sqrt(ballVelocity.x * ballVelocity.x +
                                     ballVelocity.y * ballVelocity.y);
        ballVelocity = (ballVelocity / currentMag) * currentSpeed;
      }
    };

    // Pantulan Paddle
    if (ball.getGlobalBounds().findIntersection(paddle.getGlobalBounds())) {
      ballVelocity.y = -std::abs(ballVelocity.y);
      increaseSpeed();
      paddleBounceTimer = 0.f;   // Trigger paddle bounce animation!
      brickSound.setPitch(0.7f); // lower pitch on paddle bounce
      brickSound.play();
    }

    // Pantulan Bricks
    for (auto &brick : bricks) {
      if (!brick.isDestroyed && ball.getGlobalBounds().findIntersection(
                                    brick.shape.getGlobalBounds())) {
        brick.isDestroyed = true;
        ballVelocity.y = -ballVelocity.y;
        score += brick.points;

        brickSound.setPitch(1.0f); // normal pitch on brick hits
        brickSound.play();
        spawnParticles(brick.shape.getPosition(), brick.shape.getFillColor());

        increaseSpeed();

        if (isAllBricksCleared()) {
          currentState = GameState::StageClear;
          brickSound.setPitch(1.5f); // High victory sound
          brickSound.play();
        }
        break;
      }
    }
  }

  // Update String HUD Text
  int minutes = static_cast<int>(elapsedTime) / 60;
  int seconds = static_cast<int>(elapsedTime) % 60;
  std::stringstream timeSS;
  timeSS << std::setfill('0') << std::setw(2) << minutes << ":"
         << std::setfill('0') << std::setw(2) << seconds;
  hudTimeValue.setString(timeSS.str());

  std::stringstream scoreSS;
  scoreSS << std::setfill('0') << std::setw(6) << score;
  hudScoreValue.setString(scoreSS.str());

  std::stringstream speedSS;
  speedSS << std::fixed << std::setprecision(1) << speedMultiplier << "x";
  hudSpeedValue.setString(speedSS.str());
}

void Game::render() {
  gameWindow.clear();

  // 1. RENDER IN-GAME WORLD (Virtual 320x240)
  gameWindow.setGameView();

  // Scrolling cyber-grid in Menu, Options, Playing, StageClear, and GameOver
  // states
  if (currentState == GameState::Menu || currentState == GameState::Options ||
      currentState == GameState::Playing ||
      currentState == GameState::StageClear ||
      currentState == GameState::GameOver) {
    float gridScrollSpeed = 25.f;
    float gridSpacing = 20.f;
    float startY = std::fmod(menuAnimTimer * gridScrollSpeed, gridSpacing);
    sf::Vertex line[2];

    // Dim the grid lines in active gameplay/overlays to keep it non-distracting
    int alpha =
        (currentState == GameState::Menu || currentState == GameState::Options)
            ? 30
            : 14;

    // Draw horizontal grid lines
    for (float y = startY; y < 240.f; y += gridSpacing) {
      line[0].position = sf::Vector2f(0.f, y);
      line[0].color = sf::Color(0, 120, 255, alpha);
      line[1].position = sf::Vector2f(320.f, y);
      line[1].color = sf::Color(0, 120, 255, alpha);
      gameWindow.getRenderWindow().draw(line, 2, sf::PrimitiveType::Lines);
    }
    // Draw vertical grid lines
    for (float x = 0.f; x < 320.f; x += gridSpacing) {
      line[0].position = sf::Vector2f(x, 0.f);
      line[0].color = sf::Color(0, 120, 255, alpha);
      line[1].position = sf::Vector2f(x, 240.f);
      line[1].color = sf::Color(0, 120, 255, alpha);
      gameWindow.getRenderWindow().draw(line, 2, sf::PrimitiveType::Lines);
    }
  }

  // Draw background decorative crescent bricks for the main menu
  if (currentState == GameState::Menu) {
    for (const auto &b : menuDecorBricks) {
      gameWindow.getRenderWindow().draw(b.shape);
    }
  }

  if (currentState == GameState::Playing || currentState == GameState::Menu ||
      currentState == GameState::Options ||
      currentState == GameState::StageClear ||
      currentState == GameState::GameOver) {
    drawRain();
  }
  if (currentState == GameState::Playing ||
      currentState == GameState::StageClear ||
      currentState == GameState::GameOver) {
    // Apply retro visual Y bounce offset (up and down offset, no scaling)
    sf::Vector2f origPos = paddle.getPosition();
    float visualYOffset = 0.f;
    if (paddleBounceTimer < 0.3f) {
      float t = paddleBounceTimer;
      float amplitude = 2.0f * (1.0f - (t / 0.3f)); // subtle 2px movement
      visualYOffset = amplitude * std::sin(t * 40.f);
    }
    // Draw a drop shadow behind the paddle
    sf::ConvexShape padShadow = paddle;
    padShadow.setPosition({origPos.x + 2.f, origPos.y + visualYOffset + 2.f});
    padShadow.setFillColor(
        sf::Color(10, 12, 16, 120)); // semi-transparent black shadow
    padShadow.setOutlineThickness(0.f);
    gameWindow.getRenderWindow().draw(padShadow);

    paddle.setPosition({origPos.x, origPos.y + visualYOffset});
    gameWindow.getRenderWindow().draw(paddle);

    // Draw the 3D glass highlight reflection overlay on top of the paddle
    sf::ConvexShape padHighlight = paddle;
    padHighlight.setScale({0.78f, 0.42f});
    padHighlight.move({0.f, -1.0f});
    padHighlight.setFillColor(sf::Color(
        255, 255, 255, 100)); // semi-transparent white highlight shine
    padHighlight.setOutlineThickness(0.f);
    gameWindow.getRenderWindow().draw(padHighlight);

    paddle.setPosition(origPos); // restore original physics position

    // Draw ball energy trails
    sf::CircleShape trailDot;
    trailDot.setRadius(2.2f);
    trailDot.setOrigin({2.2f, 2.2f});
    for (const auto &t : ballTrails) {
      trailDot.setPosition(t.position);
      trailDot.setFillColor(t.color);
      gameWindow.getRenderWindow().draw(trailDot);
    }

    gameWindow.getRenderWindow().draw(ball);

    // Draw glass-beveled 3D bricks
    for (const auto &brick : bricks) {
      if (!brick.isDestroyed) {
        // Main brick body
        gameWindow.getRenderWindow().draw(brick.shape);

        // 3D glass highlight reflection overlay (shrunk and offset slightly up)
        sf::ConvexShape highlight = brick.shape;
        highlight.setScale({0.75f, 0.45f});
        highlight.move({0.f, -1.0f});
        highlight.setFillColor(sf::Color(255, 255, 255, 90));
        highlight.setOutlineThickness(0.f);
        gameWindow.getRenderWindow().draw(highlight);
      }
    }
    drawParticles();
  }

  // 2. RENDER UI & HUD (Native HD Pixel-Perfect)
  gameWindow.setUIView();
  if (currentState == GameState::Playing ||
      currentState == GameState::StageClear ||
      currentState == GameState::GameOver) {
    sf::Vector2u wSize = gameWindow.getWindowSize();
    float w = static_cast<float>(wSize.x);
    float h = static_cast<float>(wSize.y);
    float sbX = w * 0.78f;
    float sbW = w * 0.22f;

    // --- 1. Draw Panel Slate Backings ---
    sf::RectangleShape panelBack;

    // Top Panel: 0 to h * 0.25f (Logo box - filled with game title)
    panelBack.setSize({sbW, h * 0.25f});
    panelBack.setPosition({sbX, 0.f});
    panelBack.setFillColor(sf::Color::Black);
    gameWindow.getRenderWindow().draw(panelBack);

    // Draw the game title inside the top panel box with neon shadows
    {
      sf::Text titleShadow = hudTitleText;
      titleShadow.setFillColor(sf::Color(0, 120, 255, 180)); // neon blue shadow
      titleShadow.setPosition(
          {hudTitleText.getPosition().x + 1.5f * (w / 854.f),
           hudTitleText.getPosition().y + 1.5f * (h / 480.f)});
      gameWindow.getRenderWindow().draw(titleShadow);
    }
    gameWindow.getRenderWindow().draw(hudTitleText);

    // Middle Panel: h * 0.25f to h * 0.75f (Stats box)
    panelBack.setSize({sbW, h * 0.50f});
    panelBack.setPosition({sbX, h * 0.25f});
    panelBack.setFillColor(sf::Color(14, 20, 35));
    gameWindow.getRenderWindow().draw(panelBack);

    // Bottom Panel: h * 0.75f to h (Fluorescent tube box)
    panelBack.setSize({sbW, h * 0.25f});
    panelBack.setPosition({sbX, h * 0.75f});
    panelBack.setFillColor(sf::Color(6, 6, 8));
    gameWindow.getRenderWindow().draw(panelBack);
    // --- 2. Draw Heavy Metallic Frames (Borders & Dividers) ---
    float frameThick = 10.f * (w / 854.f);

    sf::RectangleShape borderPlate;
    borderPlate.setFillColor(sf::Color(80, 85, 96)); // base steel grey

    // Left vertical border
    borderPlate.setSize({frameThick, h});
    borderPlate.setPosition({sbX, 0.f});
    gameWindow.getRenderWindow().draw(borderPlate);

    // Right vertical border
    borderPlate.setPosition({w - frameThick, 0.f});
    gameWindow.getRenderWindow().draw(borderPlate);

    // Top horizontal border
    borderPlate.setSize({sbW, frameThick});
    borderPlate.setPosition({sbX, 0.f});
    gameWindow.getRenderWindow().draw(borderPlate);

    // Bottom horizontal border
    borderPlate.setPosition({sbX, h - frameThick});
    gameWindow.getRenderWindow().draw(borderPlate);

    // First horizontal divider plate (at h * 0.25f)
    borderPlate.setSize({sbW, frameThick});
    borderPlate.setPosition({sbX, h * 0.25f - frameThick / 2.f});
    gameWindow.getRenderWindow().draw(borderPlate);

    // Second horizontal divider plate (at h * 0.75f)
    borderPlate.setPosition({sbX, h * 0.75f - frameThick / 2.f});
    gameWindow.getRenderWindow().draw(borderPlate);

    // --- 2.5 Draw Shiny Chrome Reflections & Deep Depth Shadows along the
    // center of the plates ---
    sf::RectangleShape reflectiveStrip;
    reflectiveStrip.setFillColor(
        sf::Color(220, 225, 235, 140)); // light chrome sheen highlight

    sf::RectangleShape shadowStrip;
    shadowStrip.setFillColor(sf::Color(35, 38, 45, 180)); // deep crease shadow

    // Vertical reflections & shadows
    // Left border
    reflectiveStrip.setSize({1.5f, h});
    reflectiveStrip.setPosition({sbX + frameThick / 2.f - 0.75f, 0.f});
    gameWindow.getRenderWindow().draw(reflectiveStrip);

    shadowStrip.setSize({1.5f, h});
    shadowStrip.setPosition({sbX + frameThick / 2.f + 1.25f, 0.f});
    gameWindow.getRenderWindow().draw(shadowStrip);

    // Right border
    reflectiveStrip.setPosition({w - frameThick / 2.f - 0.75f, 0.f});
    gameWindow.getRenderWindow().draw(reflectiveStrip);

    shadowStrip.setPosition({w - frameThick / 2.f + 1.25f, 0.f});
    gameWindow.getRenderWindow().draw(shadowStrip);

    // Horizontal reflections & shadows
    // Top border
    reflectiveStrip.setSize({sbW, 1.5f});
    reflectiveStrip.setPosition({sbX, frameThick / 2.f - 0.75f});
    gameWindow.getRenderWindow().draw(reflectiveStrip);

    shadowStrip.setSize({sbW, 1.5f});
    shadowStrip.setPosition({sbX, frameThick / 2.f + 1.25f});
    gameWindow.getRenderWindow().draw(shadowStrip);

    // Bottom border
    reflectiveStrip.setPosition({sbX, h - frameThick / 2.f - 0.75f});
    gameWindow.getRenderWindow().draw(reflectiveStrip);

    shadowStrip.setPosition({sbX, h - frameThick / 2.f + 1.25f});
    gameWindow.getRenderWindow().draw(shadowStrip);

    // Divider 1
    reflectiveStrip.setPosition({sbX, h * 0.25f - 0.75f});
    gameWindow.getRenderWindow().draw(reflectiveStrip);

    shadowStrip.setPosition({sbX, h * 0.25f + 1.25f});
    gameWindow.getRenderWindow().draw(shadowStrip);

    // Divider 2
    reflectiveStrip.setPosition({sbX, h * 0.75f - 0.75f});
    gameWindow.getRenderWindow().draw(reflectiveStrip);

    shadowStrip.setPosition({sbX, h * 0.75f + 1.25f});
    gameWindow.getRenderWindow().draw(shadowStrip);

    // --- 3. Draw 3D Bevel Highlights ---
    auto drawBevelRect = [&](float bx, float by, float bw, float bh) {
      sf::Vertex bevelLine[2];
      // Top/Left light highlights (Silver color)
      bevelLine[0].color = sf::Color(145, 150, 160);
      bevelLine[1].color = sf::Color(145, 150, 160);

      bevelLine[0].position = {bx, by};
      bevelLine[1].position = {bx + bw, by};
      gameWindow.getRenderWindow().draw(bevelLine, 2, sf::PrimitiveType::Lines);

      bevelLine[1].position = {bx, by + bh};
      gameWindow.getRenderWindow().draw(bevelLine, 2, sf::PrimitiveType::Lines);

      // Bottom/Right shadow highlights (Dark charcoal color)
      bevelLine[0].color = sf::Color(35, 38, 45);
      bevelLine[1].color = sf::Color(35, 38, 45);

      bevelLine[0].position = {bx, by + bh};
      bevelLine[1].position = {bx + bw, by + bh};
      gameWindow.getRenderWindow().draw(bevelLine, 2, sf::PrimitiveType::Lines);

      bevelLine[0].position = {bx + bw, by};
      gameWindow.getRenderWindow().draw(bevelLine, 2, sf::PrimitiveType::Lines);
    };

    drawBevelRect(sbX, 0.f, frameThick, h);            // Left border highlight
    drawBevelRect(w - frameThick, 0.f, frameThick, h); // Right border highlight
    drawBevelRect(sbX, h * 0.25f - frameThick / 2.f, sbW,
                  frameThick); // Divider 1 highlight
    drawBevelRect(sbX, h * 0.75f - frameThick / 2.f, sbW,
                  frameThick); // Divider 2 highlight

    // --- 4. Draw Industrial Rivets / Bolts ---
    auto drawRivet = [&](float rx, float ry) {
      float r = 3.f * (h / 480.f);

      // Drop shadow (offset to bottom-right)
      sf::CircleShape shadow(r);
      shadow.setOrigin({r, r});
      shadow.setFillColor(sf::Color(10, 12, 16, 160));
      shadow.setPosition({rx + 1.f, ry + 1.f});
      gameWindow.getRenderWindow().draw(shadow);

      // Main rivet body
      sf::CircleShape rivet(r);
      rivet.setOrigin({r, r});
      rivet.setFillColor(sf::Color(135, 140, 150));
      rivet.setOutlineColor(sf::Color(25, 30, 40));
      rivet.setOutlineThickness(1.f);
      rivet.setPosition({rx, ry});
      gameWindow.getRenderWindow().draw(rivet);

      // Specular reflection dot (offset to top-left)
      sf::CircleShape shiny(r * 0.35f);
      shiny.setOrigin({shiny.getRadius(), shiny.getRadius()});
      shiny.setFillColor(sf::Color(255, 255, 255, 220));
      shiny.setPosition({rx - r * 0.3f, ry - r * 0.3f});
      gameWindow.getRenderWindow().draw(shiny);
    };

    float rxLeft = sbX + frameThick / 2.f;
    float rxRight = w - frameThick / 2.f;
    for (float ry = h * 0.05f; ry < h; ry += h * 0.15f) {
      drawRivet(rxLeft, ry);
      drawRivet(rxRight, ry);
    }
    drawRivet(sbX + sbW * 0.3f, h * 0.25f);
    drawRivet(sbX + sbW * 0.7f, h * 0.25f);
    drawRivet(sbX + sbW * 0.3f, h * 0.75f);
    drawRivet(sbX + sbW * 0.7f, h * 0.75f);

    // --- 6. Draw Bottom Panel neon light tubes & round labels ---
    {
      float bottomY = h * 0.75f;
      float bottomH = h * 0.25f;

      // Draw two vertical cyan neon light tubes on the left/right edges
      auto drawNeonTube = [&](float tx) {
        // Tube backing slot
        sf::RectangleShape tubeSlot({4.f, bottomH * 0.7f});
        tubeSlot.setOrigin({2.f, 0.f});
        tubeSlot.setPosition({tx, bottomY + bottomH * 0.15f});
        tubeSlot.setFillColor(sf::Color(5, 8, 12));
        gameWindow.getRenderWindow().draw(tubeSlot);

        // Neon glow tube
        sf::RectangleShape neonLine({2.f, bottomH * 0.68f});
        neonLine.setOrigin({1.f, 0.f});
        neonLine.setPosition({tx, bottomY + bottomH * 0.16f});
        neonLine.setFillColor(sf::Color(0, 240, 255, 160)); // cyan outer glow
        gameWindow.getRenderWindow().draw(neonLine);

        sf::RectangleShape whiteCore({0.8f, bottomH * 0.68f});
        whiteCore.setOrigin({0.4f, 0.f});
        whiteCore.setPosition({tx, bottomY + bottomH * 0.16f});
        whiteCore.setFillColor(sf::Color(255, 255, 255, 220)); // white core
        gameWindow.getRenderWindow().draw(whiteCore);
      };

      drawNeonTube(sbX + frameThick + 6.f);
      drawNeonTube(w - frameThick - 6.f);

      sf::Text rText(font, "Game Version",
                     static_cast<unsigned int>(h * 0.026f));
      rText.setFillColor(sf::Color(255, 220, 0));
      sf::FloatRect rB = rText.getLocalBounds();
      rText.setPosition(
          {sbX + (sbW - rB.size.x) / 2.f, bottomY + bottomH * 0.22f});
      gameWindow.getRenderWindow().draw(rText);

      sf::Text abyssText(font, "1.0.0", static_cast<unsigned int>(h * 0.024f));
      abyssText.setFillColor(sf::Color(0, 220, 255));
      sf::FloatRect abB = abyssText.getLocalBounds();
      abyssText.setPosition(
          {sbX + (sbW - abB.size.x) / 2.f, bottomY + bottomH * 0.42f});
      gameWindow.getRenderWindow().draw(abyssText);

      sf::Text beginText(font, "BETA", static_cast<unsigned int>(h * 0.03f));
      float flashVal = (std::sin(menuAnimTimer * 5.f) + 1.f) * 0.5f;
      std::uint8_t beginAlpha = static_cast<std::uint8_t>(80 + flashVal * 175);
      beginText.setFillColor(sf::Color(255, 235, 100, beginAlpha));
      sf::FloatRect begB = beginText.getLocalBounds();
      beginText.setPosition(
          {sbX + (sbW - begB.size.x) / 2.f, bottomY + bottomH * 0.65f});
      gameWindow.getRenderWindow().draw(beginText);
    }

    gameWindow.getRenderWindow().draw(hudBackplateTime);
    gameWindow.getRenderWindow().draw(hudBackplateScore);
    gameWindow.getRenderWindow().draw(hudBackplateSpeed);

    gameWindow.getRenderWindow().draw(hudTimeBackValue);
    gameWindow.getRenderWindow().draw(hudScoreBackValue);
    gameWindow.getRenderWindow().draw(hudSpeedBackValue);

    gameWindow.getRenderWindow().draw(hudTimeLabel);
    gameWindow.getRenderWindow().draw(hudTimeValue);
    gameWindow.getRenderWindow().draw(hudScoreLabel);
    gameWindow.getRenderWindow().draw(hudScoreValue);
    gameWindow.getRenderWindow().draw(hudSpeedLabel);
    gameWindow.getRenderWindow().draw(hudSpeedValue);
  } else if (currentState == GameState::Menu) {
    gameWindow.getRenderWindow().draw(techLabel0);
    gameWindow.getRenderWindow().draw(techLabel1);
    gameWindow.getRenderWindow().draw(techLabel2);
    gameWindow.getRenderWindow().draw(techLabel3);

    gameWindow.getRenderWindow().draw(titleShadowText);
    gameWindow.getRenderWindow().draw(titleText);
    gameWindow.getRenderWindow().draw(subtitleText);

    gameWindow.getRenderWindow().draw(playText);
    gameWindow.getRenderWindow().draw(optionText);
    gameWindow.getRenderWindow().draw(exitText);
  } else if (currentState == GameState::Options) {
    gameWindow.getRenderWindow().draw(optTitleText);
    gameWindow.getRenderWindow().draw(optBox);

    gameWindow.getRenderWindow().draw(resLabelText);
    gameWindow.getRenderWindow().draw(resValueText);

    gameWindow.getRenderWindow().draw(displayLabelText);
    gameWindow.getRenderWindow().draw(displayValueText);

    gameWindow.getRenderWindow().draw(bindLeftLabelText);
    gameWindow.getRenderWindow().draw(bindLeftValueText);

    gameWindow.getRenderWindow().draw(bindRightLabelText);
    gameWindow.getRenderWindow().draw(bindRightValueText);

    gameWindow.getRenderWindow().draw(padColorLabelText);
    gameWindow.getRenderWindow().draw(padColorValueText);

    gameWindow.getRenderWindow().draw(backText);
    gameWindow.getRenderWindow().draw(helpText);
  }

  if (currentState == GameState::StageClear ||
      currentState == GameState::GameOver) {
    sf::Vector2u wSize = gameWindow.getWindowSize();
    sf::RectangleShape dimRect(
        {static_cast<float>(wSize.x), static_cast<float>(wSize.y)});
    dimRect.setFillColor(sf::Color(4, 6, 14, 185));
    gameWindow.getRenderWindow().draw(dimRect);

    if (currentState == GameState::StageClear) {
      overlayPanel.setOutlineColor(sf::Color(0, 255, 140));
      overlayPanel.setOutlineThickness(2.f);
      gameWindow.getRenderWindow().draw(overlayPanel);

      gameWindow.getRenderWindow().draw(stageClearText);
      gameWindow.getRenderWindow().draw(stageClearSubText);
    } else {
      overlayPanel.setOutlineColor(sf::Color(255, 50, 60)); // Danger red border
      overlayPanel.setOutlineThickness(2.f);
      gameWindow.getRenderWindow().draw(overlayPanel);

      gameWindow.getRenderWindow().draw(gameOverText);
      gameWindow.getRenderWindow().draw(gameOverSubText);
    }

    gameWindow.getRenderWindow().draw(continueText);
  }

  gameWindow.display();
}

void Game::initRain() {
  rainDrops.resize(250);
  for (auto &drop : rainDrops) {
    drop.position.x = static_cast<float>(rand() % 320);
    drop.position.y = static_cast<float>(rand() % 240);
    drop.speed = 120.f + static_cast<float>(rand() % 80);
    drop.length = 4.f + static_cast<float>(rand() % 4);
    drop.slant = 0.f;
    int alpha = 80 + rand() % 60;
    drop.color = sf::Color(170, 200, 255, alpha);
  }
}

void Game::updateRain(sf::Time deltaTime) {
  float dt = deltaTime.asSeconds();

  float maxWidth = 320.f;
  float currentRainMultiplier = 1.2f; // Default menu rain speed

  if (currentState == GameState::Playing) {
    maxWidth = 320.f;
    currentRainMultiplier = speedMultiplier;
  } else if (currentState == GameState::StageClear ||
             currentState == GameState::GameOver) {
    maxWidth = 320.f;
    currentRainMultiplier = 0.8f;
  }

  for (auto &drop : rainDrops) {
    float speedY = drop.speed * (1.0f + (currentRainMultiplier - 1.0f) * 0.4f);
    drop.position.y += speedY * dt;

    if (drop.position.y > 240.f) {
      drop.position.y = -10.f - static_cast<float>(rand() % 15);
      drop.position.x = static_cast<float>(rand() % static_cast<int>(maxWidth));
      drop.speed = 120.f + static_cast<float>(rand() % 80);
      drop.length = 4.f + static_cast<float>(rand() % 4);
      drop.slant = 0.f;
      int alpha = 80 + rand() % 60;
      drop.color = sf::Color(170, 200, 255, alpha);
    }

    if (drop.position.x > maxWidth) {
      drop.position.x = static_cast<float>(rand() % static_cast<int>(maxWidth));
    }
  }
}

void Game::drawRain() {
  int activeCount = 20;

  if (currentState == GameState::Playing) {
    float t =
        (speedMultiplier - 1.0f) / 1.7f; // speedMultiplier goes up to 2.7x
    if (t < 0.f)
      t = 0.f;
    if (t > 1.f)
      t = 1.f;
    activeCount = static_cast<int>(4.f + t * 30.f); // rain drops
  } else if (currentState == GameState::StageClear ||
             currentState == GameState::GameOver) {
    activeCount = 10;
  } else if (currentState == GameState::Options) {
    activeCount = 20;
  }

  if (activeCount <= 0)
    return;
  if (activeCount > static_cast<int>(rainDrops.size())) {
    activeCount = static_cast<int>(rainDrops.size());
  }

  // Draw using sf::RectangleShape for thickness (pixelated look)
  sf::RectangleShape rect;
  for (int i = 0; i < activeCount; ++i) {
    const auto &drop = rainDrops[i];
    rect.setSize(sf::Vector2f(1.f, drop.length)); // 1.f width makes it thinner
    rect.setPosition(drop.position);
    rect.setFillColor(drop.color);
    gameWindow.getRenderWindow().draw(rect);
  }
}

void Game::createBrickSoundBuffer() {
  const unsigned int sampleRate = 44100;
  const float duration = 0.08f; // 80 ms retro burst
  const unsigned int sampleCount =
      static_cast<unsigned int>(sampleRate * duration);
  std::vector<std::int16_t> samples(sampleCount);

  float phase = 0.f;
  for (unsigned int i = 0; i < sampleCount; ++i) {
    float progress = static_cast<float>(i) / sampleCount;

    // Retro arcade sweep: high pitch to low pitch (pew!)
    float freq = 1000.f - progress * 700.f;

    // Linear fade-out envelope
    float envelope = 1.f - progress;

    phase += 2.f * 3.14159265f * freq / sampleRate;
    if (phase > 2.f * 3.14159265f) {
      phase -= 2.f * 3.14159265f;
    }

    // Square wave chiptune sound
    float value = (sin(phase) > 0.f) ? 1.f : -1.f;

    samples[i] = static_cast<std::int16_t>(value * 5000.f * envelope);
  }

  (void)brickSoundBuffer.loadFromSamples(samples.data(), sampleCount, 1,
                                         sampleRate, {sf::SoundChannel::Mono});
}

void Game::spawnParticles(sf::Vector2f position, sf::Color color) {
  // Spawn 10 particles for retro block debris
  int count = 10;
  for (int i = 0; i < count; ++i) {
    Particle p;
    p.position = position;

    // Random angle and speed
    float angle = static_cast<float>(rand() % 360) * 3.14159265f / 180.f;
    float speed = 40.f + static_cast<float>(rand() % 60);

    p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
    p.color = color;
    p.lifetime = 0.f;
    p.maxLifetime =
        0.35f + static_cast<float>(rand() % 20) / 100.f; // 0.35s to 0.55s
    particles.push_back(p);
  }
}

void Game::updateParticles(float dt) {
  for (auto it = particles.begin(); it != particles.end();) {
    it->lifetime += dt;
    if (it->lifetime >= it->maxLifetime) {
      it = particles.erase(it);
    } else {
      // Apply gravity/drag (accelerate downwards for falling debris feel)
      it->velocity.y += 80.f * dt;
      it->position += it->velocity * dt;

      // Fade alpha over time
      float progress = it->lifetime / it->maxLifetime;
      std::uint8_t newAlpha =
          static_cast<std::uint8_t>(255.f * (1.f - progress));
      it->color.a = newAlpha;

      ++it;
    }
  }
}

void Game::drawParticles() {
  // Draw particles as retro 2x2 blocks
  sf::RectangleShape rect({2.f, 2.f});
  for (const auto &p : particles) {
    rect.setPosition(p.position);
    rect.setFillColor(p.color);
    gameWindow.getRenderWindow().draw(rect);
  }
}