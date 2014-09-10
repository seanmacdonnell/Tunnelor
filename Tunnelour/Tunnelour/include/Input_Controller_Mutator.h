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

#ifndef TUNNELOUR_INPUT_CONTROLLER_MUTATOR_H_
#define TUNNELOUR_INPUT_CONTROLLER_MUTATOR_H_

#include "Component.h"
#include "Game_Settings_Component.h"
#include "Avatar_Component.h"
#include "Tile_Bitmap.h"

namespace Tunnelour {
class Input_Controller_Mutator: public Component::Component_Mutator  {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Input_Controller_Mutator is a mutator run on the model
//                which collects the game settings component.
//-----------------------------------------------------------------------------
 public:
  //-------------------------------------------------------------------------
  // Description : Constructor
  //-------------------------------------------------------------------------
  Input_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Deconstructor
  //-------------------------------------------------------------------------
  virtual ~Input_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Mutator function, pass it a component to mutate.
  //-------------------------------------------------------------------------
  void Mutate(Component *const component);

  //-------------------------------------------------------------------------
  // Description : Accessors for the Game_Settings_Component
  //-------------------------------------------------------------------------
  Game_Settings_Component *const GetGameSettings();

  //-------------------------------------------------------------------------
  // Description : Did this mutator run successfully?
  //-------------------------------------------------------------------------
  bool Input_Controller_Mutator::WasSuccessful();

 private:
  bool m_found_game_settings;
  bool m_found_avatar_component;
  Game_Settings_Component *m_game_settings;
  Avatar_Component *m_avatar_controller;
  std::vector<Tile_Bitmap*> m_exit_tiles;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_INPUT_CONTROLLER_MUTATOR_H_
