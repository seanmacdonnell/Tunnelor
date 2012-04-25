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

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View::Direct3D11_View() {
  m_model = NULL;

  m_is_window_init = false;
  m_is_d3d11_init = false;

  m_screen_width  = 0;
  m_screen_height = 0;
  m_is_full_screen = false;

  m_vsync_enabled = false;
  m_screen_depth = 0;
  m_screen_near = 0;

  m_swap_chain = NULL;
  m_device = NULL;
  m_device_context = NULL;
  m_render_target_view = NULL;
  m_depth_stencil_buffer = NULL;
  m_depth_stencil_state = NULL;
  m_depth_stencil_view = NULL;
  m_raster_state = NULL;
  m_color_shader = NULL;

  m_camera = NULL;
  m_mesh = NULL;
  m_color_shader = NULL;
}

//------------------------------------------------------------------------------
Direct3D11_View::~Direct3D11_View() {
  if (m_is_window_init) {
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if (m_is_full_screen) {
      ChangeDisplaySettings(NULL, 0);
    }

    // Remove the window.
    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    // Remove the application instance.
    UnregisterClass(m_application_name, m_hinstance);
    m_hinstance = NULL;
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

    if (m_color_shader) {
      delete m_color_shader;
      m_color_shader = NULL;
    }
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::Init(Tunnelour::Component_Composite * const model) {
  View::Init(model);

  // Set Default Values
  m_is_window_init = false;
  m_is_d3d11_init = false;

  m_screen_width  = 800;
  m_screen_height = 600;
  m_is_full_screen = false;

  m_vsync_enabled = true;
  m_screen_depth = 1000.0f;
  m_screen_near = 0.1f;

  if (!m_is_initialised) {
    if (!m_is_window_init) { Init_Window(); }
    if (!m_is_d3d11_init) { Init_D3D11(); }
    if (!m_color_shader) {
      m_color_shader = new Tunnelour::Direct3D11_View_ColorShader();
      m_color_shader->Init(m_device, &m_hwnd);
    }
    m_is_initialised = true;
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::Run() {
  // Get Relevant Components from the model with the Mutator.
  Tunnelour::Direct3D11_View_Mutator mutator;

  m_model->Apply(&mutator);

  Tunnelour::Background_Component* m_background;

  if (mutator.FoundBackground())  {
    m_background = mutator.GetBackground();
    m_back_buffer_color[0] = m_background->GetRed();
    m_back_buffer_color[1] = m_background->GetGreen();
    m_back_buffer_color[2] = m_background->GetBlue();
    m_back_buffer_color[3] = m_background->GetAlpha();

  } else {
    // Default Back Buffer Color is Black
    m_back_buffer_color[0] = 0.0;
    m_back_buffer_color[1] = 0.0;
    m_back_buffer_color[2] = 0.0;
    m_back_buffer_color[3] = 1.0;
  }

  if (mutator.FoundCamera()) {
    m_camera = mutator.GetCamera();
    if (!m_camera->IsInitialised()) { m_camera->Init(); }
  }

  if (mutator.FoundMesh()) {
    m_mesh = mutator.GetMesh();
    if (!m_mesh->IsInitialised()) { m_mesh->Init(m_device); }
  }
  D3DXMATRIX viewmatrix;

  // Clear the back buffer.
  m_device_context->ClearRenderTargetView(m_render_target_view,
                                          m_back_buffer_color);

  // Clear the depth buffer.
  m_device_context->ClearDepthStencilView(m_depth_stencil_view,
                                          D3D11_CLEAR_DEPTH,
                                          1.0f,
                                          0);

  D3DXMATRIX rotationMatrix;
  D3DXVECTOR3 rotationVector = m_camera->GetRotationInRadians();
  D3DXVECTOR3 LookingAtVector = m_camera->GetLookingAtPosition();
  D3DXVECTOR3 UpVector = m_camera->GetUpDirection();
  D3DXVECTOR3 PosVector = m_camera->GetPosition();

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
  D3DXMatrixLookAtLH(&viewmatrix, &PosVector, &LookingAtVector, &UpVector);

  // Put the model vertex and index buffers on the graphics pipeline to
  // prepare them for drawing.
  // Set vertex buffer stride and offset.
  unsigned int stride = sizeof(Tunnelour::Mesh_Component::VertexType);
  unsigned int offset = 0;
  ID3D11Buffer *vertexbuffer, *indexbuffer;

  vertexbuffer = m_mesh->GetVertexBuffer();
  indexbuffer = m_mesh->GetIndexBuffer();

  // Set the vertex buffer to active in the input assembler
  // so it can be rendered.
  m_device_context->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);

  // Set the index buffer to active in the input assembler
  // so it can be rendered.
  m_device_context->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);

  // Set the type of primitive that should be rendered from
  // this vertex buffer, in this case triangles.
  m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Render the model using the color shader.
  m_color_shader->Render(m_device_context,
                         m_mesh->GetIndexCount(),
                         m_world,
                         viewmatrix,
                         m_projection);

  // Present the rendered scene to the screen.
  // Present the back buffer to the screen since rendering is complete.
  if (m_vsync_enabled) {
  // Lock to screen refresh rate.
  m_swap_chain->Present(1, 0);
  } else {
  // Present as fast as possible.
  m_swap_chain->Present(0, 0);
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
  m_hinstance = GetModuleHandle(NULL);

  // Give the application a name.
  m_application_name = L"Tunnelour";

  // Setup the windows class with default settings.
  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc   = Message_Wrapper::WindowProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = m_hinstance;
  wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
  wc.hIconSm       = wc.hIcon;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = m_application_name;
  wc.cbSize        = sizeof(WNDCLASSEX);

  // Register the window class.
  RegisterClassEx(&wc);

  // Setup the screen settings depending on whether it is running in full
  // screen or in windowed mode.
  if (m_is_full_screen) {
    // Determine the resolution of the clients desktop screen.
    m_screen_width  = GetSystemMetrics(SM_CXSCREEN);
    m_screen_height = GetSystemMetrics(SM_CYSCREEN);

    // If full screen set the screen to maximum size of the users desktop
    // and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = (DWORD)m_screen_width;
    dmScreenSettings.dmPelsHeight = (DWORD)m_screen_height;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields     = DM_BITSPERPEL |
                                    DM_PELSWIDTH |
                                    DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    // Set the position of the window to the top left corner.
    posX = posY = 0;
  } else {
    // Place the window in the middle of the screen.
    posX = (GetSystemMetrics(SM_CXSCREEN) - m_screen_width)  / 2;
    posY = (GetSystemMetrics(SM_CYSCREEN) - m_screen_height) / 2;
  }

  // Create the window with the screen settings and get the handle to it.
  m_hwnd = CreateWindowEx(NULL,
                          m_application_name,
                          m_application_name,
                          WS_TILEDWINDOW,
                          posX,
                          posY,
                          m_screen_width,
                          m_screen_height,
                          NULL,
                          NULL,
                          m_hinstance,
                          NULL);

  // Bring the window up on the screen and set it as main focus.
  ShowWindow(m_hwnd, SW_SHOW);
  SetForegroundWindow(m_hwnd);
  SetFocus(m_hwnd);

  // Hide the mouse cursor.
  ShowCursor(false);

  m_is_window_init = true;

  return;
}

//------------------------------------------------------------------------------
void Direct3D11_View::Init_D3D11() {
  IDXGIFactory* factory;
  IDXGIAdapter* adapter;
  IDXGIOutput* adapter_output;
  unsigned int num_modes, i, numerator, denominator, string_length;
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

  // Create a DirectX graphics interface factory.
  if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory),
                             reinterpret_cast<void**>(&factory)))) {
    throw Tunnelour::Exceptions::init_error("CreateDXGIFactory Failed!");
  }

  // Use the factory to create an adapter for the
  // primary graphics interface (video card).
  if (FAILED(factory->EnumAdapters(0, &adapter))) {
    throw Tunnelour::Exceptions::init_error("EnumAdapters Failed!");
  }

  // Enumerate the primary adapter output (monitor).
  if (FAILED(adapter->EnumOutputs(0, &adapter_output))) {
    throw Tunnelour::Exceptions::init_error("EnumOutputs Failed!");
  }

  // Get the number of modes that fit the
  // DXGI_FORMAT_R8G8B8A8_UNORM display format
  // for the adapter output (monitor).
  if (FAILED(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                                DXGI_ENUM_MODES_INTERLACED,
                                                &num_modes,
                                                NULL))) {
    throw Tunnelour::Exceptions::init_error("Getdisplay_mode_list Failed!");
  }

  // Create a list to hold all the possible display modes for this
  // monitor/video card combination.
  display_mode_list = new DXGI_MODE_DESC[num_modes];
  if (!display_mode_list) {
    throw Tunnelour::Exceptions::init_error("display_mode_list Failed!");
  }

  // Now fill the display mode list structures.
  if (FAILED(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                                DXGI_ENUM_MODES_INTERLACED,
                                                &num_modes,
                                                display_mode_list))) {
    throw Tunnelour::Exceptions::init_error("Getdisplay_mode_list Failed!");
  }

  // Now go through all the display modes and find the one that matches
  // the screen width and height.
  // When a match is found store the numerator and denominator of the
  // refresh rate for that monitor.
  for (i = 0; i < num_modes; i++) {
    if (display_mode_list[i].Width == (unsigned int)m_screen_width) {
      if (display_mode_list[i].Height == (unsigned int)m_screen_height) {
        numerator = display_mode_list[i].RefreshRate.Numerator;
        denominator = display_mode_list[i].RefreshRate.Denominator;
      }
    }
  }

  // Get the adapter (video card) description.
  if (FAILED(adapter->GetDesc(&adapter_desc))) {
    throw Tunnelour::Exceptions::init_error("GetDesc Failed!");
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
    throw Tunnelour::Exceptions::init_error("wcstombs_s Failed!");
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
  swap_chain_desc.BufferDesc.Width = m_screen_width;
  swap_chain_desc.BufferDesc.Height = m_screen_height;

  // Set regular 32-bit surface for the back buffer.
  swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

  // Set the refresh rate of the back buffer.
  if (m_vsync_enabled) {
      swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
  } else {
      swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
  }

  // Set the usage of the back buffer.
  swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  // Set the handle for the window to render to.
  swap_chain_desc.OutputWindow = m_hwnd;

  // Turn multisampling off.
  swap_chain_desc.SampleDesc.Count = 1;
  swap_chain_desc.SampleDesc.Quality = 0;

  // Set to full screen or windowed mode.
  if (m_is_full_screen) {
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

  // Set the feature level to DirectX 11.
  feature_level = D3D_FEATURE_LEVEL_10_0;

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
    throw Tunnelour::Exceptions::init_error("D3D11CreateDeviceAndSwapChain Failed!");
  }

  // Get the pointer to the back buffer.
  if (FAILED(m_swap_chain->GetBuffer(0,
                                     __uuidof(ID3D11Texture2D),
                                     reinterpret_cast<LPVOID*>(&back_buffer_ptr)))) {
    throw Tunnelour::Exceptions::init_error("GetBuffer Failed!");
  }

  // Create the render target view with the back buffer pointer.
  if (FAILED(m_device->CreateRenderTargetView(back_buffer_ptr,
                                              NULL,
                                              &m_render_target_view))) {
    throw Tunnelour::Exceptions::init_error("CreateRenderTargetView Failed!");
  }

  // Release pointer to the back buffer as we no longer need it.
  back_buffer_ptr->Release();
  back_buffer_ptr = 0;

  // Initialize the description of the depth buffer.
  ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));

  // Set up the description of the depth buffer.
  depth_buffer_desc.Width = m_screen_width;
  depth_buffer_desc.Height = m_screen_height;
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
    throw Tunnelour::Exceptions::init_error("CreateTexture2D Failed!");
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
    throw Tunnelour::Exceptions::init_error("CreateDepthStencilState Failed!");
  }

  // Set the depth stencil state.
  m_device_context->OMSetDepthStencilState(m_depth_stencil_state, 1);

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
    throw Tunnelour::Exceptions::init_error("CreateDepthStencilView Failed!");
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
    throw Tunnelour::Exceptions::init_error("CreateRasterizerState Failed!");
  }

  // Now set the rasterizer state.
  m_device_context->RSSetState(m_raster_state);

  // Setup the viewport for rendering.
  viewport.Width = static_cast<float>(m_screen_width);
  viewport.Height = static_cast<float>(m_screen_height);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;

  // Create the viewport.
  m_device_context->RSSetViewports(1, &viewport);

  // Setup the projection matrix.
  fieldOfView = static_cast<float>(D3DX_PI) / 4.0f;
  screen_aspect = static_cast<float>(m_screen_width) /
                 static_cast<float>(m_screen_height);

  // Create the projection matrix for 3D rendering.
  D3DXMatrixPerspectiveFovLH(&m_projection,
                             fieldOfView,
                             screen_aspect,
                             m_screen_near,
                             m_screen_depth);

  // Initialize the world matrix to the identity matrix.
  D3DXMatrixIdentity(&m_world);

  // Create an orthographic projection matrix for 2D rendering.
  D3DXMatrixOrthoLH(&m_ortho,
                    static_cast<float>(m_screen_width),
                    static_cast<float>(m_screen_height),
                    m_screen_near,
                    m_screen_depth);

  m_is_d3d11_init = true;
}

}  // namespace Tunnelour
