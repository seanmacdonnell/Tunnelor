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

#ifndef TUNNELOUR_CAMERA_CONTROLLER_H_
#define TUNNELOUR_CAMERA_CONTROLLER_H_

#include "Game_Settings_Component.h"
#include "Avatar_Controller_Mutator.h"
#include "Avatar_Component.h"
#include "Controller.h"
#include "Camera_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                middleground (Layer 0)
//-----------------------------------------------------------------------------
class Camera_Controller: public Tunnelour::Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Camera_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Camera_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:

 private:
  Tunnelour::Avatar_Component *m_avatar;
  Tunnelour::Game_Settings_Component* m_game_settings;
  Tunnelour::Camera_Component *m_camera;
  Tunnelour::Avatar_Controller_Mutator m_mutator;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CAMERA_CONTROLLER_H_