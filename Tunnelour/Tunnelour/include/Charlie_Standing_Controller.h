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

#ifndef TUNNELOUR_CHARLIE_STANDING_CONTROLLER_H_
#define TUNNELOUR_CHARLIE_STANDING_CONTROLLER_H_

#include "Avatar_State_Controller.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Charlie_Standing_Controller is a base type for controllers, they are designed
//                to read and modify the Composite_Component class (the model)
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
  int m_avatar_z_position;
  int m_vertical_jump_y_initial_velocity;
  int m_vertical_jump_x_initial_velocity;
  int m_wall_jump_y_initial_velocity;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CHARLIE_STANDING_CONTROLLER_H_
