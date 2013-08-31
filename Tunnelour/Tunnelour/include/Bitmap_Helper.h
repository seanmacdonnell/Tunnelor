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

#ifndef TUNNELOUR_BITMAP_HELPER_H_
#define TUNNELOUR_BITMAP_HELPER_H_

#include "Bitmap_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                middleground (Layer 0)
//-----------------------------------------------------------------------------
class Bitmap_Helper {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Bitmap_Helper();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Bitmap_Helper();

  //---------------------------------------------------------------------------
  // Description : Returns whether the bounds of one tile intersects or is
  //             : tangent with the other in the Y dimension
  //---------------------------------------------------------------------------
  static bool DoTheseTilesYCollide(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB);
  
  //---------------------------------------------------------------------------
  // Description : Returns whether the bounds of one tile intersects or is
  //             : tangent with the other in the Y dimension
  //---------------------------------------------------------------------------
  static bool AreTheseTilesYAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB);

  //---------------------------------------------------------------------------
  // Description : Returns whether the bounds of one tile intersects or is
  //             : tangent with the other in the X dimension
  //---------------------------------------------------------------------------
  static bool DoTheseTilesXCollide(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB);

  //---------------------------------------------------------------------------
  // Description : Returns whether the bounds of one tile intersects or is
  //             : tangent with the other in the Y dimension
  //---------------------------------------------------------------------------
  static bool AreTheseTilesXAdjacent(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB);

 protected:

 private:
  
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_BITMAP_HELPER_H_
