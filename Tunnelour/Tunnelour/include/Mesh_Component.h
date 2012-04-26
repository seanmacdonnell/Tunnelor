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
//  Author(s)  : Sean MacDonnell
//  Description: Mesh_Component contains all the pertinant mesh information
//               for a game entity.
//

#ifndef TUNNELOUR_MESH_COMPONENT_H_
#define TUNNELOUR_MESH_COMPONENT_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include "Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Mesh_Component is a renderable set of verticies and indicies
//-----------------------------------------------------------------------------
class Mesh_Component: public Tunnelour::Component {
 public:
  // Vertex format
  struct Vertex_Type {
    D3DXVECTOR3 position;
    D3DXVECTOR4 color;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Mesh_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Mesh_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init(ID3D11Device * const d3d11device);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Vertex Buffer
  //---------------------------------------------------------------------------
  ID3D11Buffer * const GetVertexBuffer();

  //---------------------------------------------------------------------------
  // Description : Accessor for the Index Buffer
  //---------------------------------------------------------------------------
  ID3D11Buffer * const GetIndexBuffer();

  //---------------------------------------------------------------------------
  // Description : Accessor for the Index Count
  //---------------------------------------------------------------------------
  int GetIndexCount();

 protected:

 private:
  ID3D11Device * m_d3d11device;
  ID3D11Buffer * m_vertexbuffer, * m_indexbuffer;
  int m_vertex_count, m_index_count;
};  // class Mesh_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_MESH_COMPONENT_H_
