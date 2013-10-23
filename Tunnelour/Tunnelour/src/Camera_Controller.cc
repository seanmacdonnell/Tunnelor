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
  m_avatar = 0;
  m_game_settings = 0;
  m_camera = 0;
}

//------------------------------------------------------------------------------
bool Camera_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  if (m_camera == 0) {
    m_camera = new Camera_Component();
    m_model->Add(m_camera);
    m_camera->Init();
  }
  Camera_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_avatar = mutator.GetAvatarComponent();
    m_game_settings = mutator.GetGameSettings();
    m_has_been_initialised = true;
  } else {
    return false;
  } 
  return true;
}

//------------------------------------------------------------------------------
bool Camera_Controller::Run() {
  Camera_Controller_Mutator mutator;
  if (!m_has_been_initialised) { return false; }
  if (m_game_settings->IsCameraFollowing()) {
    // Currently the camera is locked to the avatar.
    D3DXVECTOR3 avatar_position = m_avatar->GetPosition();
    D3DXVECTOR3 camera_position = m_camera->GetPosition();

    // Get the avatar collision block
    Avatar_Component::Collision_Block collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks);

    avatar_position.x = m_avatar->GetPosition().x + collision_block.offset_from_avatar_centre.x;
    avatar_position.y = m_avatar->GetPosition().y + collision_block.offset_from_avatar_centre.y;
    avatar_position.z = m_avatar->GetPosition().z;

    camera_position.x = avatar_position.x;
    camera_position.y = avatar_position.y - (collision_block.size.y / 2);

    m_camera->SetPosition(camera_position);
  }
  return true;
}

//---------------------------------------------------------------------------
Avatar_Component::Collision_Block Camera_Controller::GetNamedCollisionBlock(std::string id, std::list<Avatar_Component::Collision_Block> collision_blocks) {
  Avatar_Component::Collision_Block found_collision_block;

  Avatar_Component::Collision_Block* current_right_foot_collision_block = 0;

  std::list<Avatar_Component::Collision_Block>::iterator collision_block;
  for (collision_block = collision_blocks.begin(); collision_block != collision_blocks.end(); collision_block++) {
    if (collision_block->id.compare(id) == 0) {
      found_collision_block = (*collision_block);
    }
  }

  return found_collision_block;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // Tunnelour
