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

#include "Component_Composite.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Component_Composite::Component_Composite() {
}

//------------------------------------------------------------------------------
Component_Composite::~Component_Composite() {
  for each ( Tunnelour::Component* component in m_components ) {
    m_components.remove(component);
  }
}

//------------------------------------------------------------------------------
void Component_Composite::Init() {
}

//------------------------------------------------------------------------------
void Component_Composite::Add(Tunnelour::Component *component) {
  m_components.push_back(component);
}

//------------------------------------------------------------------------------
void Component_Composite::Remove(Tunnelour::Component *component) {
  m_components.remove(component);
  delete component;
}

//------------------------------------------------------------------------------
void Component_Composite::Apply(Tunnelour::Component::Component_Mutator *mutator) {
  // This is unusual, why am I applying the component to the mutator?
  // Maybe hard to read.
  for each ( Tunnelour::Component* component in m_components ) {
    mutator->Mutate(component);
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
