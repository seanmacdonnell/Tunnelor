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
#include <string>



namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Debug_Data_Display_Controller::Debug_Data_Display_Controller() : Controller() {
  m_game_settings = 0;
  m_game_metrics = 0;
  m_debug_data_text_title = 0;
  m_debug_data_fps = 0;
  m_debug_avatar_position = 0;
  m_fps = 0;
  m_is_debug_mode = false;
  m_avatar = 0;
}

//------------------------------------------------------------------------------
Debug_Data_Display_Controller::~Debug_Data_Display_Controller() {
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::Init(Tunnelour::Component_Composite * const model) {
    Tunnelour::Controller::Init(model);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::Run() {
  if (m_game_settings == 0 || m_camera == 0 || m_avatar == 0) {
    // Get game settings component from the model with the Mutator.
    Tunnelour::Debug_Data_Display_Controller_Mutator mutator;

    m_model->Apply(&mutator);
    if (mutator.FoundGameSettings())  {
      m_game_settings = mutator.GetGameSettings();
    }
    if (mutator.FoundCamera())  {
      m_camera = mutator.GetCamera();
    }
    if (mutator.FoundAvatarComponent())  {
      m_avatar = mutator.GetAvatarComponent();
    }
  } else {
	  if (m_game_metrics == 0) {
		  m_game_metrics = new Tunnelour::Game_Metrics_Component();
		  m_model->Add(m_game_metrics);
	  }
	  int top_left_window_x = static_cast<int>((m_game_settings->GetResolution().x/2) * -1);
	  int top_left_window_y = static_cast<int>(m_game_settings->GetResolution().y/2);
	  if (m_debug_data_text_title == 0) {
      m_debug_data_text_title = new Tunnelour::Text_Component();
		  m_debug_data_text_title->GetText()->font_csv_file = "resource\\tilesets\\Ariel.fnt";
		  m_debug_data_text_title->GetText()->text = "~Debug data";
      m_debug_data_text_title->GetTexture()->transparency = 0.0f;
      m_model->Add(m_debug_data_text_title);
	  }
	  if (m_debug_data_fps == 0) {
		  m_debug_data_fps = new Tunnelour::Text_Component();
		  m_debug_data_fps->GetText()->font_csv_file = "resource\\tilesets\\Ariel.fnt";
      m_debug_data_fps->GetTexture()->transparency = 0.0f;
      m_model->Add(m_debug_data_fps);
	  }
	  if (m_debug_avatar_position == 0) {
		  m_debug_avatar_position = new Tunnelour::Text_Component();
		  m_debug_avatar_position->GetText()->font_csv_file = "resource\\tilesets\\Ariel.fnt";
      m_debug_avatar_position->GetTexture()->transparency = 0.0f;
      m_model->Add(m_debug_avatar_position);
	  }

		m_debug_data_text_title->SetPosition(D3DXVECTOR3(static_cast<int>((m_camera->GetPosition().x + top_left_window_x) + (m_debug_data_text_title->GetSize()->x/2) + 10),
														                         static_cast<int>((m_camera->GetPosition().y + top_left_window_y) - (m_debug_data_text_title->GetSize()->y/2)),
														                          -2));

	  Game_Metrics_Component::FPS_Data fps_data = m_game_metrics->GetFPSData();

	  m_debug_data_fps->GetText()->text = "FPS: " + std::to_string(static_cast<long double>(fps_data.fps));
	  m_debug_data_fps->GetFrame()->index_buffer = 0;
	  m_debug_data_fps->GetTexture()->texture = 0;
	  m_debug_data_fps->GetFrame()->vertex_buffer = 0;
	  
	  m_debug_data_fps->SetPosition(D3DXVECTOR3(static_cast<int>((m_camera->GetPosition().x + top_left_window_x) + (m_debug_data_fps->GetSize()->x/2) + 10),
												                      static_cast<int>((m_camera->GetPosition().y + top_left_window_y) - (m_debug_data_fps->GetSize()->y/2) - (m_debug_data_text_title->GetSize()->y)),
												                      -2));

	  m_debug_avatar_position->GetText()->text = "x:" + std::to_string(static_cast<long double>(m_avatar->GetPosition().x)) 
                                             + ",y:" + std::to_string(static_cast<long double>(m_avatar->GetPosition().y));
	  m_debug_avatar_position->GetFrame()->index_buffer = 0;
	  m_debug_avatar_position->GetTexture()->texture = 0;
	  m_debug_avatar_position->GetFrame()->vertex_buffer = 0;


	  m_debug_avatar_position->SetPosition(D3DXVECTOR3(static_cast<int>(m_avatar->GetPosition().x),
												                             static_cast<int>(m_avatar->GetPosition().y),
												                             -3));

    if (m_is_debug_mode !=  m_game_settings->IsDebugMode()) {
      if (!m_game_settings->IsDebugMode()) {
        m_debug_data_text_title->GetTexture()->transparency = 0.0f;
        m_debug_data_fps->GetTexture()->transparency = 0.0f;
        m_debug_avatar_position->GetTexture()->transparency = 0.0f;
        m_is_debug_mode = m_game_settings->IsDebugMode();
      } else {
        m_debug_data_text_title->GetTexture()->transparency = 1.0f;
        m_debug_data_fps->GetTexture()->transparency = 1.0f;
        m_debug_avatar_position->GetTexture()->transparency = 1.0f;
        m_is_debug_mode = m_game_settings->IsDebugMode();
      }
    }

    m_debug_data_text_title->Init();
    m_debug_data_fps->Init();
    m_debug_avatar_position->Init();
  }


}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

} // Tunnelour