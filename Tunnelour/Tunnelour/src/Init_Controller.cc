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

#include "Init_Controller.h"
#include "Camera_Component.h"
#include "Game_Settings_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Init_Controller::Init_Controller() : Controller() {
}

//------------------------------------------------------------------------------
Init_Controller::~Init_Controller() {
}

//------------------------------------------------------------------------------
void Init_Controller::Init(Tunnelour::Component_Composite * const model) {
  Tunnelour::Controller::Init(model);
}

//------------------------------------------------------------------------------
void Init_Controller::Run() {
  // DO NOT DELETE THESE. PASSING DELETE CONTROL TO THE MODEL FOR THESE.
  Tunnelour::Component * game_settings = 0;
  game_settings = m_model->Add(new Tunnelour::Game_Settings_Component());
  game_settings->Init();

  m_is_finished = true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
