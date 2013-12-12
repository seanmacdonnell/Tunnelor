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

#include "Introduction_Controller.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include "Exceptions.h"
#include "String_Helper.h"
#include "Bitmap_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Introduction_Controller::Introduction_Controller() : Controller() {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_black_metadata_file_path = "";
  m_white_metadata_file_path = "";
  m_background = 0;
  m_game_name_heading = 0;
  m_author = 0;
  m_version = 0;
  m_loading = 0;
  m_z_bitmap_position = -4;
  m_z_text_position = -5;
  m_animation_tick = false;
}

//------------------------------------------------------------------------------
Introduction_Controller::~Introduction_Controller() {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_black_metadata_file_path = "";
  m_white_metadata_file_path = "";
}

//------------------------------------------------------------------------------
bool Introduction_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));

  Introduction_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_game_settings = mutator.GetGameSettings();
    m_camera = mutator.GetCamera();
    m_level = mutator.GetLevel();
    LoadTilesetMetadata();
    m_current_tileset = GetNamedTileset("Black");
    m_current_tileset_subset = GetCurrentForegroundSubset();
    m_has_been_initialised = true;
    m_heading_font_path = "resource\\tilesets\\DestroyEarthRoughBB.fnt";
    m_text_font_path = "resource\\tilesets\\Christiana.fnt";
    InitTimer();
  } else {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Introduction_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }

  if (m_background == 0) {
    // Create the Spash Black Tile
    m_background = CreateTile(128);
    m_background->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y, m_z_bitmap_position);
    m_background->SetScale(new D3DXVECTOR3((m_game_settings->GetResolution().x/128), (m_game_settings->GetResolution().y/128), 1.0f));
    m_model->Add(m_background);
  } else {
    m_background->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y, m_z_bitmap_position);
  }

  if (m_game_name_heading == 0) {
    m_game_name_heading = new Text_Component();
    m_game_name_heading->GetText()->font_csv_file = m_heading_font_path;
    m_game_name_heading->GetText()->text = "TUNNELOR";
    m_game_name_heading->SetScale(new D3DXVECTOR3(1.0f, 1.0f, 1.0f));
    m_game_name_heading->GetTexture()->transparency = 1.0f;
    m_game_name_heading->SetPosition(m_camera->GetPosition().x,
                                     m_camera->GetPosition().y + 200,
                                     m_z_text_position);
    m_game_name_heading->GetFont()->font_color = D3DXCOLOR(0.37, 0.15, 0.02, 1);
    m_game_name_heading->Init();
    m_model->Add(m_game_name_heading);
  } else {
    m_game_name_heading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + 200, m_z_text_position);
  }

  if (m_author == 0) {
    m_author = new Text_Component();
    m_author->GetText()->font_csv_file = m_text_font_path;
    m_author->GetText()->text = "Author: Sean.MacDonnell@Gmail.com";
    m_author->SetScale(new D3DXVECTOR3(1.0f, 1.0f, 1.0f));
    m_author->GetTexture()->transparency = 1.0f;
    m_author->Init();
    float position_y = m_game_name_heading->GetPosition()->y - (m_game_name_heading->GetSize().y / 2);
    position_y -= (m_author->GetSize().y /2);
    position_y -= 50;
    m_author->SetPosition(m_camera->GetPosition().x,
                          position_y,
                          m_z_text_position);

    m_model->Add(m_author);
  } else {
    float position_y = m_game_name_heading->GetPosition()->y - (m_game_name_heading->GetSize().y / 2);
    position_y -= (m_author->GetSize().y /2);
    position_y -= 50;
    m_author->SetPosition(m_camera->GetPosition().x, position_y, m_z_text_position);
  }

  if (m_version == 0) {
    m_version = new Text_Component();
    m_version->GetText()->font_csv_file = m_text_font_path;
    m_version->GetText()->text = "Version: ALPHA";
    m_version->SetScale(new D3DXVECTOR3(1.0f, 1.0f, 1.0f));
    m_version->GetTexture()->transparency = 1.0f;
    m_version->Init();
    float position_y = m_author->GetPosition()->y - (m_author->GetSize().y / 2);
    position_y -= (m_version->GetSize().y /2);
    position_y -= 50;
    m_version->SetPosition(m_camera->GetPosition().x,
                          position_y,
                          m_z_text_position);
    m_model->Add(m_version);
  } else {
    float position_y = m_author->GetPosition()->y - (m_author->GetSize().y / 2);
    position_y -= (m_version->GetSize().y /2);
    position_y -= 50;
    m_version->SetPosition(m_camera->GetPosition().x, position_y, m_z_text_position);
  }

  if (m_loading == 0) {
    m_loading = new Text_Component();
    m_loading->GetText()->font_csv_file = m_text_font_path;
    m_loading->GetText()->text = "Loading!";
    m_loading->SetScale(new D3DXVECTOR3(1.0f, 1.0f, 1.0f));
    m_loading->GetTexture()->transparency = 1.0f;
    m_loading->Init();
    float position_y = m_version->GetPosition()->y - (m_version->GetSize().y / 2);
    position_y -= (m_loading->GetSize().y /2);
    position_y -= 50;
    m_loading->SetPosition(m_camera->GetPosition().x,
                           position_y,
                           m_z_text_position);

    m_model->Add(m_loading);
  } else {
    UpdateTimer();
    if (m_animation_tick) {
      if (m_loading->GetTexture()->transparency == 1.0f) {
        m_loading->GetTexture()->transparency = 0.0f;
      } else {
        m_loading->GetTexture()->transparency = 1.0f;
      }
      float position_y = m_version->GetPosition()->y - (m_version->GetSize().y / 2);
      position_y -= (m_version->GetSize().y /2);
      position_y -= 50;
      m_loading->SetPosition(m_camera->GetPosition().x, position_y, m_z_text_position);
      m_animation_tick = false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
void Introduction_Controller::LoadTilesetMetadata() {
  Tileset_Helper::Tileset_Metadata debug_tileset_metadata;
  m_black_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Black_Tileset_0_0.txt");
  Tileset_Helper::LoadTilesetMetadataIntoStruct(m_black_metadata_file_path, &debug_tileset_metadata);
  m_tilesets.push_back(debug_tileset_metadata);

  Tileset_Helper::Tileset_Metadata dirt_tileset_metadata;
  m_white_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"White_Tileset_0_0.txt");
  Tileset_Helper::LoadTilesetMetadataIntoStruct(m_white_metadata_file_path, &dirt_tileset_metadata);
  m_tilesets.push_back(dirt_tileset_metadata);

  m_current_tileset = GetNamedTileset("White");
  m_current_tileset_subset = GetCurrentForegroundSubset();
}

//---------------------------------------------------------------------------
Tileset_Helper::Tileset_Metadata Introduction_Controller::GetNamedTileset(std::string name) {
  Tileset_Helper::Tileset_Metadata found_tileset_metadata;

  std::vector<Tileset_Helper::Tileset_Metadata>::iterator tileset_metadata;
  for (tileset_metadata = m_tilesets.begin(); tileset_metadata != m_tilesets.end(); tileset_metadata++) {
    if (tileset_metadata->name.compare(name) == 0) {
      found_tileset_metadata = (*tileset_metadata);
    }
  }

  return found_tileset_metadata;
}

//---------------------------------------------------------------------------
Tileset_Helper::Subset Introduction_Controller::GetCurrentForegroundSubset() {
  Tileset_Helper::Subset found_subset;

  std::vector<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = m_current_tileset.tilesets.begin(); tileset != m_current_tileset.tilesets.end(); tileset++) {
    if (tileset->type.compare("Foreground") == 0) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}

//------------------------------------------------------------------------------
Tile_Bitmap* Introduction_Controller::CreateTile(float base_tile_size) {
  Tileset_Helper::Line middleground_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  for (line = m_current_tileset_subset.lines.begin(); line != m_current_tileset_subset.lines.end(); line++) {
    if (line->tile_size_x == base_tile_size) {
      if (line->tile_size_y == base_tile_size) {
        middleground_line = *line;
      }
    }
  }

  Tile_Bitmap* tile = new Tile_Bitmap();
  tile->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_current_tileset.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(m_current_tileset.size_x,
                                                 m_current_tileset.size_y);
  tile->GetTexture()->tile_size = D3DXVECTOR2(middleground_line.tile_size_x,
                                              middleground_line.tile_size_y);
  tile->SetIsCollidable(true);

  int random_line_tile = rand() % middleground_line.number_of_tiles;

  float random_tile_x = random_line_tile * middleground_line.tile_size_x;
  random_tile_x += middleground_line.top_left_x;

  float random_tile_y = middleground_line.top_left_y;

  tile->GetTexture()->top_left_position = D3DXVECTOR2(random_tile_x, random_tile_y);

  tile->SetSize(D3DXVECTOR2(base_tile_size, base_tile_size));

  return tile;
}

//---------------------------------------------------------------------------
Tileset_Helper::Line Introduction_Controller::GetCurrentSizedLine(float size) {
  Tileset_Helper::Line middleground_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  for (line = m_current_tileset_subset.lines.begin(); line != m_current_tileset_subset.lines.end(); line++) {
    if (line->tile_size_x == size) {
      if (line->tile_size_y == size) {
        middleground_line = *line;
      }
    }
  }
  return middleground_line;
}

//------------------------------------------------------------------------------
bool Introduction_Controller::InitTimer() {
  // Check to see if this system supports high performance timers.
  QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));

  if (m_frequency == 0) {
    return false;
  }

  // Find out how many times the frequency counter ticks every millisecond.
  m_ticksPerMs = static_cast<float>(m_frequency / 1000);

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTime));

  return true;
}

//------------------------------------------------------------------------------
void Introduction_Controller::UpdateTimer() {
  int frames_per_millisecond = static_cast<int>(1000/5);

  INT64 currentTime;
  float timeDifference;

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

  timeDifference = static_cast<float>(currentTime - m_startTime);

  m_frameTime = timeDifference / m_ticksPerMs;

  if (m_frameTime >= frames_per_millisecond) {
    m_startTime = currentTime;
    m_frameTime = static_cast<float>(m_frameTime - frames_per_millisecond);
    m_animation_tick = true;
  }
}

} // Tunnelour