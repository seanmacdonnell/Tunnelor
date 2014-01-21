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

#ifndef LEVEL_TRANSITION_CONTROLLER_H_
#define LEVEL_TRANSITION_CONTROLLER_H_

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"
#include "Level_Transition_Controller_Mutator.h"
#include "Camera_Component.h"
#include "Tileset_Helper.h"
#include "Level_Component.h"
#include "Text_Component.h"
#include "Level_Transition_Component.h"
#include "Input_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                introduction to Tunnelor
//-----------------------------------------------------------------------------
class Level_Transition_Controller: public Controller,
                                   public Component::Component_Observer {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Level_Transition_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Level_Transition_Controller();

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
  void SetIsLoading(bool is_loading);
  
  bool IsLoading();

  void SetLevelCompleteHeadingText(std::string level_complete_heading_text);

  void SetNextLevelHeadingText(std::string next_level_heading);

  void SetNextLevelNameText(std::string next_level_name);

  void SetNextLevelBlurbText(std::string next_level_blurb);

  virtual void HandleEvent(Tunnelour::Component * const component);

 protected:

 private:
  std::vector<Tileset_Helper::Tileset_Metadata> m_tilesets;
  Tileset_Helper::Tileset_Metadata m_current_tileset;
  Tileset_Helper::Subset m_current_tileset_subset;
  void LoadTilesetMetadata();
  Tileset_Helper::Tileset_Metadata GetNamedTileset(std::string name);
  Tileset_Helper::Subset GetCurrentForegroundSubset();
  Tile_Bitmap* CreateTile(float base_tile_size);
  Tileset_Helper::Line GetCurrentSizedLine(float size);

  //---------------------------------------------------------------------------
  // Description : Switches the tileset from Debug to Dirt and vise versa
  //---------------------------------------------------------------------------
  Game_Settings_Component* m_game_settings;
  Camera_Component *m_camera;
  Level_Component *m_level;
  std::string m_tileset_filename;
  bool m_is_debug_mode;
  std::string m_black_metadata_file_path;
  std::string m_white_metadata_file_path;

  Tile_Bitmap* m_background;
  std::string m_heading_font_path;
  std::string m_text_font_path;
  int m_z_bitmap_position;
  int m_z_text_position;

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

  //---------------------------------------------------------------------------
  // Description : Initialises the timer used for the animation ticks
  //---------------------------------------------------------------------------
  bool InitTimer();

  //---------------------------------------------------------------------------
  // Description : Updates and determines if the animation needs to update
  //---------------------------------------------------------------------------
  void UpdateTimer();

  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  bool m_animation_tick;
  int m_current_animation_fps;

  Level_Transition_Component *m_level_transition_component;
  Input_Component *m_input;

  bool m_is_loading;
  bool m_is_fading;
  float m_loading_transparency;
};
}  // namespace Tunnelour
#endif  // LEVEL_TRANSITION_CONTROLLER_H_
