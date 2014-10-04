//  Copyright 2014 Sean MacDonnell
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

#include "Splash_Screen_Controller.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include "Exceptions.h"
#include "String_Helper.h"
#include "Bitmap_Helper.h"
#include "Colour_Helper.h"
#include "Tileset_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Splash_Screen_Controller::Splash_Screen_Controller() : Controller(),
  m_z_bitmap_position(-4),
  m_z_text_position(-5) {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_input = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_black_metadata_file_path = "";
  m_white_metadata_file_path = "";
  m_background = 0;
  m_game_name_heading = 0;
  m_author = 0;
  m_version = 0;
  m_animation_tick = false;
  m_splash_screen_component = 0;
}

//------------------------------------------------------------------------------
Splash_Screen_Controller::~Splash_Screen_Controller() {
  m_game_settings = 0;
  m_camera->Ignore(this);
  m_camera = 0;
  m_level = 0;
  m_input = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_black_metadata_file_path = "";
  m_white_metadata_file_path = "";
  if (m_game_name_heading != 0) { m_model->Remove(m_game_name_heading); }
  if (m_author != 0) { m_model->Remove(m_author); }
  if (m_version != 0) { m_model->Remove(m_version); }
  if (m_background != 0) { m_model->Remove(m_background); }
}

//------------------------------------------------------------------------------
bool Splash_Screen_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));

  Splash_Screen_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_game_settings = mutator.GetGameSettings();
    m_camera = mutator.GetCamera();
    m_camera->Observe(this);
    m_level = mutator.GetLevel();
    m_input = mutator.GetInput();
    LoadTilesetMetadata();
    m_current_tileset = Tileset_Helper::GetNamedTileset("Black", m_tilesets);
    m_current_tileset_subset = Tileset_Helper::GetForegroundSubset(m_current_tileset);
    m_has_been_initialised = true;
    m_heading_font_path = "resource\\tilesets\\victor_320.fnt";
    m_text_font_path = "resource\\tilesets\\victor_32.fnt";
    StartTimer();
    if (m_splash_screen_component == 0) {
      m_splash_screen_component = new Splash_Screen_Component();
      m_splash_screen_component->Init();
      m_model->Add(m_splash_screen_component);
    }
    if (m_background == 0) {
      m_background = CreateTile(128);
      m_background->SetPosition(m_camera->GetPosition().x,
                                m_camera->GetPosition().y,
                                m_z_bitmap_position);
      m_background->SetScale(m_game_settings->GetResolution().x / 128.0f,
                             m_game_settings->GetResolution().y / 128.0f,
                             1.0f);
      m_model->Add(m_background);
    }
    if (m_game_name_heading == 0) {
      m_game_name_heading = new Text_Component();
      m_game_name_heading->GetText()->font_csv_file = m_heading_font_path;
      m_game_name_heading->GetText()->text = "TUNNELOR";
      m_game_name_heading->GetFont()->font_color = Colours::Text_Blue;
      m_game_name_heading->GetTexture()->transparency = 1.0f;
      m_game_name_heading->SetPosition(0, 0, m_z_text_position);
      m_game_name_heading->GetFrame()->index_buffer = 0;
      m_game_name_heading->GetTexture()->texture = 0;
      m_game_name_heading->GetFrame()->vertex_buffer = 0;
      m_game_name_heading->Init();
      m_model->Add(m_game_name_heading);
    }
    if (m_version == 0) {
      m_version = new Text_Component();
      m_version->GetText()->font_csv_file = m_text_font_path;
      m_version->GetText()->text = "ALPHA";
      m_version->GetFont()->font_color = Colours::Text_Red;
      m_version->GetTexture()->transparency = 1.0f;
      m_version->SetPosition(0, 0, m_z_text_position);
      m_version->GetFrame()->index_buffer = 0;
      m_version->GetTexture()->texture = 0;
      m_version->GetFrame()->vertex_buffer = 0;
      m_version->Init();
      m_model->Add(m_version);
    }
    if (m_author == 0) {
      m_author = new Text_Component();
      m_author->GetText()->font_csv_file = m_text_font_path;
      m_author->GetText()->text = "SEAN.MACDONNELL@GMAIL.COM";
      m_author->GetFont()->font_color = Colours::Text_Light_Blue;
      m_author->GetTexture()->transparency = 1.0f;
      m_author->SetPosition(0, 0, m_z_text_position);
      m_author->GetFrame()->index_buffer = 0;
      m_author->GetTexture()->texture = 0;
      m_author->GetFrame()->vertex_buffer = 0;
      m_author->Init();
      m_model->Add(m_author);
    }

  } else {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Splash_Screen_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }

  if (m_background != 0) {
    m_background->SetPosition(m_camera->GetPosition().x,
                              m_camera->GetPosition().y,
                              m_z_bitmap_position);
  }

  if (m_game_name_heading != 0) {
    m_game_name_heading->SetPosition(m_camera->GetPosition().x,
                                     m_camera->GetPosition().y,
                                     m_z_text_position);
  }

  if (m_game_name_heading != 0 && m_version != 0) {
    float position_y = m_game_name_heading->GetPosition()->y - (m_game_name_heading->GetSize().y / 2.0f);
    position_y -= (m_version->GetSize().y / 2.0f);
    float position_x = m_camera->GetPosition().x + 480.0f;
    m_version->SetPosition(position_x, position_y, m_z_text_position);
  }

  if (m_version != 0 && m_author != 0) {
    float position_y = m_version->GetPosition()->y - (m_version->GetSize().y / 2.0f);
    position_y -= (m_author->GetSize().y / 2.0f);
    float position_x = m_camera->GetPosition().x + 340.0f;
    m_author->SetPosition(position_x, position_y, m_z_text_position);
  }

  IsItTimeToAnimateAFrame();
  if (m_animation_tick) {
    // Start Fading
    m_splash_screen_component->SetIsFading(true);
  }
  if (m_input->GetCurrentKeyInput().IsSpace) {
    // Start Fading
    m_splash_screen_component->SetIsFading(true);
  }

  if (m_splash_screen_component->IsFading()) {
    m_background->GetTexture()->transparency -= 0.005f;
    m_game_name_heading->GetTexture()->transparency -= 0.005f;
    m_author->GetTexture()->transparency -= 0.005f;
    m_version->GetTexture()->transparency -= 0.005f;

    if (m_background->GetTexture()->transparency < 0.0) {
      m_background->GetTexture()->transparency = 0.0f;
      m_game_name_heading->GetTexture()->transparency = 0.0f;
      m_author->GetTexture()->transparency = 0.0f;
      m_version->GetTexture()->transparency = 0.0f;

      if (m_game_name_heading != 0) {
        m_model->Remove(m_game_name_heading);
        m_game_name_heading = 0;
      }
      if (m_author != 0) {
        m_model->Remove(m_author);
        m_author = 0;
      }
      if (m_version != 0) {
        m_model->Remove(m_version);
        m_version = 0;
      }
      if (m_background != 0) {
        m_model->Remove(m_background);
        m_background = 0;
      }
      m_splash_screen_component->SetHasFaded(true);
      m_is_finished = true;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
void Splash_Screen_Controller::HandleEvent(Tunnelour::Component * const component) {
  if (component->GetType().compare("Camera_Component") == 0) {
    if (m_background != 0) {
      m_background->SetPosition(m_camera->GetPosition().x,
                                m_camera->GetPosition().y,
                                m_z_bitmap_position);
    }

    if (m_game_name_heading != 0) {
      m_game_name_heading->SetPosition(m_camera->GetPosition().x,
                                       m_camera->GetPosition().y,
                                       m_z_text_position);
    }

    if (m_game_name_heading != 0 && m_version != 0) {
      float position_y = m_game_name_heading->GetPosition()->y - (m_game_name_heading->GetSize().y / 2.0f);
      position_y -= (m_version->GetSize().y / 2.0f);
      float position_x = m_camera->GetPosition().x + 480.0f;
      m_version->SetPosition(position_x, position_y, m_z_text_position);
    }

    if (m_version != 0 && m_author != 0) {
      float position_y = m_version->GetPosition()->y - (m_version->GetSize().y / 2.0f);
      position_y -= (m_author->GetSize().y / 2.0f);
      float position_x = m_camera->GetPosition().x + 340.0f;
      m_author->SetPosition(position_x, position_y, m_z_text_position);
    }
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
void Splash_Screen_Controller::LoadTilesetMetadata() {
  Tileset_Helper::Tileset_Metadata debug_tileset_metadata;
  m_black_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Black_Tileset_0_0.txt");
  Tileset_Helper::LoadTilesetMetadataIntoStruct(m_black_metadata_file_path, &debug_tileset_metadata);
  m_tilesets.push_back(debug_tileset_metadata);
}

//------------------------------------------------------------------------------
Tile_Bitmap* Splash_Screen_Controller::CreateTile(float base_tile_size) {
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

//------------------------------------------------------------------------------
bool Splash_Screen_Controller::StartTimer() {
  // Check to see if this system supports high performance timers.
  QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));

  if (m_frequency == 0) {
    return false;
  }

  // Find out how many times the frequency counter ticks every millisecond.
  m_ticksPerMs = static_cast<float>(m_frequency / 1000);

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTime));
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_fadeout_startTime));

  return true;
}

//------------------------------------------------------------------------------
void Splash_Screen_Controller::IsItTimeToAnimateAFrame() {
  int milliseconds_per_frame = static_cast<int>(5000);

  INT64 currentTime;
  float timeDifference;

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

  timeDifference = static_cast<float>(currentTime - m_startTime);

  m_frameTime = timeDifference / m_ticksPerMs;

  if (m_frameTime >= milliseconds_per_frame) {
    m_startTime = currentTime;
    m_frameTime = static_cast<float>(m_frameTime - milliseconds_per_frame);
    m_animation_tick = true;
  }
}

//------------------------------------------------------------------------------
void Splash_Screen_Controller::UpdateFadeoutTimer() {
  int milliseconds_per_frame = static_cast<int>(5000);

  INT64 currentTime;
  float timeDifference;

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

  timeDifference = static_cast<float>(currentTime - m_fadeout_startTime);

  m_fadeout_frameTime = timeDifference / m_ticksPerMs;

  if (m_fadeout_frameTime >= milliseconds_per_frame) {
    m_fadeout_animation_tick = true;
  }
}

} // namespace Tunnelour
