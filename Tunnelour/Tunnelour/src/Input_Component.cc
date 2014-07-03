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

#include "Input_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Input_Component::Input_Component(): Component() {
  m_current_key_input.IsSpace = false;
  m_current_key_input.IsEsc = false;
  m_last_key_input.IsSpace = false;
  m_last_key_input.IsEsc = false;
  m_type = "Input_Component";
}

//------------------------------------------------------------------------------
Input_Component::~Input_Component() {
}

//------------------------------------------------------------------------------
void Input_Component::Init() {
  m_is_initialised = true;
}

//---------------------------------------------------------------------------
Input_Component::Key_Input Input_Component::GetCurrentKeyInput() {
  return m_current_key_input;
}

//---------------------------------------------------------------------------
void Input_Component::SetCurrentKeyInput(Key_Input key_input) {
  m_last_key_input = m_current_key_input;
  m_current_key_input = key_input;
}

//---------------------------------------------------------------------------
Input_Component::Key_Input Input_Component::GetLastKeyInput() {
  return m_last_key_input;
}

}  // namespace Tunnelour
