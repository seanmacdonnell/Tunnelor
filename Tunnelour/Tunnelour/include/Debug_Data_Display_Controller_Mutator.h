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

#ifndef TUNNELOUR_DEBUG_DATA_DISPLAY_CONTROLLER_MUTATOR_H_
#define TUNNELOUR_DEBUG_DATA_DISPLAY_CONTROLLER_MUTATOR_H_

#include <vector>
#include "Component.h"
#include "Game_Settings_Component.h"
#include "Avatar_Component.h"
#include "Camera_Component.h"
#include "Bitmap_Component.h"
#include "Game_Metrics_Component.h"

namespace Tunnelour {
class Debug_Data_Display_Controller_Mutator: public Component::Component_Mutator  {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Debug_Data_Display_Controller_Mutator is a mutator run on
//                the model which collects the game settings component.
//-----------------------------------------------------------------------------
 public:
  //-------------------------------------------------------------------------
  // Description : Constructor
  //-------------------------------------------------------------------------
  Debug_Data_Display_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Deconstructor
  //-------------------------------------------------------------------------
  virtual ~Debug_Data_Display_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Mutator function, pass it a component to mutate.
  //-------------------------------------------------------------------------
  void Mutate(Component * const component);

  //-------------------------------------------------------------------------
  // Description : Accessors for the Game_Settings_Component
  //-------------------------------------------------------------------------
  Game_Settings_Component* const GetGameSettings();

  //-------------------------------------------------------------------------
  // Description : Accessors for the Avatar_Component
  //-------------------------------------------------------------------------
  Avatar_Component* const GetAvatarComponent();

  //-------------------------------------------------------------------------
  // Description : Accessors for the Camera Component
  //-------------------------------------------------------------------------
  Camera_Component* const GetCamera();

  //-------------------------------------------------------------------------
  // Description : Accessors for the Game Metrics
  //-------------------------------------------------------------------------
  Game_Metrics_Component* const GetGameMetrics();

  //-------------------------------------------------------------------------
  // Description : Did the mutator complete successfully?
  //-------------------------------------------------------------------------
  bool WasSuccessful();

 private:
  bool m_found_game_settings;
  bool m_found_avatar_component;
  bool m_found_camera;
  bool m_found_game_metrics;
  Game_Settings_Component *m_game_settings;
  Avatar_Component *m_avatar_component;
  Camera_Component * m_camera;
  std::vector<Bitmap_Component*> m_avatar_collision_blocks;
  Game_Metrics_Component *m_game_metrics;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_DEBUG_DATA_DISPLAY_CONTROLLER_MUTATOR_H_
