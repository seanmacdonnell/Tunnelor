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

#ifndef SCORE_DISPLAY_CONTROLLER_H_
#define SCORE_DISPLAY_CONTROLLER_H_

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"
#include "Score_Display_Controller_Mutator.h"
#include "Camera_Component.h"
#include "Tileset_Helper.h"
#include "Level_Component.h"
#include "Text_Component.h"
#include "Input_Component.h"
#include "Game_Metrics_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                screen which displays the score to the user
//-----------------------------------------------------------------------------
class Score_Display_Controller: public Controller,
                                public Component::Component_Observer {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Score_Display_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Score_Display_Controller();

  //--------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite * const model);

  //--------------------------------------------------------------------------
  // Description : Runs this controller
  //---------------------------------------------------------------------------
  virtual bool Run();

  //---------------------------------------------------------------------------
  // Description : Handles whether the camera has moved since run
  //---------------------------------------------------------------------------
  bool IsFading();

  //---------------------------------------------------------------------------
  // Description : Handles whether the camera has moved since run
  //---------------------------------------------------------------------------
  virtual void HandleEvent(Tunnelour::Component * const component);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Private Functions
  //---------------------------------------------------------------------------

  //---------------------------------------------------------------------------
  // Description : Loads the specific tileset metadata into the object
  //---------------------------------------------------------------------------
  void LoadTilesetMetadata();

  //---------------------------------------------------------------------------
  // Description : Handles whether the camera has moved since run
  //---------------------------------------------------------------------------
  Tile_Bitmap* CreateTile(float base_tile_size);

  //---------------------------------------------------------------------------
  // Description : Initialises the timer used for the animation ticks
  //---------------------------------------------------------------------------
  bool StartTimer();

  //---------------------------------------------------------------------------
  // Description : Updates and determines if the animation needs to update
  //---------------------------------------------------------------------------
  void IsItTimeToAnimateAFrame();

  //---------------------------------------------------------------------------
  // Constant Variables
  //---------------------------------------------------------------------------
  const float m_z_bitmap_position;
  const float m_z_text_position;

  //---------------------------------------------------------------------------
  // Private Variables
  //---------------------------------------------------------------------------
  Game_Settings_Component* m_game_settings;
  Camera_Component *m_camera;
  Level_Component *m_level;
  Input_Component *m_input;
  Game_Metrics_Component *m_game_metrics;

  std::string m_tileset_filename;
  bool m_is_debug_mode;
  std::string m_black_metadata_file_path;
  std::string m_white_metadata_file_path;

  std::vector<Tileset_Helper::Tileset_Metadata> m_tilesets;
  Tileset_Helper::Tileset_Metadata m_current_tileset;
  Tileset_Helper::Subset m_current_tileset_subset;

  std::string m_heading_font_path;
  std::string m_text_font_path;

  Text_Component *m_level_complete_heading;
  std::string m_level_complete_heading_text;
  float m_level_complete_heading_y_offset;
  Text_Component *m_next_level_heading;
  std::string m_next_level_heading_text;
  float m_next_level_heading_y_offset;
  Text_Component *m_next_level_name;
  std::string m_next_level_name_text;
  float m_next_level_name_y_offset;
  Text_Component *m_next_level_blurb;
  std::string m_next_level_blurb_text;
  float m_next_level_blurb_y_offset;
  Text_Component *m_loading;
  float m_loading_y_offset;
  float m_loading_transparency;
  Tile_Bitmap* m_background;

  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  int m_current_animation_fps;
  bool m_animation_tick;

  bool m_is_fading;
};
}  // namespace Tunnelour
#endif  // SCORE_DISPLAY_CONTROLLER_H_
