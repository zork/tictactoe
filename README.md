# Accessible Tic Tac Toe
This is an example of a simple game written in C++/OpenGL that exposes its UI to the Android accessibility APIs.

## Building
* Assemble debug build:
  - $ ./gradlew assembleFatDebug
* Install debug build on connected device:
  - $ ./gradlew installFatDebug

## Project layout
Activity and native interface:
https://github.com/zork/tictactoe/tree/master/app/src/main/java/com/zork/common

Android/C++ bindings:
https://github.com/zork/tictactoe/tree/master/app/src/main/jni/platform/android

UI library:
https://github.com/zork/tictactoe/tree/master/app/src/main/jni/game/ui
