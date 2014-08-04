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

#ifndef TUNNELOUR_CHARLIE_CLIMBING_CONTROLLER_H_
#define TUNNELOUR_CHARLIE_CLIMBING_CONTROLLER_H_

#include "Avatar_State_Controller.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Controls the avatar when they are in the "Climbing" state.
//-----------------------------------------------------------------------------
class Charlie_Climbing_Controller: public Avatar_State_Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Charlie_Climbing_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Charlie_Climbing_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation
  //---------------------------------------------------------------------------
  bool Init(Component_Composite *const model);

  //---------------------------------------------------------------------------
  // Description : Runs the State Control
  //---------------------------------------------------------------------------
  virtual void Run_Avatar_State();

 protected:

 private:
  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  int m_avatar_z_position;
  int m_vertical_jump_y_initial_Velocity;
  int m_vertical_jump_x_initial_Velocity;
  int m_wall_jump_speed_offset;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CHARLIE_CLIMBING_CONTROLLER_H_
