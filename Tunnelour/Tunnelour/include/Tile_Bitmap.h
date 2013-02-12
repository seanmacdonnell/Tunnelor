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

#ifndef TUNNELOUR_TILE_BITMAP_H_
#define TUNNELOUR_TILE_BITMAP_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11tex.h>
#include "Bitmap_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Tile_Bitmap is a component for a renderable 2D Sprite
//-----------------------------------------------------------------------------
class Tile_Bitmap: public Tunnelour::Bitmap_Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Tile_Bitmap();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Tile_Bitmap();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init(ID3D11Device * const d3d11device);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Inits this components frame stucture
  //---------------------------------------------------------------------------
  void Init_Frame();

  //---------------------------------------------------------------------------
  // Description : Inits this components texture stucture
  //---------------------------------------------------------------------------
  void Init_Texture();
};  // class Tile_Bitmap
}  // namespace Tunnelour
#endif  // TUNNELOUR_TILE_BITMAP_H_
