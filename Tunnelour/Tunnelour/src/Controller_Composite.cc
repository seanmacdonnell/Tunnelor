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

#include "Controller_Composite.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Controller_Composite::Controller_Composite() {
  m_model = 0;
}

//------------------------------------------------------------------------------
Controller_Composite::~Controller_Composite() {
  while (!m_controllers.empty()) {
    delete m_controllers.front();
    m_controllers.pop_front();
  }
}

//------------------------------------------------------------------------------
bool Controller_Composite::Init(Component_Composite *model) {
  m_model = model;
  return true;
}

//------------------------------------------------------------------------------
Controller * const Controller_Composite::Add(Tunnelour::Controller * const controller) {
  m_controllers.push_back(controller);
  return m_controllers.back();
}

//---------------------------------------------------------------------------
void Controller_Composite::Remove(Tunnelour::Controller *controller) {
  m_controllers.remove(controller);
  delete controller;
}

//---------------------------------------------------------------------------
bool Controller_Composite::Run() {
  std::list<Tunnelour::Controller*>::iterator it;
  for (it = m_controllers.begin(); it != m_controllers.end(); it++) {
    if ((*it)->IsFinished()) {
      delete (*it);
      (*it) = 0;
      it = m_controllers.erase(it);
    } else {
      if ((*it)->HasBeenInitalised()) {
        if (!(*it)->Run()) {
          std::string error = "A controller has failed to run!";
          throw Exceptions::init_error(error);
        }
      } else {
        (*it)->Init(m_model);
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
