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

#include "Game_Metrics_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Game_Metrics_Component::Game_Metrics_Component(): Component() {
  m_fps_data.count = 0;
  m_fps_data.fps = 0;
  m_fps_data.startTime = 0;
  m_type = "Game_Metrics_Component";
}

//------------------------------------------------------------------------------
Game_Metrics_Component::~Game_Metrics_Component() {
}

//------------------------------------------------------------------------------
void Game_Metrics_Component::Init() {
  m_fps_data.count = 0;
  m_fps_data.fps = 0;
  m_fps_data.startTime = 0;
}

//------------------------------------------------------------------------------
Tunnelour::Game_Metrics_Component::FPS_Data Game_Metrics_Component::GetFPSData() {
  return m_fps_data;
}

//------------------------------------------------------------------------------
void Game_Metrics_Component::SetFPSData(Tunnelour::Game_Metrics_Component::FPS_Data fps_data) {
  m_fps_data = fps_data;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
