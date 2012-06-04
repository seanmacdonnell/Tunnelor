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

#ifndef TUNNELOUR_BACKGROUND_COMPONENT_H_
#define TUNNELOUR_BACKGROUND_COMPONENT_H_

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
//  Description : Background_Component contains all the information
//                for setting the backbuffer of the window.
//-----------------------------------------------------------------------------
class Background_Component: public Tunnelour::Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Background_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Background_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  //---------------------------------------------------------------------------
  // Description : Accessor for the color
  //---------------------------------------------------------------------------
  D3DXCOLOR GetColor();

  //---------------------------------------------------------------------------
  // Description : Mutator for the color
  //---------------------------------------------------------------------------
  void SetColor(D3DXCOLOR & color);
  void SetColor(float red, float green, float blue, float alpha);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  D3DXCOLOR m_color;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_BACKGROUND_COMPONENT_H_
