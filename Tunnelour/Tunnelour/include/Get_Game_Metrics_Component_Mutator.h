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

#ifndef TUNNELOUR_GET_GAME_METRICS_COMPONENT_MUTATOR_H_
#define TUNNELOUR_GET_GAME_METRICS_COMPONENT_MUTATOR_H_

#include <list>

#include "Component.h"
#include "Camera_Component.h"
#include "Game_Settings_Component.h"
#include "Game_Metrics_Component.h"
#include "Avatar_Component.h"

namespace Tunnelour {
class Get_Game_Metrics_Component_Mutator: public Tunnelour::Component::Component_Mutator {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Get_Game_Metrics_Component_Mutator is a mutator run on the model
//                which collects all the renderable components.
//-----------------------------------------------------------------------------
 public:

  //-------------------------------------------------------------------------
  // Description : Constructor
  //-------------------------------------------------------------------------
  Get_Game_Metrics_Component_Mutator();

  //-------------------------------------------------------------------------
  // Description : Deconstructor
  //-------------------------------------------------------------------------
  virtual ~Get_Game_Metrics_Component_Mutator();

  //-------------------------------------------------------------------------
  // Description : Mutator function, pass it a component to mutate.
  //-------------------------------------------------------------------------
  void Mutate(Tunnelour::Component * const component);

  //-------------------------------------------------------------------------
  // Description : Has this mutator run successfully?
  //-------------------------------------------------------------------------
  bool WasSuccessful();

  //-------------------------------------------------------------------------
  // Description : Accessor for the game metrics
  //-------------------------------------------------------------------------
  Tunnelour::Game_Metrics_Component* const Get_Game_Metrics_Component_Mutator::GetGameMetrics();


 private:
  bool m_found_game_metics;
  Tunnelour::Game_Metrics_Component * m_game_metrics;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_GET_GAME_METRICS_COMPONENT_MUTATOR_H_
