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

#include "Camera_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Camera_Component::Camera_Component() {
  m_position = new D3DXVECTOR3(0.0f, 0.0f, -10.0f);
  m_at = new D3DXVECTOR3(0.0f, 0.0f, 1.0f);
  m_up = new D3DXVECTOR3(0.0f, 1.0f, 0.0f);
  m_rotation = new D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_fov = 0;
}

//------------------------------------------------------------------------------
Camera_Component::~Camera_Component() {
  delete m_position;
  delete m_at;
  delete m_up;
  delete m_rotation;
  m_fov = 0;
}

//------------------------------------------------------------------------------
D3DXVECTOR3* Camera_Component::GetPosition() const {
  return m_position;
}

//------------------------------------------------------------------------------
void Camera_Component::SetPosition(const D3DXVECTOR3& position) {
  *m_position = position;
  Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetPosition(const float x, const float y, const float z) {
  m_position->x = x;
  m_position->y = y;
  m_position->z = z;
  Notify();
}

//------------------------------------------------------------------------------
D3DXVECTOR3* Camera_Component::GetLookingAtPosition() const {
  return m_at;
}

//------------------------------------------------------------------------------
void Camera_Component::SetLookingAtPosition(const D3DXVECTOR3& position) {
  *m_at = position;
   Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetLookingAtPosition(const float x, const float y, const float z) {
  m_at->x = x;
  m_at->y = y;
  m_at->z = z;
  Notify();
}

//------------------------------------------------------------------------------
D3DXVECTOR3* Camera_Component::GetUpDirection() const {
  return m_up;
}

//------------------------------------------------------------------------------
void Camera_Component::SetUpDirection(const D3DXVECTOR3& orientation) {
  *m_up = orientation;
  Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetUpDirection(const float x, const float y, const float z) {
  m_up->x = x;
  m_up->y = y;
  m_up->z = z;
  Notify();
}

//------------------------------------------------------------------------------
D3DXVECTOR3* Camera_Component::GetRotationInRadians() const {
  return m_rotation;
}

//------------------------------------------------------------------------------
void Camera_Component::SetRotationInRadians(const D3DXVECTOR3& rotation) {
  *m_rotation = rotation;
  Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetRotationInRadians(const float pitch, const float yaw, const float roll) {
  m_rotation->x = pitch;
  m_rotation->y = yaw;
  m_rotation->z = roll;
  Notify();
}

//------------------------------------------------------------------------------
const float Camera_Component::GetFieldOfViewInRadians() const {
  return m_fov;
}

//------------------------------------------------------------------------------
void Camera_Component::SetFieldOfViewInRadians(float radians) {
  m_fov = radians;
  Notify();
}

} // namespace Tunnelour
