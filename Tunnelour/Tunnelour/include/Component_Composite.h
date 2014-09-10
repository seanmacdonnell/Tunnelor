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

#ifndef TUNNELOUR_COMPONENT_COMPOSITE_H_
#define TUNNELOUR_COMPONENT_COMPOSITE_H_

#include "Component.h"
#include <list>
#include <utility>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Component_Composite contains all the code required to
//                maintain a list of components.
//                This class is a model.
//-----------------------------------------------------------------------------
class Component_Composite: public Component {
 public:
  //---------------------------------------------------------------------------
  // Author(s)   : Sean MacDonnell
  // Description : Inheritable Observer class for this component.
  //---------------------------------------------------------------------------
  class Component_Composite_Type_Observer {
   public:
    //-------------------------------------------------------------------------
    // Description : HandleEvent is called when the component changes.
    //-------------------------------------------------------------------------
    virtual void HandleEventAdd(Tunnelour::Component * const component) = 0;
    virtual void HandleEventRemove(Tunnelour::Component * const component) = 0;
  };

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
  Tunnelour::Component * const Add(Tunnelour::Component * component);

  //---------------------------------------------------------------------------
  // Description : Removes the provided component from the manager
  //---------------------------------------------------------------------------
  void Remove(Tunnelour::Component * component);

  //---------------------------------------------------------------------------
  // Description : Applies the mutator to all the components in this composite
  //---------------------------------------------------------------------------
  void Apply(Tunnelour::Component::Component_Mutator * const mutator);

  //---------------------------------------------------------------------------
  // Description : Observe all components of this type
  //---------------------------------------------------------------------------
  void ObserveType(Component_Composite_Type_Observer* component_observer, std::string type);

  //---------------------------------------------------------------------------
  // Description : Stop observing all components of this type
  //---------------------------------------------------------------------------
  void IgnoreType(Component_Composite_Type_Observer* component_observer, std::string type);

 protected:
  //---------------------------------------------------------------------------
  // Description : Component Storage
  //---------------------------------------------------------------------------
  std::list<Tunnelour::Component*> m_components;

  //---------------------------------------------------------------------------
  // Description : Notify all type observers
  //---------------------------------------------------------------------------
  void NotifyOnAddType(Tunnelour::Component * component);
  void NotifyOnRemoveType(Tunnelour::Component * component);

 private:
  std::list<std::pair<std::string, Component_Composite_Type_Observer*>> m_type_observers;

};
}
#endif  // TUNNELOUR_COMPONENT_COMPOSITE_H_
