//  Copyright 2012 Sean MacDonnell
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include "Bitmap_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Bitmap_Helper::Bitmap_Helper() {
}

//------------------------------------------------------------------------------
Bitmap_Helper::~Bitmap_Helper() {
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::DoTheseTilesYCollide(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_top, a_tile_bottom;
  a_tile_top = TileA->GetPosition().y + static_cast<float>(TileA->GetSize().y / 2);
  a_tile_bottom = TileA->GetPosition().y - static_cast<float>(TileA->GetSize().y / 2);

  float b_tile_top, b_tile_bottom;
  b_tile_top = TileB->GetPosition().y + static_cast<float>(TileB->GetSize().y / 2);
  b_tile_bottom = TileB->GetPosition().y - static_cast<float>(TileB->GetSize().y / 2);

  if (b_tile_top < a_tile_top && b_tile_top > a_tile_bottom) {
    return true;
  }

  if (b_tile_bottom < a_tile_top && b_tile_bottom > a_tile_bottom) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::AreTheseTilesYAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_top, a_tile_bottom;
  a_tile_top = TileA->GetPosition().y + static_cast<float>(TileA->GetSize().y / 2);
  a_tile_bottom = TileA->GetPosition().y - static_cast<float>(TileA->GetSize().y / 2);

  float b_tile_top, b_tile_bottom;
  b_tile_top = TileB->GetPosition().y + static_cast<float>(TileB->GetSize().y / 2);
  b_tile_bottom = TileB->GetPosition().y - static_cast<float>(TileB->GetSize().y / 2);

  if (b_tile_top == a_tile_top  || b_tile_top == a_tile_bottom) {
    return true;
  }
    
  if (b_tile_bottom == a_tile_top  || b_tile_bottom == a_tile_bottom) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::DoTheseTilesXCollide(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_left, a_tile_right;
  a_tile_left = TileA->GetPosition().x - static_cast<float>(TileA->GetSize().x / 2);
  a_tile_right = TileA->GetPosition().x + static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_left, b_tile_right;
  b_tile_left = TileB->GetPosition().x - static_cast<float>(TileB->GetSize().x / 2);
  b_tile_right = TileB->GetPosition().x + static_cast<float>(TileB->GetSize().x / 2);

  if (b_tile_left > a_tile_left && b_tile_left < a_tile_right) {
    return true;
  }

  if (b_tile_right > a_tile_left && b_tile_right < a_tile_right) {
    return true;
  }

  if (b_tile_left == a_tile_left && b_tile_right == a_tile_right) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::AreTheseTilesXAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_left, a_tile_right;
  a_tile_left = TileA->GetPosition().x - static_cast<float>(TileA->GetSize().x / 2);
  a_tile_right = TileA->GetPosition().x + static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_left, b_tile_right;
  b_tile_left = TileB->GetPosition().x - static_cast<float>(TileB->GetSize().x / 2);
  b_tile_right = TileB->GetPosition().x + static_cast<float>(TileB->GetSize().x / 2);

  if (b_tile_left == a_tile_left  || b_tile_left == a_tile_right) {
    return true;
  }
    
  if (b_tile_right == a_tile_left  || b_tile_right == a_tile_right) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::AreTheseTilesLeftXAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  float a_tile_left;
  a_tile_left = TileA->GetPosition().x - static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_left;
  b_tile_left = TileB->GetPosition().x - static_cast<float>(TileB->GetSize().x / 2);

  if (b_tile_left == a_tile_left) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::AreTheseTilesRightXAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  float a_tile_right;
  a_tile_right = TileA->GetPosition().x + static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_right;
  b_tile_right = TileB->GetPosition().x + static_cast<float>(TileB->GetSize().x / 2);

  if (b_tile_right == a_tile_right) {
    return true;
  }

  return false;
}


} // Tunnelour