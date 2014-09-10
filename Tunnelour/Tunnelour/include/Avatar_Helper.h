//  Copyright 2014 Sean MacDonnell
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

#ifndef TUNNELOUR_AVATAR_HELPER_H_
#define TUNNELOUR_AVATAR_HELPER_H_

#include "Bitmap_Component.h"
#include "Avatar_Component.h"
#include "Tile_Bitmap.h"
#include "Tileset_Helper.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                middleground (Layer 0)
//-----------------------------------------------------------------------------
class Avatar_Helper {
 public:
  //---------------------------------------------------------------------------
  // Description : This struct contains information about a Tile Collision
  //             : usually used to store the tile the avatar has collided with
  //---------------------------------------------------------------------------
  struct Tile_Collision {
    Bitmap_Component* colliding_tile;
    std::string collision_side;
  };

  //---------------------------------------------------------------------------
  // Description : This struct contains a stored avatar state and position
  //---------------------------------------------------------------------------
  struct Avatar_Stored_State {
    Avatar_Component::Avatar_State state;
    D3DXVECTOR3 position;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Avatar_Helper();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Avatar_Helper();

  //---------------------------------------------------------------------------
  // Description : Returns true if the avatar is standing on a floor
  //---------------------------------------------------------------------------
  static bool IsAvatarFloorAdjacent(Avatar_Component *m_avatar, std::vector<Tile_Bitmap*> *adjacent_tiles, std::vector<Tile_Bitmap*> *floor_tiles);

  static Avatar_Component::Avatar_Collision_Block GetNamedCollisionBlock(std::string id, std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks);

  static Bitmap_Component* Avatar_Helper::CollisionBlockToBitmapComponent(Avatar_Component::Avatar_Collision_Block avatar_collision_block, D3DXVECTOR3 position);
  
  static void Avatar_Helper::SetAvatarState(Avatar_Component *m_avatar, std::wstring tileset_path, std::vector<Tileset_Helper::Animation_Tileset_Metadata> *animation_metadata, std::string new_state_parent_name, std::string new_state_name, std::string direction, std::string *current_metadata_file_path, Tileset_Helper::Animation_Tileset_Metadata *current_metadata, Tileset_Helper::Animation_Subset *current_animation_subset);

  static Avatar_Component::Avatar_Collision_Block TilesetCollisionBlockToAvatarCollisionBlock(Tileset_Helper::Avatar_Collision_Block tileset_avatar_collision_block, float tileset_animation_top_left_y, int state_index, std::string direction);
  
  static void AlignAvatarOnLastContactingFoot(Avatar_Component *avatar);

  static void AlignAvatarOnRightFoot(Avatar_Component *avatar);

  static void AlignAvatarOnLeftFoot(Avatar_Component *avatar);

  static void AlignAvatarOnLastAvatarCollisionBlockRightBottom(Avatar_Component *avatar);

  static void AlignAvatarOnLastAvatarCollisionBlockLeftBottom(Avatar_Component *avatar);

  static void MoveAvatarTileAdjacent(Avatar_Component *avatar, std::string direction, Bitmap_Component* tile);

  static void SetAvatarStateAnimationFrame(Avatar_Component *avatar, unsigned int new_state_index, Tileset_Helper::Animation_Subset *current_animation_subset);

  static bool IsAvatarWallColliding(Avatar_Component *avatar, std::vector<Tile_Collision> *out_collisions, std::vector<Tile_Bitmap*> *wall_tiles);

  static void AlignAvatarOnLastAvatarCollisionBlock(Avatar_Component *avatar);

  static bool IsAvatarFloorColliding(Avatar_Component *avatar, std::vector<Avatar_Helper::Tile_Collision> *out_collisions, std::vector<Tile_Bitmap*> *floor_tiles);

  static bool CanAvatarGrabALedge(Avatar_Component *avatar, std::vector<Avatar_Helper::Tile_Collision> *out_collisions, std::vector<Tile_Bitmap*> *ledge_tiles);

  static void AlignAvatarOnLastLedgeEdge(Avatar_Component *avatar, Avatar_Helper::Tile_Collision ledge);

  static void AlignAvatarOnLastHand(Avatar_Component *avatar);

  static void AlignAvatarOnLastAvatarCollisionBlockRightTop(Avatar_Component *avatar);

  static void AlignAvatarOnLastAvatarCollisionBlockLeftTop(Avatar_Component *avatar);

 protected:

 private:
  
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_HELPER_H_
