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

#include "Tunnelour_Controller.h"
#include "Init_Controller.h"
#include "Background_Controller.h"
#include "Middleground_Controller.h"
#include "Avatar_Controller.h"
#include "Input_Controller.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Tunnelour_Controller::Tunnelour_Controller() {
}

//------------------------------------------------------------------------------
Tunnelour_Controller::~Tunnelour_Controller() {
}

//------------------------------------------------------------------------------
void Tunnelour_Controller::Init(Tunnelour::Component_Composite * const model) {
  Controller_Composite::Init(model);

  Tunnelour::Controller* controller;
  controller = Add(new Tunnelour::Init_Controller());
  controller->Init(m_model);
  
  controller = Add(new Tunnelour::Avatar_Controller());
  controller->Init(m_model);

  controller = Add(new Tunnelour::Background_Controller());
  controller->Init(m_model);
  
  controller = Add(new Tunnelour::Middleground_Controller());
  controller->Init(m_model);
  
  controller = Add(new Tunnelour::Input_Controller());
  controller->Init(m_model);
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
