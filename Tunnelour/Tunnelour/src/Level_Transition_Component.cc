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

#include "Level_Transition_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Level_Transition_Component::Level_Transition_Component(): Component() {
  m_type = "Level_Transition_Component";
  m_first_level = "Level 0";
  m_is_loading = false;
}

//------------------------------------------------------------------------------
Level_Transition_Component::~Level_Transition_Component() {
}

//------------------------------------------------------------------------------
void Level_Transition_Component::Init() {
  m_is_initialised = true;
}

//---------------------------------------------------------------------------
std::string Level_Transition_Component::GetFirstLevel() {
  return m_first_level;
}

//---------------------------------------------------------------------------
bool Level_Transition_Component::IsLoading() {
  return m_is_loading;
}

//---------------------------------------------------------------------------
void Level_Transition_Component::SetIsLoading(bool is_loading) {
  m_is_loading = is_loading;
}

}  // namespace Tunnelour
