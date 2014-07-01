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
  m_distance_traveled = 0;
  m_seconds_past = 0.0;
  m_type = "Game_Metrics_Component";
}

//------------------------------------------------------------------------------
Game_Metrics_Component::~Game_Metrics_Component() {
  m_fps_data.count = 0;
  m_fps_data.fps = 0;
  m_fps_data.startTime = 0;
  m_distance_traveled = 0;
  m_seconds_past = 0.0;
}

//------------------------------------------------------------------------------
void Game_Metrics_Component::Init() {
  m_fps_data.count = 0;
  m_fps_data.fps = 0;
  m_fps_data.startTime = 0;
  m_distance_traveled = 0;
  m_seconds_past = 0.0;
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
long double Game_Metrics_Component::GetDistanceTraveled() {
  return m_distance_traveled;
}

//------------------------------------------------------------------------------
void Game_Metrics_Component::SetDistanceTraveled(long double distance) {
  m_distance_traveled = distance;
}

//------------------------------------------------------------------------------
long double Game_Metrics_Component::GetSecondsPast() {
  return m_seconds_past;
}

//------------------------------------------------------------------------------
void Game_Metrics_Component::SetSecondsPast(long double seconds_past) {
  m_seconds_past = seconds_past;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
