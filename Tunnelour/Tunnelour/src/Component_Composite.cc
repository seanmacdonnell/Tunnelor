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

#include "Component_Composite.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Component_Composite::Component_Composite() : Component() {
}

//------------------------------------------------------------------------------
Component_Composite::~Component_Composite() {
  while (!m_components.empty()) {
    delete m_components.front();
    m_components.pop_front();
  }
}

//------------------------------------------------------------------------------
void Component_Composite::Init() {
}

//------------------------------------------------------------------------------
Tunnelour::Component * const Component_Composite::Add(Tunnelour::Component * component) {
  m_components.push_back(component);
  return m_components.back();
}

//------------------------------------------------------------------------------
void Component_Composite::Remove(Tunnelour::Component * component) {
  m_components.remove(component);
  delete component;
}

//------------------------------------------------------------------------------
void Component_Composite::Apply(Tunnelour::Component::Component_Mutator * const mutator) {
  if (mutator && !m_components.empty()) {
    std::list<Tunnelour::Component*>::iterator it;
    for (it = m_components.begin(); it != m_components.end(); ) {
      (*mutator).Mutate(*it++);
    }
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
