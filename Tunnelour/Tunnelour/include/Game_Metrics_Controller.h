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

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation
//              : and movement of the game_metrics_component.
//-----------------------------------------------------------------------------
class Game_Metrics_Controller: public Controller,
                         public Component::Component_Observer {
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

  virtual void HandleEvent(Tunnelour::Component * const component);
  
  int WhatsTheDistanceBetweenThesePoints(D3DXVECTOR2 point_1, D3DXVECTOR2 point_2);

  void ResetGameMetrics();

 protected:

 private:
  bool InitTimer();
  void UpdateTimer();

  Avatar_Component *m_avatar;
  Game_Settings_Component *m_game_settings;
  Game_Metrics_Component *m_game_metrics;

  //---------------------------------------------------------------------------
  // Description : Creates a Bitmap Component using the given collision block
  //---------------------------------------------------------------------------
  Avatar_Component::Avatar_Collision_Block GetNamedCollisionBlock(std::string id, std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks);
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
