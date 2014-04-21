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

#include "Direct3D11_View.h"
#include "Message_Wrapper.h"
#include "Exceptions.h"
#include "Direct3D11_View_Mutator.h"
#include "Direct3D11_View_FontShader.h"
#include "String_Helper.h"
#include "Bitmap_Helper.h"
#include "Get_Game_Metrics_Component_Mutator.h"


namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View::Direct3D11_View() {
  m_model = NULL;

  m_is_window_init = false;
  m_is_d3d11_init = false;
  m_swap_chain = NULL;
  m_device = NULL;
  m_device_context = NULL;
  m_render_target_view = NULL;
  m_depth_stencil_buffer = NULL;
  m_depth_stencil_state = NULL;
  m_depth_stencil_view = NULL;
  m_raster_state = NULL;
  m_depthDisabledStencilState = NULL;
  m_alphaEnableBlendingState = NULL;
  m_alphaDisableBlendingState = NULL;

  m_font_shader = NULL;
  m_transparent_shader = NULL;
  m_debug_shader = 0;
  m_game_settings = 0;
  m_game_metrics = 0;
  m_avatar = 0;
}

//------------------------------------------------------------------------------
Direct3D11_View::~Direct3D11_View() {
  if (m_is_window_init) {
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if (m_game_settings->IsFullScreen()) {
      ChangeDisplaySettings(NULL, 0);
    }

    // Remove the window.
    DestroyWindow(m_game_settings->GetHWnd());
    m_game_settings->SetHWnd(NULL);

    // Remove the application instance.
    UnregisterClass(m_application_name, m_game_settings->GetHInstance());
    m_game_settings->SetHInstance(NULL);
  }

  if (m_is_d3d11_init) {
    // Before shutting down set to windowed mode or
    // when you release the swap chain it will throw an exception.
    if (m_swap_chain) {
      m_swap_chain->SetFullscreenState(false, NULL);
    }

    if (m_raster_state) {
      m_raster_state->Release();
      m_raster_state = 0;
    }

    if (m_depth_stencil_view) {
      m_depth_stencil_view->Release();
      m_depth_stencil_view = 0;
    }

    if (m_depth_stencil_state) {
      m_depth_stencil_state->Release();
      m_depth_stencil_state = 0;
    }

    if (m_depth_stencil_buffer) {
      m_depth_stencil_buffer->Release();
      m_depth_stencil_buffer = 0;
    }

    if (m_render_target_view) {
      m_render_target_view->Release();
      m_render_target_view = 0;
    }

    if (m_device_context) {
      m_device_context->Release();
      m_device_context = 0;
    }

    if (m_device) {
      m_device->Release();
      m_device = 0;
    }

    if (m_swap_chain) {
      m_swap_chain->Release();
      m_swap_chain = 0;
    }

    if (m_font_shader) {
      delete m_font_shader;
      m_font_shader = NULL;
    }

    if (m_transparent_shader) {
      delete m_transparent_shader;
      m_transparent_shader = NULL;
    }

    if (m_debug_shader) {
      delete m_debug_shader;
      m_debug_shader = NULL;
    }
    
    if (m_depthDisabledStencilState) {
      m_depthDisabledStencilState->Release();
      m_depthDisabledStencilState = NULL;
    }

    if (m_alphaEnableBlendingState) {
      m_alphaEnableBlendingState->Release();
      m_alphaEnableBlendingState = NULL;
    }
  }

  if (!m_texture_map.empty()) {
    for (std::map<std::wstring, ID3D11ShaderResourceView*>::iterator texture = m_texture_map.begin(); texture != m_texture_map.end(); texture++) {
      texture->second->Release();
      texture->second = 0;
    }
  }

  m_model->IgnoreType(this, "Bitmap_Component");
  m_model->IgnoreType(this, "Text_Component");
  m_model->IgnoreType(this, "Avatar_Component");

  m_renderables.Layer_00.clear();
  m_renderables.Layer_01.clear();
  m_renderables.Layer_02.clear();
  m_renderables.Layer_03.clear();
  m_renderables.Layer_04.clear();
  m_renderables.Layer_05.clear();

  m_camera = 0;
  m_game_settings = 0;
  m_game_metrics = 0;
  m_avatar = 0;
}

//------------------------------------------------------------------------------
void Direct3D11_View::Init(Component_Composite * const model) {
  View::Init(model);

  m_model->ObserveType(this, "Bitmap_Component");
  m_model->ObserveType(this, "Text_Component");
  m_model->ObserveType(this, "Avatar_Component");

  Direct3D11_View_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_camera = mutator.GetCamera();
    if (mutator.GetGameSettings() != 0) {
      m_game_metrics = mutator.GetGameMetrics();
    }
    m_game_settings = mutator.GetGameSettings();

    if (mutator.GetAvatar() != 0) {
      Bitmap_Renderable *bitmap_renderable = new Bitmap_Renderable();
      m_avatar = mutator.GetAvatar();
      bitmap_renderable->bitmap = m_avatar;
      bitmap_renderable->frame = m_avatar->GetFrame();
      bitmap_renderable->texture = m_avatar->GetTexture();
      bitmap_renderable->frame_centre = m_avatar->GetFrameCentre();
      bitmap_renderable->scale = m_avatar->GetScale();
      bitmap_renderable->position = m_avatar->GetPosition();

      m_renderables.Avatars.push_back(bitmap_renderable);
    }


    m_is_initialised = true;
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::Run() {
  // Get Relevant Components from the model with the Mutator.
  if (!m_is_initialised) {
    Init(m_model);
  } else {
    if (!m_is_window_init) { Init_Window(); }
    if (!m_is_d3d11_init) { Init_D3D11(); }
    if (!m_font_shader) {
      m_font_shader = new Direct3D11_View_FontShader();
      m_font_shader->Init(m_device, &(m_game_settings->GetHWnd()));
    }
    if (!m_transparent_shader) {
      m_transparent_shader = new Direct3D11_View_TransparentShader();
      m_transparent_shader->Init(m_device, &(m_game_settings->GetHWnd()));
    }
    if (!m_debug_shader) {
      m_debug_shader = new Direct3D11_View_DebugShader();
      m_debug_shader->Init(m_device, &(m_game_settings->GetHWnd()));
    }

    // <BeginScene>
    D3DXMATRIX *viewmatrix = new D3DXMATRIX();

    // Clear the back buffer.
    m_device_context->ClearRenderTargetView(m_render_target_view,
                                            m_game_settings->GetColor());

    // Clear the depth buffer.
    m_device_context->ClearDepthStencilView(m_depth_stencil_view,
                                            D3D11_CLEAR_DEPTH,
                                            1.0f,
                                            0);

    Render_Camera(m_camera, viewmatrix);

    TurnOnAlphaBlending();
    
    // Sort the Renderables by Z order.
    Render_Bitmaps(m_renderables.Layer_00, viewmatrix);
    Render_Bitmaps(m_renderables.Layer_01, viewmatrix);
    Render_Bitmaps(m_renderables.Avatars, viewmatrix);
    if (m_avatar != 0) {
      m_avatar->SetLastRenderedPosition(*(m_avatar->GetPosition()));
    }
    Render_Bitmaps(m_renderables.Layer_02, viewmatrix);
    Render_Texts(m_renderables.Layer_03, viewmatrix);
    Render_Bitmaps(m_renderables.Layer_04, viewmatrix);
    Render_Texts(m_renderables.Layer_05, viewmatrix);

    TurnOffAlphaBlending();

    // Present the rendered scene to the screen.
    // Present the back buffer to the screen since rendering is complete.
    if (m_game_settings->IsVSyncEnabled()) {
      // Lock to screen refresh rate.
      m_swap_chain->Present(1, 0);
    } else {
      // Present as fast as possible.
      m_swap_chain->Present(0, 0);
    }

    if (m_game_metrics != 0 ) {
      Game_Metrics_Component::FPS_Data fps_data = m_game_metrics->GetFPSData();
      fps_data.count++;

      if (timeGetTime() >= (fps_data.startTime + 1000)) {
        fps_data.fps = fps_data.count;
        fps_data.count = 0;
        fps_data.startTime = timeGetTime();
      }

      m_game_metrics->SetFPSData(fps_data);
    } else {
      Get_Game_Metrics_Component_Mutator mutator;
      m_model->Apply(&mutator);
      if (mutator.WasSuccessful()) {
        m_game_metrics = mutator.GetGameMetrics();
      }
    }
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::HandleEventAdd(Tunnelour::Component * const component) {
  if (component->GetType().compare("Bitmap_Component") == 0) {
    // Found Bitmap_Component
    Tunnelour::Bitmap_Component *bitmap = 0;
    bitmap = static_cast<Tunnelour::Bitmap_Component*>(component);
    if (!bitmap->IsInitialised()) {
      bitmap->Init(); 
    }
    Bitmap_Renderable *bitmap_renderable = new Bitmap_Renderable();
    bitmap_renderable->bitmap = bitmap;
    bitmap_renderable->frame = bitmap->GetFrame();
    bitmap_renderable->texture = bitmap->GetTexture();
    bitmap_renderable->frame_centre = bitmap->GetFrameCentre();
    bitmap_renderable->scale = bitmap->GetScale();
    bitmap_renderable->position = bitmap->GetPosition();

    if (bitmap->GetPosition()->z == 0) {
      m_renderables.Layer_00.push_back(bitmap_renderable);
    }
    if (bitmap->GetPosition()->z == -1) {
      m_renderables.Layer_01.push_back(bitmap_renderable);
    }
    if (bitmap->GetPosition()->z == -2) {
      m_renderables.Layer_02.push_back(bitmap_renderable);
    }
    if (bitmap->GetPosition()->z == -4) {
      m_renderables.Layer_04.push_back(bitmap_renderable);
    }
  }

  if (component->GetType().compare("Text_Component") == 0) {
    // Found Text_Component
    Tunnelour::Text_Component *text = 0;
    text = static_cast<Tunnelour::Text_Component*>(component);
    if (!text->IsInitialised()) {  text->Init(); }
    Text_Renderable *text_renderable = new Text_Renderable();
    text_renderable->text = text;
    text_renderable->frame = text->GetFrame();
    text_renderable->texture = text->GetTexture();
    text_renderable->frame_centre = text->GetFrameCentre();
    text_renderable->scale = text->GetScale();
    text_renderable->position = text->GetPosition();
    text_renderable->font = text->GetFont();

    if (text->GetPosition()->z == -3) {
      m_renderables.Layer_03.push_back(text_renderable);
    } else if (text->GetPosition()->z == -5) {
      m_renderables.Layer_05.push_back(text_renderable);
    }
  }

  if (component->GetType().compare("Avatar_Component") == 0) {
    if (m_avatar == 0) {
      m_avatar = static_cast<Tunnelour::Avatar_Component*>(component);

      Bitmap_Renderable *bitmap_renderable = new Bitmap_Renderable();
      bitmap_renderable->bitmap = m_avatar;
      bitmap_renderable->frame = m_avatar->GetFrame();
      bitmap_renderable->texture = m_avatar->GetTexture();
      bitmap_renderable->frame_centre = m_avatar->GetFrameCentre();
      bitmap_renderable->scale = m_avatar->GetScale();
      bitmap_renderable->position = m_avatar->GetPosition();

      m_renderables.Avatars.push_back(bitmap_renderable);
    }
  }
  if (component->GetType().compare("Game_Metrics_Component") == 0) {
    m_game_metrics = static_cast<Tunnelour::Game_Metrics_Component*>(component);
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::HandleEventRemove(Tunnelour::Component * const component){
  if (component->GetType().compare("Bitmap_Component") == 0) {
    // Found Bitmap_Component
    Tunnelour::Bitmap_Component *bitmap_component = 0;
    bitmap_component = static_cast<Tunnelour::Bitmap_Component*>(component);
    std::vector<Bitmap_Renderable*>::iterator found_bitmap_renderable;
    if (bitmap_component->GetPosition()->z == 0) {
      std::vector<Bitmap_Renderable*>::iterator bitmap_renderable;
      for (bitmap_renderable = m_renderables.Layer_00.begin(); bitmap_renderable != m_renderables.Layer_00.end(); bitmap_renderable++) {
        if ((*bitmap_renderable)->bitmap->GetID() == bitmap_component->GetID()) {
          found_bitmap_renderable = bitmap_renderable; 
        }
      }
      m_renderables.Layer_00.erase(found_bitmap_renderable);
    } else  if (bitmap_component->GetPosition()->z == -1) {
      std::vector<Bitmap_Renderable*>::iterator bitmap_renderable;
      for (bitmap_renderable = m_renderables.Layer_01.begin(); bitmap_renderable != m_renderables.Layer_01.end(); bitmap_renderable++) {
        if ((*bitmap_renderable)->bitmap->GetID() == bitmap_component->GetID()) {
          found_bitmap_renderable = bitmap_renderable; 
        }
      }
      m_renderables.Layer_01.erase(found_bitmap_renderable);
    } else  if (bitmap_component->GetPosition()->z == -2) {
      std::vector<Bitmap_Renderable*>::iterator bitmap_renderable;
      for (bitmap_renderable = m_renderables.Layer_02.begin(); bitmap_renderable != m_renderables.Layer_02.end(); bitmap_renderable++) {
        if ((*bitmap_renderable)->bitmap->GetID() == bitmap_component->GetID()) {
          found_bitmap_renderable = bitmap_renderable; 
        }
      }
      m_renderables.Layer_02.erase(found_bitmap_renderable);
    } else  if (bitmap_component->GetPosition()->z == -4) {
      std::vector<Bitmap_Renderable*>::iterator bitmap_renderable;
      for (bitmap_renderable = m_renderables.Layer_04.begin(); bitmap_renderable != m_renderables.Layer_04.end(); bitmap_renderable++) {
        if ((*bitmap_renderable)->bitmap->GetID() == bitmap_component->GetID()) {
          found_bitmap_renderable = bitmap_renderable; 
        }
      }
      m_renderables.Layer_04.erase(found_bitmap_renderable);
    }
  } else if (component->GetType().compare("Text_Component") == 0) {
    // Found Text_Component
    Tunnelour::Text_Component *text_component = 0;
    text_component = static_cast<Tunnelour::Text_Component*>(component);
    std::vector<Text_Renderable*>::iterator found_text_renderable;
    std::vector<Text_Renderable*>::iterator text_renderable;
    if (text_component->GetPosition()->z == -3) {
      for (text_renderable = m_renderables.Layer_03.begin(); text_renderable != m_renderables.Layer_03.end(); text_renderable++) {
        if ((*text_renderable)->text->GetID() == text_component->GetID()) {
          found_text_renderable = text_renderable;
        }
      }
      m_renderables.Layer_03.erase(found_text_renderable);
    } else if (text_component->GetPosition()->z == -5) {
      for (text_renderable = m_renderables.Layer_05.begin(); text_renderable != m_renderables.Layer_05.end(); text_renderable++) {
        if ((*text_renderable)->text->GetID() == text_component->GetID()) {
          found_text_renderable = text_renderable;
        }
      }
      m_renderables.Layer_05.erase(found_text_renderable);
    }
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Direct3D11_View::Init_Window() {
  WNDCLASSEX wc;
  DEVMODE dmScreenSettings;
  int posX, posY;

  // Get the instance of this application.
  m_game_settings->SetHInstance(GetModuleHandle(NULL));

  // Give the application a name.
  m_application_name = L"Tunnelour";

  // Setup the windows class with default settings.
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = Message_Wrapper::WindowProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = m_game_settings->GetHInstance();
  wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
  wc.hIconSm       = wc.hIcon;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = m_application_name;
  wc.cbSize        = sizeof(WNDCLASSEX);

  // Register the window class.
  RegisterClassEx(&wc);

  // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
  if (m_game_settings->IsFullScreen())  {
    // If full screen set the screen to maximum size of the users desktop and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = (DWORD)m_game_settings->GetResolution().x;
    dmScreenSettings.dmPelsHeight = (DWORD)m_game_settings->GetResolution().y;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    // Set the position of the window to the top left corner.
    posX = posY = 0;
  }  else  {
    // Place the window in the middle of the screen.
    posX = static_cast<int>(GetSystemMetrics(SM_CXSCREEN) - m_game_settings->GetResolution().x) / 2;
    posY = static_cast<int>(GetSystemMetrics(SM_CYSCREEN) - m_game_settings->GetResolution().y) / 2;
  }

  RECT r = {0, 0, static_cast<long>(m_game_settings->GetResolution().x), static_cast<long>(m_game_settings->GetResolution().y)};
  int winFlags = WS_OVERLAPPEDWINDOW;
  AdjustWindowRectEx(&r, winFlags, FALSE, WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME);

  // Create the window with the screen settings and get the handle to it.
  m_game_settings->SetHWnd(CreateWindowEx(WS_EX_APPWINDOW|WS_EX_DLGMODALFRAME,
                          m_application_name,
                          m_application_name,
                          winFlags,
                          posX, posY, r.right, r.bottom, NULL, NULL, m_game_settings->GetHInstance(), NULL));

  // Bring the window up on the screen and set it as main focus.
  ShowWindow(m_game_settings->GetHWnd(), SW_SHOW);
  SetForegroundWindow(m_game_settings->GetHWnd());
  SetFocus(m_game_settings->GetHWnd());

  m_is_window_init = true;

  return;
}

//------------------------------------------------------------------------------
void Direct3D11_View::Init_D3D11() {
  IDXGIFactory* factory;
  IDXGIAdapter* adapter;
  IDXGIOutput* adapter_output;
  unsigned int num_modes, i, string_length;
  // This is initalised to 0 as it is not required in windowed mode.
  unsigned int numerator = 0, denominator = 0;
  DXGI_MODE_DESC* display_mode_list;
  DXGI_ADAPTER_DESC adapter_desc;
  int error;
  DXGI_SWAP_CHAIN_DESC swap_chain_desc;
  D3D_FEATURE_LEVEL feature_level;
  ID3D11Texture2D* back_buffer_ptr;
  D3D11_TEXTURE2D_DESC depth_buffer_desc;
  D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
  D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
  D3D11_RASTERIZER_DESC raster_desc;
  D3D11_VIEWPORT viewport;
  float fieldOfView, screen_aspect;
  D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
  D3D11_BLEND_DESC blendStateDescription;

  // Create a DirectX graphics interface factory.
  if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory),
                             reinterpret_cast<void**>(&factory)))) {
    throw Exceptions::init_error("CreateDXGIFactory Failed!");
  }

  // Use the factory to create an adapter for the
  // primary graphics interface (video card).
  if (FAILED(factory->EnumAdapters(0, &adapter))) {
    throw Exceptions::init_error("EnumAdapters Failed!");
  }

  // Enumerate the primary adapter output (monitor).
  if (FAILED(adapter->EnumOutputs(0, &adapter_output))) {
    throw Exceptions::init_error("EnumOutputs Failed!");
  }

  // Get the number of modes that fit the
  // DXGI_FORMAT_R8G8B8A8_UNORM display format
  // for the adapter output (monitor).
  if (FAILED(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                                DXGI_ENUM_MODES_INTERLACED,
                                                &num_modes,
                                                NULL))) {
    throw Exceptions::init_error("Getdisplay_mode_list Failed!");
  }

  // Create a list to hold all the possible display modes for this
  // monitor/video card combination.
  display_mode_list = new DXGI_MODE_DESC[num_modes];
  if (!display_mode_list) {
    throw Exceptions::init_error("display_mode_list Failed!");
  }

  // Now fill the display mode list structures.
  if (FAILED(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                                DXGI_ENUM_MODES_INTERLACED,
                                                &num_modes,
                                                display_mode_list))) {
    throw Exceptions::init_error("Getdisplay_mode_list Failed!");
  }

  // Now go through all the display modes and find the one that matches
  // the screen width and height.
  // When a match is found store the numerator and denominator of the
  // refresh rate for that monitor.
  // This doesen't work for windowed mode.
  for (i = 0; i < num_modes; i++) {
    if (display_mode_list[i].Width == (unsigned int)m_game_settings->GetResolution().x) {
      if (display_mode_list[i].Height == (unsigned int)m_game_settings->GetResolution().y) {
        numerator = display_mode_list[i].RefreshRate.Numerator;
        denominator = display_mode_list[i].RefreshRate.Denominator;
      }
    }
  }

  // Get the adapter (video card) description.
  if (FAILED(adapter->GetDesc(&adapter_desc))) {
    throw Exceptions::init_error("GetDesc Failed!");
  }

  // Store the dedicated video card memory in megabytes.
  m_videoCardMemory = static_cast<int>(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

  // Convert the name of the video card to a character array and store it.
  error = wcstombs_s(&string_length,
                     m_video_card_description,
                     128,
                     adapter_desc.Description,
                     128);
  if (error != 0) {
    throw Exceptions::init_error("wcstombs_s Failed!");
  }

  // Release the display mode list.
  delete [] display_mode_list;
  display_mode_list = 0;

  // Release the adapter output.
  adapter_output->Release();
  adapter_output = 0;

  // Release the adapter.
  adapter->Release();
  adapter = 0;

  // Release the factory.
  factory->Release();
  factory = 0;

  // Initialize the swap chain description.
  ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

  // Set to a single back buffer.
  swap_chain_desc.BufferCount = 1;

  // Set the width and height of the back buffer.
  swap_chain_desc.BufferDesc.Width = static_cast<unsigned int>(m_game_settings->GetResolution().x);
  swap_chain_desc.BufferDesc.Height = static_cast<unsigned int>(m_game_settings->GetResolution().y);

  // Set regular 32-bit surface for the back buffer.
  swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

  // Set the refresh rate of the back buffer.
  if (m_game_settings->IsVSyncEnabled()) {
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
  } else {
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
  }

  // Set the usage of the back buffer.
  swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  // Set the handle for the window to render to.
  swap_chain_desc.OutputWindow = m_game_settings->GetHWnd();

  // Turn multisampling off.
  swap_chain_desc.SampleDesc.Count = 1;
  swap_chain_desc.SampleDesc.Quality = 0;

  // Set to full screen or windowed mode.
  if (m_game_settings->IsFullScreen()) {
    swap_chain_desc.Windowed = false;
  } else {
    swap_chain_desc.Windowed = true;
  }

  // Set the scan line ordering and scaling to unspecified.
  swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  // Discard the back buffer contents after presenting.
  swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  // Don't set the advanced flags.
  swap_chain_desc.Flags = 0;

  // find the feature level of the graphics card.
  if (FAILED(D3D11CreateDevice(NULL,
                               D3D_DRIVER_TYPE_HARDWARE,
                               NULL,
                               0,
                               NULL,
                               0,
                               D3D11_SDK_VERSION,
                               NULL,
                               &feature_level,
                               NULL))) {
      throw Exceptions::init_error("Finding Feature Level Failed!");
  }

  // Create the Swap Chain, Direct3D device, and Direct3D device context.
  if (FAILED(D3D11CreateDeviceAndSwapChain(NULL,
                                           D3D_DRIVER_TYPE_HARDWARE,
                                           NULL,
                                           0,
                                           &feature_level,
                                           1,
                                           D3D11_SDK_VERSION,
                                           &swap_chain_desc,
                                           &m_swap_chain,
                                           &m_device,
                                           NULL,
                                           &m_device_context))) {
    throw Exceptions::init_error("D3D11CreateDeviceAndSwapChain Failed!");
  }

  // Get the pointer to the back buffer.
  if (FAILED(m_swap_chain->GetBuffer(0,
                                     __uuidof(ID3D11Texture2D),
                                     reinterpret_cast<LPVOID*>(&back_buffer_ptr)))) {
    throw Exceptions::init_error("GetBuffer Failed!");
  }

  // Create the render target view with the back buffer pointer.
  if (FAILED(m_device->CreateRenderTargetView(back_buffer_ptr,
                                              NULL,
                                              &m_render_target_view))) {
    throw Exceptions::init_error("CreateRenderTargetView Failed!");
  }

  // Release pointer to the back buffer as we no longer need it.
  back_buffer_ptr->Release();
  back_buffer_ptr = 0;

  // Initialize the description of the depth buffer.
  ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));

  // Set up the description of the depth buffer.
  depth_buffer_desc.Width = static_cast<unsigned int>(m_game_settings->GetResolution().x);
  depth_buffer_desc.Height = static_cast<unsigned int>(m_game_settings->GetResolution().y);
  depth_buffer_desc.MipLevels = 1;
  depth_buffer_desc.ArraySize = 1;
  depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depth_buffer_desc.SampleDesc.Count = 1;
  depth_buffer_desc.SampleDesc.Quality = 0;
  depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depth_buffer_desc.CPUAccessFlags = 0;
  depth_buffer_desc.MiscFlags = 0;

  // Create the texture for the depth buffer using the filled out description.
  if (FAILED(m_device->CreateTexture2D(&depth_buffer_desc,
                                       NULL,
                                       &m_depth_stencil_buffer))) {
    throw Exceptions::init_error("CreateTexture2D Failed!");
  }

  // Initialize the description of the stencil state.
  ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

  // Set up the description of the stencil state.
  depth_stencil_desc.DepthEnable = true;
  depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

  depth_stencil_desc.StencilEnable = true;
  depth_stencil_desc.StencilReadMask = 0xFF;
  depth_stencil_desc.StencilWriteMask = 0xFF;

  // Stencil operations if pixel is front-facing.
  depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Stencil operations if pixel is back-facing.
  depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Create the depth stencil state.
  if (FAILED(m_device->CreateDepthStencilState(&depth_stencil_desc,
                                               &m_depth_stencil_state))) {
    throw Exceptions::init_error("CreateDepthStencilState Failed!");
  }

  // Set the depth stencil state.
  TurnZBufferOff();

  // Initialize the depth stencil view.
  ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

  // Set up the depth stencil view description.
  depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depth_stencil_view_desc.Texture2D.MipSlice = 0;

  // Create the depth stencil view.
  if (FAILED(m_device->CreateDepthStencilView(m_depth_stencil_buffer,
                                              &depth_stencil_view_desc,
                                              &m_depth_stencil_view))) {
    throw Exceptions::init_error("CreateDepthStencilView Failed!");
  }

  // Bind the render target view and depth stencil buffer to the
  // output render pipeline.
  m_device_context->OMSetRenderTargets(1,
                                       &m_render_target_view,
                                       m_depth_stencil_view);

  // Setup the raster description which will determine how and
  // what polygons will be drawn.
  raster_desc.AntialiasedLineEnable = false;
  raster_desc.CullMode = D3D11_CULL_BACK;
  raster_desc.DepthBias = 0;
  raster_desc.DepthBiasClamp = 0.0f;
  raster_desc.DepthClipEnable = true;
  raster_desc.FillMode = D3D11_FILL_SOLID;
  raster_desc.FrontCounterClockwise = false;
  raster_desc.MultisampleEnable = false;
  raster_desc.ScissorEnable = false;
  raster_desc.SlopeScaledDepthBias = 0.0f;

  // Create the rasterizer state from the description we just filled out.
  if (FAILED(m_device->CreateRasterizerState(&raster_desc,
                                             &m_raster_state))) {
    throw Exceptions::init_error("CreateRasterizerState Failed!");
  }

  // Now set the rasterizer state.
  m_device_context->RSSetState(m_raster_state);

  // Setup the viewport for rendering.
  viewport.Width = static_cast<float>(m_game_settings->GetResolution().x);
  viewport.Height = static_cast<float>(m_game_settings->GetResolution().y);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;

  // Create the viewport.
  m_device_context->RSSetViewports(1, &viewport);

  // Setup the projection matrix.
  fieldOfView = static_cast<float>(D3DX_PI) / 4.0f;
  screen_aspect = static_cast<float>(m_game_settings->GetResolution().x) /
                 static_cast<float>(m_game_settings->GetResolution().y);

  // Initialize the world matrix to the identity matrix.
  D3DXMatrixIdentity(&m_world);

  // Create an orthographic projection matrix for 2D rendering.
  D3DXMatrixOrthoLH(&m_ortho,
                    static_cast<float>(m_game_settings->GetResolution().x),
                    static_cast<float>(m_game_settings->GetResolution().y),
                    m_game_settings->GetScreenNear(),
                    m_game_settings->GetScreenDepth());

  // Clear the second depth stencil state before setting the parameters.
  ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

  // Now create a second depth stencil state which turns off the Z buffer for
  // 2D rendering.  The only difference is
  // that DepthEnable is set to false, all other parameters are the same as
  // the other depth stencil state.
  depthDisabledStencilDesc.DepthEnable = false;
  depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
  depthDisabledStencilDesc.StencilEnable = true;
  depthDisabledStencilDesc.StencilReadMask = 0xFF;
  depthDisabledStencilDesc.StencilWriteMask = 0xFF;
  depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Create the state using the device.
  if (FAILED(m_device->CreateDepthStencilState(&depthDisabledStencilDesc,
                                               &m_depthDisabledStencilState)))  {
    throw Exceptions::init_error("CreateDepthStencilState Failed!");
  }

  // Clear the blend state description.
  ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

  // Create an alpha enabled blend state description.
  blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
  blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

  // Create the blend state using the description.
  if (FAILED(m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState))) {
    throw Exceptions::init_error("CreateBlendState Failed!");
  }

  // Modify the description to create an alpha disabled blend state description.
  blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

  // Create the blend state using the description.
  if (FAILED(m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState))) {
    throw Exceptions::init_error("CreateBlendState Failed!");
  }

  m_is_d3d11_init = true;
}

//------------------------------------------------------------------------------
void Direct3D11_View::Render_Bitmaps(std::vector<Direct3D11_View::Bitmap_Renderable*> renderables, D3DXMATRIX *viewmatrix) {
  std::vector<Bitmap_Renderable*>::size_type layer_size = renderables.size();
  for (unsigned int i = 0; i < layer_size; i++) {
    if (renderables[i]->frame->vertex_buffer == 0) {
      D3D11_BUFFER_DESC vertexBufferDesc;
      D3D11_SUBRESOURCE_DATA vertexData;

      // Set up the description of the static vertex buffer.
      vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      vertexBufferDesc.ByteWidth = sizeof(Frame_Component::Vertex_Type) * renderables[i]->frame->vertex_count;
      vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      vertexBufferDesc.MiscFlags = 0;
      vertexBufferDesc.StructureByteStride = 0;

      // Give the subresource structure a pointer to the vertex data.
      vertexData.pSysMem = renderables[i]->frame->vertices;
      vertexData.SysMemPitch = 0;
      vertexData.SysMemSlicePitch = 0;

      // Now create the vertex buffer.
      if (FAILED(m_device->CreateBuffer(&vertexBufferDesc,
                                        &vertexData,
                                        &(renderables[i]->frame->vertex_buffer)))) {
        throw Exceptions::init_error("CreateBuffer (vertex_buffer) Failed!");
      }
    }
    if (renderables[i]->frame->index_buffer == 0) {
      D3D11_BUFFER_DESC indexBufferDesc;
      D3D11_SUBRESOURCE_DATA indexData;

      // Set up the description of the static index buffer.
      indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      indexBufferDesc.ByteWidth = sizeof(unsigned int) * renderables[i]->frame->index_count;
      indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      indexBufferDesc.CPUAccessFlags = 0;
      indexBufferDesc.MiscFlags = 0;
      indexBufferDesc.StructureByteStride = 0;

      // Give the subresource structure a pointer to the index data.
      indexData.pSysMem = renderables[i]->frame->indices;
      indexData.SysMemPitch = 0;
      indexData.SysMemSlicePitch = 0;

      // Create the index buffer.
      if (FAILED(m_device->CreateBuffer(&indexBufferDesc,
                                        &indexData,
                                        &(renderables[i]->frame->index_buffer)))) {
        throw Exceptions::init_error("CreateBuffer (index_buffer) Failed!");
      }
    }
    if (renderables[i]->texture->texture == 0) { 
      std::map<std::wstring, ID3D11ShaderResourceView*>::iterator stored_texture;
      stored_texture = m_texture_map.find(renderables[i]->texture->texture_path);
      if (stored_texture == m_texture_map.end()) {
        ID3D11ShaderResourceView* texture;
        if (FAILED(D3DX11CreateShaderResourceViewFromFile(m_device,
                                                          renderables[i]->texture->texture_path.c_str(),
                                                          NULL,
                                                          NULL,
                                                          &texture,
                                                          NULL)))  {
          throw Exceptions::init_error("Loading font file failed!");
        }
        m_texture_map[renderables[i]->texture->texture_path] = texture;
        renderables[i]->texture->texture = texture;
      } else {
        renderables[i]->texture->texture = (*stored_texture).second;
      }
    }
    if (IsThisBitmapComponentVisable(renderables[i])) {
      Render_Bitmap(renderables[i], viewmatrix);
    }
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::Render_Texts(std::vector<Direct3D11_View::Text_Renderable*> renderables, D3DXMATRIX *viewmatrix) {
  std::vector<Bitmap_Renderable*>::size_type layer_size = renderables.size();
  for (unsigned int i = 0; i < layer_size; i++) {
    if (renderables[i]->frame->vertex_buffer == 0) {
      D3D11_BUFFER_DESC vertexBufferDesc;
      D3D11_SUBRESOURCE_DATA vertexData;

      // Set up the description of the static vertex buffer.
      vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      vertexBufferDesc.ByteWidth = sizeof(Text_Component::Vertex_Type) * renderables[i]->frame->vertex_count;
      vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      vertexBufferDesc.MiscFlags = 0;
      vertexBufferDesc.StructureByteStride = 0;

      // Give the subresource structure a pointer to the vertex data.
      vertexData.pSysMem = renderables[i]->frame->vertices;
      vertexData.SysMemPitch = 0;
      vertexData.SysMemSlicePitch = 0;

      // Now create the vertex buffer.
      if (FAILED(m_device->CreateBuffer(&vertexBufferDesc,
                                        &vertexData,
                                        &(renderables[i]->frame->vertex_buffer)))) {
        throw Exceptions::init_error("CreateBuffer (vertex_buffer) Failed!");
      }
    }
    if (renderables[i]->frame->index_buffer == 0) {
      D3D11_BUFFER_DESC indexBufferDesc;
      D3D11_SUBRESOURCE_DATA indexData;
      // Set up the description of the static index buffer.
      indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      indexBufferDesc.ByteWidth = sizeof(unsigned int) * renderables[i]->frame->index_count;
      indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      indexBufferDesc.CPUAccessFlags = 0;
      indexBufferDesc.MiscFlags = 0;
      indexBufferDesc.StructureByteStride = 0;

      // Give the subresource structure a pointer to the index data.
      indexData.pSysMem = renderables[i]->frame->indices;
      indexData.SysMemPitch = 0;
      indexData.SysMemSlicePitch = 0;

      // Create the index buffer.
      if (FAILED(m_device->CreateBuffer(&indexBufferDesc,
                                        &indexData,
                                        &(renderables[i]->frame->index_buffer)))) {
        throw Exceptions::init_error("CreateBuffer (index_buffer) Failed!");
      }
    }
    if (renderables[i]->texture->texture == 0) { 
      std::map<std::wstring, ID3D11ShaderResourceView*>::iterator stored_texture;
      stored_texture = m_texture_map.find(renderables[i]->texture->texture_path);
      if (stored_texture == m_texture_map.end()) {
        ID3D11ShaderResourceView* texture;
        if (FAILED(D3DX11CreateShaderResourceViewFromFile(m_device,
                                                          renderables[i]->texture->texture_path.c_str(),
                                                          NULL,
                                                          NULL,
                                                          &texture,
                                                          NULL)))  {
          throw Exceptions::init_error("Loading font file failed!");
        }
        m_texture_map[renderables[i]->texture->texture_path] = texture;
        renderables[i]->texture->texture = texture;
      } else {
        renderables[i]->texture->texture = (*stored_texture).second;
      }
    }
    // By putting an ELSE here, you fix the bug which displays corrupt text for the first 
    // render cycle, but the text shows up slower.
    // I've chosen to live with the bug.
    Render_Text(renderables[i], viewmatrix);
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::Render_Camera(Camera_Component *camera, D3DXMATRIX *viewmatrix) {
  D3DXMATRIX rotationMatrix;
  D3DXVECTOR3 rotationVector = camera->GetRotationInRadians();
  D3DXVECTOR3 LookingAtVector = camera->GetLookingAtPosition();
  D3DXVECTOR3 UpVector = camera->GetUpDirection();
  D3DXVECTOR3 PosVector = camera->GetPosition();

  // Create the rotation matrix from the yaw, pitch, and roll values.
  D3DXMatrixRotationYawPitchRoll(&rotationMatrix,
                                 rotationVector.x,
                                 rotationVector.y,
                                 rotationVector.z);

  // Transform the lookAt and up vector by the rotation matrix so
  // the view is correctly rotated at the origin.
  D3DXVec3TransformCoord(&LookingAtVector, &LookingAtVector, &rotationMatrix);
  D3DXVec3TransformCoord(&UpVector, &UpVector, &rotationMatrix);

  // Translate the rotated camera position to the location of the viewer.
  LookingAtVector = PosVector + LookingAtVector;

  // Finally create the view matrix from the three updated vectors.
  D3DXMatrixLookAtLH(viewmatrix, &PosVector, &LookingAtVector, &UpVector);
}

//------------------------------------------------------------------------------
void Direct3D11_View::Render_Bitmap(Bitmap_Renderable* bitmap,
                                    D3DXMATRIX *viewmatrix) {
  // Put the model vertex and index buffers on the graphics pipeline to
  // prepare them for drawing.
  unsigned int stride, offset;

  D3DXMATRIX RotateMatrix_x, RotateMatrix_y, RotateMatrix_z;
  D3DXMATRIX TranslateMatrix;
  D3DXMATRIX OffsetMatrix;
  D3DXMATRIX ScaleMatrix;
  D3DXMatrixIdentity(&ScaleMatrix);

  // Offset from centre
  D3DXVECTOR3 SubsetOffsetMatrix = *bitmap->frame_centre;
  D3DXMatrixTranslation(&OffsetMatrix,
                        -1 * SubsetOffsetMatrix.x,
                        -1 * SubsetOffsetMatrix.y,
                        -1 * SubsetOffsetMatrix.z);

  D3DXMATRIX MeshMatrix;
  D3DXMatrixIdentity(&MeshMatrix);

  // Scale
  D3DXMatrixScaling(&ScaleMatrix,
                    bitmap->scale->x,
                    bitmap->scale->y,
                    bitmap->scale->z);

  // Translate
  D3DXMatrixTranslation(&TranslateMatrix,
                        bitmap->position->x,
                        bitmap->position->y,
                        bitmap->position->z);


  m_world = OffsetMatrix * ScaleMatrix * TranslateMatrix;

  ID3D11Buffer *vertexbuffer = bitmap->frame->vertex_buffer;
  ID3D11Buffer *indexbuffer = bitmap->frame->index_buffer;

  // Set vertex buffer stride and offset.
  stride = sizeof(Bitmap_Component::Vertex_Type);
  offset = 0;

  // Set the vertex buffer to active in the input assembler
  // so it can be rendered.
  m_device_context->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);

  // Set the index buffer to active in the input assembler
  // so it can be rendered.
  m_device_context->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);

  // Set the type of primitive that should be rendered from
  // this vertex buffer, in this case triangles.
  m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  if (m_game_settings->IsDebugMode()) {
    // Render the model using the color shader.
    m_debug_shader->Render(m_device_context,
                           bitmap->frame->index_count,
                           m_world,
                           *viewmatrix,
                           m_ortho,
                           bitmap->texture->texture,
                           bitmap->texture->transparency);
  } else {
    // Render the model using the color shader.
    m_transparent_shader->Render(m_device_context,
                                 bitmap->frame->index_count,
                                 m_world,
                                 *viewmatrix,
                                 m_ortho,
                                 bitmap->texture->texture,
                                 bitmap->texture->transparency);
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::Render_Text(Text_Renderable *text,
                                  D3DXMATRIX *viewmatrix) {
  unsigned int stride, offset;
  D3DXCOLOR pixelColor;
  ID3D11Buffer *vertexbuffer, *indexbuffer;

  D3DXMATRIX RotateMatrix_x, RotateMatrix_y, RotateMatrix_z;
  D3DXMATRIX TranslateMatrix;
  D3DXMATRIX OffsetMatrix;
  D3DXMATRIX ScaleMatrix;
  D3DXMatrixIdentity(&ScaleMatrix);

  D3DXVECTOR3 SubsetOffsetMatrix = *text->frame_centre;
  D3DXMatrixTranslation(&OffsetMatrix,
                        -1 * SubsetOffsetMatrix.x,
                        -1 * SubsetOffsetMatrix.y,
                        -1 * SubsetOffsetMatrix.z);

  D3DXMATRIX MeshMatrix;
  D3DXMatrixIdentity(&MeshMatrix);

  // Scale
  D3DXMatrixScaling(&ScaleMatrix,
                    text->scale->x,
                    text->scale->y,
                    text->scale->z);

  // Translate
  D3DXMatrixTranslation(&TranslateMatrix,
                        text->position->x,
                        text->position->y,
                        text->position->z);

  m_world = OffsetMatrix * TranslateMatrix * ScaleMatrix;

  vertexbuffer = text->frame->vertex_buffer;
  indexbuffer = text->frame->index_buffer;

  // Set vertex buffer stride and offset.
  stride = sizeof(Text_Component::Vertex_Type);
  offset = 0;

  // Set the vertex buffer to active in the input assembler so it can be rendered.
  m_device_context->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);

  // Set the index buffer to active in the input assembler so it can be rendered.
  m_device_context->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);

  // Set the type of primitive that should be rendered from this vertex buffer,
  // in this case triangles.
  m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Create a pixel color vector with the input sentence color.
  pixelColor = text->font->font_color;

  // Render the text using the font shader.
  m_font_shader->Render(m_device_context,
                        text->frame->index_count,
                        m_world,
                        *viewmatrix,
                        m_ortho,
                        text->texture->texture,
                        pixelColor,
                        text->texture->transparency);
}

void Direct3D11_View::TurnOnAlphaBlending() {
  float blendFactor[4];

  // Setup the blend factor.
  blendFactor[0] = 0.0f;
  blendFactor[1] = 0.0f;
  blendFactor[2] = 0.0f;
  blendFactor[3] = 0.0f;

  // Turn on the alpha blending.
  m_device_context->OMSetBlendState(m_alphaEnableBlendingState,
                                    blendFactor,
                                    0xffffffff);

  return;
}

void Direct3D11_View::TurnOffAlphaBlending() {
  float blendFactor[4];

  // Setup the blend factor.
  blendFactor[0] = 0.0f;
  blendFactor[1] = 0.0f;
  blendFactor[2] = 0.0f;
  blendFactor[3] = 0.0f;

  // Turn off the alpha blending.
  m_device_context->OMSetBlendState(m_alphaDisableBlendingState,
                                    blendFactor,
                                    0xffffffff);

  return;
}

void Direct3D11_View::TurnZBufferOn() {
  m_device_context->OMSetDepthStencilState(m_depth_stencil_state, 1);
  return;
}

void Direct3D11_View::TurnZBufferOff() {
  m_device_context->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
  return;
}

bool Direct3D11_View::IsThisBitmapComponentVisable(Bitmap_Renderable *bitmap) {
  if (bitmap->texture->transparency == 0.0f) {
    return false;
  }

  // At least one vertex in TileA is contained in the TileB.
  float a_tile_top, a_tile_bottom;
  a_tile_top = bitmap->position->y + static_cast<float>(bitmap->bitmap->GetSize().y / 2);
  a_tile_bottom = bitmap->position->y - static_cast<float>(bitmap->bitmap->GetSize().y / 2);

  float b_tile_top, b_tile_bottom;
  b_tile_top = m_camera->GetPosition().y + static_cast<float>(m_game_settings->GetResolution().y / 2);
  b_tile_bottom = m_camera->GetPosition().y - static_cast<float>(m_game_settings->GetResolution().y / 2);

  if (b_tile_top < a_tile_top && b_tile_top > a_tile_bottom) {
    return true;
  }

  if (a_tile_top < b_tile_top && a_tile_bottom > b_tile_bottom) {
    return true;
  }

  if (b_tile_bottom < a_tile_top && b_tile_bottom > a_tile_bottom) {
    return true;
  }

  if (a_tile_top == b_tile_top  && a_tile_bottom == b_tile_bottom) {
    return true;
  }

  // At least one vertex in TileA is contained in the TileB.
  float a_tile_left, a_tile_right;
  a_tile_left = bitmap->position->x - static_cast<float>(bitmap->bitmap->GetSize().x / 2);
  a_tile_right = bitmap->position->x + static_cast<float>(bitmap->bitmap->GetSize().x / 2);

  float b_tile_left, b_tile_right;
  b_tile_left = m_camera->GetPosition().x - static_cast<float>(m_game_settings->GetResolution().x / 2);
  b_tile_right = m_camera->GetPosition().x + static_cast<float>(m_game_settings->GetResolution().x / 2);

  if (b_tile_left > a_tile_left && b_tile_left < a_tile_right) {
    return true;
  }

  if (b_tile_left > a_tile_left && b_tile_left > b_tile_right) {
    return true;
  }

  if (b_tile_right > a_tile_left && b_tile_right < a_tile_right) {
    return true;
  }

  if (b_tile_left == a_tile_left && b_tile_right == a_tile_right) {
    return true;
  }

  return false;
}

}  // namespace Tunnelour
