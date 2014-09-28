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

#ifndef TUNNELOUR_COLOR_HELPER_H
#define TUNNELOUR_COLOR_HELPER_H

#include <d3dx10math.h>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Colour_Helper contains colour variables for the game
//-----------------------------------------------------------------------------
  namespace Colours {
     const D3DXCOLOR Text_Blue = D3DXCOLOR(0.24705882352941178f,
                                           0.4549019607843137f,
                                           0.7098039215686275f,
                                           1.0f);

     const D3DXCOLOR Text_Red = D3DXCOLOR(0.7529411764705882f,
                                          0.0f,
                                          0.0f,
                                          1.0f);

     const D3DXCOLOR Text_Light_Blue = D3DXCOLOR(0.3333333333333333f,
                                                 0.611764705882353f,
                                                 0.788235294117647f,
                                                 1.0f);
  }  // namespace Colours
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_COMPONENT_H
