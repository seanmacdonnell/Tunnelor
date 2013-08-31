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

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"
#include "Avatar_Controller_Mutator.h"
#include "Avatar_Component.h"
#include <windows.h>
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
  void Generate_Avatar_Tile();

  void Generate_Avatar();

  void Run_Avatar_State(Avatar_Controller_Mutator *mutator);
  void Run_Standing_State(Avatar_Controller_Mutator *mutator);
  void Run_Walking_State(Avatar_Controller_Mutator *mutator);
  void Run_Falling_State(Avatar_Controller_Mutator *mutator);

  bool Is_Avatar_Platform_Adjacent(Avatar_Controller_Mutator *mutator);
  bool Is_Avatar_Platform_Colliding(Avatar_Controller_Mutator *mutator, std::list<Tunnelour::Bitmap_Component*> *out_colliding_border_tiles, Tunnelour::Bitmap_Component *out_collision_block);
  void Load_Tilesets(std::wstring wtileset_path);

  D3DXVECTOR3 Align_Avatar_On_Right_Foot(Avatar_Controller_Mutator *mutator);

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

  bool Init_Timer();
  void Update_Timer();

  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  bool m_animation_tick;
  int m_current_animation_fps;

  void ChangeAvatarState(std::string new_state_name, std::string direction);
  void UpdateAvatarState(int new_state_index);

  float m_current_y_velocity;
  float m_current_x_velocity;
  float m_gravity;
  float m_init_velocity;
  float m_angle; //RADIANS

  D3DXVECTOR3 CalculateArc();
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_CONTROLLER_H_
