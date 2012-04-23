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

#ifndef TUNNELOUR_CONTROLLER_COMPOSITE_H_
#define TUNNELOUR_CONTROLLER_COMPOSITE_H_

#include <list>

#include "Controller.h"
#include "Component_Composite.h"

namespace Tunnelour {
class Controller_Composite {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Controller_Composite();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Controller_Composite();

  //---------------------------------------------------------------------------
  // Description : Initialise this Composite Controller
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Add a Controller
  //---------------------------------------------------------------------------
  Tunnelour::Controller * const Add(Tunnelour::Controller * const controller);

  //---------------------------------------------------------------------------
  // Description : Remove a Controller
  //---------------------------------------------------------------------------
  void Remove(Tunnelour::Controller * controller);

  //---------------------------------------------------------------------------
  // Description : Run all Controllers
  //---------------------------------------------------------------------------
  void Run();

 protected:
  Tunnelour::Component_Composite *m_model;
  std::list<Tunnelour::Controller*> m_controllers;

 private:
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CONTROLLER_COMPOSITE_H_
