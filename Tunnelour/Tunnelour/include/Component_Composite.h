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

#ifndef TUNNELOUR_COMPONENT_COMPOSITE_H_
#define TUNNELOUR_COMPONENT_COMPOSITE_H_

#include "Component.h"

#include <list>

namespace Tunnelour {
  class Component_Composite: public Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Component_Composite();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Component_Composite();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component Manager
  //---------------------------------------------------------------------------
  void Init();

  //---------------------------------------------------------------------------
  // Description : Adds a copy of the provided component into the manager
  //---------------------------------------------------------------------------
  void Add(Tunnelour::Component *component);

  //---------------------------------------------------------------------------
  // Description : Removes the component with the provided id from the manager
  //---------------------------------------------------------------------------
  void Remove(Tunnelour::Component *component);

  //---------------------------------------------------------------------------
  // Description : Returns a pointer to a component which matches the id
  //---------------------------------------------------------------------------
  void Apply(Tunnelour::Component::Component_Mutator *mutator);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Component Storage
  //---------------------------------------------------------------------------
  std::list<Tunnelour::Component*> m_components;
};
}
#endif  // TUNNELOUR_COMPONENT_COMPOSITE_H_
