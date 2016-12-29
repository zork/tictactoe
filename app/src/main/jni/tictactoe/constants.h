////
// constants.h
////

#pragma once

namespace Tictactoe {

extern const char kButtonImage[];

enum Difficulty {
  kDifficultyEasy,
  kDifficultyHard,
  kDifficultyImpossible,
};

enum Player {
  kPlayerNone,
  kPlayerX,
  kPlayerO,
};

}  // namespace Tictactoe
