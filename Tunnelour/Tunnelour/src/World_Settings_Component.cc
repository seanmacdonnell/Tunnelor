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
  m_gravity = 0;
}

//------------------------------------------------------------------------------
World_Settings_Component::~World_Settings_Component() {
}

//------------------------------------------------------------------------------
void World_Settings_Component::Init() {
  // Default Values
  // Proper gravity is approx 1000cm/s(sq) which is approx 571px/s(sq)
  // 571/1000 = 0.571 which I am changing to 0.4
  m_gravity = 0.4;

  // Default Values
  // Max Velocity is 200km/h or 5555cm/s
  // Which is 5.555cm\mil and 3.17px\mil changed to 4.
  m_max_velocity = 4;
}

//------------------------------------------------------------------------------
float World_Settings_Component::GetGravityInPixPerMs() {
  return m_gravity;
}

//------------------------------------------------------------------------------
void World_Settings_Component::SetGravity(int gravity) {
  m_gravity = gravity;
}

//------------------------------------------------------------------------------
float World_Settings_Component::GetMaxVelocityInPixPerMs() {
  return m_max_velocity;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
