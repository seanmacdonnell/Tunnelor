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

#ifndef TUNNELOUR_DIRECT3D11_VIEW_H_
#define TUNNELOUR_DIRECT3D11_VIEW_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

#include <windows.h>
#include <iostream>

#include "Component_Composite.h"
#include "View.h"
#include "Background_Component.h"
#include "Component.h"
#include "Direct3D11_View_ColorShader.h"
#include "Camera_Component.h"
#include "Mesh_Component.h"

namespace Tunnelour {
class Direct3D11_View : public Tunnelour::View {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Direct3D11_View contains all the information
//                for using rendering a directx 11 environment.
//                It also contains all the windows window management code.
//                The code here was copied and pasted and modified from
//                the tutorials at http://rastertek.com/dx11tut02.html
//-----------------------------------------------------------------------------
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Direct3D11_View();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Direct3D11_View();

  //---------------------------------------------------------------------------
  // Description : Initialises this model for Running.
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : View Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:

 private:
  LPCWSTR m_application_name;
  HINSTANCE m_hinstance;
  HWND m_hwnd;
  int m_screen_width;
  int m_screen_height;

  bool m_is_window_init;
  bool m_is_d3d11_init;
  bool m_is_full_screen;

  float m_screen_depth;
  float m_screen_near;

  void Init_Window();
  void Init_D3D11();

  bool m_vsync_enabled;
  int m_videoCardMemory;
  char m_video_card_description[128];
  IDXGISwapChain * m_swap_chain;
  ID3D11Device * m_device;
  ID3D11DeviceContext * m_device_context;
  ID3D11RenderTargetView * m_render_target_view;
  ID3D11Texture2D * m_depth_stencil_buffer;
  ID3D11DepthStencilState * m_depth_stencil_state;
  ID3D11DepthStencilView * m_depth_stencil_view;
  ID3D11RasterizerState * m_raster_state;
  D3DXMATRIX m_projection;
  D3DXMATRIX m_world;
  D3DXMATRIX m_ortho;

  Tunnelour::Camera_Component * m_camera;
  Tunnelour::Mesh_Component * m_mesh;
  Tunnelour::Background_Component * m_background;
  Tunnelour::Direct3D11_View_ColorShader * m_color_shader;
};
}  // namespace Tunnelour

#endif  // TUNNELOUR_DIRECT3D11_VIEW_H_
