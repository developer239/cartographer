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
    minimap = cropArea(map, minimapPosX, minimapPosY, minimapSize, minimapSize);
    mappedArea = minimap.clone();
  }

  void HandleEvent(SDL_Event& event) override {
    // moveBy random from 0 to 20
    int MOVE_BY = rand() % 25;

    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_DOWN:
          minimapPosY = std::min(map.rows - minimapSize, minimapPosY + MOVE_BY);
          break;
        case SDLK_UP:
          minimapPosY = std::max(0, minimapPosY - MOVE_BY);
          break;
        case SDLK_LEFT:
          minimapPosX = std::max(0, minimapPosX - MOVE_BY);
          break;
        case SDLK_RIGHT:
          minimapPosX = std::min(map.cols - minimapSize, minimapPosX + MOVE_BY);
          break;
        default:
          break;
      }

      minimap =
          cropArea(map, minimapPosX, minimapPosY, minimapSize, minimapSize);

      auto result = stitch(mappedArea, minimap);
      mappedArea = result.stitched;
      mappedAreaLastLocation = result.matchLoc;
    }
  }

  void OnUpdate(Core::Window& window, Core::Renderer& renderer) override {
    mapView = map.clone();
    auto expectedLocationOnMap = templateMatch(mapView, minimap);
    cv::cvtColor(mapView, mapView, cv::COLOR_GRAY2RGB);
    cv::rectangle(
        mapView,
        expectedLocationOnMap,
        cv::Point(
            expectedLocationOnMap.x + minimap.cols,
            expectedLocationOnMap.y + minimap.rows
        ),
        cv::Scalar(0, 0, 255),
        2,
        8,
        0
    );

    // mapped
    auto mappedAreaView = mappedArea.clone();
    auto lastKnownMappedAreaLocation = mappedAreaLastLocation;
    cv::rectangle(
        mappedAreaView,
        lastKnownMappedAreaLocation,
        cv::Point(
            lastKnownMappedAreaLocation.x + minimap.cols,
            lastKnownMappedAreaLocation.y + minimap.rows
        ),
        cv::Scalar(0, 0, 255),
        2,
        8,
        0
    );
    cv::imshow("mapped area", mappedAreaView);
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
  cv::Mat mappedArea;
  cv::Point mappedAreaLastLocation;
  int minimapPosX = 0;
  int minimapPosY = 0;
  int minimapSize = 200;
  cv::Mat mapView;
};
