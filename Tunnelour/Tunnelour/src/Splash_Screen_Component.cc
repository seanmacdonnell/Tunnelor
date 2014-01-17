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

#include "Splash_Screen_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Splash_Screen_Component::Splash_Screen_Component(): Component() {
  m_type = "Splash_Screen_Component";
  m_is_fading = false;
  m_has_faded = false;
}

//------------------------------------------------------------------------------
Splash_Screen_Component::~Splash_Screen_Component() {
}

//------------------------------------------------------------------------------
void Splash_Screen_Component::Init() {
  m_is_initialised = true;
}

//---------------------------------------------------------------------------
bool Splash_Screen_Component::IsFading() {
  return m_is_fading;
}

//---------------------------------------------------------------------------
void Splash_Screen_Component::SetIsFading(bool is_fading) {
  m_is_fading = is_fading;
}

//---------------------------------------------------------------------------
bool Splash_Screen_Component::HasFaded() {
  return m_has_faded;
}

//---------------------------------------------------------------------------
void Splash_Screen_Component::SetHasFaded(bool has_faded) {
  m_has_faded = has_faded;
}

}  // namespace Tunnelour
