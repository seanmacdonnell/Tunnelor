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

#ifndef SCREEN_WIPEOUT_CONTROLLER_H_
#define SCREEN_WIPEOUT_CONTROLLER_H_

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"

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
class Screen_Wipeout_Controller: public Controller,
                                 public Component::Component_Observer {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Screen_Wipeout_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Screen_Wipeout_Controller();

  //--------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

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

  std::string m_tileset_filename;
  std::string m_black_metadata_file_path;
  std::string m_white_metadata_file_path;

  Tile_Bitmap* m_top_slash;
  Tile_Bitmap* m_bottom_slash;
  Tile_Bitmap* m_background;
  int m_z_bitmap_position;
  int m_z_text_position;

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
};
}  // namespace Tunnelour
#endif  // SCREEN_WIPEOUT_CONTROLLER_H_
