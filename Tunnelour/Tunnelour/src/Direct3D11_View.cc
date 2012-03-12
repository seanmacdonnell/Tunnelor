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

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View::Direct3D11_View() {
  m_model = NULL;
}

//------------------------------------------------------------------------------
Direct3D11_View::~Direct3D11_View() {
  if (m_is_window_created) {
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
}

//------------------------------------------------------------------------------
void Direct3D11_View::Init(Tunnelour::Component_Composite *model) {
  m_model = model;

  // Default windowed resolution is 800x600
  m_screen_width  = 800;
  m_screen_height = 600;

  m_is_window_created = false;
  m_is_full_screen = false;
}

//------------------------------------------------------------------------------
void Direct3D11_View::Run() {
  if (!m_is_window_created) { Create_Window(); }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Direct3D11_View::Create_Window() {
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

  m_is_window_created = true;

  return;
}

}  // namespace Tunnelour
