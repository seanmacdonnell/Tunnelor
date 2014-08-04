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
Camera_Component::Camera_Component(): Component() {
  m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_at = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_up = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_fov = 0;
  m_last_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_type = "Camera_Component";
}

//------------------------------------------------------------------------------
Camera_Component::~Camera_Component() {
  m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_at = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_up = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_fov = 0;
  m_last_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

//------------------------------------------------------------------------------
void Camera_Component::Init() {
  m_position = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
  m_at = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
  m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
  m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);  
  m_last_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  m_is_initialised = true;
}

//------------------------------------------------------------------------------
D3DXVECTOR3 Camera_Component::GetPosition() {
  return m_position;
}

//------------------------------------------------------------------------------
void Camera_Component::SetPosition(D3DXVECTOR3 const & position) {
  m_position.x = position.x;
  m_position.y = position.y;
  m_position.z = -10;
  Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetPosition(float x, float y, float z) {
  D3DXVECTOR3 position =  D3DXVECTOR3(x, y, z);
  position.z = -10;
  m_position = position; 
  Notify();
}

//------------------------------------------------------------------------------
D3DXVECTOR3 Camera_Component::GetLookingAtPosition() {
  return m_at;
}

//------------------------------------------------------------------------------
void Camera_Component::SetLookingAtPosition(D3DXVECTOR3 const & at) {
  m_at = at;
  Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetLookingAtPosition(float x, float y, float z) {
  m_at = D3DXVECTOR3(x, y, z);
  Notify();
}

//------------------------------------------------------------------------------
D3DXVECTOR3 Camera_Component::GetUpDirection() {
  return m_up;
}

//------------------------------------------------------------------------------
void Camera_Component::SetUpDirection(D3DXVECTOR3 const & up) {
  m_up = up;
  Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetUpDirection(float x, float y, float z) {
  m_up = D3DXVECTOR3(x, y, z);
  Notify();
}

//------------------------------------------------------------------------------
D3DXVECTOR3 Camera_Component::GetRotationInRadians() {
  return m_rotation;
}

//------------------------------------------------------------------------------
void Camera_Component::SetRotationInRadians(const D3DXVECTOR3& rotation) {
  m_rotation = rotation;
  Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetRotationInRadians(float pitch, float yaw, float roll) {
  m_rotation = D3DXVECTOR3(pitch, yaw, roll);
}

//------------------------------------------------------------------------------
float Camera_Component::GetFieldOfViewInRadians() {
  return m_fov;
}

//------------------------------------------------------------------------------
void Camera_Component::SetFieldOfViewInRadians(float radians) {
  m_fov = radians;
  Notify();
}


//------------------------------------------------------------------------------
D3DXVECTOR3 Camera_Component::GetLastPosition() {
  return m_last_position;
}

//------------------------------------------------------------------------------
void Camera_Component::SetLastPosition(D3DXVECTOR3 const & last_position) {
  m_last_position = last_position;
  Notify();
}

//------------------------------------------------------------------------------
void Camera_Component::SetLastPosition(float x, float y, float z) {
  m_last_position = D3DXVECTOR3(x, y, z);
  Notify();
}


}  // namespace Tunnelour
