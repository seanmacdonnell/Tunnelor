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

#include "Procedural_Level_Tile_Controller.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include "Exceptions.h"
#include "String_Helper.h"
#include "Bitmap_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Procedural_Level_Tile_Controller::Procedural_Level_Tile_Controller() : Level_Tile_Controller() {
}

//------------------------------------------------------------------------------
Procedural_Level_Tile_Controller::~Procedural_Level_Tile_Controller() {
}

//------------------------------------------------------------------------------
bool Procedural_Level_Tile_Controller::Init(Component_Composite * const model) {
  return Level_Tile_Controller::Init(model);
}

//------------------------------------------------------------------------------
bool Procedural_Level_Tile_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }

  if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
    SwitchTileset();
  }

  return true;
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::CreateLevel() {
  m_current_level_name = m_level->GetCurrentLevel().level_name;
  m_created_tiles = GenerateTunnelFromMetadata(m_level->GetCurrentLevel());
  SwitchTileset();
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::HandleEvent(Tunnelour::Component * const component) {
  if (component->GetType().compare("Camera_Component") == 0) {
    Run();
  }
}

} // Tunnelour