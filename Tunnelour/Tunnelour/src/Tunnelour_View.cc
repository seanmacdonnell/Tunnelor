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

#include "Tunnelour_View.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Tunnelour_View::Tunnelour_View() {
  m_D3D11_view = NULL;
}

//------------------------------------------------------------------------------
Tunnelour_View::~Tunnelour_View() {
}

//------------------------------------------------------------------------------
void Tunnelour_View::Init(Tunnelour::Component_Composite *model) {
  View_Composite::Init(model);

  m_D3D11_view = new Tunnelour::Direct3D11_View();
  m_D3D11_view->Init(model);

  Add(m_D3D11_view);
}

//------------------------------------------------------------------------------
void Tunnelour_View::Run() {
  if (m_D3D11_view == NULL) { 
    throw Tunnelour::Exceptions::uninitialised_error(
      "DirectX 3D 11 View is Uninitialised!");
  }

  m_D3D11_view->Run();
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
