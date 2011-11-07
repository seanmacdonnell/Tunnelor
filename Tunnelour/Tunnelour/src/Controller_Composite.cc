//  Copyright 2011 Sean MacDonnell
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

#include "Controller_Composite.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Controller_Composite::Controller_Composite() {
  m_model = 0;
}

//------------------------------------------------------------------------------
Controller_Composite::~Controller_Composite() {
  for each ( Tunnelour::Controller* controller in m_controllers ) {
    m_controllers.remove(controller);
  }
}

//------------------------------------------------------------------------------
void Controller_Composite::Init(Tunnelour::Component_Composite *model) {
  m_model = model;
}

//---------------------------------------------------------------------------
void Controller_Composite::Add(Tunnelour::Controller *controller) {
  m_controllers.push_back(controller);
}

//---------------------------------------------------------------------------
void Controller_Composite::Remove(Tunnelour::Controller *controller) {
  m_controllers.remove(controller);
  delete controller;
}

//---------------------------------------------------------------------------
void Controller_Composite::Run() {
  for each ( Tunnelour::Controller* controller in m_controllers) {
    controller->Run();
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
