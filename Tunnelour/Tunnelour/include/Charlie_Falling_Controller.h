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

#ifndef TUNNELOUR_CHARLIE_FALLING_CONTROLLER_H_
#define TUNNELOUR_CHARLIE_FALLING_CONTROLLER_H_

#include "Avatar_State_Controller.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Runs the Avatar Falling State
//-----------------------------------------------------------------------------
class Charlie_Falling_Controller: public Avatar_State_Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Charlie_Falling_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Charlie_Falling_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation
  //---------------------------------------------------------------------------
  bool Init(Component_Composite *const model);

  //---------------------------------------------------------------------------
  // Description : Runs the Avatar State
  //---------------------------------------------------------------------------
  virtual void Run_Avatar_State();

 protected:
  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------

 private:
  int m_avatar_z_position;
  int m_vertical_jump_y_initial_velocity;
  int m_vertical_jump_x_initial_velocity;
  int m_wall_jump_y_initial_velocity;
  float m_falling_point_of_safe_landing;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CHARLIE_FALLING_CONTROLLER_H_
