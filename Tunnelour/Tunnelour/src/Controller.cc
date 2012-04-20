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

#include "Controller.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Controller::Controller() {
  m_model = 0;
  m_is_finished = false;
}

//------------------------------------------------------------------------------
Controller::~Controller() {
}

//------------------------------------------------------------------------------
void Controller::Init(Tunnelour::Component_Composite *model) {
  m_model = model;
}

//------------------------------------------------------------------------------
void Controller::Run() {
}

//------------------------------------------------------------------------------
bool Controller::IsFinished() {
  return m_is_finished;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
