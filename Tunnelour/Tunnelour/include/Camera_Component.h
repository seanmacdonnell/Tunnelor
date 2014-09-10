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

#ifndef TUNNELOUR_CAMERA_COMPONENT_H_
#define TUNNELOUR_CAMERA_COMPONENT_H_

//-----------------------------------------------------------------------------
// NOTE(sean): d3d11 is not required for this class, I am inclduing it as a
//             tempory fix for a compatibility error of including d3dx10math
//             before d3d11.
// TODO(sean): Either replace d3dx10math with a different maths library or fix
//             the include order dependancy.
//-----------------------------------------------------------------------------
#include <d3d11.h>
#include <d3dx10math.h>
#include "Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Camera_Component contains all the information
//               for using a camera in a 3d environment.
//-----------------------------------------------------------------------------
class Camera_Component: public Tunnelour::Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Camera_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  ~Camera_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  void Init();

  //---------------------------------------------------------------------------
  // Description : Returns the position vector
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetPosition();

  //---------------------------------------------------------------------------
  // Description : Sets the position vector
  //---------------------------------------------------------------------------
  void SetPosition(D3DXVECTOR3 const & position);
  void SetPosition(float x, float y, float z);

  //---------------------------------------------------------------------------
  // Description : Returns the vector which indicates the position that this
  //               camera is looking at
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetLookingAtPosition();

  //---------------------------------------------------------------------------
  // Description : Sets the vector which indicates the position that this
  //               camera is looking at
  //---------------------------------------------------------------------------
  void SetLookingAtPosition(D3DXVECTOR3 const & at);
  void SetLookingAtPosition(float x, float y, float z);

  //---------------------------------------------------------------------------
  // Description : Returns the vector which indicates which direction is
  //               "up" for this camera.
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetUpDirection();

  //---------------------------------------------------------------------------
  // Description : Sets the vector which indicates which direction is
  //               "up" for this camera.
  //---------------------------------------------------------------------------
  void SetUpDirection(D3DXVECTOR3 const & up);
  void SetUpDirection(float x, float y, float z);

  //---------------------------------------------------------------------------
  // Description : Returns the rotation vector in radians (x0.0174532925f)
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetRotationInRadians();

  //---------------------------------------------------------------------------
  // Description : Sets the rotation vector in radians (x0.0174532925f)
  //---------------------------------------------------------------------------
  void SetRotationInRadians(D3DXVECTOR3 const & rotation);
  void SetRotationInRadians(float pitch, float yaw, float roll);

  //---------------------------------------------------------------------------
  // Description : Returns the field of view in radians
  //---------------------------------------------------------------------------
  float GetFieldOfViewInRadians();

  //---------------------------------------------------------------------------
  // Description : Sets the fov in radians
  //---------------------------------------------------------------------------
  void SetFieldOfViewInRadians(float radians);

  //---------------------------------------------------------------------------
  // Description : Returns the position vector
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetLastPosition();

  //---------------------------------------------------------------------------
  // Description : Sets the position vector
  //---------------------------------------------------------------------------
  void SetLastPosition(D3DXVECTOR3 const & last_position);
  void SetLastPosition(float x, float y, float z);

 protected:
  D3DXVECTOR3 m_position;
  D3DXVECTOR3 m_at;
  D3DXVECTOR3 m_up;
  D3DXVECTOR3 m_rotation;
  D3DXVECTOR3 m_last_position;
  float m_fov;

 private:
};
}  // namespace Tunnelour
#endif  //  TUNNELOUR_CAMERA_COMPONENT_H_
