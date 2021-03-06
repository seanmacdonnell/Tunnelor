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

#ifndef TUNNELOUR_SPLASH_SCREEN_COMPONENT_H_
#define TUNNELOUR_SPLASH_SCREEN_COMPONENT_H_

#include "Component.h"
#include <vector>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Splash_Screen_Component contains all the data for the current 
//              : game level and the past game level.
//-----------------------------------------------------------------------------
class Splash_Screen_Component: public Tunnelour::Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Splash_Screen_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  ~Splash_Screen_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  void Init();
  
  bool IsFading();

  void SetIsFading(bool is_fading);

  bool HasFaded();

  void SetHasFaded(bool has_faded);

 protected:

 private:
 bool m_is_fading;
 bool m_has_faded;
};
}  // namespace Tunnelour
#endif  //  TUNNELOUR_SPLASH_SCREEN_COMPONENT_H_
