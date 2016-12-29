////
// main_menu.cpp
////

#include "tictactoe/ui/main_menu.h"

#include "game/ui/grid.h"

namespace {
const char kMainMenuTitle[] = "Main Menu";
const char kTicTacToeLabel[] = "Tic Tac Toe";
const char kNewGameLabel[] = "New Game";
const char kEasyLabel[] = "Easy";
const char kHardLabel[] = "Hard";
const char kImpossibleLabel[] = "Impossible";
}

namespace Tictactoe {

MainMenu::MainMenu(Listener* listener) : listener_(listener) {
  // Set the title.
  SetTitle(kMainMenuTitle);

  // Add the status label.
  auto status_label = std::make_unique<ui::Label>();
  status_label->SetLayoutHeight(100);
  status_label->SetLayoutHAlign(ui::View::kHAlignCenter);
  status_label->SetLayoutVAlign(ui::View::kVAlignTop);
  status_label->SetTextHAlign(ui::View::kHAlignCenter);
  status_label->SetFontSize(72);
  status_label->SetText(kTicTacToeLabel);
  AddView(std::move(status_label));

  // Add the buttons in a grid layout.
  auto grid = std::make_unique<ui::Grid>();
  grid->SetAccessibilityLabel(kNewGameLabel);
  grid->SetColumns(1);
  easy_button_ = AddDifficultyButton(grid.get(), kEasyLabel);
  hard_button_ = AddDifficultyButton(grid.get(), kHardLabel);
  impossible_button_ = AddDifficultyButton(grid.get(), kImpossibleLabel);

  // Add the grid to this view.
  AddView(std::move(grid));
}

MainMenu::~MainMenu() {}

// private:
ui::Button* MainMenu::AddDifficultyButton(ui::View* parent, const char* label) {
  auto button = std::make_unique<ui::Button>();
  ui::Button* button_ptr = button.get();
  button->SetImage(kButtonImage);
  button->SetText(label);
  button->SetButtonListener(this);
  parent->AddView(std::move(button));

  return button_ptr;
}

// ui::Button::Listener:
void MainMenu::OnClick(ui::Button* button) {
  if (button == easy_button_) {
    listener_->OnStartGame(kDifficultyEasy);
  } else if (button == hard_button_) {
    listener_->OnStartGame(kDifficultyHard);
  } else if (button == impossible_button_) {
    listener_->OnStartGame(kDifficultyImpossible);
  }
}

}  // namespace Tictactoe
