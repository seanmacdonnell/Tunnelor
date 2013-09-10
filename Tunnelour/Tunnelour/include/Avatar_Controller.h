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

#ifndef TUNNELOUR_AVATAR_CONTROLLER_H_
#define TUNNELOUR_AVATAR_CONTROLLER_H_

#include <windows.h>
#include <vector>
#include <string>
#include <list>

#include "Avatar_Component.h"
#include "Avatar_Controller_Mutator.h"
#include "Component_Composite.h"
#include "Controller.h"
#include "Game_Settings_Component.h"
#include "Tile_Bitmap.h"
#include "Tileset_Helper.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                middleground (Layer 0)
//-----------------------------------------------------------------------------
class Avatar_Controller: public Tunnelour::Controller {
 public:
  struct Collision {
    Tunnelour::Tile_Bitmap* a_tile;
    Tunnelour::Tile_Bitmap* b_tile;
    D3DXVECTOR2 a_tile_collision_point;
    D3DXVECTOR2 b_tile_collision_point;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Avatar_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Avatar_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Generates the Avatar_Component tile
  //---------------------------------------------------------------------------
  void GenerateAvatarTile();

  //---------------------------------------------------------------------------
  // Description : Creates the Avatar_Component and adds it to the model
  //---------------------------------------------------------------------------
  void GenerateAvatar();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the state of the avatar
  //---------------------------------------------------------------------------
  void RunAvatarState(Avatar_Controller_Mutator *mutator);

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the standing state of the avatar
  //---------------------------------------------------------------------------
  void RunStandingState(Avatar_Controller_Mutator *mutator);

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the walking state of the avatar
  //---------------------------------------------------------------------------
  void RunWalkingState(Avatar_Controller_Mutator *mutator);

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the falling state of the avatar
  //---------------------------------------------------------------------------
  void RunFallingState(Avatar_Controller_Mutator *mutator);

  //---------------------------------------------------------------------------
  // Description : Returns true if the avatar is standing on a platform
  //---------------------------------------------------------------------------
  bool IsAvatarPlatformAdjacent(Avatar_Controller_Mutator *mutator);

  //---------------------------------------------------------------------------
  // Description : Returns true if the avatar is colliding with a platform
  //---------------------------------------------------------------------------
  bool IsAvatarPlatformColliding(Avatar_Controller_Mutator *mutator,
                                 std::list<Tunnelour::Bitmap_Component*> *out_colliding_border_tiles,
                                 Tunnelour::Bitmap_Component *out_collision_block);

  //---------------------------------------------------------------------------
  // Description : Loads all the tile animations into the controller
  //---------------------------------------------------------------------------
  void LoadTilesets(std::wstring wtileset_path);

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with the right foot position
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  D3DXVECTOR3 AlignAvatarOnRightFoot(Avatar_Controller_Mutator *mutator);

  //---------------------------------------------------------------------------
  // Description : Initialises the timer used for the animation ticks
  //---------------------------------------------------------------------------
  bool InitTimer();

  //---------------------------------------------------------------------------
  // Description : Updates and determines if the animation needs to update
  //---------------------------------------------------------------------------
  void UpdateTimer();

  //---------------------------------------------------------------------------
  // Description : Changes the avatar animation and state and/or direction
  //---------------------------------------------------------------------------
  void ChangeAvatarState(std::string new_state_name, std::string direction);

  //---------------------------------------------------------------------------
  // Description : Updates the avatar state animation
  //---------------------------------------------------------------------------
  void UpdateAvatarState(int new_state_index);

  //---------------------------------------------------------------------------
  // Description : Returns the lowest collision block on the avatar
  //---------------------------------------------------------------------------
  Avatar_Component::Collision_Block GetLowestCollisionBlock();

  //---------------------------------------------------------------------------
  // Description : Creates a Bitmap Component using the given collision block
  //---------------------------------------------------------------------------
  Tunnelour::Bitmap_Component CollisionBlockToBitmapComponent(Avatar_Component::Collision_Block collision_block);


  //---------------------------------------------------------------------------
  // Description : Creates a Bitmap Component using the given collision block
  //---------------------------------------------------------------------------
  Avatar_Component::Collision_Block GetNamedCollisionBlock(std::string id, std::list<Avatar_Component::Collision_Block> collision_blocks);

  //---------------------------------------------------------------------------
  // Description : Creates an Avatar_Component::Collision_Block from a
  //             : Tileset_Helper::Collision_Block
  //---------------------------------------------------------------------------
  Avatar_Component::Collision_Block TilesetCollisionBlockToAvatarCollisionBlock(Tileset_Helper::Collision_Block tileset_collision_block,
                                                                                int tileset_animation_top_left_y,
                                                                                int state_index);

  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  Tunnelour::Avatar_Component *m_avatar;
  Tunnelour::Game_Settings_Component* m_game_settings;
  bool m_has_avatar_been_generated;
  std::string m_running_metadata_file_path;
  std::string m_walking_metadata_file_path;
  std::string m_standing_metadata_file_path;
  std::string m_falling_metadata_file_path;
  std::string m_current_metadata_file_path;
  Tileset_Helper::Animation_Tileset_Metadata m_running_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_walking_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_standing_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_falling_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_current_metadata;
  Tileset_Helper::Animation_Subset m_current_animation_subset;
  std::list<Tileset_Helper::Animation_Tileset_Metadata> m_animation_metadata;
  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  bool m_animation_tick;
  int m_current_animation_fps;
  Tunnelour::World_Settings_Component *m_world_settings;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_CONTROLLER_H_
