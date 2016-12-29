////
// game_board.h
////

#pragma once

#include "base/macros.h"
#include "game/ui/button.h"
#include "game/ui/view.h"
#include "tictactoe/constants.h"

namespace ui {
class Label;
}

namespace Tictactoe {

class GameBoard : public ui::View, public ui::Button::Listener {
 public:
  class Listener {
   public:
    virtual ~Listener() {}
    virtual void OnCloseBoard() = 0;
  };

  GameBoard(Listener* listener, Difficulty difficulty);
  ~GameBoard() override;
  DISALLOW_COPY_AND_ASSIGN(GameBoard);

 private:
  class DelayedTurn;
  friend class DelayedTurn;

  void AddBoardSpace(ui::View* board, int index);
  void PlaceMark(int space);
  void TakeComputerTurn();
  bool CheckForWinner();
  void SetWinner(Player player);
  void UpdateTurnLabel();
  int FindWinningMove(Player player);
  int FindBestMove(Player player);

  // InputListener:
  bool OnKeyEvent(const KeyEvent& event) override;

  // ui::Button::Listener:
  void OnClick(ui::Button* button) override;

  Listener* listener_;
  Difficulty difficulty_;

  Player turn_;
  Player board_[9];

  DelayedTurn* pending_turn_;

  ui::Label* status_label_;
  ui::View* board_buttons_[9];
  ui::View* board_x_labels_[9];
  ui::View* board_o_labels_[9];
};

}  // namespace Tictactoe
