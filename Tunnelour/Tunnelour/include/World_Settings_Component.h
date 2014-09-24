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

#ifndef TUNNELOUR_WORLD_SETTINGS_COMPONENT_H_
#define TUNNELOUR_WORLD_SETTINGS_COMPONENT_H_

#include "Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : World_Settings_Component contains all the information
//                for setting the game up.
//-----------------------------------------------------------------------------
class World_Settings_Component: public Tunnelour::Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  World_Settings_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~World_Settings_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  //---------------------------------------------------------------------------
  // Description : Accessor for the gravity per millisecond
  //---------------------------------------------------------------------------
  float GetGravityInPixPerMs();

  //---------------------------------------------------------------------------
  // Description : Accessor for the gravity per frame
  //---------------------------------------------------------------------------
  float GetGravityInPixPerFrame();

  //---------------------------------------------------------------------------
  // Description : Accessor for the max velocity
  //---------------------------------------------------------------------------
  float GetMaxVelocityInPixPerMs();

  //---------------------------------------------------------------------------
  // Description : Accessor for the max velocity per frame
  //---------------------------------------------------------------------------
  float GetMaxVelocityInPixPerFrame();

  //---------------------------------------------------------------------------
  // Description : Accessor for the factor of pixels to metres 
  //---------------------------------------------------------------------------
  long double GetPixelToMeterFactor();

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  float m_gravity_px_per_ms;
  float m_gravity_px_per_frame;
  float m_max_velocity_px_per_ms;
  float m_max_velocity_px_per_frame;
  long double m_pixels_to_metres;

};
}  // namespace Tunnelour
#endif  // TUNNELOUR_BACKGROUND_COLOR_COMPONENT_H_
