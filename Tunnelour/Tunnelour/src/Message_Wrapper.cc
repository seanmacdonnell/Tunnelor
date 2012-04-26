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

#include "Message_Wrapper.h"

namespace Tunnelour {
//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Message_Wrapper::Message_Wrapper() {
}

//------------------------------------------------------------------------------
Message_Wrapper::~Message_Wrapper() {
}

//------------------------------------------------------------------------------
LRESULT CALLBACK Message_Wrapper::WindowProc(HWND hWnd,
                                             UINT message,
                                             WPARAM wParam,
                                             LPARAM lParam) {
  switch (message) {
    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    } break;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

//------------------------------------------------------------------------------
bool Message_Wrapper::isWM_QUIT() {
  if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))  {
    if (m_msg.message == WM_QUIT)  return true;
    TranslateMessage(&m_msg);
    DispatchMessage(&m_msg);
  }

  return false;
}

//------------------------------------------------------------------------------
MSG Message_Wrapper::GetLastMessage() {
  return m_msg;
}
}  // namespace API_Wrapper
