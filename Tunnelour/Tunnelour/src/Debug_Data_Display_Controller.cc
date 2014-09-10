//  Copyright 2014 Sean MacDonnell
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-1
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
#include "Bitmap_Helper.h"
#include "String_Helper.h"
#include <iomanip>      // std::setprecision

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
  m_avatar_state_display = 0;
  m_avatar_velocity_display = 0;
  m_avatar_jumping_distance_display = 0;
  m_avatar_jumping_height_display = 0;
  m_avatar_distance_traveled_display = 0;
  m_avatar_seconds_past_display = 0;
  m_fps = 0;
  m_is_debug_mode = false;
  m_avatar = 0;
  m_font_path = "";
  m_has_been_initialised = false;
  z_position = -3;
  m_bitmap_z_position = -4;
  m_jumping_distance = 0;
  m_jumping_height = 0;
}

//------------------------------------------------------------------------------
Debug_Data_Display_Controller::~Debug_Data_Display_Controller() {
  m_game_settings = 0;
  m_game_metrics = 0;
  m_heading = 0;
  m_fps_display = 0;
  m_avatar_position_display = 0;
  m_avatar_state_display = 0;
  m_avatar_velocity_display = 0;
  m_avatar_jumping_distance_display = 0;
  m_avatar_jumping_height_display = 0;
  m_avatar_distance_traveled_display = 0;
  m_avatar_seconds_past_display = 0;
  m_fps = 0;
  m_is_debug_mode = false;
  m_avatar = 0;
  m_has_been_initialised = false;
}

//------------------------------------------------------------------------------
bool Debug_Data_Display_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  m_font_path = "resource\\tilesets\\Ariel.fnt";
  Debug_Data_Display_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful())  {
    m_game_settings = mutator.GetGameSettings();
    m_camera = mutator.GetCamera();
    m_avatar = mutator.GetAvatarComponent();
    m_game_metrics = mutator.GetGameMetrics();
    m_has_been_initialised = true;

    // Create Collision Blocks For Avatar
    m_debug_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Debug_Tileset.txt");
    Tileset_Helper::LoadTilesetMetadataIntoStruct(m_debug_metadata_file_path, &m_debug_tileset_metadata);

  } else {
    return false;
  }
  m_has_been_initialised = true;
  return true;
}

//------------------------------------------------------------------------------
bool Debug_Data_Display_Controller::Run() {
  if (!m_has_been_initialised) {
    Init(m_model);
    return false;
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
    if (m_avatar_state_display == 0) {
      CreateAvatarStateDisplay();
    }
    if (m_avatar_velocity_display == 0) {
      CreateAvatarVelocityDisplay();
    }
    if (m_avatar_jumping_distance_display == 0) {
      CreateAvatarDistanceDisplay();
    }
    if (m_avatar_jumping_height_display == 0) {
      CreateAvatarHeightDisplay();
    }
    if (m_avatar_distance_traveled_display == 0) {
      CreateAvatarDistanceTraveledDisplay();
    }
    if (m_avatar_seconds_past_display == 0) {
      CreateAvatarSecondsPastDisplay();
    }


    if (!m_avatar->GetState().avatar_collision_blocks.empty()) {
      std::vector<Tile_Bitmap*>::iterator collision_bitmap;
      for (collision_bitmap = m_collision_bitmaps.begin(); collision_bitmap != m_collision_bitmaps.end(); collision_bitmap++) {
        m_model->Remove((*collision_bitmap));
      }
      m_collision_bitmaps.clear();

      std::vector<Avatar_Component::Avatar_Collision_Block>::iterator avatar_collision_block;
      std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks = m_avatar->GetState().avatar_collision_blocks;
      for (avatar_collision_block = avatar_collision_blocks.begin(); avatar_collision_block != avatar_collision_blocks.end(); avatar_collision_block++) {
        Tile_Bitmap *avatar_collision_block_bitmap = Bitmap_Helper::CollisionBlockToBitmapComponent((*avatar_collision_block), m_avatar, m_debug_tileset_metadata, m_game_settings->GetTilesetPath());
        D3DXVECTOR3 *position = avatar_collision_block_bitmap->GetPosition();
        position->z = m_bitmap_z_position;
        avatar_collision_block_bitmap->SetPosition(*position);
        m_collision_bitmaps.push_back(avatar_collision_block_bitmap);
        m_model->Add(avatar_collision_block_bitmap);
        if (m_is_debug_mode) {
          avatar_collision_block_bitmap->GetTexture()->transparency = 1.0f;
        }
      }
    }

    if (m_is_debug_mode !=  m_game_settings->IsDebugMode()) {
      if (!m_game_settings->IsDebugMode()) {
        m_heading->GetTexture()->transparency = 0.0f;
        m_fps_display->GetTexture()->transparency = 0.0f;
        m_avatar_position_display->GetTexture()->transparency = 0.0f;
        m_avatar_state_display->GetTexture()->transparency = 0.0f;
        m_avatar_velocity_display->GetTexture()->transparency = 0.0f;
        m_avatar_jumping_distance_display->GetTexture()->transparency = 0.0f;
        m_avatar_jumping_height_display->GetTexture()->transparency = 0.0f;
        m_avatar_distance_traveled_display->GetTexture()->transparency = 0.0f;
        m_avatar_seconds_past_display->GetTexture()->transparency = 0.0f;
        std::vector<Tile_Bitmap*>::iterator collision_bitmap;
        for (collision_bitmap = m_collision_bitmaps.begin(); collision_bitmap != m_collision_bitmaps.end(); collision_bitmap++) {
          (*collision_bitmap)->GetTexture()->transparency = 0.0f;
          (*collision_bitmap)->Init();
        }
        m_is_debug_mode = m_game_settings->IsDebugMode();
      } else {
        m_heading->GetTexture()->transparency = 1.0f;
        m_fps_display->GetTexture()->transparency = 1.0f;
        m_avatar_position_display->GetTexture()->transparency = 1.0f;
        m_avatar_state_display->GetTexture()->transparency = 1.0f;
        m_avatar_velocity_display->GetTexture()->transparency = 1.0f;
        m_avatar_jumping_distance_display->GetTexture()->transparency = 1.0f;
        m_avatar_jumping_height_display->GetTexture()->transparency = 1.0f;
        m_avatar_distance_traveled_display->GetTexture()->transparency = 1.0f;
        m_avatar_seconds_past_display->GetTexture()->transparency = 1.0f;
        std::vector<Tile_Bitmap*>::iterator collision_bitmap;
        for (collision_bitmap = m_collision_bitmaps.begin(); collision_bitmap != m_collision_bitmaps.end(); collision_bitmap++) {
          (*collision_bitmap)->GetTexture()->transparency = 1.0f;
          (*collision_bitmap)->Init();
        }
        m_is_debug_mode = m_game_settings->IsDebugMode();
      }
    }

    UpdateDebugDataHeading();
    UpdateFPSDisplay();
    UpdateAvatarPositionDisplay();
    UpdateAvatarStateDisplay();
    UpdateAvatarVelocityDisplay();
    UpdateAvatarDistanceDisplay();
    UpdateAvatarHeightDisplay();
    UpdateAvatarDistanceTraveledDisplay();
    UpdateAvatarSecondsPastDisplay();
  }
  return true;
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
  m_heading->SetPosition(D3DXVECTOR3(0, 0, z_position));
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
                                     z_position));
  
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateFPSDisplay() {
  m_fps_display = new Text_Component();
  m_fps_display->GetText()->font_csv_file = m_font_path;
  m_fps_display->GetTexture()->transparency = 0.0f;
  m_fps_display->SetPosition(D3DXVECTOR3(0, 0, z_position));
  m_model->Add(m_fps_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateFPSDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                          m_game_settings->GetResolution().y / 2;
  Game_Metrics_Component::FPS_Data fps_data = m_game_metrics->GetFPSData();
  std::string fps_text = "F.P.S: " + std::to_string(static_cast<long double>(fps_data.fps));
  if (fps_text.compare(m_fps_display->GetText()->text) != 0) { 
    m_fps_display->GetText()->text = fps_text;
    m_fps_display->GetFrame()->index_buffer = 0;
    m_fps_display->GetTexture()->texture = 0;
    m_fps_display->GetFrame()->vertex_buffer = 0;
    m_fps_display->Init();
  }
  float m_fps_display_x = top_left_window_x +
                          m_fps_display->GetSize().x / 2 +
                          10;  // This is the offset from the top
  float m_fps_display_y = top_left_window_y -
                          m_fps_display->GetSize().y / 2 -
                          m_heading->GetSize().y;
  m_fps_display->SetPosition(D3DXVECTOR3(m_fps_display_x,
                                         m_fps_display_y,
                                         z_position));
  
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateAvatarPositionDisplay() {
  m_avatar_position_display = new Text_Component();
  m_avatar_position_display->GetText()->font_csv_file = m_font_path;
  m_avatar_position_display->GetTexture()->transparency = 0.0f;
  m_avatar_position_display->SetPosition(D3DXVECTOR3(0, 0, z_position));
  m_model->Add(m_avatar_position_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateAvatarPositionDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;
  std::string avatar_position_x = std::to_string(static_cast<long double>(m_avatar->GetPosition()->x));
  std::string avatar_position_y = std::to_string(static_cast<long double>(m_avatar->GetPosition()->y));
  std::string position_text = "Avatar Pos: x:" + avatar_position_x  + ",y:" + avatar_position_y;
  if (position_text.compare(m_avatar_position_display->GetText()->text) != 0) {
    m_avatar_position_display->GetText()->text = position_text;
    m_avatar_position_display->GetFrame()->index_buffer = 0;
    m_avatar_position_display->GetTexture()->texture = 0;
    m_avatar_position_display->GetFrame()->vertex_buffer = 0;
    m_avatar_position_display->Init();
  }
  float m_avatar_display_x = top_left_window_x +
                             m_avatar_position_display->GetSize().x / 2 +
                             10;  // This is the offset from the top
  float m_avatar_display_y = m_fps_display->GetBottomRightPostion().y -
                             m_avatar_position_display->GetSize().y / 2;

  m_avatar_position_display->SetPosition(D3DXVECTOR3(m_avatar_display_x,
                                                     m_avatar_display_y,
                                                     z_position));
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateAvatarStateDisplay() {
  m_avatar_state_display = new Text_Component();
  m_avatar_state_display->GetText()->font_csv_file = m_font_path;
  m_avatar_state_display->GetTexture()->transparency = 0.0f;
  m_avatar_state_display->SetPosition(D3DXVECTOR3(0, 0, z_position));
  m_model->Add(m_avatar_state_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateAvatarStateDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;
  std::string avatar_state = m_avatar->GetState().parent_state;
  std::string avatar_subset = m_avatar->GetState().state;
  std::string avatar_subset_index = std::to_string(static_cast<long double>(m_avatar->GetState().state_index));
  std::string avatar_direction = m_avatar->GetState().direction;
  std::string state_text = "Avatar State: " + avatar_state  + "::" + avatar_subset + " " + avatar_subset_index + "(" + avatar_direction + ")";
  if (state_text.compare(m_avatar_state_display->GetText()->text) != 0) {
    m_avatar_state_display->GetText()->text = state_text;
    m_avatar_state_display->GetFrame()->index_buffer = 0;
    m_avatar_state_display->GetTexture()->texture = 0;
    m_avatar_state_display->GetFrame()->vertex_buffer = 0;
    m_avatar_state_display->Init();
  }
  float m_avatar_display_x = top_left_window_x +
                             m_avatar_state_display->GetSize().x / 2 +
                             10;  // This is the offset from the top
  float m_avatar_display_y = m_avatar_position_display->GetBottomRightPostion().y -
                             m_avatar_state_display->GetSize().y / 2;

  m_avatar_state_display->SetPosition(D3DXVECTOR3(m_avatar_display_x,
                                                     m_avatar_display_y,
                                                     z_position));
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateAvatarVelocityDisplay() {
  m_avatar_velocity_display = new Text_Component();
  m_avatar_velocity_display->GetText()->font_csv_file = m_font_path;
  m_avatar_velocity_display->GetTexture()->transparency = 0.0f;
  m_avatar_velocity_display->SetPosition(D3DXVECTOR3(0, 0, z_position));
  m_model->Add(m_avatar_velocity_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateAvatarVelocityDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;
  std::string avatar_velocity_x = std::to_string(static_cast<long double>(m_avatar->GetVelocity().x));
  std::string avatar_velocity_y = std::to_string(static_cast<long double>(m_avatar->GetVelocity().y));
  std::string velocity_text = "Avatar Velocity: x:" + avatar_velocity_x  + ",y:" + avatar_velocity_y;
  if (velocity_text.compare(m_avatar_velocity_display->GetText()->text) != 0) {
    m_avatar_velocity_display->GetText()->text = velocity_text;
    m_avatar_velocity_display->GetFrame()->index_buffer = 0;
    m_avatar_velocity_display->GetTexture()->texture = 0;
    m_avatar_velocity_display->GetFrame()->vertex_buffer = 0;
    m_avatar_velocity_display->Init();
  }

  float m_avatar_display_x = top_left_window_x +
                             m_avatar_velocity_display->GetSize().x / 2 +
                             10;  // This is the offset from the top
  float m_avatar_display_y = m_avatar_state_display->GetBottomRightPostion().y -
                             m_avatar_velocity_display->GetSize().y / 2;

  m_avatar_velocity_display->SetPosition(D3DXVECTOR3(m_avatar_display_x,
                                                     m_avatar_display_y,
                                                     z_position));
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateAvatarDistanceDisplay() {
  m_avatar_jumping_distance_display = new Text_Component();
  m_avatar_jumping_distance_display->GetText()->font_csv_file = m_font_path;
  m_avatar_jumping_distance_display->GetTexture()->transparency = 0.0f;
  m_avatar_jumping_distance_display->SetPosition(D3DXVECTOR3(0, 0, z_position));
  m_model->Add(m_avatar_jumping_distance_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateAvatarDistanceDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;

  std::string current_parent_state = m_avatar->GetState().parent_state;
  if (current_parent_state.compare("Charlie_Jumping") == 0) {
    std::string last_parent_state = m_avatar->GetLastRenderedState().parent_state;
    if (last_parent_state.compare("Charlie_Jumping") != 0) {
      m_jumping_distance = 0;
    }
    float distance_moved = m_avatar->GetPosition()->x - m_avatar->GetLastRenderedPosition().x;
    m_jumping_distance += distance_moved;
  }

  std::string distance_string = std::to_string(static_cast<long double>(m_jumping_distance));
  std::string distance_text = "Avatar Jump Distance: " + distance_string;
  if (distance_text.compare(m_avatar_jumping_distance_display->GetText()->text) != 0) {
    m_avatar_jumping_distance_display->GetText()->text = distance_text;
    m_avatar_jumping_distance_display->GetFrame()->index_buffer = 0;
    m_avatar_jumping_distance_display->GetTexture()->texture = 0;
    m_avatar_jumping_distance_display->GetFrame()->vertex_buffer = 0;
    m_avatar_jumping_distance_display->Init();
  }

  float m_avatar_display_x = top_left_window_x +
                             m_avatar_jumping_distance_display->GetSize().x / 2 +
                             10;  // This is the offset from the top
  float m_avatar_display_y = m_avatar_velocity_display->GetBottomRightPostion().y -
                             m_avatar_jumping_distance_display->GetSize().y / 2;

  m_avatar_jumping_distance_display->SetPosition(D3DXVECTOR3(m_avatar_display_x,
                                                     m_avatar_display_y,
                                                     z_position));
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateAvatarHeightDisplay() {
  m_avatar_jumping_height_display = new Text_Component();
  m_avatar_jumping_height_display->GetText()->font_csv_file = m_font_path;
  m_avatar_jumping_height_display->GetTexture()->transparency = 0.0f;
  m_avatar_jumping_height_display->SetPosition(D3DXVECTOR3(0, 0, z_position));
  m_model->Add(m_avatar_jumping_height_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateAvatarHeightDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;

  if (m_avatar->GetState().parent_state.compare("Charlie_Jumping") == 0) {
    if (m_avatar->GetLastRenderedState().parent_state.compare("Charlie_Jumping") != 0) {
      m_jumping_height = 0;
    }
    float current_y = m_avatar->GetPosition()->y;
    float last_y = m_avatar->GetLastRenderedPosition().y;
    float distance_moved = current_y - last_y;
    if (distance_moved > 0) {
      m_jumping_height += distance_moved;
    }
  }

  std::string distance_string = std::to_string(static_cast<long double>(m_jumping_height));
  std::string height_text = "Avatar Jump Height: " + distance_string;
  if (height_text.compare(m_avatar_jumping_height_display->GetText()->text) != 0) {
    m_avatar_jumping_height_display->GetText()->text = height_text;
    m_avatar_jumping_height_display->GetFrame()->index_buffer = 0;
    m_avatar_jumping_height_display->GetTexture()->texture = 0;
    m_avatar_jumping_height_display->GetFrame()->vertex_buffer = 0;
    m_avatar_jumping_height_display->Init();
  }

  float m_avatar_display_x = top_left_window_x +
                             m_avatar_jumping_height_display->GetSize().x / 2 +
                             10;  // This is the offset from the top
  float m_avatar_display_y = m_avatar_jumping_distance_display->GetBottomRightPostion().y -
                             m_avatar_jumping_height_display->GetSize().y / 2;

  m_avatar_jumping_height_display->SetPosition(D3DXVECTOR3(m_avatar_display_x,
                                                     m_avatar_display_y,
                                                     z_position));
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateAvatarDistanceTraveledDisplay() {
  m_avatar_distance_traveled_display = new Text_Component();
  m_avatar_distance_traveled_display->GetText()->font_csv_file = m_font_path;
  m_avatar_distance_traveled_display->GetTexture()->transparency = 0.0f;
  m_avatar_distance_traveled_display->SetPosition(D3DXVECTOR3(0, 0, z_position));
  m_model->Add(m_avatar_distance_traveled_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateAvatarDistanceTraveledDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;

  std::ostringstream distance;
  distance << std::setprecision(2);
  distance << std::fixed;
  distance << m_game_metrics->GetDistanceTraveled();

  std::string travelled_text = "Distance Traveled: " + distance.str() + " meters";
  if (travelled_text.compare(m_avatar_distance_traveled_display->GetText()->text) != 0) {
    m_avatar_distance_traveled_display->GetText()->text = travelled_text;
    m_avatar_distance_traveled_display->GetFrame()->index_buffer = 0;
    m_avatar_distance_traveled_display->GetTexture()->texture = 0;
    m_avatar_distance_traveled_display->GetFrame()->vertex_buffer = 0;
    m_avatar_distance_traveled_display->Init();
  }

  float m_avatar_display_x = top_left_window_x +
                             m_avatar_distance_traveled_display->GetSize().x / 2 +
                             10;  // This is the offset from the top
  float m_avatar_display_y = m_avatar_jumping_height_display->GetBottomRightPostion().y -
                             m_avatar_distance_traveled_display->GetSize().y / 2;

  m_avatar_distance_traveled_display->SetPosition(D3DXVECTOR3(m_avatar_display_x,
                                                     m_avatar_display_y,
                                                     z_position));
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::CreateAvatarSecondsPastDisplay() {
  m_avatar_seconds_past_display = new Text_Component();
  m_avatar_seconds_past_display->GetText()->font_csv_file = m_font_path;
  m_avatar_seconds_past_display->GetTexture()->transparency = 0.0f;
  m_avatar_seconds_past_display->SetPosition(D3DXVECTOR3(0, 0, z_position));
  m_model->Add(m_avatar_seconds_past_display);
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller::UpdateAvatarSecondsPastDisplay() {
  float top_left_window_x = m_camera->GetPosition().x -
                            m_game_settings->GetResolution().x / 2;
  float top_left_window_y = m_camera->GetPosition().y +
                            m_game_settings->GetResolution().y / 2;


  std::ostringstream seconds;
  seconds << std::setprecision(2) << std::fixed << m_game_metrics->GetSecondsPast();
  std::string seconds_text = "Seconds Past: " + seconds.str();
  if (seconds_text.compare(m_avatar_seconds_past_display->GetText()->text) != 0) {
    m_avatar_seconds_past_display->GetText()->text = seconds_text;
    m_avatar_seconds_past_display->GetFrame()->index_buffer = 0;
    m_avatar_seconds_past_display->GetTexture()->texture = 0;
    m_avatar_seconds_past_display->GetFrame()->vertex_buffer = 0;
    m_avatar_seconds_past_display->Init();
  }
  float m_avatar_display_x = top_left_window_x +
                             m_avatar_seconds_past_display->GetSize().x / 2 +
                             10;  // This is the offset from the top
  float m_avatar_display_y = m_avatar_distance_traveled_display->GetBottomRightPostion().y -
                             m_avatar_seconds_past_display->GetSize().y / 2;

  m_avatar_seconds_past_display->SetPosition(D3DXVECTOR3(m_avatar_display_x,
                                                     m_avatar_display_y,
                                                     z_position));
}

}  // Tunnelour
