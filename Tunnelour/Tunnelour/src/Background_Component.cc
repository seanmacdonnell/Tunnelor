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

#include "Background_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Background_Component::Background_Component(): Component() {
  // red, green, blue, alpha
  // Set to Black as Default
  m_color = D3DXCOLOR(0.0, 0.0, 0.0, 1.0);
  m_type = "Background_Component";
}

//------------------------------------------------------------------------------
Background_Component::~Background_Component() {
}

//------------------------------------------------------------------------------
void Background_Component::Init() {
}

//------------------------------------------------------------------------------
D3DXCOLOR Background_Component::GetColor() {
  return m_color;
}

//------------------------------------------------------------------------------
void Background_Component::SetColor(D3DXCOLOR & color) {
  m_color = color;
}

//------------------------------------------------------------------------------
void Background_Component::SetColor(float red, float green, float blue, float alpha) {
  m_color = D3DXCOLOR(red, green, blue, alpha);
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
