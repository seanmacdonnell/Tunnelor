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

#ifndef TUNNELOUR_INPUT_CONTROLLER_H_
#define TUNNELOUR_INPUT_CONTROLLER_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include "Component_Composite.h"
#include "Controller.h"
#include "Game_Settings_Component.h"
#include "Avatar_Component.h"


namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller adds all the components to the model
//                which are require for an intialised state.
//-----------------------------------------------------------------------------
class Input_Controller: public Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Input_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Input_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual void Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:
  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Direct Input Variables
  //---------------------------------------------------------------------------
  bool InitDirectInput();

  //---------------------------------------------------------------------------
  // Description : Reads the current state of the keyboard
  //---------------------------------------------------------------------------
  bool ReadKeyboard();

  //---------------------------------------------------------------------------
  // Description : Reads the current state of the mouse
  //---------------------------------------------------------------------------
  bool ReadMouse();

  //---------------------------------------------------------------------------
  // Description : Processes all current input
  //---------------------------------------------------------------------------
  void ProcessInput();

 private:
  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  Game_Settings_Component *m_game_settings;
  IDirectInput8 *m_directInput;
  IDirectInputDevice8 *m_keyboard;
  IDirectInputDevice8 *m_mouse;
  unsigned char m_keyboardState[256];
  DIMOUSESTATE m_mouseState;
  int m_screenWidth, m_screenHeight;
  int m_mouseX, m_mouseY;
  Avatar_Component *m_avatar_component;
  bool m_dik_grave_pressed;  // Tilda
  bool m_has_been_initalised;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_INPUT_CONTROLLER_H_
