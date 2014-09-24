//  Copyright 2013 Sean MacDonnell
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

#include "World_Settings_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
World_Settings_Component::World_Settings_Component(): Component() {
  m_type = "World_Settings_Component";
  m_gravity_px_per_ms = 0.0f;
  m_gravity_px_per_frame = 0.0f;
  m_max_velocity_px_per_ms = 0.0f;
  m_max_velocity_px_per_frame = 0.0f;
  m_pixels_to_metres = 0.0f;
}

//------------------------------------------------------------------------------
World_Settings_Component::~World_Settings_Component() {
}

//------------------------------------------------------------------------------
void World_Settings_Component::Init() {
  // Default Values
  // Proper gravity is approx 1000cm/s(sq) which is approx 571px/s(sq)
  // 571/1000 = 0.571 which I am changing to 0.4
  m_gravity_px_per_ms = 0.4f;

  // Magic number
  m_gravity_px_per_frame = -8.0f;

  // Default Values
  // Max Velocity is 200km/h or 5555cm/s
  // Which is 5.555cm\mil and 3.17px\mil changed to 4.
  m_max_velocity_px_per_ms = 4.0f;

  // Magic number
  m_max_velocity_px_per_frame = -512.0f;

  // There are 0.0175 pixels per meter in this world
  m_pixels_to_metres = 0.0175;
}

//------------------------------------------------------------------------------
float World_Settings_Component::GetGravityInPixPerMs() {
  return m_gravity_px_per_ms;
}

//------------------------------------------------------------------------------
float World_Settings_Component::GetGravityInPixPerFrame() {
  return m_gravity_px_per_frame;
}

//------------------------------------------------------------------------------
float World_Settings_Component::GetMaxVelocityInPixPerMs() {
  return m_max_velocity_px_per_ms;
}

//------------------------------------------------------------------------------
float World_Settings_Component::GetMaxVelocityInPixPerFrame() {
  return m_max_velocity_px_per_frame;
}

//------------------------------------------------------------------------------
long double World_Settings_Component::GetPixelToMeterFactor() {
  return m_pixels_to_metres;
}
//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
