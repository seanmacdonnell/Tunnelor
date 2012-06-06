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

#ifndef TUNNELOUR_BITMAP_COMPONENT_H_
#define TUNNELOUR_BITMAP_COMPONENT_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11tex.h>
#include "Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Bitmap_Component is a component for a renderable 2D Sprite
//-----------------------------------------------------------------------------
class Bitmap_Component: public Tunnelour::Component {
 public:
  struct Vertex_Type  {
    D3DXVECTOR3 position;
    D3DXVECTOR2 texture;
  };

  struct Frame {
    ID3D11Buffer * vertex_buffer;
    int vertex_count;
    ID3D11Buffer * index_buffer;
    int index_count;
  };

  struct Texture {
    ID3D11ShaderResourceView * texture;
    WCHAR * texture_name;
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
  virtual void Init(ID3D11Device * const d3d11device);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Frame
  //---------------------------------------------------------------------------
  Frame * const GetFrame();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Frame
  //---------------------------------------------------------------------------
  void SetFrame(Frame * frame);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Texture
  //---------------------------------------------------------------------------
  Texture * const GetTexture();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Texture
  //---------------------------------------------------------------------------
  void SetTexture(Texture * texture);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Position
  //---------------------------------------------------------------------------
  D3DXVECTOR2 * const GetPosition();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Position
  //---------------------------------------------------------------------------
  void SetPosition(D3DXVECTOR2 * const position);

  //---------------------------------------------------------------------------
  // Description : Mutator for the Size
  //---------------------------------------------------------------------------
  D3DXVECTOR2 * const GetSize();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Size
  //---------------------------------------------------------------------------
  void SetSize(D3DXVECTOR2 * const size);

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

  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  ID3D11Device * m_d3d11device;
  D3DXVECTOR2 m_position;
  D3DXVECTOR2 m_size;
  Frame  * m_frame;
  Texture * m_texture;
};  // class Bitmap_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_BITMAP_COMPONENT_H_
