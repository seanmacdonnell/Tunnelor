//  Copyright 2011 Sean MacDonnell
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
#include "Direct3D11_View_Init_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View::Direct3D11_View() {
  m_model = NULL;

  m_is_window_init = false;
  m_is_d3d11_init = false;

  // Default Values
  m_screen_width  = 800;
  m_screen_height = 600;
  m_is_full_screen = false;

  m_vsync_enabled = true;
  m_screen_depth = 1000.0f;
  m_screen_near = 0.1f;

  m_swapChain = NULL;
  m_device = NULL;
  m_deviceContext = NULL;
  m_renderTargetView = NULL;
  m_depthStencilBuffer = NULL;
  m_depthStencilState = NULL;
  m_depthStencilView = NULL;
  m_rasterState = NULL;
}

//------------------------------------------------------------------------------
Direct3D11_View::~Direct3D11_View() {
  if (m_is_window_init) {
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if(m_is_full_screen)
    {
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
	  // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	  if(m_swapChain)
	  {
		  m_swapChain->SetFullscreenState(false, NULL);
	  }

	  if(m_rasterState)
	  {
		  m_rasterState->Release();
		  m_rasterState = 0;
	  }

	  if(m_depthStencilView)
	  {
		  m_depthStencilView->Release();
		  m_depthStencilView = 0;
	  }

	  if(m_depthStencilState)
	  {
		  m_depthStencilState->Release();
		  m_depthStencilState = 0;
	  }

	  if(m_depthStencilBuffer)
	  {
		  m_depthStencilBuffer->Release();
		  m_depthStencilBuffer = 0;
	  }

	  if(m_renderTargetView)
	  {
		  m_renderTargetView->Release();
		  m_renderTargetView = 0;
	  }

	  if(m_deviceContext)
	  {
		  m_deviceContext->Release();
		  m_deviceContext = 0;
	  }

	  if(m_device)
	  {
		  m_device->Release();
		  m_device = 0;
	  }

	  if(m_swapChain)
	  {
		  m_swapChain->Release();
		  m_swapChain = 0;
	  }
  }
}

//------------------------------------------------------------------------------
void Direct3D11_View::Init(Tunnelour::Component_Composite *model) {
  m_model = model;

  if (!m_is_initialised) {
    if (!m_is_window_init) { Init_Window(); }
    if (!m_is_d3d11_init) { Init_D3D11(); }
    m_is_initialised = true;
  }

}

//------------------------------------------------------------------------------
void Direct3D11_View::Run() {
  // Get Relevant components from the model.
  //Tunnelour::Component::Component_Mutator mutator2;
  Tunnelour::Direct3D11_View_Init_Mutator mutator;

  m_model->Apply(&mutator);
  if (mutator.m_found_back_buffer_color)  {
    m_back_buffer_color[0] = mutator.m_back_buffer_color[0];
	  m_back_buffer_color[1] = mutator.m_back_buffer_color[1];
	  m_back_buffer_color[2] = mutator.m_back_buffer_color[2];
	  m_back_buffer_color[3] = mutator.m_back_buffer_color[3];
  } else {
    //Default Back Buffer Color is Black
    m_back_buffer_color[0] = 0.0;
	  m_back_buffer_color[1] = 0.0;
	  m_back_buffer_color[2] = 0.0;
	  m_back_buffer_color[3] = 1.0;
  }

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, m_back_buffer_color);
    
	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

  // Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
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
  HRESULT result;
  IDXGIFactory* factory;
  IDXGIAdapter* adapter;
  IDXGIOutput* adapterOutput;
  unsigned int numModes, i, numerator, denominator, stringLength;
  DXGI_MODE_DESC* displayModeList;
  DXGI_ADAPTER_DESC adapterDesc;
  int error;
  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  D3D_FEATURE_LEVEL featureLevel;
  ID3D11Texture2D* backBufferPtr;
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  D3D11_RASTERIZER_DESC rasterDesc;
  D3D11_VIEWPORT viewport;
  float fieldOfView, screenAspect;

  // Create a DirectX graphics interface factory.
  result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("CreateDXGIFactory Failed!");
  }

  // Use the factory to create an adapter for the primary graphics interface (video card).
  result = factory->EnumAdapters(0, &adapter);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("EnumAdapters Failed!");
  }

  // Enumerate the primary adapter output (monitor).
  result = adapter->EnumOutputs(0, &adapterOutput);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("EnumOutputs Failed!");
  }

  // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
  result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("GetDisplayModeList Failed!");
  }

  // Create a list to hold all the possible display modes for this monitor/video card combination.
  displayModeList = new DXGI_MODE_DESC[numModes];
  if(!displayModeList)
  {
    throw Tunnelour::Exceptions::initialising_error("displayModeList Failed!");
  }

  // Now fill the display mode list structures.
  result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("GetDisplayModeList Failed!");
  }

  // Now go through all the display modes and find the one that matches the screen width and height.
  // When a match is found store the numerator and denominator of the refresh rate for that monitor.
  for(i=0; i<numModes; i++)
  {
    if(displayModeList[i].Width == (unsigned int)m_screen_width)
    {
      if(displayModeList[i].Height == (unsigned int)m_screen_height)
      {
        numerator = displayModeList[i].RefreshRate.Numerator;
        denominator = displayModeList[i].RefreshRate.Denominator;
      }
    }
  }

  // Get the adapter (video card) description.
  result = adapter->GetDesc(&adapterDesc);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("GetDesc Failed!");
  }

  // Store the dedicated video card memory in megabytes.
  m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

  // Convert the name of the video card to a character array and store it.
  error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
  if(error != 0)
  {
    throw Tunnelour::Exceptions::initialising_error("wcstombs_s Failed!");
  }

  // Release the display mode list.
  delete [] displayModeList;
  displayModeList = 0;

  // Release the adapter output.
  adapterOutput->Release();
  adapterOutput = 0;

  // Release the adapter.
  adapter->Release();
  adapter = 0;

  // Release the factory.
  factory->Release();
  factory = 0;

  // Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

  // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

  // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = m_screen_width;
    swapChainDesc.BufferDesc.Height = m_screen_height;

  // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

  // Set the refresh rate of the back buffer.
  if(m_vsync_enabled)
  {
      swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
  }
  else
  {
      swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
  }

  // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = m_hwnd;

  // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

  // Set to full screen or windowed mode.
  if(m_is_full_screen)
  {
    swapChainDesc.Windowed = false;
  }
  else
  {
    swapChainDesc.Windowed = true;
  }

  // Set the scan line ordering and scaling to unspecified.
  swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  // Discard the back buffer contents after presenting.
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  // Don't set the advanced flags.
  swapChainDesc.Flags = 0;

  // Set the feature level to DirectX 11.
  featureLevel = D3D_FEATURE_LEVEL_10_0;

  // Create the swap chain, Direct3D device, and Direct3D device context.
  result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
                       D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("D3D11CreateDeviceAndSwapChain Failed!");
  }

  // Get the pointer to the back buffer.
  result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("GetBuffer Failed!");
  }

  // Create the render target view with the back buffer pointer.
  result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("CreateRenderTargetView Failed!");
  }

  // Release pointer to the back buffer as we no longer need it.
  backBufferPtr->Release();
  backBufferPtr = 0;

  // Initialize the description of the depth buffer.
  ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

  // Set up the description of the depth buffer.
  depthBufferDesc.Width = m_screen_width;
  depthBufferDesc.Height = m_screen_height;
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  // Create the texture for the depth buffer using the filled out description.
  result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("CreateTexture2D Failed!");
  }

  // Initialize the description of the stencil state.
  ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

  // Set up the description of the stencil state.
  depthStencilDesc.DepthEnable = true;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

  depthStencilDesc.StencilEnable = true;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;

  // Stencil operations if pixel is front-facing.
  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Stencil operations if pixel is back-facing.
  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Create the depth stencil state.
  result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("CreateDepthStencilState Failed!");
  }

  // Set the depth stencil state.
  m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

  // Initialize the depth stencil view.
  ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

  // Set up the depth stencil view description.
  depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  // Create the depth stencil view.
  result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("CreateDepthStencilView Failed!");
  }

  // Bind the render target view and depth stencil buffer to the output render pipeline.
  m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

  // Setup the raster description which will determine how and what polygons will be drawn.
  rasterDesc.AntialiasedLineEnable = false;
  rasterDesc.CullMode = D3D11_CULL_BACK;
  rasterDesc.DepthBias = 0;
  rasterDesc.DepthBiasClamp = 0.0f;
  rasterDesc.DepthClipEnable = true;
  rasterDesc.FillMode = D3D11_FILL_SOLID;
  rasterDesc.FrontCounterClockwise = false;
  rasterDesc.MultisampleEnable = false;
  rasterDesc.ScissorEnable = false;
  rasterDesc.SlopeScaledDepthBias = 0.0f;

  // Create the rasterizer state from the description we just filled out.
  result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
  if(FAILED(result))
  {
    throw Tunnelour::Exceptions::initialising_error("CreateRasterizerState Failed!");
  }

  // Now set the rasterizer state.
  m_deviceContext->RSSetState(m_rasterState);
  
  // Setup the viewport for rendering.
    viewport.Width = (float)m_screen_width;
    viewport.Height = (float)m_screen_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

  // Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

  // Setup the projection matrix.
  fieldOfView = (float)D3DX_PI / 4.0f;
  screenAspect = (float)m_screen_width / (float)m_screen_height;

  // Create the projection matrix for 3D rendering.
  D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, m_screen_near, m_screen_depth);

  // Initialize the world matrix to the identity matrix.
  D3DXMatrixIdentity(&m_worldMatrix);

  // Create an orthographic projection matrix for 2D rendering.
  D3DXMatrixOrthoLH(&m_orthoMatrix, (float)m_screen_width, (float)m_screen_height, m_screen_near, m_screen_depth);

  m_is_d3d11_init = true;
}

}  // namespace Tunnelour
