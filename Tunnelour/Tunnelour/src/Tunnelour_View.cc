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

#include "Tunnelour_View.h"
#include "Exceptions.h"

#include "Direct3D11_View.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Tunnelour_View::Tunnelour_View() {
  m_initialized = false;
}

//------------------------------------------------------------------------------
Tunnelour_View::~Tunnelour_View() {
}

//------------------------------------------------------------------------------
void Tunnelour_View::Init(Tunnelour::Component_Composite * const model) {
    if (m_initialized) { return; }

    View_Composite::Init(model);

    Tunnelour::View* D3D11_view;
    D3D11_view = Add(new Tunnelour::Direct3D11_View());
    D3D11_view->Init(model);

    m_initialized = true;
}

//------------------------------------------------------------------------------
void Tunnelour_View::Run() {
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
