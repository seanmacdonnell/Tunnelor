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

#ifndef TUNNELOUR_CONTROLLER_H_
#define TUNNELOUR_CONTROLLER_H_

#include "Component_Composite.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Controller is a base type for controllers, they are designed
//                to read and modify the Composite_Component class (the model)
//-----------------------------------------------------------------------------
class Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite *const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

  //---------------------------------------------------------------------------
  // Description : Accessor for whether this controller can be removed
  //---------------------------------------------------------------------------
  bool IsFinished();

  //---------------------------------------------------------------------------
  // Description : Accessor for whether this controller can be removed
  //---------------------------------------------------------------------------
  bool HasBeenInitalised();

 protected:
  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  Component_Composite *m_model;

  // Set m_is_finished to true when this controller is no longer required
  bool m_is_finished;
  bool m_has_been_initialised;

 private:
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CONTROLLER_H_
