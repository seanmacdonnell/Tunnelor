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

#ifndef TUNNELOUR_CAMERA_COMPONENT_H_
#define TUNNELOUR_CAMERA_COMPONENT_H_

#include "Component.h"

//-----------------------------------------------------------------------------
// NOTE: d3d11 is not required for this class, I am inclduing it as a  
//       tempory fix for a compatibility error of including d3dx10math 
//       before d3d11.
// TODO: Either replace d3dx10math with a different maths library or fix
//       the include order dependancy.
//-----------------------------------------------------------------------------
#include <d3d11.h>
#include <d3dx10math.h>

namespace Tunnelour {
  class Camera_Component: public Tunnelour::Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Camera_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Camera_Component();

  //---------------------------------------------------------------------------
  // Description : Returns the position vector
  //---------------------------------------------------------------------------
  const D3DXVECTOR3* GetPosition() const;

  //---------------------------------------------------------------------------
  // Description : Sets the position vector
  //---------------------------------------------------------------------------
  void SetPosition(const D3DXVECTOR3& position);
  void SetPosition(const float x, const float y, const float z);

  //---------------------------------------------------------------------------
  // Description : Returns the vector which indicates what this camera is lookin at
  //---------------------------------------------------------------------------
  const D3DXVECTOR3* GetLookingAtPosition() const;

  //---------------------------------------------------------------------------
  // Description : Sets the position vector
  //---------------------------------------------------------------------------
  void SetLookingAtPosition(const D3DXVECTOR3& position);
  void SetLookingAtPosition(const float x, const float y, const float z);

  //---------------------------------------------------------------------------
  // Description : Returns the vector which indicates what this camera is lookin at
  //---------------------------------------------------------------------------
  const D3DXVECTOR3* GetUpDirection() const;

  //---------------------------------------------------------------------------
  // Description : Sets the position vector
  //---------------------------------------------------------------------------
  void SetUpDirection(const D3DXVECTOR3& orientation);
  void SetUpDirection(const float x, const float y, const float z);

  //---------------------------------------------------------------------------
  // Description : Returns the field of view in radians
  //---------------------------------------------------------------------------
  const float GetFieldOfViewInRadians() const;

  //---------------------------------------------------------------------------
  // Description : Sets the fov in radians
  //---------------------------------------------------------------------------
  void SetFieldOfViewInRadians(float radians);

 protected:
  D3DXVECTOR3 *m_position;
  D3DXVECTOR3 *m_at;
  D3DXVECTOR3 *m_up;
  float m_fov;

 private:

};
} // namespace Tunnelour
#endif  // TUNNELOUR_CAMERA_COMPONENT_H_
