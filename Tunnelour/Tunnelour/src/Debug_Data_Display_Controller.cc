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
#include "Background_Controller_Mutator.h"
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
  m_debug_position = 0;
  m_debug_last_position = 0;
  m_fps = 0;
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
  if (m_game_settings == 0 || m_camera == 0) {
    // Get game settings component from the model with the Mutator.
    Tunnelour::Background_Controller_Mutator mutator;

    m_model->Apply(&mutator);
    if (mutator.FoundGameSettings())  {
      m_game_settings = mutator.GetGameSettings();
    }
    if (mutator.FoundCamera())  {
      m_camera = mutator.GetCamera();
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
		  m_debug_data_text_title->Init();

		  m_model->Add(m_debug_data_text_title);
	  }
	  if (m_debug_data_fps == 0) {
		  m_debug_data_fps = new Tunnelour::Text_Component();
		  m_debug_data_fps->GetText()->font_csv_file = "resource\\tilesets\\Ariel.fnt";
		  m_model->Add(m_debug_data_fps);
	  }
	  if (m_debug_position == 0) {
		  m_debug_position = new Tunnelour::Text_Component();
		  m_debug_position->GetText()->font_csv_file = "resource\\tilesets\\Ariel.fnt";
		  m_model->Add(m_debug_position);
	  }
    if (m_debug_last_position == 0) {
		  m_debug_last_position = new Tunnelour::Text_Component();
		  m_debug_last_position->GetText()->font_csv_file = "resource\\tilesets\\Ariel.fnt";
		  m_model->Add(m_debug_last_position);
    }

	  m_debug_last_position->GetText()->text = "pos: x:" + std::to_string(static_cast<long double>(m_debug_data_text_title->GetPosition().x)) 
                                               + " y:" + std::to_string(static_cast<long double>(m_debug_data_text_title->GetPosition().y));

		m_debug_data_text_title->SetPosition(D3DXVECTOR3(static_cast<int>((m_camera->GetPosition().x + top_left_window_x) + (m_debug_data_text_title->GetSize()->x/2)),
														                         static_cast<int>((m_camera->GetPosition().y + top_left_window_y) - (m_debug_data_text_title->GetSize()->y/2)),
														                          -2));

	  Game_Metrics_Component::FPS_Data fps_data = m_game_metrics->GetFPSData();

	  m_debug_data_fps->GetText()->text = "fps: " + std::to_string(static_cast<long double>(fps_data.fps));
	  m_debug_data_fps->GetFrame()->index_buffer = 0;
	  m_debug_data_fps->GetTexture()->texture = 0;
	  m_debug_data_fps->GetFrame()->vertex_buffer = 0;
	  m_debug_data_fps->Init();
	  m_debug_data_fps->SetPosition(D3DXVECTOR3(static_cast<int>((m_camera->GetPosition().x + top_left_window_x) + (m_debug_data_fps->GetSize()->x/2)),
												                      static_cast<int>((m_camera->GetPosition().y + top_left_window_y) - (m_debug_data_fps->GetSize()->y/2) - (m_debug_data_text_title->GetSize()->y)),
												                      -2));

	  m_debug_position->GetText()->text = "pos: x:" + std::to_string(static_cast<long double>(m_debug_data_text_title->GetPosition().x)) 
                                          + " y:" + std::to_string(static_cast<long double>(m_debug_data_text_title->GetPosition().y));
	  m_debug_position->GetFrame()->index_buffer = 0;
	  m_debug_position->GetTexture()->texture = 0;
	  m_debug_position->GetFrame()->vertex_buffer = 0;
	  m_debug_position->Init();
	  m_debug_position->SetPosition(D3DXVECTOR3(static_cast<int>((m_camera->GetPosition().x + top_left_window_x) + (m_debug_position->GetSize()->x/2)),
												                      static_cast<int>((m_camera->GetPosition().y + top_left_window_y) - (m_debug_position->GetSize()->y/2) - (m_debug_data_text_title->GetSize()->y + m_debug_data_fps->GetSize()->y)),
												                      -2));



	  m_debug_last_position->GetFrame()->index_buffer = 0;
	  m_debug_last_position->GetTexture()->texture = 0;
	  m_debug_last_position->GetFrame()->vertex_buffer = 0;
	  m_debug_last_position->Init();
	  m_debug_last_position->SetPosition(D3DXVECTOR3(static_cast<int>((m_camera->GetPosition().x + top_left_window_x) + (m_debug_last_position->GetSize()->x/2)),
												                           static_cast<int>((m_camera->GetPosition().y + top_left_window_y) - (m_debug_last_position->GetSize()->y/2) - (m_debug_data_text_title->GetSize()->y + m_debug_data_fps->GetSize()->y +  m_debug_position->GetSize()->y)),
												                           -2));
    
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

} // Tunnelour