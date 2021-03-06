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

#ifndef TUNNELOUR_DIRECT3D11_VIEW_MUTATOR_H_
#define TUNNELOUR_DIRECT3D11_VIEW_MUTATOR_H_

#include <list>

#include "Component.h"
#include "Camera_Component.h"
#include "Game_Settings_Component.h"
#include "Game_Metrics_Component.h"
#include "Avatar_Component.h"

namespace Tunnelour {
class Direct3D11_View_Mutator: public Tunnelour::Component::Component_Mutator {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Direct3D11_View_Mutator is a mutator run on the model
//                which collects all the renderable components.
//-----------------------------------------------------------------------------
 public:

  //-------------------------------------------------------------------------
  // Description : Constructor
  //-------------------------------------------------------------------------
  Direct3D11_View_Mutator();

  //-------------------------------------------------------------------------
  // Description : Deconstructor
  //-------------------------------------------------------------------------
  virtual ~Direct3D11_View_Mutator();

  //-------------------------------------------------------------------------
  // Description : Mutator function, pass it a component to mutate.
  //-------------------------------------------------------------------------
  void Mutate(Tunnelour::Component * const component);

  //-------------------------------------------------------------------------
  // Description : Has this mutator run successfully?
  //-------------------------------------------------------------------------
  bool WasSuccessful();

  //-------------------------------------------------------------------------
  // Description : Accessor for the Camera Component
  //-------------------------------------------------------------------------
  Tunnelour::Camera_Component* const GetCamera();

  //-------------------------------------------------------------------------
  // Description : Accessor for the Game_Settings_Component
  //-------------------------------------------------------------------------
  Tunnelour::Game_Settings_Component* const GetGameSettings();

  //-------------------------------------------------------------------------
  // Description : Accessor for the game metrics
  //-------------------------------------------------------------------------
  Tunnelour::Game_Metrics_Component* const Direct3D11_View_Mutator::GetGameMetrics();

  //-------------------------------------------------------------------------
  // Description : Returns the avatar
  //-------------------------------------------------------------------------
  Tunnelour::Avatar_Component* const Direct3D11_View_Mutator::GetAvatar();

 private:
  bool m_found_camera, m_found_avatar, m_found_game_settings, m_found_game_metics;
  Tunnelour::Camera_Component * m_camera;
  Tunnelour::Game_Settings_Component * m_game_settings;
  Tunnelour::Game_Metrics_Component * m_game_metrics;
  Tunnelour::Avatar_Component * m_avatar;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_DIRECT3D11_VIEW_MUTATOR_H_
