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

#include <stdexcept>

#include "Engine.h"
#include "Message_Wrapper.h"
#include "Exceptions.h"
#include "Tunnelour_View.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Engine::Engine() {
  m_model = NULL;
  m_controller = NULL;
  m_view = NULL;
}

//------------------------------------------------------------------------------
Engine::~Engine() {
  if (m_view != NULL) { delete m_view; }
  if (m_controller != NULL) { delete m_controller; }
  if (m_model != NULL) { delete m_model; }
}

//------------------------------------------------------------------------------
void Engine::Init(bool IncludeView, bool IncludeController) {
  m_model = new Tunnelour::Component_Composite();
  m_model->Init();

  if (IncludeView) {
    m_view = new Tunnelour::Tunnelour_View();
    m_view->Init(m_model);
  }

  if (IncludeController) {
    m_controller = new Tunnelour::Controller_Composite();
    m_controller->Init(m_model);
  }
}

//------------------------------------------------------------------------------
int Engine::Start() {
  if ((!IsControllerInit()) && (!IsViewInit())) {
    throw Tunnelour::Exceptions::uninitialised_error(
      "You need to initialise the View and/or the Controller!");
  }

  return Loop();
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
bool Engine::IsModelInit() {
  if (m_model == NULL) { return false; }
  return true;
}

//------------------------------------------------------------------------------
bool Engine::IsViewInit() {
  if (m_view == NULL) { return false; }
  return true;
}

//------------------------------------------------------------------------------
bool Engine::IsControllerInit() {
  if (m_controller == NULL) { return false; }
  return true;
}

//------------------------------------------------------------------------------
int Engine::Loop() {
  Tunnelour::Message_Wrapper message_handler;

  // MainLoop
  while (!message_handler.isWM_QUIT()) {
    if (IsControllerInit()) { m_controller->Run(); }
    if (IsViewInit()) { m_view->Run(); }
  }

  return message_handler.GetLastMessage().wParam;
}

}  // namespace Engine
