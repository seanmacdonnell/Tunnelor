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

#ifndef TUNNELOUR_AVATAR_CONTROLLER_H_
#define TUNNELOUR_AVATAR_CONTROLLER_H_

#include <string>
#include <vector>

#include "Avatar_Component.h"
#include "Avatar_Helper.h"
#include "Avatar_Controller_Mutator.h"
#include "Bitmap_Helper.h"
#include "Component_Composite.h"
#include "Controller.h"
#include "Charlie_Standing_Controller.h"
#include "Charlie_Falling_Controller.h"
#include "Charlie_Running_Controller.h"
#include "Charlie_Jumping_Controller.h"
#include "Charlie_Climbing_Controller.h"
#include "Exceptions.h"
#include "Game_Settings_Component.h"
#include "Level_Component.h"
#include "String_Helper.h"
#include "Tile_Bitmap.h"
#include "Tileset_Helper.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the creation and mutation
//                of the avatar_component.
//-----------------------------------------------------------------------------
class Avatar_Controller: public Controller,
                         public Component_Composite::Component_Composite_Type_Observer {
 public:
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
  virtual bool Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

  //---------------------------------------------------------------------------
  // Description : Called when handling an "Add" call in the model
  //---------------------------------------------------------------------------
  virtual void HandleEventAdd(Tunnelour::Component * const component);

  //---------------------------------------------------------------------------
  // Description : Called when handling a "Remove" call in the model
  //---------------------------------------------------------------------------
  virtual void HandleEventRemove(Tunnelour::Component * const component);

  //---------------------------------------------------------------------------
  // Description : Hides the avatar component so it isn't rendered
  //---------------------------------------------------------------------------
  void HideAvatar();

  //---------------------------------------------------------------------------
  // Description : Shows the avatar component so it can be rendered
  //---------------------------------------------------------------------------
  void ShowAvatar();

  //---------------------------------------------------------------------------
  // Description : Resets the Avatar_Component tile to defaults
  //---------------------------------------------------------------------------
  void ResetAvatarToDefaults();

 protected:
 private:
  //---------------------------------------------------------------------------
  // Description : Generates the Avatar_Component tile
  //---------------------------------------------------------------------------
  void CreateAvatar();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the state of the avatar
  //---------------------------------------------------------------------------
  void RunAvatarState();

  //---------------------------------------------------------------------------
  // Description : Loads all the tile animations into the controller
  //---------------------------------------------------------------------------
  void LoadTilesets(std::wstring wtileset_path);

  //---------------------------------------------------------------------------
  // Description : Initialises the timer used for the animation ticks
  //---------------------------------------------------------------------------
  bool InitTimer();

  //---------------------------------------------------------------------------
  // Description : Updates and determines if the animation needs to update
  //---------------------------------------------------------------------------
  void UpdateTimer();

  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  Avatar_Component *m_avatar;
  Game_Settings_Component* m_game_settings;
  Level_Component *m_level;
  World_Settings_Component *m_world_settings;

  std::string m_current_metadata_file_path;
  Tileset_Helper::Animation_Tileset_Metadata m_current_metadata;
  Tileset_Helper::Animation_Subset m_current_animation_subset;
  std::vector<Tileset_Helper::Animation_Tileset_Metadata> m_animation_metadata;

  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  int m_last_frame_time;
  bool m_animation_tick;
  int m_current_animation_fps;

  std::vector<Tile_Bitmap*> m_floor_tiles;
  std::vector<Tile_Bitmap*> m_wall_tiles;
  std::vector<Tile_Bitmap*> m_ledge_tiles;

  float m_y_fallen;

  int m_distance_traveled;

  Bitmap_Component *m_currently_grabbed_tile;
  Avatar_Helper::Tile_Collision m_adjacent_wall;
  bool m_is_moving_continuously;

  int m_vertical_jump_y_initial_velocity;
  int m_vertical_jump_x_initial_velocity;

  int m_wall_jump_y_initial_velocity;

  std::string m_avatar_initial_direction;
  std::string m_avatar_initial_parent_state;
  std::string m_avatar_initial_state;
  D3DXVECTOR3 m_avatar_initial_position;

  Charlie_Standing_Controller m_charlie_standing_controller;
  Charlie_Falling_Controller m_charlie_falling_controller;
  Charlie_Running_Controller m_charlie_running_controller;
  Charlie_Jumping_Controller m_charlie_jumping_controller;
  Charlie_Climbing_Controller m_charlie_climbing_controller;

  float m_avatar_z_position;

  std::wstring m_running_file_name;
  std::wstring m_standing_file_name;
  std::wstring m_falling_file_name;
  std::wstring m_jumping_file_name;
  std::wstring m_climbing_file_name;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_CONTROLLER_H_
