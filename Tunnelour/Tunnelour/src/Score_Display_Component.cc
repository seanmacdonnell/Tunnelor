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

#include "Score_Display_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Score_Display_Component::Score_Display_Component(): Component() {
  m_type = "Score_Display_Component";
  m_first_level = "Level 0";
  m_is_loading = false;
}

//------------------------------------------------------------------------------
Score_Display_Component::~Score_Display_Component() {
}

//------------------------------------------------------------------------------
void Score_Display_Component::Init() {
  m_is_initialised = true;
}

//---------------------------------------------------------------------------
std::string Score_Display_Component::GetFirstLevel() {
  return m_first_level;
}

//---------------------------------------------------------------------------
bool Score_Display_Component::IsLoading() {
  return m_is_loading;
}

//---------------------------------------------------------------------------
void Score_Display_Component::SetIsLoading(bool is_loading) {
  m_is_loading = is_loading;
}

}  // namespace Tunnelour
