//  Copyright 2009 Sean MacDonnell
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

#include "View_Composite.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
View_Composite::View_Composite() {
  m_model = 0;
}

//------------------------------------------------------------------------------
View_Composite::~View_Composite() {
  for each ( Tunnelour::View* view in m_views ) {
    m_views.remove(view);
  }
}

//------------------------------------------------------------------------------
void View_Composite::Init(Tunnelour::Component_Composite* model) {
  m_model = model;
}

//---------------------------------------------------------------------------
void View_Composite::Add(Tunnelour::View *view) {
  m_views.push_back(view);
}

//---------------------------------------------------------------------------
void View_Composite::Remove(Tunnelour::View *view) {
  m_views.remove(view);
  delete view;
}

//---------------------------------------------------------------------------
void View_Composite::Run() {
  for each ( Tunnelour::View* view in m_views ) {
    view->Run();
  }
}
//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
