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

#include "Debug_Data_Display_Controller.h"
#include "Debug_Data_Display_Controller_Mutator.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Debug_Data_Display_Controller::Debug_Data_Display_Controller() : Controller() {
  m_game_settings = 0;
  m_game_metrics = 0;
  m_heading = 0;
  m_fps_display = 0;
  m_avatar_position_display = 0;
  m_fps = 0;
  m_is_debug_mode = false;
  m_avatar = 0;
  m_font_path = "";
  m_has_been_initialised = false;
}

//------------------------------------------------------------------------------
Debug_Data_Display_Controller::~Debug_Data_Display_Controller() {
  m_game_settings = 0;
  m_game_metrics = 0;
  m_heading = 0;
  m_fps_display = 0;
  m_avatar_position_display = 0;
  m_fps = 0;
  m_is_debug_mode = false;
  m_avatar = 0;
  m_has_been_initialised = false;
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  m_font_path = "resource\\tilesets\\Ariel.fnt";
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::Run() {
  if (!m_has_been_initialised) {
    Debug_Data_Display_Controller_Mutator mutator;
    m_model->Apply(&mutator);
    if (mutator.WasSuccessful())  {
      m_game_settings = mutator.GetGameSettings();
      m_camera = mutator.GetCamera();
      m_avatar = mutator.GetAvatarComponent();
      m_has_been_initialised = true;
      m_game_metrics = new Game_Metrics_Component();
      m_model->Add(m_game_metrics);
    }
  } else {
    if (m_heading == 0) {
      CreateDebugDataHeading();
    }
    if (m_fps_display == 0) {
      CreateFPSDisplay();
    }
    if (m_avatar_position_display == 0) {
      CreateAvatarPositionDisplay();
    }

    if (m_is_debug_mode !=  m_game_settings->IsDebugMode()) {
      if (!m_game_settings->IsDebugMode()) {
        m_heading->GetTexture()->transparency = 0.0f;
        m_fps_display->GetTexture()->transparency = 0.0f;
        m_avatar_position_display->GetTexture()->transparency = 0.0f;
        m_is_debug_mode = m_game_settings->IsDebugMode();
      } else {
        m_heading->GetTexture()->transparency = 1.0f;
        m_fps_display->GetTexture()->transparency = 1.0f;
        m_avatar_position_display->GetTexture()->transparency = 1.0f;
        m_is_debug_mode = m_game_settings->IsDebugMode();
      }
    }

    UpdateDebugDataHeading();
    UpdateFPSDisplay();
    UpdateAvatarPositionDisplay();
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateDebugDataHeading() {
  m_heading = new Text_Component();
  m_heading->GetText()->font_csv_file = m_font_path;
  m_heading->GetText()->text = "DEBUG MODE";
  m_heading->GetTexture()->transparency = 0.0f;
  m_model->Add(m_heading);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateDebugDataHeading() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;
  float debug_data_text_title_x = top_left_window_x +
                                  m_heading->GetSize().x / 2 +
                                  10;  // This is the offset from the top
  float debug_data_text_title_y = top_left_window_y -
                                  m_heading->GetSize().y / 2;
  m_heading->SetPosition(D3DXVECTOR3(debug_data_text_title_x,
                                                   debug_data_text_title_y,
                                                   -3));
  m_heading->Init();
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateFPSDisplay() {
  m_fps_display = new Text_Component();
  m_fps_display->GetText()->font_csv_file = m_font_path;
  m_fps_display->GetTexture()->transparency = 0.0f;
  m_model->Add(m_fps_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateFPSDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                          m_game_settings->GetResolution().y / 2;
  Game_Metrics_Component::FPS_Data fps_data = m_game_metrics->GetFPSData();
  std::string fps = std::to_string(static_cast<long double>(fps_data.fps));
  m_fps_display->GetText()->text = "fps: " + fps;
  m_fps_display->GetFrame()->index_buffer = 0;
  m_fps_display->GetTexture()->texture = 0;
  m_fps_display->GetFrame()->vertex_buffer = 0;
  float m_fps_display_x = top_left_window_x +
                            m_fps_display->GetSize().x / 2 +
                            10;  // This is the offset from the top
  float m_fps_display_y = top_left_window_y -
                            m_fps_display->GetSize().y / 2 -
                            m_heading->GetSize().y;
  m_fps_display->SetPosition(D3DXVECTOR3(m_fps_display_x,
                                          m_fps_display_y,
                                          -3.0));
  m_fps_display->Init();
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateAvatarPositionDisplay() {
  m_avatar_position_display = new Text_Component();
  m_avatar_position_display->GetText()->font_csv_file = m_font_path;
  m_avatar_position_display->GetTexture()->transparency = 0.0f;
  m_model->Add(m_avatar_position_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateAvatarPositionDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;
  std::string avatar_position_x = std::to_string(static_cast<long double>(m_avatar->GetPosition().x));
  std::string avatar_position_y = std::to_string(static_cast<long double>(m_avatar->GetPosition().y));
  m_avatar_position_display->GetText()->text = "x:" + avatar_position_x  +
                                                    ",y:" + avatar_position_y;
  m_avatar_position_display->GetFrame()->index_buffer = 0;
  m_avatar_position_display->GetTexture()->texture = 0;
  m_avatar_position_display->GetFrame()->vertex_buffer = 0;
  D3DXVECTOR3 m_avatar_position_display_position;
  m_avatar_position_display_position = m_avatar->GetPosition();
  m_avatar_position_display_position.z = -3;
  m_avatar_position_display->SetPosition(m_avatar_position_display_position);
  m_avatar_position_display->Init();
}
}  // Tunnelour
