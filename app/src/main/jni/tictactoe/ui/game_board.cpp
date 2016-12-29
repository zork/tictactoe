////
// game_board.h
////

#include "tictactoe/ui/game_board.h"

#include "base/logging.h"
#include "base/thread/task.h"
#include "base/util/random.h"
#include "game/input/key_event.h"
#include "game/input/keycodes.h"
#include "game/ui/grid.h"
#include "game/ui/label.h"
#include "game/ui/root_view.h"

namespace {
const char kGameBoardImage[] = "assets/ui/game_board.pcx";
const char kXImage[] = "assets/ui/x_image.pcx";
const char kOImage[] = "assets/ui/o_image.pcx";

const char kGameBoardTitle[] = "Game Board";
const char kBoardSpaceLabel[] = "Empty space";
const char kXTurnLabel[] = "X's turn";
const char kOTurnLabel[] = "O's turn";
const char kXWinsLabel[] = "X wins!";
const char kOWinsLabel[] = "O wins!";
const char kDrawLabel[] = "Draw";
const char kXLabel[] = "X";
const char kOLabel[] = "O";

const char kPlacedAnnouncement[] = " placed in ";
const char kPlayerName[][16] = {"None", "X", "O"};
const char kBoardSpaceName[][16] = {
    "top left",     "top center",  "top right",     "center left",  "center",
    "center right", "bottom left", "bottom center", "bottom right",
};
}

namespace Tictactoe {

class GameBoard::DelayedTurn : public Task {
 public:
  DelayedTurn(GameBoard* parent) : parent_(parent) {}
  DISALLOW_COPY_AND_ASSIGN(DelayedTurn);

  void Cancel() { parent_ = nullptr; }
  void Execute() override {
    if (parent_)
      parent_->TakeComputerTurn();
  }

 private:
  GameBoard* parent_;
};

GameBoard::GameBoard(Listener* listener, Difficulty difficulty)
    : listener_(listener),
      difficulty_(difficulty),
      turn_(kPlayerX),
      pending_turn_(nullptr) {
  // Set the title.
  SetTitle(kGameBoardTitle);

  // Add the status label.
  auto status_label = std::make_unique<ui::Label>();
  status_label->SetLayoutHeight(100);
  status_label->SetLayoutHAlign(ui::View::kHAlignCenter);
  status_label->SetLayoutVAlign(ui::View::kVAlignTop);
  status_label->SetTextHAlign(ui::View::kHAlignCenter);
  status_label->SetFontSize(72);
  status_label->SetAccessibilityLive(ui::kAccessibilityLivePolite);
  status_label_ = status_label.get();
  AddView(std::move(status_label));

  // Add the image of the game board.
  auto board_image = std::make_unique<ui::Image>();
  board_image->SetImage(kGameBoardImage);

  auto grid = std::make_unique<ui::Grid>();
  grid->SetColumns(3);
  grid->SetCellWidth(300);
  grid->SetCellHeight(300);

  for (int i = 0; i < 9; ++i) {
    AddBoardSpace(grid.get(), i);
    board_[i] = kPlayerNone;
  }

  board_image->AddView(std::move(grid));
  AddView(std::move(board_image));

  UpdateTurnLabel();
}

GameBoard::~GameBoard() {
  if (pending_turn_)
    pending_turn_->Cancel();
}

// private:
void GameBoard::AddBoardSpace(ui::View* board, int index) {
  auto view = std::make_unique<ui::View>();

  auto button = std::make_unique<ui::Button>();
  board_buttons_[index] = button.get();
  button->SetTag(index);
  button->SetAccessibilityLabel(kBoardSpaceLabel);
  button->SetButtonListener(this);
  button->SetLayoutFill(true);
  view->AddView(std::move(button));

  auto x_label = std::make_unique<ui::Image>();
  board_x_labels_[index] = x_label.get();
  x_label->SetImage(kXImage);
  x_label->SetAccessibilityLabel(kXLabel);
  x_label->SetVisible(false);
  view->AddView(std::move(x_label));

  auto o_label = std::make_unique<ui::Image>();
  board_o_labels_[index] = o_label.get();
  o_label->SetImage(kOImage);
  o_label->SetAccessibilityLabel(kOLabel);
  o_label->SetVisible(false);
  view->AddView(std::move(o_label));

  board->AddView(std::move(view));
}

void GameBoard::PlaceMark(int space) {
  DCHECK_NE(turn_, kPlayerNone);
  DCHECK_LT(space, 9);
  DCHECK_EQ(board_[space], kPlayerNone);

  // Update the board.
  board_[space] = turn_;

  // Send accessibility announcement for the placed item.
  std::string text = kPlayerName[turn_];
  text += kPlacedAnnouncement;
  text += kBoardSpaceName[space];
  root_view()->AccessibilityAnnounce(text);

  // Update the UI.
  board_buttons_[space]->SetVisible(false);
  if (turn_ == kPlayerX) {
    board_x_labels_[space]->SetVisible(true);
  } else {
    board_o_labels_[space]->SetVisible(true);
  }

  if (!CheckForWinner()) {
    turn_ = turn_ == kPlayerX ? kPlayerO : kPlayerX;
    UpdateTurnLabel();
  }
}

void GameBoard::TakeComputerTurn() {
  DCHECK_EQ(turn_, kPlayerO);
  pending_turn_ = nullptr;

  if (difficulty_ != kDifficultyEasy) {
    // Check if there is a winning move.
    int space = FindWinningMove(kPlayerO);
    if (space != -1) {
      PlaceMark(space);
      return;
    }

    // Block any winning move.
    space = FindWinningMove(kPlayerX);
    if (space != -1) {
      PlaceMark(space);
      return;
    }
  }

  // On impossible, find the best remaining move.
  if (difficulty_ == kDifficultyImpossible) {
    int space = FindBestMove(kPlayerO);
    if (space != -1) {
      PlaceMark(space);
      return;
    }
  }

  // Place in a random space.
  // Count the empty spaces.
  int empty_count = 0;
  for (int i = 0; i < 9; ++i) {
    if (board_[i] == kPlayerNone)
      empty_count++;
  }
  // Chose a random space from the ones available.
  int space_offset = Random::get()->NextDouble() * empty_count;
  DCHECK_LT(space_offset, 9);

  // Find the empty space and place the mark.
  for (int i = 0; i < 9; ++i) {
    if (board_[i] == kPlayerNone) {
      if (space_offset) {
        space_offset--;
      } else {
        PlaceMark(i);
        return;
      }
    }
  }
}

bool GameBoard::CheckForWinner() {
  DCHECK_NE(turn_, kPlayerNone);
  // Check rows and columns.
  for (int i = 0; i < 3; ++i) {
    // Check column.
    if (board_[i] != kPlayerNone) {
      if (board_[i + 3] == board_[i] && board_[i + 6] == board_[i]) {
        SetWinner(board_[i]);
        return true;
      }
    }

    // Check row.
    if (board_[3 * i] != kPlayerNone) {
      if (board_[3 * i + 1] == board_[3 * i] &&
          board_[3 * i + 2] == board_[3 * i]) {
        SetWinner(board_[3 * i]);
        return true;
      }
    }
  }

  // Diagonals
  if (board_[4] != kPlayerNone) {
    // Top left to bottom right.
    if (board_[0] == board_[4] && board_[8] == board_[4]) {
      SetWinner(board_[4]);
      return true;
    }

    // Top right to bottom left.
    if (board_[2] == board_[4] && board_[6] == board_[4]) {
      SetWinner(board_[4]);
      return true;
    }
  }

  // Check for available spaces.
  for (int i = 0; i < 9; ++i) {
    if (board_[i] == kPlayerNone)
      return false;
  }

  // No spaces remain, it's a draw.
  SetWinner(kPlayerNone);
  return true;
}

void GameBoard::SetWinner(Player player) {
  DCHECK_NE(turn_, kPlayerNone);

  turn_ = kPlayerNone;

  switch (player) {
    case kPlayerNone:
      status_label_->SetText(kDrawLabel);
      break;
    case kPlayerX:
      status_label_->SetText(kXWinsLabel);
      break;
    case kPlayerO:
      status_label_->SetText(kOWinsLabel);
      break;
  }
}

void GameBoard::UpdateTurnLabel() {
  DCHECK_NE(turn_, kPlayerNone);

  if (turn_ == kPlayerX) {
    status_label_->SetText(kXTurnLabel);
  } else {
    status_label_->SetText(kOTurnLabel);
  }
}

int GameBoard::FindWinningMove(Player player) {
  // Top left space
  if (board_[0] == kPlayerNone) {
    if ((board_[1] == player && board_[2] == player) ||  // Row
        (board_[3] == player && board_[6] == player) ||  // Column
        (board_[4] == player && board_[8] == player)) {  // Diagonal
      return 0;
    }
  }

  // Top middle space
  if (board_[1] == kPlayerNone) {
    if ((board_[0] == player && board_[2] == player) ||  // Row
        (board_[4] == player && board_[7] == player)) {  // Column
      return 1;
    }
  }

  // Top right space
  if (board_[2] == kPlayerNone) {
    if ((board_[0] == player && board_[1] == player) ||  // Row
        (board_[5] == player && board_[8] == player) ||  // Column
        (board_[6] == player && board_[4] == player)) {  // Diagonal
      return 2;
    }
  }

  // Center left space
  if (board_[3] == kPlayerNone) {
    if ((board_[4] == player && board_[5] == player) ||  // Row
        (board_[0] == player && board_[6] == player)) {  // Column
      return 3;
    }
  }

  // Center space
  if (board_[4] == kPlayerNone) {
    if ((board_[3] == player && board_[5] == player) ||  // Row
        (board_[1] == player && board_[7] == player) ||  // Column
        (board_[0] == player && board_[8] == player) ||  // Diagonal
        (board_[2] == player && board_[6] == player)) {  // Diagonal
      return 4;
    }
  }

  // Center right space
  if (board_[5] == kPlayerNone) {
    if ((board_[3] == player && board_[4] == player) ||  // Row
        (board_[2] == player && board_[8] == player)) {  // Column
      return 5;
    }
  }

  // Bottom left space
  if (board_[6] == kPlayerNone) {
    if ((board_[7] == player && board_[8] == player) ||  // Row
        (board_[0] == player && board_[3] == player) ||  // Column
        (board_[4] == player && board_[2] == player)) {  // Diagonal
      return 6;
    }
  }

  // Bottom middle space
  if (board_[7] == kPlayerNone) {
    if ((board_[6] == player && board_[8] == player) ||  // Row
        (board_[1] == player && board_[4] == player)) {  // Column
      return 7;
    }
  }

  // Bottom right space
  if (board_[8] == kPlayerNone) {
    if ((board_[6] == player && board_[7] == player) ||  // Row
        (board_[2] == player && board_[5] == player) ||  // Column
        (board_[0] == player && board_[4] == player)) {  // Diagonal
      return 8;
    }
  }

  // No winning move.
  return -1;
}

int GameBoard::FindBestMove(Player player) {
  DCHECK_NE(player, kPlayerNone);
  const Player opponent = (player == kPlayerX) ? kPlayerO : kPlayerX;

  // Try the center.
  if (board_[4] == kPlayerNone) {
    return 4;
  }

  // If the opponent has two opposite corners and no side, grab a side.
  if ((board_[0] == opponent && board_[2] == kPlayerNone &&
       board_[6] == kPlayerNone && board_[8] == opponent) ||
      (board_[0] == kPlayerNone && board_[2] == opponent &&
       board_[6] == opponent && board_[8] == kPlayerNone)) {
    if (board_[1] == kPlayerNone && board_[3] == kPlayerNone &&
        board_[5] == kPlayerNone && board_[7] == kPlayerNone) {
      return 1;
    }
  }

  // Grab a corner.
  if (board_[0] != player && board_[2] != player && board_[6] != player &&
      board_[8] != player) {
    if (board_[0] == kPlayerNone && board_[8] == kPlayerNone)
      return 0;
    if (board_[2] == kPlayerNone && board_[6] == kPlayerNone)
      return 2;
  }

  // If we have a corner, grab the matching side.
  // Top left corner.
  if (board_[0] == player) {
    // Try the middle right.
    if (board_[1] == kPlayerNone && board_[2] == kPlayerNone &&
        board_[5] == kPlayerNone) {
      return 5;
    }
    // Try the middle bottom.
    if (board_[3] == kPlayerNone && board_[6] == kPlayerNone &&
        board_[7] == kPlayerNone) {
      return 7;
    }
  }

  // Top right corner.
  if (board_[2] == player) {
    // Try the middle left.
    if (board_[0] == kPlayerNone && board_[1] == kPlayerNone &&
        board_[3] == kPlayerNone) {
      return 3;
    }
    // Try the middle bottom.
    if (board_[5] == kPlayerNone && board_[8] == kPlayerNone &&
        board_[7] == kPlayerNone) {
      return 7;
    }
  }

  // Nothing to be done.
  return -1;
}

// InputListener:
bool GameBoard::OnKeyEvent(const KeyEvent& event) {
  if (event.key_code() == VKEY_BACK) {
    if (event.type() == KeyEvent::kKeyUp) {
      listener_->OnCloseBoard();
    }
    return true;
  }
  return false;
}

// ui::Button::Listener:
void GameBoard::OnClick(ui::Button* button) {
  if (turn_ != kPlayerX || board_[button->tag()] != kPlayerNone)
    return;

  PlaceMark(button->tag());

  if (turn_ == kPlayerO) {
    root_view()->PostUiTaskDelayed(std::make_unique<DelayedTurn>(this),
                                   TimeInterval::FromSeconds(0.5));
  }
}

}  // namespace Tictactoe
