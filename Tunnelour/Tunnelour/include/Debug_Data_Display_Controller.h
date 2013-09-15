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

#ifndef TUNNELOUR_DEBUG_DATA_DISPLAY_CONTROLLER_H_
#define TUNNELOUR_DEBUG_DATA_DISPLAY_CONTROLLER_H_

#include <string>
#include "Text_Component.h"
#include "Controller.h"
#include "Game_Settings_Component.h"
#include "Game_Metrics_Component.h"
#include "Camera_Component.h"
#include "Debug_Data_Display_Controller_Mutator.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                data display which shows debug data in text components
//                when the game is in debug mode.
//-----------------------------------------------------------------------------
class Debug_Data_Display_Controller: public Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Debug_Data_Display_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Debug_Data_Display_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual void Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Creates the data debug heading
  //---------------------------------------------------------------------------
  void CreateDebugDataHeading();

  //---------------------------------------------------------------------------
  // Description : Updates the location of the heading
  //---------------------------------------------------------------------------
  void UpdateDebugDataHeading();

  //---------------------------------------------------------------------------
  // Description : Creates the fps display
  //---------------------------------------------------------------------------
  void CreateFPSDisplay();

  //---------------------------------------------------------------------------
  // Description : Updates the fps location and value
  //---------------------------------------------------------------------------
  void UpdateFPSDisplay();

  //---------------------------------------------------------------------------
  // Description : Creates the avatar position display
  //---------------------------------------------------------------------------
  void CreateAvatarPositionDisplay();

  //---------------------------------------------------------------------------
  // Description : Updates the avatar position display
  //---------------------------------------------------------------------------
  void UpdateAvatarPositionDisplay();

  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  Game_Settings_Component *m_game_settings;
  Game_Metrics_Component *m_game_metrics;
  Text_Component *m_heading;
  Text_Component *m_fps_display;
  Text_Component *m_avatar_position_display;
  Camera_Component *m_camera;
  Avatar_Component *m_avatar;
  std::string m_font_path;
  long double m_fps;
  bool m_is_debug_mode;
  bool m_has_been_initialised;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_DEBUG_DATA_DISPLAY_CONTROLLER_H_
