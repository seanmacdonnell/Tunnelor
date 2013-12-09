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
    Remove(m_components.front());
    //delete m_components.front();
    //m_components.pop_front();
  }
}

//------------------------------------------------------------------------------
void Component_Composite::Init() {
}

//------------------------------------------------------------------------------
Tunnelour::Component * const Component_Composite::Add(Tunnelour::Component * component) {
  m_components.push_back(component);
  NotifyOnAddType(component);
  return m_components.back();
}

//------------------------------------------------------------------------------
void Component_Composite::Remove(Tunnelour::Component * component) {
  m_components.remove(component);
  NotifyOnRemoveType(component);
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
void Component_Composite::ObserveType(Component_Composite_Type_Observer* component_observer, std::string type) {
  m_type_observers.remove(std::pair<std::string, Component_Composite_Type_Observer*>(type, component_observer));
  m_type_observers.push_back(std::pair<std::string, Component_Composite_Type_Observer*>(type, component_observer));
}
//------------------------------------------------------------------------------
void Component_Composite::IgnoreType(Component_Composite_Type_Observer* component_observer, std::string type) {
  m_type_observers.remove(std::pair<std::string, Component_Composite_Type_Observer*>(type, component_observer)); 
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
void Component_Composite::NotifyOnAddType(Tunnelour::Component * component) {
  for each(std::pair<std::string, Component_Composite_Type_Observer*> observer in m_type_observers) {
    if (observer.first.compare(component->GetType()) == 0) {
      observer.second->HandleEventAdd(component);
    }
  }
}

void Component_Composite::NotifyOnRemoveType(Tunnelour::Component * component) {
  for each(std::pair<std::string, Component_Composite_Type_Observer*> observer in m_type_observers) {
    if (observer.first.compare(component->GetType()) == 0) {
      if (observer.second != 0) { observer.second->HandleEventRemove(component); }
    }
  }
}

}  // namespace Tunnelour
