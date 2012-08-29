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

#ifndef TUNNELOUR_DIRECT3D11_VIEW_FONTSHADER_H_
#define TUNNELOUR_DIRECT3D11_VIEW_FONTSHADER_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Direct3D11_View_FontShader contains all the information
//                for using rendering fonts from files.
//                The code here was copied and pasted and modified from
//                the tutorials at http://rastertek.com/dx11tut05.html
//-----------------------------------------------------------------------------
class Direct3D11_View_FontShader {
 public:
  //---------------------------------------------------------------------------
  // Description : Struct for the world, view and projection matrices
  //---------------------------------------------------------------------------
  struct MatrixBufferType {
    D3DXMATRIX world;
    D3DXMATRIX view;
    D3DXMATRIX projection;
  };

  struct PixelBufferType
	 {
		  D3DXVECTOR4 pixelColor;
	 };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Direct3D11_View_FontShader();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  ~Direct3D11_View_FontShader();

  //---------------------------------------------------------------------------
  // Description : Initialises this class.
  //---------------------------------------------------------------------------
  void Init(ID3D11Device *d3d11device, HWND *hwnd);

  //---------------------------------------------------------------------------
  // Description : Renders the world using the provided matricies.
  //---------------------------------------------------------------------------
  void Render(ID3D11DeviceContext* devicecontext,
              int index,
              D3DXMATRIX world,
              D3DXMATRIX view,
              D3DXMATRIX projection,
              ID3D11ShaderResourceView* texture,
              D3DXVECTOR4 pixelcolor);

  //---------------------------------------------------------------------------
  // Description : Returns whether this class has been initalised
  //---------------------------------------------------------------------------
  bool IsInitialised();

 private:
  //---------------------------------------------------------------------------
  // Description : Has this component been initialised?
  //---------------------------------------------------------------------------
  bool m_is_initialised;

  //---------------------------------------------------------------------------
  // Description : Class Variables
  //---------------------------------------------------------------------------
  HWND *m_hwnd;
  ID3D11Device *m_d3d11device;
  ID3D11VertexShader *m_vertexshader;
  ID3D11PixelShader *m_pixelshader;
  ID3D11InputLayout *m_layout;
  ID3D11Buffer *m_matrixbuffer;
  ID3D11SamplerState* m_sampleState;
	 ID3D11Buffer* m_pixelbuffer;

  wchar_t *m_vertexshaderfile, *m_pixelshaderfile;


};  // class Direct3D11_View_FontShader
}  // namespace Tunnelour
#endif  // TUNNELOUR_DIRECT3D11_VIEW_FONTSHADER_H_
