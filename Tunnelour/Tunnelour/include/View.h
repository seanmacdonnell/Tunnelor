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

#ifndef TUNNELOUR_VIEW_H_
#define TUNNELOUR_VIEW_H_

#include "Component_Composite.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : View is a base type for an object which interprets the
//                provided Component_Composite (model) into output.
//-----------------------------------------------------------------------------
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
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : View Runner
  //---------------------------------------------------------------------------
  virtual void Run();

  //---------------------------------------------------------------------------
  // Description : Has this View been Inisalised?
  //---------------------------------------------------------------------------
  virtual bool IsInitialised();

 protected:
  Tunnelour::Component_Composite *m_model;
  bool m_is_initialised;

 private:
};
}  // namespace Tunnelour

#endif  // TUNNELOUR_VIEW_H_
