//  Copyright 2009 Sean MacDonnell
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

#include <vector>

using std::vector;

namespace Tunnelour {
class Mesh_Component: public Tunnelour::Component {
 public:
  // Vertex format
   struct VertexType {
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DXVECTOR2 texcoord;
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
  virtual void Init(ID3D11Device* pD3D10Device);

 protected:

 private:
  ID3D11Device* m_d3d11device;
  ID3D11Buffer *m_vertexbuffer, *m_indexbuffer;
  int m_vertexCount, m_indexcount;

}; // class Mesh_Component
} // namespace Tunnelour
#endif  // TUNNELOUR_MESH_COMPONENT_H_
