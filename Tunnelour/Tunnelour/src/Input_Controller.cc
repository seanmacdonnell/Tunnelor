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

#include "Input_Controller.h"
#include "Camera_Component.h"
#include "Game_Settings_Component.h"
#include "Input_Controller_Mutator.h"
#include "Exceptions.h"
#include "Get_Avatar_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Input_Controller::Input_Controller() : Controller() {
  m_game_settings = 0;
  m_avatar_component = 0;
  m_dik_grave_pressed = false;
  m_input_component = 0;
}

//------------------------------------------------------------------------------
Input_Controller::~Input_Controller() {
  // Release the mouse.
  if (m_mouse) {
    m_mouse->Unacquire();
    m_mouse->Release();
    m_mouse = 0;
  }

  // Release the keyboard.
  if (m_keyboard) {
    m_keyboard->Unacquire();
    m_keyboard->Release();
    m_keyboard = 0;
  }

  // Release the main interface to direct input.
  if (m_directInput) {
    m_directInput->Release();
    m_directInput = 0;
  }

  m_game_settings = 0;
  m_avatar_component = 0;
  m_dik_grave_pressed = false;
}

//------------------------------------------------------------------------------
bool Input_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  if (m_input_component == 0) {
    m_input_component = new Input_Component();
    m_input_component->Init();
    m_model->Add(m_input_component);
  }

  Input_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_mouseX = 0;
    m_mouseY = 0;

    m_game_settings = mutator.GetGameSettings();
    if (m_game_settings->GetHInstance() != 0) {
      if (InitDirectInput()) {
        m_has_been_initialised = true;
      }
    }
  } else {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Input_Controller::InitDirectInput() {
  if (m_game_settings->GetHInstance() && m_game_settings->GetHWnd()) {
    // Store the screen size which will be used for positioning the mouse cursor.
    m_screenWidth = static_cast<int>(m_game_settings->GetResolution().x);
    m_screenHeight = static_cast<int>(m_game_settings->GetResolution().y);

    // Initialize the main direct input interface.
    if (FAILED(DirectInput8Create(m_game_settings->GetHInstance(),
                                  DIRECTINPUT_VERSION,
                                  IID_IDirectInput8,
                                  reinterpret_cast<void**>(&m_directInput),
                                  NULL)))   {
      std::string error = "Input_Controller DirectInput8Create Failed!";
      throw Exceptions::run_error(error);
    }

    // Initialize the direct input interface for the keyboard.
    if (FAILED(m_directInput->CreateDevice(GUID_SysKeyboard,
                                          &m_keyboard,
                                          NULL))) {
      std::string error = "Input_Controller CreateDevice Keyboard Failed!";
      throw Exceptions::run_error(error);
    }

    // Set the data format.
    // In this case since it is a keyboard we can use
    // the predefined data format.
    if (FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard))) {
      std::string error = "Input_Controller SetDataFormat Failed!";
      throw Exceptions::run_error(error);
    }

    // Set the cooperative level of the keyboard
    // to not share with other programs.
    if (FAILED(m_keyboard->SetCooperativeLevel(m_game_settings->GetHWnd(),
                                              DISCL_FOREGROUND | DISCL_EXCLUSIVE))) {
      std::string error = "Input_Controller SetCooperativeLevel Failed!";
      throw Exceptions::run_error(error);
    }

    // Now acquire the keyboard.
    if (FAILED(m_keyboard->Acquire())) {
      // std::string error = "Input_Controller Acquire Failed!";
      // throw Exceptions::run_error(error);
      return false;
    }

    // Initialize the direct input interface for the mouse.
    if (FAILED(m_directInput->CreateDevice(GUID_SysMouse,
                                          &m_mouse,
                                          NULL))) {
      std::string error = "Input_Controller CreateDevice Mouse Failed!";
      throw Exceptions::run_error(error);
    }

    // Set the data format for the mouse using
    // the pre-defined mouse data format.
    if (FAILED(m_mouse->SetDataFormat(&c_dfDIMouse))) {
      std::string error = "Input_Controller CreateDevice Mouse Failed!";
      throw Exceptions::run_error(error);
    }

    // Set the cooperative level of the mouse
    // to share with other programs.
    if (FAILED(m_mouse->SetCooperativeLevel(m_game_settings->GetHWnd(),
                                           DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
      std::string error = "Input_Controller SetCooperativeLevel Failed!";
      throw Exceptions::run_error(error);
    }

    // Acquire the mouse.
    if (FAILED(m_mouse->Acquire())) {
      // std::string error = "Input_Controller Acquire Failed!";
      // throw Exceptions::run_error(error);
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
bool Input_Controller::Run() {
  if (!m_has_been_initialised) {
    return false;
  } else {
    // Read the current state of the keyboard.
    if (!ReadKeyboard()) {
      throw Exceptions::run_error("Input_Controller ReadKeyboard Failed!");
    }

    // Read the current state of the mouse.
    if (!ReadMouse()) {
      throw Exceptions::run_error("Input_Controller ReadMouse Failed!");
    }

    // Process the changes in the mouse and keyboard.
    ProcessInput();
  }
  return true;
}

//------------------------------------------------------------------------------
bool Input_Controller::ReadKeyboard() {
  HRESULT result;

  // Read the keyboard device.
  result = m_keyboard->GetDeviceState(sizeof(m_keyboardState),
                                    (LPVOID)&m_keyboardState);
  if (FAILED(result)) {
    // If the keyboard lost focus or was not acquired then try to get control back.
    if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
      m_keyboard->Acquire();
    } else {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
bool Input_Controller::ReadMouse() {
  HRESULT result;

  // Read the mouse device.
  result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
  if (FAILED(result)) {
    // If the mouse lost focus or was not acquired then try to get control back.
    if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
      m_mouse->Acquire();
    }  else {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
void Input_Controller::ProcessInput() {
  // Update the location of the mouse cursor based on the change of
  // the mouse location during the frame.
  m_mouseX += m_mouseState.lX;
  m_mouseY += m_mouseState.lY;

  // Ensure the mouse location doesn't exceed the screen width or height.
  if (m_mouseX < 0)  { m_mouseX = 0; }
  if (m_mouseY < 0)  { m_mouseY = 0; }

  if (m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
  if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

  if (m_avatar_component != 0) {
    Avatar_Component::Avatar_State command;

    if (m_keyboardState[DIK_RIGHT] & 0x80)  {
      command.direction = "Right";
      command.state = "Running";
    }

    if (m_keyboardState[DIK_LEFT] & 0x80)  {
      command.direction = "Left";
      command.state = "Running";
    }

    if (m_keyboardState[DIK_DOWN] & 0x80)  {
      command.direction = "";
      command.state = "Down";
    }

    if (m_keyboardState[DIK_LSHIFT] & 0x80)  {
      command.state = "Looking";
    }

    if (m_keyboardState[DIK_LMENU] & 0x80)  {
      command.state = "Jumping";
    }

    #ifdef _DEBUG
    if (m_keyboardState[DIK_GRAVE] & 0x80)  {
      m_dik_grave_pressed = true;
    } else {
      if (m_dik_grave_pressed) {
        m_game_settings->SetIsDebugMode(!m_game_settings->IsDebugMode());
        m_dik_grave_pressed = false;
      }
    }
    #endif

    m_avatar_component->SetCommand(command);
  } else {
    Get_Avatar_Mutator mutator;
    m_model->Apply(&mutator);
    if (mutator.WasSuccessful()) {
      m_avatar_component = mutator.GetAvatarComponent();
    }
  }

  Input_Component::Key_Input key_input;

  if (m_keyboardState[DIK_SPACE] & 0x80)  {
    key_input.IsSpace = true;
  } else {
    key_input.IsSpace = false;
  }

  if (m_keyboardState[DIK_ESCAPE] & 0x80)  {
    key_input.IsEsc = true;
  } else {
    key_input.IsEsc = false;
  }

  if (m_keyboardState[DIK_RIGHT] & 0x80)  {
    key_input.IsRight = true;
  } else {
    key_input.IsRight = false;
  }

  if (m_keyboardState[DIK_LEFT] & 0x80)  {
    key_input.IsLeft = true;
  } else {
    key_input.IsLeft = false;
  }

  if (m_keyboardState[DIK_DOWN] & 0x80)  {
    key_input.IsDown = true;
  } else {
    key_input.IsDown = false;
  }

  if (m_keyboardState[DIK_UP] & 0x80)  {
    key_input.IsUp = true;
  } else {
    key_input.IsUp = false;
  }

  m_input_component->SetCurrentKeyInput(key_input);

  return;
}


//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
