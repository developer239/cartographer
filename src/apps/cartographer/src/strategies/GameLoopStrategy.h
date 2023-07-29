#pragma once

#include "../common/Map.h"
#include "core/AssetStore.h"
#include "core/IStrategy.h"
#include "core/Renderer.h"
#include "core/Window.h"
#include "utility/MatrixAsTexture.h"

class GameLoopStrategy : public Core::IStrategy {
 public:
  void Init(Core::Window& window, Core::Renderer& renderer) override {
    Core::AssetStore::Instance()
        .AddFont("pico8-font-10", "assets/fonts/Roboto-Medium.ttf", 24);
    Core::AssetStore::Instance()
        .AddFont("pico8-font-10-small", "assets/fonts/Roboto-Medium.ttf", 16);

    SDL_SetWindowSize(window.Get().get(), windowWidth, windowHeight);

    int mazeWidth = windowWidth;
    int mazeHeight = windowHeight;
    int pathSize = 30;

    map = generateMaze(mazeHeight, mazeWidth, pathSize);
    minimap = cropArea(map, minimapPosX, minimapPosY, 100, 100);
  }

  void HandleEvent(SDL_Event& event) override {
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_DOWN:
          minimapPosY += 10;
          break;
        case SDLK_UP:
          minimapPosY -= 10;
          break;
        case SDLK_LEFT:
          minimapPosX -= 10;
          break;
        case SDLK_RIGHT:
          minimapPosX += 10;
          break;
        default:
          break;
      }
      minimap = cropArea(map, minimapPosX, minimapPosY, 100, 100);
    }
  }

  void OnUpdate(Core::Window& window, Core::Renderer& renderer) override {
    mapView = map.clone();

    auto location = templateMatch(mapView, minimap);
    cv::cvtColor(mapView, mapView, cv::COLOR_GRAY2RGB);
    cv::rectangle(
        mapView,
        location,
        cv::Point(location.x + minimap.cols, location.y + minimap.rows),
        cv::Scalar(0, 0, 255),
        2,
        8,
        0
    );
  }

  void OnRender(Core::Window& window, Core::Renderer& renderer) override {
    Utility::matrixAsTexture(renderer.Get().get(), &mapView);
  }

  void OnBeforeRender(Core::Window& window, Core::Renderer& renderer) override {
  }

  void OnAfterRender(Core::Window& window, Core::Renderer& renderer) override {}

 private:
  int windowWidth = 800;
  int windowHeight = 800;
  cv::Mat map;
  cv::Mat minimap;
  int minimapPosX = 0;
  int minimapPosY = 0;
  cv::Mat mapView;
};
