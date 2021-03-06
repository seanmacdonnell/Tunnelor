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

#include "Screen_Wipeout_Controller.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include "Exceptions.h"
#include "String_Helper.h"
#include "Bitmap_Helper.h"
#include "Level_Transition_Controller_Mutator.h"
#include "Tileset_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Screen_Wipeout_Controller::Screen_Wipeout_Controller() : Controller(),
  m_z_bitmap_position(-4.0f),
  m_z_text_position(-5.0f) {
  m_game_settings = 0;
  m_camera = 0;
  m_tileset_filename = "";
  m_black_metadata_file_path = "";
  m_white_metadata_file_path = "";
  m_top_slash = 0;
  m_bottom_slash = 0;
  m_animation_tick = false;
  m_background = 0;
}

//------------------------------------------------------------------------------
Screen_Wipeout_Controller::~Screen_Wipeout_Controller() {
  m_game_settings = 0;
  if (m_camera != 0) {
    m_camera->Ignore(this);
    m_camera = 0;
  }
  m_tileset_filename = "";
  m_black_metadata_file_path = "";
  m_white_metadata_file_path = "";

  if (m_top_slash != 0) {
    m_model->Remove(m_top_slash);
    m_top_slash = 0;
  }
  if (m_bottom_slash != 0) {
    m_model->Remove(m_bottom_slash);
    m_bottom_slash = 0;
  }
  if (m_background != 0) {
    m_model->Remove(m_background);
    m_background = 0;
  }

  m_animation_tick = false;
}

//------------------------------------------------------------------------------
bool Screen_Wipeout_Controller::Init(Component_Composite * const model) {
  bool result = true;
  Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));

  Level_Transition_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_game_settings = mutator.GetGameSettings();
    m_camera = mutator.GetCamera();
    m_camera->Observe(this);
    LoadTilesetMetadata();
    m_current_tileset = Tileset_Helper::GetNamedTileset("Black", m_tilesets);
    m_current_tileset_subset = Tileset_Helper::GetForegroundSubset(m_current_tileset);
    m_has_been_initialised = true;
    StartTimer();
    if (m_top_slash == 0) {
      // Create the Spash Black Tile
      m_top_slash = CreateTile(128);
      m_top_slash->SetPosition(m_camera->GetPosition().x,
                               m_camera->GetPosition().y + (m_game_settings->GetResolution().y / 1.5f),
                               m_z_bitmap_position);
      m_top_slash->SetScale((m_game_settings->GetResolution().x / 128.0f),
                           ((m_game_settings->GetResolution().y / 128.0f) / 4.0f),
                             1.0f);
      m_model->Add(m_top_slash);
    }
    if (m_bottom_slash == 0) {
      // Create the Splash Black Tile
      m_bottom_slash = CreateTile(128);
      m_bottom_slash->SetPosition(m_camera->GetPosition().x,
                                  m_camera->GetPosition().y - (m_game_settings->GetResolution().y / 1.5f),
                                  m_z_bitmap_position);
      m_bottom_slash->SetScale((m_game_settings->GetResolution().x / 128.0f),
                                ((m_game_settings->GetResolution().y / 128.0f) / 4.0f),
                                1.0f);
      m_model->Add(m_bottom_slash);
    }
  } else {
    result = false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Screen_Wipeout_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }

  if (m_top_slash != 0 && m_bottom_slash != 0) {
    IsItTimeToAnimateAFrame();
    if (m_animation_tick) {
      m_top_slash->SetPosition(m_camera->GetPosition().x,
                               m_camera->GetPosition().y + (m_game_settings->GetResolution().y / 1.5f),
                               m_z_bitmap_position);
      m_bottom_slash->SetPosition(m_camera->GetPosition().x,
                                  m_camera->GetPosition().y - (m_game_settings->GetResolution().y / 1.5f),
                                  m_z_bitmap_position);

      float bottom = m_top_slash->GetBottomRightPostion().y;
      float top = m_bottom_slash->GetTopLeftPostion().y;
      if (bottom <= top) {
        m_model->Remove(m_top_slash);
        m_top_slash = 0;
        m_model->Remove(m_bottom_slash);
        m_bottom_slash = 0;
        if (m_background == 0) {
          m_background = CreateTile(128);
          m_background->SetPosition(m_camera->GetPosition().x,
                                    m_camera->GetPosition().y,
                                    m_z_bitmap_position);
          m_background->SetScale((m_game_settings->GetResolution().x / 128.0f),
                                  (m_game_settings->GetResolution().y / 128.0f),
                                  1.0f);
          m_model->Add(m_background);
          m_is_finished = true;
        }
      } else {
        m_top_slash->SetScale(m_top_slash->GetScale()->x,
                              m_top_slash->GetScale()->y + 0.2f,
                              1.0f);
        m_bottom_slash->SetScale(m_bottom_slash->GetScale()->x,
                                 m_bottom_slash->GetScale()->y + 0.2f,
                                 1.0f);
      }
    }
  }

  if (m_background != 0) {
    m_background->SetPosition(m_camera->GetPosition().x,
                              m_camera->GetPosition().y,
                              m_z_bitmap_position);
  }

  return true;
}

//------------------------------------------------------------------------------
void Screen_Wipeout_Controller::HandleEvent(Tunnelour::Component * const component) {
  if (component->GetType().compare("Camera_Component") == 0) {
    Run();
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
void Screen_Wipeout_Controller::LoadTilesetMetadata() {
  Tileset_Helper::Tileset_Metadata debug_tileset_metadata;
  m_black_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Black_Tileset_0_0.txt");
  Tileset_Helper::LoadTilesetMetadataIntoStruct(m_black_metadata_file_path, &debug_tileset_metadata);
  m_tilesets.push_back(debug_tileset_metadata);
}

//------------------------------------------------------------------------------
Tile_Bitmap* Screen_Wipeout_Controller::CreateTile(float base_tile_size) {
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
bool Screen_Wipeout_Controller::StartTimer() {
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
void Screen_Wipeout_Controller::IsItTimeToAnimateAFrame() {
  int milliseconds_per_frame = static_cast<int>(1000/5);

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

}  // namespace Tunnelour
