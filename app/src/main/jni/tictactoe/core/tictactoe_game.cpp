////
// tictactoe_game.cpp
////

#include "tictactoe/core/tictactoe_game.h"

// static
std::unique_ptr<SimpleGame> SimpleGame::Create(
    PlatformDelegate* platform_delegate) {
  return std::make_unique<Tictactoe::TictactoeGame>(platform_delegate);
}

namespace Tictactoe {

TictactoeGame::TictactoeGame(PlatformDelegate* platform_delegate)
    : SimpleGame(platform_delegate) {}

TictactoeGame::~TictactoeGame() {}

// private:
// SimpleGame:
void TictactoeGame::OnCreate() {
  SetView(std::make_unique<MainMenu>(this));
}

// GameBoard::Listener:
void TictactoeGame::OnCloseBoard() {
  SetView(std::make_unique<MainMenu>(this));
}

// MainMenu::Listener:
void TictactoeGame::OnStartGame(Difficulty difficulty) {
  SetView(std::make_unique<GameBoard>(this, difficulty));
}

}  // namespace Tictactoe
