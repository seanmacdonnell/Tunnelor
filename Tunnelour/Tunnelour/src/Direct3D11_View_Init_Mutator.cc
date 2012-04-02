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

#include "Direct3D11_View_Init_Mutator.h"
#include "Background_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View_Init_Mutator::Direct3D11_View_Init_Mutator() {
  m_back_buffer_color[0] = 0.0;
  m_back_buffer_color[1] = 0.0;
  m_back_buffer_color[2] = 0.0;
  m_back_buffer_color[3] = 1.0;
  m_found_back_buffer_color = false;
}

//------------------------------------------------------------------------------
Direct3D11_View_Init_Mutator::~Direct3D11_View_Init_Mutator() {
}

//------------------------------------------------------------------------------
void Direct3D11_View_Init_Mutator::Mutate(Tunnelour::Component *component) {
  Tunnelour::Background_Component *background_c = dynamic_cast<Tunnelour::Background_Component*>(component);
  if (background_c) {
    //Found Background_Component
    m_back_buffer_color[0] = background_c->GetRed();
	  m_back_buffer_color[1] = background_c->GetGreen();
	  m_back_buffer_color[2] = background_c->GetBlue();
	  m_back_buffer_color[3] = background_c->GetAlpha();
    m_found_back_buffer_color = true;
  }
}

}  // namespace Tunnelour
