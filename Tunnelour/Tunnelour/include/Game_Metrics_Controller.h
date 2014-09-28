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

#ifndef TUNNELOUR_GAME_METRICS_CONTROLLER_H_
#define TUNNELOUR_GAME_METRICS_CONTROLLER_H_

#include "Game_Settings_Component.h"
#include "Game_Metrics_Controller_Mutator.h"
#include "Avatar_Component.h"
#include "Controller.h"
#include "Game_Metrics_Component.h"
#include "World_Settings_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : The Game_Metrics_Controller records the avatars
//              : distance travelled (in pixels) and the current time.
//-----------------------------------------------------------------------------
class Game_Metrics_Controller: public Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Game_Metrics_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Game_Metrics_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

  void ResetGameMetrics();

 protected:

 private:
  bool StartTimer();
  void IsItTimeToAnimateAFrame();

  Avatar_Component *m_avatar;
  Game_Settings_Component *m_game_settings;
  Game_Metrics_Component *m_game_metrics;
  World_Settings_Component *m_world_settings;
  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  int m_last_frame_time;
  bool m_animation_tick;
  int m_current_animation_fps;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_GAME_METRICS_CONTROLLER_H_
