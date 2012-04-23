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

#ifndef TUNNELOUR_CONTROLLER_H_
#define TUNNELOUR_CONTROLLER_H_

#include "Component_Composite.h"

namespace Tunnelour {
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
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual void Run();

  //---------------------------------------------------------------------------
  // Description : Accessor for whether this controller can be removed
  //---------------------------------------------------------------------------
  bool IsFinished();

 protected:
  Tunnelour::Component_Composite *m_model;
  bool m_is_finished;

 private:
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CONTROLLER_H_
