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

#include "Component.h"
#include "Component_ID.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Component::Component() {
  m_id = Tunnelour::Component_ID::GetInstance()->Next();
  m_is_initialised = false;
}

//------------------------------------------------------------------------------
Component::~Component() {
}

//------------------------------------------------------------------------------
void Component::Init() {
}

//------------------------------------------------------------------------------
int Component::GetID() {
  return m_id;
}

//------------------------------------------------------------------------------
void Component::Observe(Component_Observer* component_observer) {
  m_observers.push_back(component_observer);
}
//------------------------------------------------------------------------------
void Component::Ignore(Component_Observer* component_observer) {
  m_observers.remove(component_observer);
}

//------------------------------------------------------------------------------
bool Component::IsInitialised() {
  return m_is_initialised;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Component::Notify() {
  for each( Component_Observer* observer in m_observers ) {
    observer->HandleEvent(*this);
  }
}

}  // namespace Tunnelour
