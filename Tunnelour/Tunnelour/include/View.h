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

#ifndef TUNNELOUR_VIEW_H_
#define TUNNELOUR_VIEW_H_

#include "Component_Composite.h"

namespace Tunnelour {
class View: public Tunnelour::Component_Composite {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  View();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~View();

  //---------------------------------------------------------------------------
  // Description : Mutator for this views model
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite *model);

  //---------------------------------------------------------------------------
  // Description : View Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:
  Tunnelour::Component_Composite *m_model;

 private:
};
}  // namespace Tunnelour

#endif  // TUNNELOUR_VIEW_H_
