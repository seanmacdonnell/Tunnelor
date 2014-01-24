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

#ifndef TUNNELOUR_LEVEL_CONTROLLER_H_
#define TUNNELOUR_LEVEL_CONTROLLER_H_

#include "Game_Settings_Component.h"
#include "Level_Controller_Mutator.h"
#include "Avatar_Component.h"
#include "Controller.h"
#include "Level_Component.h"
#include "Text_Component.h"
#include "Camera_Component.h"
#include "Background_Controller.h"
#include "Middleground_Controller.h"
#include "Splash_Screen_Component.h"
#include "Level_Tile_Controller.h"
#include "Level_Transition_Controller.h"
#include "Avatar_Controller.h"
#include "Screen_Wipeout_Controller.h"
#include "Game_Over_Screen_Controller.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation
//              : and movement of the camera_component.
//-----------------------------------------------------------------------------
class Level_Controller: public Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Level_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Level_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

 protected:

 private:
  Avatar_Controller *m_avatar_controller;
  Avatar_Component *m_avatar;
  Camera_Component *m_camera;
  Game_Settings_Component *m_game_settings;
  Level_Component *m_level;

  std::vector<Level_Component::Level_Metadata> m_levels;

  //---------------------------------------------------------------------------
  // Description : Creates a Bitmap Component using the given collision block
  //--------------------------------------------------------------------------
  void LoadLevelMetadata();
  Level_Component::Level_Metadata LoadLevelMetadataIntoStruct(std::string metadata_path);
  void LoadLevelCSVIntoStruct(std::string metadata_path, Level_Component::Level_Metadata *out_struct);
  Avatar_Component::Avatar_Collision_Block GetNamedCollisionBlock(std::string id, std::list<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks);
  Level_Component::Level_Metadata GetNamedLevel(std::string level_name);

  Text_Component *m_level_name_heading;
  Text_Component *m_level_blurb;
  std::string m_font_path;
  int m_z_position;

  Level_Component::Level_Metadata m_next_level;

  Tunnelour::Background_Controller *m_background_controller;
  Tunnelour::Middleground_Controller *m_middleground_controller;
  Tunnelour::Level_Tile_Controller *m_level_tile_controller;
  Tunnelour::Splash_Screen_Component *m_splash_screen_component;
  Tunnelour::Level_Transition_Controller *m_level_transition_controller;
  Tunnelour::Screen_Wipeout_Controller *m_screen_wipeout_controller;
  Tunnelour::Game_Over_Screen_Controller *m_game_over_screen_controller;

  bool m_has_transition_been_initalised;
  bool m_has_level_been_destroyed;
  bool m_has_level_been_created;
  bool m_has_level_been_added;
  bool m_has_level_been_shown;
  bool m_has_splash_screen_faded;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_LEVEL_CONTROLLER_H_
