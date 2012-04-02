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

#ifndef TUNNELOUR_COMPONENT_H_
#define TUNNELOUR_COMPONENT_H_

#include <list>

namespace Tunnelour {
class Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Mutator for this component type
  //---------------------------------------------------------------------------
  class Component_Mutator {
   public:
    //-------------------------------------------------------------------------
    // Description : Mutator function, pass it a component to mutate.
    //-------------------------------------------------------------------------
    virtual void Mutate(Tunnelour::Component *component) = 0;
  };
  //---------------------------------------------------------------------------
  // Description : Inheritable Observer class for this component.
  //---------------------------------------------------------------------------
  class Component_Observer {
   public:
    //-------------------------------------------------------------------------
    // Description : Constructor
    //-------------------------------------------------------------------------
    Component_Observer();

    //-------------------------------------------------------------------------
    // Description : Deconstructor
    //-------------------------------------------------------------------------
    virtual ~Component_Observer();

    //-------------------------------------------------------------------------
    // Description : handleEvent is called when the component changes.
    //-------------------------------------------------------------------------
    virtual void handleEvent(const Component&) = 0;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  //---------------------------------------------------------------------------
  // Description : Accessor for the unique component id
  //---------------------------------------------------------------------------
  int GetID();

  //---------------------------------------------------------------------------
  // Description : Observe this component
  //---------------------------------------------------------------------------
  void Observe(Component_Observer* component_observer);

  //---------------------------------------------------------------------------
  // Description : Stop observing this component
  //---------------------------------------------------------------------------
  void Ignore(Component_Observer* component_observer);

 protected:
  //---------------------------------------------------------------------------
  // Description : Notify all observers
  //---------------------------------------------------------------------------
  void Notify();

 private:
  //---------------------------------------------------------------------------
  // Description : The unqiue identifier for this specifc component
  //---------------------------------------------------------------------------
  int m_id;
  //---------------------------------------------------------------------------
  // Description : Map of Component_Observers Observing this component
  //               Is a map because it makes it smoother to delete Observers
  //               who want to ignore this component.
  // TODO(Sean) :  Evaluate whether a Map or a List or a Vector is faster for
  //               this.
  //---------------------------------------------------------------------------
  std::list<Component_Observer* const> m_observers;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_COMPONENT_H_
