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

#ifndef TUNNELOUR_CAMERA_CONTROLLER_H_
#define TUNNELOUR_CAMERA_CONTROLLER_H_

#include <vector>

#include "Game_Settings_Component.h"
#include "Camera_Controller_Mutator.h"
#include "Avatar_Component.h"
#include "Controller.h"
#include "Camera_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation
//              : and movement of the camera_component.
//-----------------------------------------------------------------------------
class Camera_Controller: public Controller,
                         public Component::Component_Observer,
                         public Component_Composite::Component_Composite_Type_Observer {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Camera_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Camera_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

  //---------------------------------------------------------------------------
  // Description : Handing for when the avatar is repositioned
  //---------------------------------------------------------------------------
  virtual void HandleEvent(Tunnelour::Component * const component);

  //---------------------------------------------------------------------------
  // Description : Called when handling an "Add" call in the model
  //---------------------------------------------------------------------------
  virtual void HandleEventAdd(Tunnelour::Component * const component);

  //---------------------------------------------------------------------------
  // Description : Called when handling a "Remove" call in the model
  //---------------------------------------------------------------------------
  virtual void HandleEventRemove(Tunnelour::Component * const component);

  //---------------------------------------------------------------------------
  // Description : Returns how far the avatar has traveled in x from
  //               the stationary position.
  //---------------------------------------------------------------------------
  float HowFarHasAvatarTravelled();

  //---------------------------------------------------------------------------
  // Description : Returns how far the avatar has traveled in x from
  //               the last rendered position of the avatar.
  //---------------------------------------------------------------------------
  float HowFarHasAvatarTravelledLastFrame();

  //---------------------------------------------------------------------------
  // Description : Generates a value between the current camera position and
  //               the desired camera position which will give a smooth
  //               transition between the points.
  // Returns     : An offset which is added to the last camera position.
  //---------------------------------------------------------------------------
  float CalculateSmoothSnapXOffset(float camera_position_x);

  //---------------------------------------------------------------------------
  // Description : Generates a value between the current camera position and
  //               the desired camera position which will give a smooth
  //               transition between the points.
  // Returns     : An offset which is added to the last camera position.
  //---------------------------------------------------------------------------
  float CalculateSmoothSnapYOffset(float camera_position_y);

 private:
  Avatar_Component *m_avatar;
  Game_Settings_Component *m_game_settings;
  Camera_Component *m_camera;
  bool m_is_shaking;
  float m_radius;
  float m_randomAngle;
  std::vector<Tile_Bitmap*> m_floor_tiles;
  Bitmap_Component *m_adjacent_floor_tile;
  int m_distance_travelled;
  int m_leash_length;
  D3DXVECTOR2 m_stationary_avatar_position;
  Avatar_Component::Avatar_State m_last_state;
  int m_max_x_look_distance;
  int m_max_y_look_distance;
  Input_Component *m_input;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_CAMERA_CONTROLLER_H_
