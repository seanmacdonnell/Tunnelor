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

#ifndef TUNNELOUR_TUNNELOUR_VIEW_H_
#define TUNNELOUR_TUNNELOUR_VIEW_H_

#include "View_Composite.h"
#include "Component_Composite.h"


namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Tunnelour_View is the specialised view for the
//                tunnelour game program.
//-----------------------------------------------------------------------------
class Tunnelour_View: public Tunnelour::View_Composite {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Tunnelour_View();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Tunnelour_View();

  //---------------------------------------------------------------------------
  // Description : Initialisation Function
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : View Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:
  bool m_initialized;

 private:
};
}  // namespace Tunnelour

#endif  // TUNNELOUR_TUNNELOUR_VIEW_H_
