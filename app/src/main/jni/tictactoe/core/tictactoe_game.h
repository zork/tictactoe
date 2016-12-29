////
// tictactoe_game.h
////

#pragma once

#include "base/macros.h"
#include "game/simple_game.h"
#include "tictactoe/ui/game_board.h"
#include "tictactoe/ui/main_menu.h"

namespace Tictactoe {

class TictactoeGame : public SimpleGame,
                      public GameBoard::Listener,
                      public MainMenu::Listener {
 public:
  TictactoeGame(PlatformDelegate* platform_delegate);
  ~TictactoeGame() override;
  DISALLOW_COPY_AND_ASSIGN(TictactoeGame);

 private:
  // SimpleGame:
  void OnCreate() override;

  // GameBoard::Listener:
  void OnCloseBoard() override;

  // MainMenu::Listener:
  void OnStartGame(Difficulty difficulty) override;
};

}  // namespace Tictactoe
