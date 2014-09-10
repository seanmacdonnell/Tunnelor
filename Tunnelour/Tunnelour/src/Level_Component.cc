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

#include "Level_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Level_Component::Level_Component(): Component() {
  m_type = "Level_Component";
  m_is_complete = false;
}

//------------------------------------------------------------------------------
Level_Component::~Level_Component() {
}

//------------------------------------------------------------------------------
void Level_Component::Init() {
  m_is_initialised = true;
}

//---------------------------------------------------------------------------
Level_Component::Level_Metadata Level_Component::GetCurrentLevel() {
  return m_current_level_metadata;
}

//---------------------------------------------------------------------------
void Level_Component::SetCurrentLevel(Level_Metadata current_level) {
  m_current_level_metadata = current_level;
  m_is_complete = false;
}

//---------------------------------------------------------------------------
bool Level_Component::IsComplete() {
  return m_is_complete;
}

//---------------------------------------------------------------------------
void Level_Component::SetIsComplete(bool is_complete) {
  m_is_complete = is_complete;
}

}  // namespace Tunnelour
