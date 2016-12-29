////
// main_menu.h
////

#pragma once

#include "base/macros.h"
#include "game/ui/button.h"
#include "game/ui/view.h"
#include "tictactoe/constants.h"

namespace Tictactoe {

class MainMenu : public ui::View, public ui::Button::Listener {
 public:
  class Listener {
   public:
    virtual ~Listener() {}
    virtual void OnStartGame(Difficulty difficulty) = 0;
  };

  MainMenu(Listener* listener);
  ~MainMenu() override;
  DISALLOW_COPY_AND_ASSIGN(MainMenu);

 private:
  ui::Button* AddDifficultyButton(ui::View* parent, const char* label);

  // ui::Button::Listener:
  void OnClick(ui::Button* button) override;

  Listener* listener_;

  ui::Button* easy_button_;
  ui::Button* hard_button_;
  ui::Button* impossible_button_;
};

}  // namespace Tictactoe
