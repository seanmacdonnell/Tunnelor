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

#include "Input_Controller.h"
#include "Camera_Component.h"
#include "Game_Settings_Component.h"
#include "Input_Controller_Mutator.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Input_Controller::Input_Controller() : Controller() {
  m_game_settings = 0;
  m_avatar_component = 0;
  m_has_direct_input_been_init = false;
  m_dik_grave_pressed = false;
}

//------------------------------------------------------------------------------
Input_Controller::~Input_Controller() {
  // Release the mouse.
  if(m_mouse) {
    m_mouse->Unacquire();
    m_mouse->Release();
    m_mouse = 0;
  }

  // Release the keyboard.
  if(m_keyboard) {
    m_keyboard->Unacquire();
    m_keyboard->Release();
    m_keyboard = 0;
  }

  // Release the main interface to direct input.
  if(m_directInput) {
    m_directInput->Release();
    m_directInput = 0;
  }

}

//------------------------------------------------------------------------------
void Input_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);

  // Initialize the location of the mouse on the screen.
  m_mouseX = 0;
  m_mouseY = 0;

  Input_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.FoundGameSettings()) {
    m_game_settings = mutator.GetGameSettings();
  }

  if (mutator.FoundAvatarComponent()) {
    m_avatar_component = mutator.GetAvatarComponent();
  }

  if (!m_has_direct_input_been_init) {
    Init_DirectInput();
  }
}

//------------------------------------------------------------------------------
void Input_Controller::Init_DirectInput() {
  if (m_game_settings != 0) {
    if (m_game_settings->GetHInstance() != NULL && m_game_settings->GetHWnd() != NULL) {
      // Store the screen size which will be used for positioning the mouse cursor.
      m_screenWidth = static_cast<int>(m_game_settings->GetResolution().x);
      m_screenHeight = static_cast<int>(m_game_settings->GetResolution().y);

      // Initialize the main direct input interface.
      if (FAILED(DirectInput8Create(m_game_settings->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL)))   {
        throw Exceptions::run_error("Input_Controller DirectInput8Create Failed!");
      }

      // Initialize the direct input interface for the keyboard.

      if(FAILED(m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL))) {
        throw Exceptions::run_error("Input_Controller CreateDevice Keyboard Failed!");
      }

      // Set the data format.  In this case since it is a keyboard we can use the predefined data format.
      if(FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard))) {
        throw Exceptions::run_error("Input_Controller SetDataFormat Failed!");
      }

      // Set the cooperative level of the keyboard to not share with other programs.
      if(FAILED(m_keyboard->SetCooperativeLevel(m_game_settings->GetHWnd(), DISCL_FOREGROUND | DISCL_EXCLUSIVE))) {
        throw Exceptions::run_error("Input_Controller SetCooperativeLevel Failed!");
      }

      // Now acquire the keyboard.
      if(FAILED(m_keyboard->Acquire())) {
        throw Exceptions::run_error("Input_Controller Acquire Failed!");
      }

      // Initialize the direct input interface for the mouse.
      if(FAILED(m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL))) {
        throw Exceptions::run_error("Input_Controller CreateDevice Mouse Failed!");
      }

      // Set the data format for the mouse using the pre-defined mouse data format.
      if(FAILED(m_mouse->SetDataFormat(&c_dfDIMouse))) {
        throw Exceptions::run_error("Input_Controller CreateDevice Mouse Failed!");
      }

      // Set the cooperative level of the mouse to share with other programs.

      if(FAILED(m_mouse->SetCooperativeLevel(m_game_settings->GetHWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
        throw Exceptions::run_error("Input_Controller SetCooperativeLevel Failed!");
      }

      // Acquire the mouse.
      if(FAILED(m_mouse->Acquire())) {
        throw Exceptions::run_error("Input_Controller Acquire Failed!");
      }

      m_has_direct_input_been_init = true;
    }
  }
}

//------------------------------------------------------------------------------
void Input_Controller::Run() {
  if (m_game_settings == 0 || m_avatar_component == 0) {
    Input_Controller_Mutator mutator;
    m_model->Apply(&mutator);
    if (mutator.FoundGameSettings()) {
      m_game_settings = mutator.GetGameSettings();
    }
    if (mutator.FoundAvatarComponent()) {
      m_avatar_component = mutator.GetAvatarComponent();
    }
  }

  if (!m_has_direct_input_been_init) {
    Init_DirectInput();
  }

  if (m_has_direct_input_been_init) {
    // Read the current state of the keyboard.
    if(!ReadKeyboard())	{
      throw Exceptions::run_error("Input_Controller ReadKeyboard Failed!");
    }

    // Read the current state of the mouse.
    if(!ReadMouse()){
      throw Exceptions::run_error("Input_Controller ReadMouse Failed!");
    }

    // Process the changes in the mouse and keyboard.
    ProcessInput();
  }
}

//------------------------------------------------------------------------------
bool Input_Controller::ReadKeyboard() {
	HRESULT result;

	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
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
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
void Input_Controller::ProcessInput() {
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if(m_mouseX < 0)  { m_mouseX = 0; }
	if(m_mouseY < 0)  { m_mouseY = 0; }
	
	if(m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
	
  Avatar_Component::Avatar_State command;

	if(m_keyboardState[DIK_RIGHT] & 0x80)	{
    command.direction = "Right";
    command.state = "Walking";
    if(m_keyboardState[DIK_LSHIFT] & 0x80)	{
      command.state = "Running";
    }
	}

  if(m_keyboardState[DIK_LEFT] & 0x80)	{
    command.direction = "Left";
    command.state = "Walking";
    if(m_keyboardState[DIK_LSHIFT] & 0x80)	{
      command.state = "Running";
    }
	}

  if(m_keyboardState[DIK_GRAVE] & 0x80)	{
    m_dik_grave_pressed = true;
	} else {
   if (m_dik_grave_pressed) {
     m_game_settings->SetIsDebugMode(!m_game_settings->IsDebugMode());
     m_dik_grave_pressed = false;
   }
  }

  if (m_avatar_component != 0) {
    m_avatar_component->SetCommand(command);
  }

	return;
}


//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
