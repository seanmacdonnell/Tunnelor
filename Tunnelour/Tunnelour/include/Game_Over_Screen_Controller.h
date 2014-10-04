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

#ifndef TUNNELOR_GAME_OVER_SCREEN_CONTROLLER_H_
#define TUNNELOR_GAME_OVER_SCREEN_CONTROLLER_H_

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Over_Screen_Controller_Mutator.h"
#include "Camera_Component.h"
#include "Tileset_Helper.h"
#include "Level_Component.h"
#include "Text_Component.h"
#include "Input_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                game over screen!
//-----------------------------------------------------------------------------
class Game_Over_Screen_Controller: public Controller,
                                   public Component::Component_Observer {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Game_Over_Screen_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Game_Over_Screen_Controller();

  //--------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  void Move_Bitmaps();

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  void Fade_Bitmaps();

  //---------------------------------------------------------------------------
  // Description : Triggers when the camera moves
  //---------------------------------------------------------------------------
  virtual void HandleEvent(Tunnelour::Component * const component);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Private Functions
  //---------------------------------------------------------------------------
  void LoadTilesetMetadata();
  Tile_Bitmap* CreateTile(float base_tile_size);

  //---------------------------------------------------------------------------
  // Private Constant Variables
  //---------------------------------------------------------------------------
  const float m_z_bitmap_position;
  const float m_z_text_position;

  //---------------------------------------------------------------------------
  // Private Variables
  //---------------------------------------------------------------------------
  std::vector<Tileset_Helper::Tileset_Metadata> m_tilesets;
  Tileset_Helper::Tileset_Metadata m_current_tileset;
  Tileset_Helper::Subset m_current_tileset_subset;
  Camera_Component *m_camera;
  Game_Settings_Component* m_game_settings;
  Level_Component *m_level;
  std::string m_tileset_filename;
  std::string m_black_metadata_file_path;
  Tile_Bitmap* m_background;
  Text_Component *m_game_name_heading;
  Text_Component *m_author;
  Text_Component *m_version;
  Text_Component *m_thank_you;
  std::string m_heading_font_path;
  std::string m_text_font_path;
  std::string m_text_font_path_size_64;

  //---------------------------------------------------------------------------
  // Description : Initialises the timers used for the animation ticks
  //---------------------------------------------------------------------------
  bool StartTimers();

  //---------------------------------------------------------------------------
  // Description : Updates and determines if the animation needs to update
  //---------------------------------------------------------------------------
  void IsItTimeToAnimateAFrame();
  void IsItTimeToFadeout();
  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  bool m_animation_tick;
  float m_fadeout_frameTime;
  bool m_fadeout_animation_tick;
  INT64 m_fadeout_startTime;
  Input_Component *m_input;
  float m_loading_transparency;
  bool m_is_fading;
  bool m_is_debug_mode;
  bool m_has_space_been_pressed;
};
}  // namespace Tunnelour
#endif  // TUNNELOR_GAME_OVER_SCREEN_CONTROLLER_H_
