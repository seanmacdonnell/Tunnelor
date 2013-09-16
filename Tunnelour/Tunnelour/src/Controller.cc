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
  m_has_been_initialised = false;
}

//------------------------------------------------------------------------------
Controller::~Controller() {
  m_model = 0;
  m_is_finished = false;
  m_has_been_initialised = false;
}

//------------------------------------------------------------------------------
bool Controller::Init(Component_Composite *const model) {
  m_model = model;
  return true;
}

//------------------------------------------------------------------------------
bool Controller::Run() {
  if (!IsFinished()) {
  }
  return true;
}

//------------------------------------------------------------------------------
bool Controller::IsFinished() {
  return m_is_finished;
}

//------------------------------------------------------------------------------
bool Controller::HasBeenInitalised() {
  return m_has_been_initialised;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
