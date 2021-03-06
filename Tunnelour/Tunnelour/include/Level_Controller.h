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

#ifndef TUNNELOUR_LEVEL_CONTROLLER_H_
#define TUNNELOUR_LEVEL_CONTROLLER_H_

#include "Game_Settings_Component.h"
#include "Level_Controller_Mutator.h"
#include "Avatar_Component.h"
#include "Controller.h"
#include "Level_Component.h"
#include "Text_Component.h"
#include "Camera_Component.h"
#include "Splash_Screen_Component.h"
#include "Level_Tile_Controller.h"
#include "Level_Transition_Controller.h"
#include "Avatar_Controller.h"
#include "Screen_Wipeout_Controller.h"
#include "Game_Over_Screen_Controller.h"
#include "Game_Metrics_Controller.h"
#include "Score_Display_Controller.h"
#include "Input_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation
//              : and teardown of the middleground, background and foreground
//              : tiles which make up the game.
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
  void LoadLevelMetadata();
  Level_Component::Level_Metadata LoadLevelMetadataIntoStruct(std::string metadata_path);
  void LoadLevelCSVIntoStruct(std::string metadata_path, Level_Component::Level_Metadata *out_struct);
  Avatar_Component::Avatar_Collision_Block GetNamedCollisionBlock(std::string id, std::list<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks);
  Level_Component::Level_Metadata GetNamedLevel(std::string level_name);

  const int m_z_position;

  Avatar_Component *m_avatar;

  // Components gathered by the mutator
  Camera_Component *m_camera;
  Game_Settings_Component *m_game_settings;
  Tunnelour::Splash_Screen_Component *m_splash_screen_component;
  Tunnelour::Input_Component *m_input_component;

  Level_Component *m_level;
  std::vector<Level_Component::Level_Metadata> m_levels;

  Text_Component *m_level_name_heading;
  Text_Component *m_level_blurb;
  std::string m_font_path;

  Level_Component::Level_Metadata m_next_level;
  Level_Component::Level_Metadata m_current_level;

  // These are components and controllers which are owned by this controller
  Tunnelour::Level_Tile_Controller *m_level_tile_controller;
  Tunnelour::Level_Transition_Controller *m_level_transition_controller;
  Tunnelour::Screen_Wipeout_Controller *m_screen_wipeout_controller;
  Tunnelour::Game_Over_Screen_Controller *m_game_over_screen_controller;
  Tunnelour::Game_Metrics_Controller *m_game_metrics_controller;
  Tunnelour::Score_Display_Controller *m_score_display_controller;
  Tunnelour::Avatar_Controller *m_avatar_controller;

  bool m_has_transition_been_initalised;
  bool m_has_level_been_destroyed;
  bool m_has_level_been_created;
  bool m_has_level_been_added;
  bool m_has_level_been_shown;
  bool m_has_splash_screen_faded;

  bool m_has_avatar_been_reset;

  std::vector<Tile_Bitmap*> m_exit_tiles;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_LEVEL_CONTROLLER_H_
