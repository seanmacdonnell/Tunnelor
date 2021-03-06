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

#ifndef TUNNELOUR_COMPONENT_H_
#define TUNNELOUR_COMPONENT_H_

#include <list>
#include <string>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Component is a base type for storing all the information
//                about the game world.
//-----------------------------------------------------------------------------
class Component {
 public:
  //---------------------------------------------------------------------------
  // Author(s)   : Sean MacDonnell
  // Description : Mutator for this component type, a class which modifies
  //               the component by overloading the Mutate function call.
  //---------------------------------------------------------------------------
  class Component_Mutator {
   public:
    //-------------------------------------------------------------------------
    // Description : Mutator function, pass it a component to mutate.
    //-------------------------------------------------------------------------
    virtual void Mutate(Tunnelour::Component * const component) = 0;
  };
  //---------------------------------------------------------------------------
  // Author(s)   : Sean MacDonnell
  // Description : Inheritable Observer class for this component.
  //---------------------------------------------------------------------------
  class Component_Observer {
   public:
    //-------------------------------------------------------------------------
    // Description : HandleEvent is called when the component changes.
    //-------------------------------------------------------------------------
    virtual void HandleEvent(Tunnelour::Component * const component) = 0;
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

  //---------------------------------------------------------------------------
  // Description : Returns whether this component has been initalised
  //---------------------------------------------------------------------------
  bool IsInitialised();

  //---------------------------------------------------------------------------
  // Description : Accessor for the type of this component
  //---------------------------------------------------------------------------
  std::string const GetType();

  //---------------------------------------------------------------------------
  // Description : Mutator for the type of this component
  //---------------------------------------------------------------------------
  void SetType(std::string const type);

 protected:
  //---------------------------------------------------------------------------
  // Description : Notify all observers
  //---------------------------------------------------------------------------
  void Notify();

  //---------------------------------------------------------------------------
  // Description : Has this component been initialised?
  //---------------------------------------------------------------------------
  bool m_is_initialised;

  //---------------------------------------------------------------------------
  // Description : Type of this component as a string.
  //---------------------------------------------------------------------------
  std::string m_type;

protected:
  //---------------------------------------------------------------------------
  // Description : The unqiue identifier for this specifc component
  //---------------------------------------------------------------------------
  int m_id;
  //---------------------------------------------------------------------------
  // Description : List of Component_Observers Observing this component
  //               Is a list because it makes it smoother to delete Observers
  //               who want to ignore this component.
  // TODO(Sean) :  Evaluate whether a Map or a List or a Vector is faster for
  //               this.
  //---------------------------------------------------------------------------
  std::list<Component_Observer* const> m_observers;

 private:

};
}  // namespace Tunnelour
#endif  // TUNNELOUR_COMPONENT_H_
