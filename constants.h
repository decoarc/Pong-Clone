#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
  constexpr int kWindowWidth = 1280;
  constexpr int kWindowHeight = 720;
  constexpr int kGameWidth = 80;
  constexpr int kGameHeight = 24;
  
  constexpr int kCellWidth = kWindowWidth / kGameWidth;
  constexpr int kCellHeight = kWindowHeight / kGameHeight;

  constexpr int kTopOffset = 2; 
  constexpr int kBottomOffset = 1;

  constexpr int kPaddleHeight = 5;
  constexpr int kPaddleMargin = 2;
  constexpr int kRightPaddleMargin = 1;
  constexpr int kBallSize = 20;
}

#endif

