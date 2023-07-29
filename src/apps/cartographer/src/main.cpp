#include "core/Loop.h"

#include "./strategies/GameLoopStrategy.h"

int main() {
  GameLoopStrategy gameLoop;

  auto loop = Core::Loop({&gameLoop});
  loop.Run();
}
