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
  void Place_Avatar_Tile(Avatar_Controller_Mutator *mutator);
  void Load_Tilesets(std::wstring wtileset_path);

  Tunnelour::Avatar_Component *m_avatar;
  
  Tunnelour::Game_Settings_Component* m_game_settings;

  bool m_has_avatar_been_generated;

  std::string m_running_metadata_file_path;
  std::string m_walking_metadata_file_path;
  std::string m_standing_metadata_file_path;

  Tileset_Helper::Animation_Tileset_Metadata m_running_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_walking_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_standing_metadata;

  bool Init_Timer();
  void Update_Timer();

  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  bool m_animation_tick;
  int m_current_animation_fps;

  bool DoTheseTilesXCollide(Tunnelour::Frame_Component* TileA, Tunnelour::Frame_Component* TileB);
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_CONTROLLER_H_
