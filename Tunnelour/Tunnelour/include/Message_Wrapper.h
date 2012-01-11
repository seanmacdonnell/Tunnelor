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

#ifndef TUNNELOUR_MESSAGE_WRAPPER_H_
#define TUNNELOUR_MESSAGE_WRAPPER_H_

#include <windows.h>
#include <windowsx.h>

namespace Tunnelour {
class Message_Wrapper {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Message_Wrapper();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Message_Wrapper();

  //----------------------------------------------------------------------------
  // Description : WinProc is a windows callback function which handles windows
  //               messages
  //               It catches WM_DESTROY which is sent when the game window is
  //               closed and calls WM_QUIT so the main loop can end WinMain
  //               properly.
  // Parameters  : HWND hWnd: Handle to the window the message came from.
  //               UINT message: Identifier for the message.
  //               WPARAM wParam: Additional Paramaters for the message
  //               LPARAM lParam: Additional Paramaters for the message
  // Return      : Always returns 0 when catching a message as this tells
  //               windows that the message has been handled.
  //----------------------------------------------------------------------------
  static LRESULT CALLBACK Message_Wrapper::WindowProc(HWND hWnd,
                                                      UINT message,
                                                      WPARAM wParam,
                                                      LPARAM lParam);

  //----------------------------------------------------------------------------
  // Description : HandleMessages uses PeekMessage to check if the Windows
  //               Message Queue has a message waiting which it needs to handle.
  //               This only handles WM_QUIT messages, all others are translated
  //               and dispatched.
  // Return      : bool false - If the message is WM_QUIT
  //               bool true  - At all other times.
  //----------------------------------------------------------------------------
  bool Message_Wrapper::isWM_QUIT();

  //----------------------------------------------------------------------------
  // Description : This function returns the last message read by HandleMessages
  // Return      : a MSG struct.
  //----------------------------------------------------------------------------
  MSG Message_Wrapper::GetLastMessage();

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Current MSG from the MSG Queue
  //---------------------------------------------------------------------------
  MSG m_msg;
};
}  // namespace Engine

#endif  // TUNNELOUR_MESSAGE_WRAPPER_H_
