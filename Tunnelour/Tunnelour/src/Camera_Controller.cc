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

#include "Camera_Controller.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Camera_Controller::Camera_Controller() : Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_camera = 0;
  has_been_initialised = false;
}

//------------------------------------------------------------------------------
Camera_Controller::~Camera_Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_camera = 0;
  has_been_initialised = false;
}

//------------------------------------------------------------------------------
void Camera_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);

  m_camera = new Camera_Component();
  m_model->Add(m_camera);
  m_camera->Init();
}

//------------------------------------------------------------------------------
void Camera_Controller::Run() {
  Camera_Controller_Mutator mutator;
  if (!has_been_initialised) {
    m_model->Apply(&mutator);
    if (mutator.WasSuccessful()) {
      m_avatar = mutator.GetAvatarComponent();
      m_game_settings = mutator.GetGameSettings();
      has_been_initialised = true;
    }
  } else {
    if (m_game_settings->IsCameraFollowing()) {
      // Currently the camera is locked to the avatar.
      D3DXVECTOR3 avatar_position = m_avatar->GetPosition();
      D3DXVECTOR3 camera_position = m_camera->GetPosition();
      camera_position.x = avatar_position.x;
      camera_position.y = avatar_position.y;

      m_camera->SetPosition(camera_position);
    }
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // Tunnelour
