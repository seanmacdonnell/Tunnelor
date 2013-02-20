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

#ifndef TUNNELOUR_MIDDLEGROUND_CONTROLLER_MUTATOR_H_
#define TUNNELOUR_MIDDLEGROUND_CONTROLLER_MUTATOR_H_

#include "Component.h"
#include "Game_Settings_Component.h"

namespace Tunnelour {
class Middleground_Controller_Mutator: public Tunnelour::Component::Component_Mutator  {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Middleground_Controller_Mutator is a mutator run on the model
//                which collects the game settings component.
//-----------------------------------------------------------------------------
 public:
  //-------------------------------------------------------------------------
  // Description : Constructor
  //-------------------------------------------------------------------------
  Middleground_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Deconstructor
  //-------------------------------------------------------------------------
  virtual ~Middleground_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Mutator function, pass it a component to mutate.
  //-------------------------------------------------------------------------
  void Mutate(Tunnelour::Component * const component);

  //-------------------------------------------------------------------------
  // Description : Has this component been found in the model?
  //-------------------------------------------------------------------------
  bool FoundGameSettings();

  //-------------------------------------------------------------------------
  // Description : Accessors for the Game_Settings_Component
  //-------------------------------------------------------------------------
  Tunnelour::Game_Settings_Component* const GetGameSettings();

 private:
  bool m_found_game_settings;
  Tunnelour::Game_Settings_Component * m_game_settings;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_MIDDLEGROUND_CONTROLLER_MUTATOR_H_
