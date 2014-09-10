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

#ifndef TUNNELOUR_GEOMETRY_HELPER_H_
#define TUNNELOUR_GEOMETRY_HELPER_H_

#include "D3dx9math.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : 
//-----------------------------------------------------------------------------
class Geometry_Helper {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Geometry_Helper();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Geometry_Helper();

  static bool DoTheseLinesIntersect(D3DXVECTOR2 line_a_begin, D3DXVECTOR2 line_a_end, D3DXVECTOR2 line_b_begin, D3DXVECTOR2 line_b_end, D3DXVECTOR2 *out_intersecting_point);

  static double WhatsTheDistanceBetweenThesePoints(D3DXVECTOR2 point_1, D3DXVECTOR2 point_2);

 protected:

 private:
  
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_GEOMETRY_HELPER_H_
