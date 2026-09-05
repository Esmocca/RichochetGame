#include "../include/GameLogic.h"

GameWindow::GameWindow(const std::string &title, unsigned int width,
                       unsigned int height)
    : windowTitle(title), virtualSize(width, height), isFullscreen(false) {
  window.create(sf::VideoMode({854, 480}), title, sf::Style::Default,
                sf::State::Windowed);
  window.setFramerateLimit(60);

  handleResize(854, 480);
}

GameWindow::~GameWindow() {
  if (window.isOpen()) {
    window.close();
  }
}

void GameWindow::clear() { window.clear(sf::Color(12, 14, 24)); }

void GameWindow::display() { window.display(); }

bool GameWindow::isOpen() const { return window.isOpen(); }

std::optional<sf::Event> GameWindow::pollEvent() { return window.pollEvent(); }

void GameWindow::handleResize(unsigned int newWidth, unsigned int newHeight) {
  // 1. Game View (Virtual 320x240 dengan Aspect Ratio Preserved)
  float targetAspectRatio =
      static_cast<float>(virtualSize.x) / static_cast<float>(virtualSize.y);
  float windowAspectRatio =
      static_cast<float>(newWidth) / static_cast<float>(newHeight);

  sf::FloatRect viewport(sf::Vector2f{0.f, 0.f}, sf::Vector2f{1.f, 1.f});

  if (windowAspectRatio > targetAspectRatio) {
    float viewportWidth = targetAspectRatio / windowAspectRatio;
    viewport.position.x = (1.f - viewportWidth) / 2.f;
    viewport.size.x = viewportWidth;
  } else {
    float viewportHeight = windowAspectRatio / targetAspectRatio;
    viewport.position.y = (1.f - viewportHeight) / 2.f;
    viewport.size.y = viewportHeight;
  }

  gameView =
      sf::View(sf::FloatRect({0.f, 0.f}, {static_cast<float>(virtualSize.x),
                                          static_cast<float>(virtualSize.y)}));
  gameView.setViewport(viewport);
  
  uiView = sf::View(sf::FloatRect({0.f, 0.f}, {static_cast<float>(newWidth),
                                               static_cast<float>(newHeight)}));
}

void GameWindow::setGameView() { window.setView(gameView); }

void GameWindow::setUIView() { window.setView(uiView); }

void GameWindow::setResolution(unsigned int width, unsigned int height,
                               bool fullscreen) {
  isFullscreen = fullscreen;

  if (isFullscreen) {
    window.create(sf::VideoMode::getDesktopMode(), windowTitle,
                  sf::State::Fullscreen);
  } else {
    window.create(sf::VideoMode({width, height}), windowTitle,
                  sf::Style::Default, sf::State::Windowed);
  }

  window.setFramerateLimit(60);
  sf::Vector2u size = window.getSize();
  handleResize(size.x, size.y);
}

void GameWindow::toggleFullscreen() { setResolution(854, 480, !isFullscreen); }
