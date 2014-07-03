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

#ifndef TUNNELOUR_LEVEL_CONTROLLER_MUTATOR_H_
#define TUNNELOUR_LEVEL_CONTROLLER_MUTATOR_H_

#include "Component.h"
#include "Game_Settings_Component.h"
#include "Camera_Component.h"
#include "Splash_Screen_Component.h"
#include "Input_Component.h"

namespace Tunnelour {
class Level_Controller_Mutator: public Component::Component_Mutator  {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Level_Controller_Mutator is a mutator run on the model
//                which collects the relevant camera components
//-----------------------------------------------------------------------------
 public:
  //-------------------------------------------------------------------------
  // Description : Constructor
  //-------------------------------------------------------------------------
  Level_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Deconstructor
  //-------------------------------------------------------------------------
  virtual ~Level_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Mutator function, pass it a component to mutate.
  //-------------------------------------------------------------------------
  void Mutate(Tunnelour::Component * const component);

  //-------------------------------------------------------------------------
  // Description : Accessors for the Game_Settings_Component
  //-------------------------------------------------------------------------
  Game_Settings_Component* const GetGameSettings();

  //-------------------------------------------------------------------------
  // Description : Accessors for the Game_Settings_Component
  //-------------------------------------------------------------------------
  Camera_Component* const GetCamera();

  Splash_Screen_Component* GetSplashScreen();

  Input_Component* GetInputComponent();

  //-------------------------------------------------------------------------
  // Description : Accessors for the Game_Settings_Component
  //-------------------------------------------------------------------------
  bool WasSuccessful();

 private:
  bool m_found_game_settings;
  bool m_found_camera;
  bool m_found_spash_screen;
  bool m_found_input;
  Game_Settings_Component * m_game_settings;
  Camera_Component * m_camera;
  Tunnelour::Splash_Screen_Component *m_splash_screen_component;
  Input_Component *m_input;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_LEVEL_CONTROLLER_MUTATOR_H_
