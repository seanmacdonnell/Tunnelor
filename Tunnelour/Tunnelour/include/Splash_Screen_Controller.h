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

#ifndef TUNNELOUR_SPLASH_SCREEN_CONTROLLER_H_
#define TUNNELOUR_SPLASH_SCREEN_CONTROLLER_H_

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"
#include "Splash_Screen_Controller_Mutator.h"
#include "Camera_Component.h"
#include "Tileset_Helper.h"
#include "Level_Component.h"
#include "Text_Component.h"
#include "Splash_Screen_Component.h"
#include "Input_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                introduction to Tunnelor
//-----------------------------------------------------------------------------
class Splash_Screen_Controller: public Controller,
                                public Component::Component_Observer {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Splash_Screen_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Splash_Screen_Controller();

  //--------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

  //---------------------------------------------------------------------------
  // Description : Handles any movement by the camera
  //---------------------------------------------------------------------------
  virtual void HandleEvent(Tunnelour::Component * const component);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Initialises the timer used for the animation ticks
  //---------------------------------------------------------------------------
  bool StartTimer();

  //---------------------------------------------------------------------------
  // Description : Updates and determines if the animation needs to update
  //---------------------------------------------------------------------------
  void IsItTimeToAnimateAFrame();

  //---------------------------------------------------------------------------
  // Description : Is it time to fade?
  //---------------------------------------------------------------------------
  void UpdateFadeoutTimer();

  //---------------------------------------------------------------------------
  // Description : Is it time to fade?
  //---------------------------------------------------------------------------
  void LoadTilesetMetadata();

  //---------------------------------------------------------------------------
  // Description : Creates a new dummy tile with the specified size 
  //---------------------------------------------------------------------------
  Tile_Bitmap* CreateTile(float base_tile_size);

  //---------------------------------------------------------------------------
  // Description : Member Variables
  //---------------------------------------------------------------------------
  const float m_z_bitmap_position;
  const float m_z_text_position;

  Tileset_Helper::Subset m_current_tileset_subset;
  std::vector<Tileset_Helper::Tileset_Metadata> m_tilesets;
  Tileset_Helper::Tileset_Metadata m_current_tileset;

  Game_Settings_Component* m_game_settings;
  Camera_Component *m_camera;
  Level_Component *m_level;
  Input_Component *m_input;

  std::string m_tileset_filename;
  bool m_is_debug_mode;
  std::string m_black_metadata_file_path;
  std::string m_white_metadata_file_path;

  Tile_Bitmap* m_background;
  std::string m_heading_font_path;
  std::string m_text_font_path;

  Text_Component *m_game_name_heading;
  Text_Component *m_author;
  Text_Component *m_version;

  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  bool m_animation_tick;
  int m_current_animation_fps;

  float m_fadeout_frameTime;
  bool m_fadeout_animation_tick;
  INT64 m_fadeout_startTime;

  INT64 m_fadeout_frequency;

  Splash_Screen_Component *m_splash_screen_component;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_SPLASH_SCREEN_CONTROLLER_H_
