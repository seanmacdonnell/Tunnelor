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

#ifndef TUNNELOUR_FRAME_COMPONENT_H_
#define TUNNELOUR_FRAME_COMPONENT_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11tex.h>
#include "Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Frame_Component is a component for a frame of verticies
//                and indices.
//-----------------------------------------------------------------------------
class Frame_Component: public Tunnelour::Component {
 public:
  struct Vertex_Type {
    Vertex_Type() : position(-1, -1, -1),
                    texture(-1, -1) {
    }
    D3DXVECTOR3 position;
    D3DXVECTOR2 texture;
  };

  struct Frame {
    Vertex_Type* vertices;
    ID3D11Buffer * vertex_buffer;
    int vertex_count;
    unsigned int* indices;
    ID3D11Buffer * index_buffer;
    int index_count;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Frame_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Frame_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  //---------------------------------------------------------------------------
  // Description : Accessor for the Frame
  //---------------------------------------------------------------------------
  Frame * const GetFrame();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Frame
  //---------------------------------------------------------------------------
  void SetFrame(Frame * frame);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Position
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetPosition();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Position
  //---------------------------------------------------------------------------
  void SetPosition(D3DXVECTOR3 position);

  //---------------------------------------------------------------------------
  // Description : Mutator for the Position
  //---------------------------------------------------------------------------
  void SetPosition(float x, float y, float z);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Scale
  //---------------------------------------------------------------------------
  D3DXVECTOR3 * const GetScale();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Scale
  //---------------------------------------------------------------------------
  void SetScale(D3DXVECTOR3 * const scale);

  //---------------------------------------------------------------------------
  // Description : Mutator for the Size
  //---------------------------------------------------------------------------
  D3DXVECTOR2 GetSize();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Size
  //---------------------------------------------------------------------------
  void SetSize(D3DXVECTOR2 size);

  //---------------------------------------------------------------------------
  // Description : Mutator for the Size
  //---------------------------------------------------------------------------
  void SetSize(float size_x, float size_y);

  //---------------------------------------------------------------------------
  // Description : Returns the calculated centre point of the frame
  //---------------------------------------------------------------------------
  D3DXVECTOR3 const GetFrameCentre();

  //---------------------------------------------------------------------------
  // Description : Returns the calculated Top Left Position of the frame
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetTopLeftPostion();

  //---------------------------------------------------------------------------
  // Description : Returns the calculated Bottom Right Position of the frame
  //---------------------------------------------------------------------------
  D3DXVECTOR3 GetBottomRightPostion();


 protected:
  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  D3DXVECTOR3 m_position;
  D3DXVECTOR3 m_scale;
  D3DXVECTOR2 m_size;

  Frame  * m_frame;

  D3DXVECTOR3 m_centre;

 private:
};  // class Frame_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_FRAME_COMPONENT_H_
