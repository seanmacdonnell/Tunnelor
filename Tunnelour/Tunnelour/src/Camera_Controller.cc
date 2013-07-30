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
}

//------------------------------------------------------------------------------
Camera_Controller::~Camera_Controller() {
}

//------------------------------------------------------------------------------
void Camera_Controller::Init(Tunnelour::Component_Composite * const model) {
  Tunnelour::Controller::Init(model);

  Tunnelour::Component * camera = 0;
  camera = m_model->Add(new Tunnelour::Camera_Component());
  camera->Init();
  m_camera = dynamic_cast<Tunnelour::Camera_Component*>(camera);
}

//------------------------------------------------------------------------------
void Camera_Controller::Run() {
  if (!m_mutator.FoundAvatarComponent() || !m_mutator.FoundGameSettings()) {
    m_model->Apply(&m_mutator);
    if (m_mutator.FoundAvatarComponent()) {
      m_avatar = m_mutator.GetAvatarComponent();
    } else {
      return;
    }
    if (m_mutator.FoundGameSettings()) {
      m_game_settings = m_mutator.GetGameSettings();
    } else {
      return;
    }
  }

  if (m_game_settings->IsCameraFollowing()) {
    D3DXVECTOR3 avatar_position = m_avatar->GetPosition();
    D3DXVECTOR3 camera_position = m_camera->GetPosition();
    camera_position.x = avatar_position.x;
    camera_position.y = avatar_position.y;

    m_camera->SetPosition(camera_position);
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

} // Tunnelour