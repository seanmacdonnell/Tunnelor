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

#ifndef TUNNELOUR_VIEW_COMPOSITE_H_
#define TUNNELOUR_VIEW_COMPOSITE_H_

#include <list>
#include "Component_Composite.h"
#include "View.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : View_Composite maintains a list of View objects.
//-----------------------------------------------------------------------------
class View_Composite {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  View_Composite();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~View_Composite();

  //---------------------------------------------------------------------------
  // Description : Initialise this View_Composite
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Add a View
  // Return      : Pointer to the Added view.
  //---------------------------------------------------------------------------
  Tunnelour::View* Add(Tunnelour::View *view);

  //---------------------------------------------------------------------------
  // Description : Remove a View
  //---------------------------------------------------------------------------
  void Remove(Tunnelour::View *view);

  //---------------------------------------------------------------------------
  // Description : Run all Views
  //---------------------------------------------------------------------------
  void Run();

 protected:
  Tunnelour::Component_Composite* m_model;
  std::list<Tunnelour::View*> m_views;
  bool m_initialized;
 private:
};
}
#endif  // TUNNELOUR_VIEW_COMPOSITE_H_
