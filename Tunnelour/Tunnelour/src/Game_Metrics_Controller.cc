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

#include "Game_Metrics_Controller.h"
#include "Geometry_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Game_Metrics_Controller::Game_Metrics_Controller() : Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_game_metrics = 0;
}

//------------------------------------------------------------------------------
Game_Metrics_Controller::~Game_Metrics_Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_game_metrics = 0;
}

//------------------------------------------------------------------------------
bool Game_Metrics_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  if (m_game_metrics == 0) {
    m_game_metrics = new Game_Metrics_Component();
    m_model->Add(m_game_metrics);
    m_game_metrics->Init();
  }
  Game_Metrics_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_avatar = mutator.GetAvatarComponent();
    m_game_settings = mutator.GetGameSettings();
    m_world_settings = mutator.GetWorldSettings();
    StartTimer();
    m_has_been_initialised = true;
  } else {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Game_Metrics_Controller::Run() {
  Game_Metrics_Controller_Mutator mutator;
  if (m_has_been_initialised) {
    D3DXVECTOR2 current_avatar_position;
    current_avatar_position.x = m_avatar->GetPosition()->x;
    current_avatar_position.y = 0;
    D3DXVECTOR2 last_avatar_position;
    last_avatar_position.x = m_avatar->GetLastRenderedPosition().x;
    last_avatar_position.y = 0;

    long double distance_traveled = 0;
    distance_traveled = Geometry_Helper::WhatsTheDistanceBetweenThesePoints(current_avatar_position,
                                                                            last_avatar_position);

    // The distance can't be higher than the max velocity per frame
    // If its the case the avatar has likely been reset.
    // GetMaxVelocityInPixPerFrame is a negative number
    if (distance_traveled <= (m_world_settings->GetMaxVelocityInPixPerFrame() * -1)) {
      long double distance_traveled_metres = distance_traveled * m_world_settings->GetPixelToMeterFactor();
      distance_traveled_metres += m_game_metrics->GetDistanceTraveled();
      m_game_metrics->SetDistanceTraveled(distance_traveled_metres);
    }

    IsItTimeToAnimateAFrame();
  } else {
    Init(m_model);
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
void Game_Metrics_Controller::ResetGameMetrics() {
  m_game_metrics->SetDistanceTraveled(0);
  m_game_metrics->SetSecondsPast(0);
  StartTimer();
}


//------------------------------------------------------------------------------
bool Game_Metrics_Controller::StartTimer() {
  // Check to see if this system supports high performance timers.
  QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));

  if (m_frequency == 0) {
    return false;
  }

  // Find out how many times the frequency counter ticks every millisecond.
  m_ticksPerMs = static_cast<float>(m_frequency / 1000);

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTime));

  return true;
}

//------------------------------------------------------------------------------
void Game_Metrics_Controller::IsItTimeToAnimateAFrame() {
  INT64 currentTime;
  float timeDifference;

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

  timeDifference = static_cast<float>(currentTime - m_startTime);

  m_frameTime = timeDifference / m_ticksPerMs;

  m_game_metrics->SetSecondsPast(m_frameTime/1000);
}


//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
