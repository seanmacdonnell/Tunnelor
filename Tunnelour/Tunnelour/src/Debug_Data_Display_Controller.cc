//  Copyright 2012 Sean MacDonnell
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
  z_position = -3;
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
bool Debug_Data_Display_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  m_font_path = "resource\\tilesets\\Ariel.fnt";
  Debug_Data_Display_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful())  {
    m_game_settings = mutator.GetGameSettings();
    m_camera = mutator.GetCamera();
    m_avatar = mutator.GetAvatarComponent();
    m_has_been_initialised = true;
    m_game_metrics = new Game_Metrics_Component();
    m_model->Add(m_game_metrics);
    // Create Collision Blocks For Avatar
    
    m_debug_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Debug_Tileset_0_5.txt");
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
        std::vector<Tile_Bitmap*>::iterator collision_bitmap;
        for (collision_bitmap = m_collision_bitmaps.begin(); collision_bitmap != m_collision_bitmaps.end(); collision_bitmap++) {
          (*collision_bitmap)->GetTexture()->transparency = 0.0f;
          (*collision_bitmap)->Init();
        }
        //m_avatar->GetTexture()->transparency = 0.0f;
        m_is_debug_mode = m_game_settings->IsDebugMode();
      } else {
        m_heading->GetTexture()->transparency = 1.0f;
        m_fps_display->GetTexture()->transparency = 1.0f;
        m_avatar_position_display->GetTexture()->transparency = 1.0f;
        std::vector<Tile_Bitmap*>::iterator collision_bitmap;
        for (collision_bitmap = m_collision_bitmaps.begin(); collision_bitmap != m_collision_bitmaps.end(); collision_bitmap++) {
          (*collision_bitmap)->GetTexture()->transparency = 1.0f;
          (*collision_bitmap)->Init();
        }
        //m_avatar->GetTexture()->transparency = 1.0f;
        m_is_debug_mode = m_game_settings->IsDebugMode();
      }
    }

    UpdateDebugDataHeading();
    UpdateFPSDisplay();
    UpdateAvatarPositionDisplay();
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
  m_fps_display->GetText()->text = "F.P.S: " + fps;
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
                                         z_position));
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
  std::string avatar_position_x = std::to_string(static_cast<long double>(m_avatar->GetPosition()->x));
  std::string avatar_position_y = std::to_string(static_cast<long double>(m_avatar->GetPosition()->y));
  m_avatar_position_display->GetText()->text = "Avatar Pos: x:" + avatar_position_x  +
                                                          ",y:" + avatar_position_y;
  m_avatar_position_display->GetFrame()->index_buffer = 0;
  m_avatar_position_display->GetTexture()->texture = 0;
  m_avatar_position_display->GetFrame()->vertex_buffer = 0;

  float m_avatar_display_x = top_left_window_x +
                             m_avatar_position_display->GetSize().x / 2 +
                             10;  // This is the offset from the top
  float m_avatar_display_y = m_fps_display->GetBottomRightPostion().y -
                             m_avatar_position_display->GetSize().y / 2;

  m_avatar_position_display->SetPosition(D3DXVECTOR3(m_avatar_display_x,
                                                     m_avatar_display_y,
                                                     z_position));

  m_avatar_position_display->Init();
}
}  // Tunnelour
