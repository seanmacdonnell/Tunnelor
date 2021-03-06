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

#ifndef TUNNELOUR_DIRECT3D11_VIEW_H_
#define TUNNELOUR_DIRECT3D11_VIEW_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "winmm.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

#include <windows.h>
#include <mmsystem.h>
#include <list>
#include <map>
#include <vector>

#include "Component_Composite.h"
#include "View.h"
#include "Component.h"
#include "Direct3D11_View_TextureShader.h"
#include "Direct3D11_View_FontShader.h"
#include "Direct3D11_View_TransparentShader.h"
#include "Camera_Component.h"
#include "Bitmap_Component.h"
#include "Text_Component.h"
#include "Game_Settings_Component.h"
#include "Game_Metrics_Component.h"
#include "Direct3D11_View_DebugShader.h"
#include "Avatar_Component.h"
#include "Frame_Component.h"
#include "Bitmap_Component.h"
#include "Text_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Direct3D11_View contains all the information
//                for using rendering a DirectX 11 environment.
//                It also contains all the windows window management code.
//                The code here was copied and pasted and modified from
//                the tutorials at http://rastertek.com/dx11tut011.html
//-----------------------------------------------------------------------------
class Direct3D11_View : public Tunnelour::View,
                        public Component_Composite::Component_Composite_Type_Observer {
 public:
  struct Bitmap_Renderable {
    Tunnelour::Bitmap_Component* bitmap;
    Frame_Component::Frame *frame;
    Bitmap_Component::Texture *texture;
    D3DXVECTOR3 *frame_centre;
    D3DXVECTOR3 *scale;
    D3DXVECTOR3 *position;
  };

  struct Text_Renderable {
    Tunnelour::Text_Component* text;
    Frame_Component::Frame *frame;
    Bitmap_Component::Texture *texture;
    D3DXVECTOR3 *frame_centre;
    D3DXVECTOR3 *scale;
    D3DXVECTOR3 *position;
    Text_Component::Font *font;
  };

  struct Renderables {
    std::vector<Bitmap_Renderable*> Layer_00; // Background (0)
    std::vector<Bitmap_Renderable*> Layer_01; // Middleground (-1)
    std::vector<Bitmap_Renderable*> Avatars;  // Avatars (-1.5)
    std::vector<Bitmap_Renderable*> Layer_02; // Foreground (-2)
    std::vector<Text_Renderable*> Layer_03;   // Foreground Text (-3)
    std::vector<Bitmap_Renderable*> Layer_04; // Splash, Loading, Menu (-4)
    std::vector<Text_Renderable*> Layer_05;   // Splash, Loading, Menu Text (-5)
  };

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

  virtual void HandleEventAdd(Tunnelour::Component * const component);
  virtual void HandleEventRemove(Tunnelour::Component * const component);
  virtual void HandleEventUpdate(Tunnelour::Component * const component);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Initialise the window
  //---------------------------------------------------------------------------
  void Init_Window();

  //---------------------------------------------------------------------------
  // Description : Initialise Direct3D11
  //---------------------------------------------------------------------------
  void Init_D3D11();

  //---------------------------------------------------------------------------
  // Description : Render the components
  //---------------------------------------------------------------------------
  void Render_Bitmaps(std::vector<Bitmap_Renderable*> renderables,
                      D3DXMATRIX *viewmatrix);

  //---------------------------------------------------------------------------
  // Description : Render the components
  //---------------------------------------------------------------------------
  void Render_Texts(std::vector<Text_Renderable*> renderables, 
                    D3DXMATRIX *viewmatrix);

  //---------------------------------------------------------------------------
  // Description : Render the Camera
  //---------------------------------------------------------------------------
  void Render_Camera(Tunnelour::Camera_Component *camera,
                     D3DXMATRIX *viewmatrix);

  //---------------------------------------------------------------------------
  // Description : Render a Bitmap
  //---------------------------------------------------------------------------
  void Render_Bitmap(Bitmap_Renderable *bitmap,
                     D3DXMATRIX *viewmatrix);

  //---------------------------------------------------------------------------
  // Description : Render Text
  //---------------------------------------------------------------------------
  void Render_Text(Text_Renderable *text,
                   D3DXMATRIX *viewmatrix);

  //---------------------------------------------------------------------------
  // Description : Turn on Alpha Blending
  //---------------------------------------------------------------------------
  void TurnOnAlphaBlending();

  //---------------------------------------------------------------------------
  // Description : Turn off Alpha Blending
  //---------------------------------------------------------------------------
  void TurnOffAlphaBlending();

  void TurnZBufferOn();

  void TurnZBufferOff();

  bool IsThisBitmapComponentVisable(Bitmap_Renderable *bitmap);

  //---------------------------------------------------------------------------
  // Description : Class Variables
  //---------------------------------------------------------------------------
  LPCWSTR m_application_name;
  bool m_is_window_init;
  bool m_is_d3d11_init;
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
  ID3D11DepthStencilState * m_depthDisabledStencilState;
  ID3D11BlendState* m_alphaEnableBlendingState;
  ID3D11BlendState* m_alphaDisableBlendingState;
  D3DXMATRIX m_world;
  D3DXMATRIX m_ortho;

  //---------------------------------------------------------------------------
  // Description : Shaders
  //---------------------------------------------------------------------------
  Tunnelour::Direct3D11_View_FontShader* m_font_shader;
  Tunnelour::Direct3D11_View_TransparentShader* m_transparent_shader;
  Tunnelour::Direct3D11_View_DebugShader* m_debug_shader;

  //---------------------------------------------------------------------------
  // Description : Model Components
  //---------------------------------------------------------------------------
  std::map<std::wstring, ID3D11ShaderResourceView*> m_texture_map;

  Renderables m_renderables;
  Tunnelour::Camera_Component * m_camera;
  Tunnelour::Game_Settings_Component * m_game_settings;
  Tunnelour::Game_Metrics_Component * m_game_metrics;
  Tunnelour::Avatar_Component * m_avatar;
};
}  // namespace Tunnelour

#endif  // TUNNELOUR_DIRECT3D11_VIEW_H_
