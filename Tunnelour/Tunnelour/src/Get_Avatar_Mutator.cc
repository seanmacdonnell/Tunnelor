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

#include "Get_Avatar_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Get_Avatar_Mutator::Get_Avatar_Mutator() {
  m_found_avatar_component = false;
  m_avatar_controller = 0;
}

//------------------------------------------------------------------------------
Get_Avatar_Mutator::~Get_Avatar_Mutator() {
  m_found_avatar_component = false;
  m_avatar_controller = 0;
}

//------------------------------------------------------------------------------
void Get_Avatar_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Avatar_Component") == 0) {
    m_avatar_controller = static_cast<Avatar_Component*>(component);
    m_found_avatar_component = true;
  }
}

//------------------------------------------------------------------------------
Avatar_Component* const Get_Avatar_Mutator::GetAvatarComponent() {
  return m_avatar_controller;
}

//------------------------------------------------------------------------------
bool Get_Avatar_Mutator::WasSuccessful() {
  return m_found_avatar_component;
}
}  // namespace Tunnelour
