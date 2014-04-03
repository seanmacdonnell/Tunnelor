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
#include "String_Helper.h"

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
bool Bitmap_Helper::DoTheseTilesIntersect(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  bool is_y_colliding = DoTheseTilesYCollide(TileA, TileB);
  bool is_x_colliding = DoTheseTilesXCollide(TileA, TileB);

  return is_y_colliding && is_x_colliding;
}

//------------------------------------------------------------------------------
std::string Bitmap_Helper::DoesTileACollideOnTheTopOrBottom(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one y vertex in TileA is contained in the TileB.
  std::string collision_side = "";

  float a_tile_top, a_tile_bottom;
  a_tile_top = TileA->GetPosition()->y + static_cast<float>(TileA->GetSize().y / 2);
  a_tile_bottom = TileA->GetPosition()->y - static_cast<float>(TileA->GetSize().y / 2);

  float b_tile_top, b_tile_bottom;
  b_tile_top = TileB->GetPosition()->y + static_cast<float>(TileB->GetSize().y / 2);
  b_tile_bottom = TileB->GetPosition()->y - static_cast<float>(TileB->GetSize().y / 2);

  if (b_tile_top < a_tile_top && b_tile_top > a_tile_bottom) {
    collision_side = "Bottom";
  }

  if (b_tile_bottom < a_tile_top && b_tile_bottom > a_tile_bottom) {
    collision_side = "Top";
  }

  if (a_tile_top < b_tile_top && a_tile_top > b_tile_bottom) {
    collision_side = "Top";
  }

  if (a_tile_bottom < b_tile_top && a_tile_bottom > b_tile_bottom) {
    collision_side = "Bottom";
  }

  if (a_tile_top == b_tile_top  && a_tile_bottom == b_tile_bottom) {
    collision_side = "Adjacent";
  }

  return collision_side;
}

//------------------------------------------------------------------------------
std::string Bitmap_Helper::DoesTileACollideOnTheRightOrLeft(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.
  std::string collision_side = "";

  float a_tile_left, a_tile_right;
  a_tile_left = TileA->GetPosition()->x - static_cast<float>(TileA->GetSize().x / 2);
  a_tile_right = TileA->GetPosition()->x + static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_left, b_tile_right;
  b_tile_left = TileB->GetPosition()->x - static_cast<float>(TileB->GetSize().x / 2);
  b_tile_right = TileB->GetPosition()->x + static_cast<float>(TileB->GetSize().x / 2);

  if (b_tile_left > a_tile_left && b_tile_left < a_tile_right) {
    collision_side = "Right";
  }

  if (b_tile_right < a_tile_right && b_tile_right > a_tile_left) {
    collision_side = "Left";
  }

  if (a_tile_left > b_tile_left && a_tile_left < b_tile_right) {
    collision_side = "Left";
  }

  if (a_tile_right < b_tile_right && a_tile_right > b_tile_left) {
    collision_side = "Right";
  }

  if (b_tile_left == a_tile_left && b_tile_right == a_tile_right) {
    collision_side = "Adjacent";
  }

  return collision_side;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::DoTheseTilesYCollide(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one y vertex in TileA is contained in the TileB.

  float a_tile_top, a_tile_bottom;
  a_tile_top = TileA->GetPosition()->y + static_cast<float>(TileA->GetSize().y / 2);
  a_tile_bottom = TileA->GetPosition()->y - static_cast<float>(TileA->GetSize().y / 2);

  float b_tile_top, b_tile_bottom;
  b_tile_top = TileB->GetPosition()->y + static_cast<float>(TileB->GetSize().y / 2);
  b_tile_bottom = TileB->GetPosition()->y - static_cast<float>(TileB->GetSize().y / 2);

  if (b_tile_top < a_tile_top && b_tile_top > a_tile_bottom) {
    return true;
  }

  if (b_tile_bottom < a_tile_top && b_tile_bottom > a_tile_bottom) {
    return true;
  }

  if (a_tile_top < b_tile_top && a_tile_top > b_tile_bottom) {
    return true;
  }

  if (a_tile_bottom < b_tile_top && a_tile_bottom > b_tile_bottom) {
    return true;
  }

  if (a_tile_top == b_tile_top  || a_tile_bottom == b_tile_bottom) {
    return true;
  }



  return false;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::AreTheseTilesYAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_top, a_tile_bottom;
  a_tile_top = TileA->GetTopLeftPostion().y;
  a_tile_bottom = TileA->GetBottomRightPostion().y;

  float b_tile_top, b_tile_bottom;
  b_tile_top = TileB->GetTopLeftPostion().y;
  b_tile_bottom = TileB->GetBottomRightPostion().y;

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
  a_tile_left = TileA->GetPosition()->x - static_cast<float>(TileA->GetSize().x / 2);
  a_tile_right = TileA->GetPosition()->x + static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_left, b_tile_right;
  b_tile_left = TileB->GetPosition()->x - static_cast<float>(TileB->GetSize().x / 2);
  b_tile_right = TileB->GetPosition()->x + static_cast<float>(TileB->GetSize().x / 2);

  if (b_tile_left > a_tile_left && b_tile_left < a_tile_right) {
    return true;
  }

  if (b_tile_right < a_tile_right && b_tile_right > a_tile_left) {
    return true;
  }

  if (a_tile_left > b_tile_left && a_tile_left < b_tile_right) {
    return true;
  }

  if (a_tile_right < b_tile_right && a_tile_right > b_tile_left) {
    return true;
  }

  if (b_tile_left == a_tile_left || b_tile_right == a_tile_right) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::AreTheseTilesXAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_left, a_tile_right;
  a_tile_left = TileA->GetPosition()->x - static_cast<float>(TileA->GetSize().x / 2);
  a_tile_right = TileA->GetPosition()->x + static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_left, b_tile_right;
  b_tile_left = TileB->GetPosition()->x - static_cast<float>(TileB->GetSize().x / 2);
  b_tile_right = TileB->GetPosition()->x + static_cast<float>(TileB->GetSize().x / 2);

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
  a_tile_left = TileA->GetPosition()->x - static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_left;
  b_tile_left = TileB->GetPosition()->x - static_cast<float>(TileB->GetSize().x / 2);

  if (b_tile_left == a_tile_left) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Bitmap_Helper::AreTheseTilesRightXAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  float a_tile_right;
  a_tile_right = TileA->GetPosition()->x + static_cast<float>(TileA->GetSize().x / 2);

  float b_tile_right;
  b_tile_right = TileB->GetPosition()->x + static_cast<float>(TileB->GetSize().x / 2);

  if (b_tile_right == a_tile_right) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
Tile_Bitmap* Bitmap_Helper::CollisionBlockToBitmapComponent(Avatar_Component::Avatar_Collision_Block avatar_collision_block, Bitmap_Component *avatar, Tileset_Helper::Tileset_Metadata debug_tileset_metadata, std::wstring tileset_path) {
  Tile_Bitmap *collision_bitmap = new Tile_Bitmap();

  Tileset_Helper::Line middleground_line = (*debug_tileset_metadata.tilesets.begin()->lines.begin());

  collision_bitmap->GetTexture()->transparency = 0.0f;

  std::wstring texture_path = tileset_path;
  texture_path += String_Helper::StringToWString(debug_tileset_metadata.filename);
  collision_bitmap->GetTexture()->texture_path = texture_path;
  collision_bitmap->GetTexture()->texture_size = D3DXVECTOR2(debug_tileset_metadata.size_x,
                                                            debug_tileset_metadata.size_y);
  collision_bitmap->GetTexture()->tile_size = D3DXVECTOR2(middleground_line.tile_size_x,
                                                          middleground_line.tile_size_y);

  D3DXVECTOR3 collision_bitmap_position;
  collision_bitmap_position.x = avatar->GetPosition()->x + avatar_collision_block.offset_from_avatar_centre.x;
  collision_bitmap_position.y = avatar->GetPosition()->y + avatar_collision_block.offset_from_avatar_centre.y;
  
  float random_line_tile = 0;
  if (avatar_collision_block.id.compare("Left_Foot") == 0) {
    random_line_tile = 2;
    collision_bitmap_position.z = -2.0;
  }
  if (avatar_collision_block.id.compare("Right_Foot") == 0) {
    random_line_tile = 3;
    collision_bitmap_position.z = -2.0;
  }
  if (avatar_collision_block.id.compare("Avatar") == 0) {
    if (avatar_collision_block.is_contacting) {
      random_line_tile = 1;
    } else {
      random_line_tile = 0;
    }
    collision_bitmap_position.z = -0.5;
  }

  collision_bitmap->SetPosition(collision_bitmap_position);

  float random_tile_x = random_line_tile * middleground_line.tile_size_x;
  random_tile_x += middleground_line.top_left_x;

  float random_tile_y = middleground_line.top_left_y;

  collision_bitmap->GetTexture()->top_left_position = D3DXVECTOR2(random_tile_x, random_tile_y);

  D3DXVECTOR2 size = avatar_collision_block.size;
  collision_bitmap->SetSize(D3DXVECTOR2(middleground_line.tile_size_x, middleground_line.tile_size_y));
  collision_bitmap->SetScale(new D3DXVECTOR3((avatar_collision_block.size.x/128), (avatar_collision_block.size.y/128), 1.0f));
  //collision_bitmap->SetSize(middleground_line.tile_size_x, middleground_line.tile_size_x);

  return collision_bitmap;
}

} // Tunnelour