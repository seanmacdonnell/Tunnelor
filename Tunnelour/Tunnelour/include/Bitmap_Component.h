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

#ifndef TUNNELOUR_BITMAP_COMPONENT_H_
#define TUNNELOUR_BITMAP_COMPONENT_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11tex.h>
#include "Frame_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Bitmap_Component is a component for a renderable 2D Sprite
//-----------------------------------------------------------------------------
class Bitmap_Component: public Tunnelour::Frame_Component {
 public:
  struct Texture {
    ID3D11ShaderResourceView * texture;
    std::wstring texture_path;
    float transparency;
    D3DXVECTOR2 top_left_position;
    D3DXVECTOR2 tile_size;
    D3DXVECTOR2 texture_size;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Bitmap_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Bitmap_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  //---------------------------------------------------------------------------
  // Description : Accessor for the Texture
  //---------------------------------------------------------------------------
  Texture * const GetTexture();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Texture
  //---------------------------------------------------------------------------
  void SetTexture(Texture texture);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Velocity
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetVelocity();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Velocity
  //---------------------------------------------------------------------------
  void SetVelocity(D3DXVECTOR3 velocity);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Angle in radians!
  //---------------------------------------------------------------------------
  float GetAngle();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Angle
  //---------------------------------------------------------------------------
  void SetAngle(float angle);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Angle in radians!
  //---------------------------------------------------------------------------
  float GetTransparency();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Angle
  //---------------------------------------------------------------------------
  void SetTransparency(float transparency);


 protected:
  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  Texture * m_texture;
  D3DXVECTOR3 m_velocity;
  float m_angle;

 private:
};  // class Bitmap_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_BITMAP_COMPONENT_H_
