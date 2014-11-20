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

#ifndef TUNNELOUR_CHARLIE_STANDING_CONTROLLER_H_
#define TUNNELOUR_CHARLIE_STANDING_CONTROLLER_H_

#include "Avatar_State_Controller.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This class controls actions of the avatar while they are
//                in the standing state
//-----------------------------------------------------------------------------
class Charlie_Standing_Controller: public Avatar_State_Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Charlie_Standing_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Charlie_Standing_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Charlie_Standing_Controller
  //---------------------------------------------------------------------------
  bool Init(Component_Composite *const model);

  //---------------------------------------------------------------------------
  // Description : Virtual function in order to run the avatar state
  //---------------------------------------------------------------------------
  virtual void Run_Avatar_State();

 protected:
  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------

 private:
  const float m_avatar_z_position;
  const float m_vertical_jump_y_initial_velocity;
  const float m_vertical_jump_x_initial_velocity;
  const float m_gap_jump_y_initial_velocity;
  const float m_gap_jump_x_initial_velocity;
  const float m_takeoff_gap_jump_y_initial_velocity;
  const float m_takeoff_gap_jump_x_initial_velocity;
  const float m_running_x_velocity;
  const float m_takeoff_running_x_velocity;
  const float m_takeoff_running_x_distance;
  const float m_stopping_from_running_distance;
  const float m_overbalancing_x_velocity;
  const float m_overbalancing_y_velocity;
  const float m_wall_jump_y_initial_velocity;
  const float m_wall_jump_x_initial_velocity;
  const float m_wall_jump_boost_distance_threshold;
  const float m_wall_colliding_mid_y_initial_velocity;
  const float m_wall_colliding_mid_x_initial_velocity;
  const float m_safe_falling_limit;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CHARLIE_STANDING_CONTROLLER_H_
