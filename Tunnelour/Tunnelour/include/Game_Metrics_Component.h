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

#ifndef TUNNELOUR_GAME_METRICS_COMPONENT_H_
#define TUNNELOUR_GAME_METRICS_COMPONENT_H_

//-----------------------------------------------------------------------------
// NOTE(Sean): d3d11 is not required for this class, I am inclduing it as a
//             tempory fix for a compatibility error of including d3dx10math
//             before d3d11.
// TODO(Sean): Either replace d3dx10math with a different maths library or fix
//             the include order dependancy.
//-----------------------------------------------------------------------------
#include <d3d11.h>
#include <d3dx10math.h>
#include "Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Game_Metrics_Component contains all the information
//                for setting the game up.
//-----------------------------------------------------------------------------
class Game_Metrics_Component: public Tunnelour::Component {
 public:
   struct FPS_Data {
	    int fps;
	    int count;
	    unsigned long startTime;
   };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Game_Metrics_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Game_Metrics_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  //---------------------------------------------------------------------------
  // Description : Accessor for the resolution
  //---------------------------------------------------------------------------
  Tunnelour::Game_Metrics_Component::FPS_Data GetFPSData();

  //---------------------------------------------------------------------------
  // Description : Mutator for the resolution
  //---------------------------------------------------------------------------
  void SetFPSData(Tunnelour::Game_Metrics_Component::FPS_Data fps_data);

  //---------------------------------------------------------------------------
  // Description : Accessor for the resolution
  //---------------------------------------------------------------------------
  long double GetDistanceTraveled();

  //---------------------------------------------------------------------------
  // Description : Mutator for the distance
  //---------------------------------------------------------------------------
  void SetDistanceTraveled(long double distance);

  //---------------------------------------------------------------------------
  // Description : Accessor for the resolution
  //---------------------------------------------------------------------------
  long double GetSecondsPast();

  //---------------------------------------------------------------------------
  // Description : Mutator for the distance
  //---------------------------------------------------------------------------
  void SetSecondsPast(long double seconds_past);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  Tunnelour::Game_Metrics_Component::FPS_Data m_fps_data;
  long double m_distance_traveled;
  long double m_seconds_past;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_GAME_METRICS_COMPONENT_H_
