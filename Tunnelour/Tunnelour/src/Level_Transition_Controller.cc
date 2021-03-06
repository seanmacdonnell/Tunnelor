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

#include "Level_Transition_Controller.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include "Exceptions.h"
#include "String_Helper.h"
#include "Bitmap_Helper.h"
#include "Colour_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Level_Transition_Controller::Level_Transition_Controller() : Controller() {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_black_metadata_file_path = "";
  m_white_metadata_file_path = "";
  m_level_complete_heading = 0;
  m_next_level_heading = 0;
  m_next_level_name = 0;
  m_next_level_blurb = 0;
  m_loading = 0;
  m_z_bitmap_position = -4;
  m_z_text_position = -5;
  m_animation_tick = false;
  m_level_transition_component = 0;
  m_input = 0;
  m_background = 0;
  m_level_complete_heading_text = " ";
  m_next_level_heading_text = " ";
  m_next_level_name_text = " ";
  m_next_level_blurb_text = " ";
  m_is_loading = false;
  m_is_fading = false;
  m_loading_transparency = 0.0f;
  m_level_complete_heading_y_offset = 0.0f;
  m_next_level_heading_y_offset = 0.0f;
  m_next_level_name_y_offset = 0.0f;
  m_next_level_blurb_y_offset = 0.0f;
  m_loading_y_offset = 0.0f;
  m_looking_key = 0;
  m_looking_key_text = "";
  m_looking_key_y_offset = 0.0f;
}

//------------------------------------------------------------------------------
Level_Transition_Controller::~Level_Transition_Controller() {
  m_game_settings = 0;
  if (m_camera != 0) {
    m_camera->Ignore(this);
    m_camera = 0;
  }
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_black_metadata_file_path = "";
  m_white_metadata_file_path = "";
  if (m_background != 0) {
    m_model->Remove(m_background);
    m_background = 0;
  }
  if (m_level_complete_heading != 0) {
    m_model->Remove(m_level_complete_heading);
    m_level_complete_heading = 0;
  }
  if (m_next_level_heading != 0) {
    m_model->Remove(m_next_level_heading);
    m_next_level_heading = 0;
  }
  if (m_next_level_name != 0) {
    m_model->Remove(m_next_level_name);
    m_next_level_name = 0;
  }
  if (m_next_level_blurb != 0) {
    m_model->Remove(m_next_level_blurb);
    m_next_level_blurb = 0;
  }
  if (m_loading != 0) {
    m_model->Remove(m_loading);
    m_loading = 0;
  }
  if (m_level_transition_component != 0) {
    m_model->Remove(m_level_transition_component);
    m_level_transition_component = 0;
  }
  if (m_looking_key != 0) {
    m_model->Remove(m_looking_key);
    m_looking_key = 0;
  }
  m_loading_transparency = 0.0f;
}

//------------------------------------------------------------------------------
bool Level_Transition_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));

  Level_Transition_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {  
    m_level_complete_heading_y_offset = 200.0f;
    m_next_level_heading_y_offset = 50.0f;
    m_next_level_name_y_offset = -50.0f;
    m_next_level_blurb_y_offset = -150.0f;
    m_looking_key_y_offset = -200.0f;
    m_loading_y_offset = -250.0f;
    m_game_settings = mutator.GetGameSettings();
    m_camera = mutator.GetCamera();
    m_camera->Observe(this);
    m_level = mutator.GetLevel();
    m_input = mutator.GetInput();
    LoadTilesetMetadata();
    m_current_tileset = GetNamedTileset("Black");
    m_current_tileset_subset = GetCurrentForegroundSubset();
    m_has_been_initialised = true;
    m_heading_font_path = "resource\\tilesets\\victor_192.fnt";
    m_text_font_path = "resource\\tilesets\\victor_32.fnt";
    StartTimer();
    if (m_level_transition_component == 0) {
      m_level_transition_component = new Level_Transition_Component();
      m_level_transition_component->Init();
      m_model->Add(m_level_transition_component);
    }
    if (m_background == 0) {
      // Create the Spash Black Tile
      m_background = CreateTile(128);
      m_background->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y, m_z_bitmap_position);
      m_background->SetScale(D3DXVECTOR3((m_game_settings->GetResolution().x/128), (m_game_settings->GetResolution().y/128), 1.0f));
      m_model->Add(m_background);        
    }
    if (m_level_complete_heading == 0) {
      m_level_complete_heading = new Text_Component();
      m_level_complete_heading->GetText()->font_csv_file = m_heading_font_path;
      m_level_complete_heading->GetText()->text = m_level_complete_heading_text;
      m_level_complete_heading->GetTexture()->transparency = 1.0f;
      m_level_complete_heading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_level_complete_heading_y_offset, m_z_text_position);
      m_level_complete_heading->GetFont()->font_color = Colours::Text_Blue;
      m_level_complete_heading->GetFrame()->index_buffer = 0;
      m_level_complete_heading->GetTexture()->texture = 0;
      m_level_complete_heading->GetFrame()->vertex_buffer = 0;
      m_model->Add(m_level_complete_heading);
      }
    if (m_next_level_heading == 0) {
      m_next_level_heading = new Text_Component();
      m_next_level_heading->GetText()->font_csv_file = m_text_font_path;
      m_next_level_heading->GetText()->text = m_next_level_heading_text;
      m_next_level_heading->GetTexture()->transparency = 1.0f;
      m_next_level_heading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_heading_y_offset, m_z_text_position);
      m_next_level_heading->GetFrame()->index_buffer = 0;
      m_next_level_heading->GetTexture()->texture = 0;
      m_next_level_heading->GetFrame()->vertex_buffer = 0;
      m_next_level_heading->GetFont()->font_color = Colours::Text_Light_Blue; 
      m_model->Add(m_next_level_heading);
    }
    if (m_next_level_name == 0) {
      m_next_level_name = new Text_Component();
      m_next_level_name->GetText()->font_csv_file = m_text_font_path;
      m_next_level_name->GetText()->text = m_next_level_name_text;
      m_next_level_name->GetTexture()->transparency = 1.0f;
      m_next_level_name->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_name_y_offset, m_z_text_position);
      m_next_level_name->GetFrame()->index_buffer = 0;
      m_next_level_name->GetTexture()->texture = 0;
      m_next_level_name->GetFrame()->vertex_buffer = 0;
      m_next_level_name->GetFont()->font_color = Colours::Text_Light_Blue; 
      m_model->Add(m_next_level_name);
    }
    if (m_next_level_blurb == 0) {
      m_next_level_blurb = new Text_Component();
      m_next_level_blurb->GetText()->font_csv_file = m_text_font_path;
      m_next_level_blurb->GetText()->text = m_next_level_blurb_text;
      m_next_level_blurb->GetTexture()->transparency = 1.0f;
      m_next_level_blurb->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_blurb_y_offset, m_z_text_position);
      m_next_level_blurb->GetFrame()->index_buffer = 0;
      m_next_level_blurb->GetTexture()->texture = 0;
      m_next_level_blurb->GetFrame()->vertex_buffer = 0;
      m_next_level_blurb->GetFont()->font_color = Colours::Text_Light_Blue; 
      m_model->Add(m_next_level_blurb);
    }
    if (m_looking_key == 0) {
      m_looking_key = new Text_Component();
      m_looking_key->GetText()->font_csv_file = m_text_font_path;
      m_looking_key->GetText()->text = m_next_level_blurb_text;
      m_looking_key->GetText()->text = "Hold SHIFT and use the direction keys to look around!";
      m_looking_key->GetTexture()->transparency = 1.0f;
      m_looking_key->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_looking_key_y_offset, m_z_text_position);
      m_looking_key->GetFrame()->index_buffer = 0;
      m_looking_key->GetTexture()->texture = 0;
      m_looking_key->GetFrame()->vertex_buffer = 0;
      m_looking_key->GetFont()->font_color = Colours::Text_Light_Blue; 
      m_model->Add(m_looking_key);
    }
    if (m_loading == 0) {
      m_loading = new Text_Component();
      m_loading->GetText()->font_csv_file = m_text_font_path;
      m_loading->GetText()->text = "Loading!";
      m_loading->GetTexture()->transparency = 1.0f;
      m_loading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_loading_y_offset, m_z_text_position);
      m_loading->GetFrame()->index_buffer = 0;
      m_loading->GetTexture()->texture = 0;
      m_loading->GetFrame()->vertex_buffer = 0;
      m_loading->GetFont()->font_color = Colours::Text_Light_Blue; 
      m_model->Add(m_loading);
      SetIsLoading(true);
    } 
  } else {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Level_Transition_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { 
    Init(m_model); 
  }

  if (m_background != 0) {
    m_background->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y, m_z_bitmap_position);
  }
  
  if (m_level_complete_heading != 0) {
    if (m_level_complete_heading_text.compare(m_level_complete_heading->GetText()->text) != 0) {
      m_level_complete_heading->GetText()->text = m_level_complete_heading_text;
      m_level_complete_heading->GetFrame()->index_buffer = 0;
      m_level_complete_heading->GetTexture()->texture = 0;
      m_level_complete_heading->GetFrame()->vertex_buffer = 0;
      m_level_complete_heading->Init();
      m_level_complete_heading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_level_complete_heading_y_offset, m_z_text_position);
    }
  }

  if (m_next_level_heading != 0) {
    if (m_next_level_heading_text.compare(m_next_level_heading->GetText()->text) != 0) {
      m_next_level_heading->GetText()->text = m_next_level_heading_text;
      m_next_level_heading->GetFrame()->index_buffer = 0;
      m_next_level_heading->GetTexture()->texture = 0;
      m_next_level_heading->GetFrame()->vertex_buffer = 0;
      m_next_level_heading->Init();
      m_next_level_heading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_heading_y_offset, m_z_text_position);
    }
  }
  
  if (m_next_level_name != 0) {
    if (m_next_level_name_text.compare(m_next_level_name->GetText()->text) != 0) {
      m_next_level_name->GetText()->text = m_next_level_name_text;
      m_next_level_name->GetFrame()->index_buffer = 0;
      m_next_level_name->GetTexture()->texture = 0;
      m_next_level_name->GetFrame()->vertex_buffer = 0;
      m_next_level_name->Init();
      m_next_level_name->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_name_y_offset, m_z_text_position);
    }
  }
  
  if (m_next_level_blurb != 0) {
    if (m_next_level_blurb_text.compare(m_next_level_blurb->GetText()->text) != 0) {
      m_next_level_blurb->GetText()->text = m_next_level_blurb_text;
      m_next_level_blurb->GetFrame()->index_buffer = 0;
      m_next_level_blurb->GetTexture()->texture = 0;
      m_next_level_blurb->GetFrame()->vertex_buffer = 0;
      m_next_level_blurb->Init();
      m_next_level_blurb->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_blurb_y_offset, m_z_text_position);
    }
  }

  if (m_looking_key != 0) {
    m_looking_key->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_looking_key_y_offset, m_z_text_position);
  }


  if (m_loading != 0) {
    IsItTimeToAnimateAFrame();
    if (m_animation_tick && m_background != 0) {
      if (m_loading->GetTexture()->transparency > 0.0f) {
        m_loading_transparency = m_loading->GetTexture()->transparency;
        m_loading->GetTexture()->transparency = 0.0f;
      } else {
        m_loading->GetTexture()->transparency = m_loading_transparency;
      }

      m_animation_tick = false;
    }

    if (m_loading->GetText()->text.compare("Loading!") == 0) {
      if (!m_is_loading) {
        m_loading->GetTexture()->transparency = 1.0f;
        m_loading->GetText()->text = "Loading Complete Press Space to Continue";
        m_loading->GetFrame()->index_buffer = 0;
        m_loading->GetTexture()->texture = 0;
        m_loading->GetFrame()->vertex_buffer = 0;
        m_loading->Init();
        m_loading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_loading_y_offset, m_z_text_position);
      }
    }

    if (m_loading->GetText()->text.compare("Loading Complete Press Space to Continue") == 0) {
      if (m_input->GetCurrentKeyInput().IsSpace) {
        m_is_fading = true;
      }
    }
  }

  if (m_is_fading) {
    m_background->GetTexture()->transparency = m_background->GetTexture()->transparency - 0.05f;
    m_level_complete_heading->GetTexture()->transparency = m_level_complete_heading->GetTexture()->transparency - 0.05f;
    m_next_level_heading->GetTexture()->transparency = m_next_level_heading->GetTexture()->transparency - 0.05f;
    m_next_level_name->GetTexture()->transparency = m_next_level_name->GetTexture()->transparency - 0.05f;
    m_next_level_blurb->GetTexture()->transparency = m_next_level_blurb->GetTexture()->transparency - 0.05f;
    m_looking_key->GetTexture()->transparency = m_looking_key->GetTexture()->transparency - 0.05f;
    if (m_loading->GetTexture()->transparency != 0.0f) {
      m_loading->GetTexture()->transparency = m_loading->GetTexture()->transparency - 0.1f;
    } else {
      m_loading_transparency = m_loading_transparency - 0.1f;
    }

    if (m_background->GetTexture()->transparency < 0.0f) {
      m_background->GetTexture()->transparency = 0.0f;
      m_level_complete_heading->GetTexture()->transparency = 0.0f;
      m_next_level_heading->GetTexture()->transparency = 0.0f;
      m_next_level_name->GetTexture()->transparency = 0.0f;
      m_next_level_blurb->GetTexture()->transparency = 0.0f;
      m_looking_key->GetTexture()->transparency = 0.0f;
      m_loading->GetTexture()->transparency = 0.0f;
      if (m_background != 0) {
        m_model->Remove(m_background);
        m_background = 0;
      }
      if (m_level_complete_heading != 0) {
        m_model->Remove(m_level_complete_heading);
        m_level_complete_heading = 0;
      }
      if (m_next_level_heading != 0) {
        m_model->Remove(m_next_level_heading);
        m_next_level_heading = 0;
      }
      if (m_next_level_name != 0) {
        m_model->Remove(m_next_level_name);
        m_next_level_name = 0;
      }
      if (m_next_level_blurb != 0) {
        m_model->Remove(m_next_level_blurb);
        m_next_level_blurb = 0;
      }
      if (m_looking_key != 0) {
        m_model->Remove(m_looking_key);
        m_looking_key = 0;
      }
      if (m_loading != 0) {
        m_model->Remove(m_loading);
        m_loading = 0;
      }
      m_is_finished = true;
    }
  } 

  return true;
}

//------------------------------------------------------------------------------
void Level_Transition_Controller::SetIsLoading(bool is_loading) {
  m_is_loading = is_loading;
}

//------------------------------------------------------------------------------
bool Level_Transition_Controller::IsLoading() {
  return m_is_loading;
}

//------------------------------------------------------------------------------
bool Level_Transition_Controller::IsFading() {
  return m_is_fading;
}

//------------------------------------------------------------------------------
void Level_Transition_Controller::SetLevelCompleteHeadingText(std::string level_complete_heading_text) {
  m_level_complete_heading_text = level_complete_heading_text;
}

//------------------------------------------------------------------------------
void Level_Transition_Controller::SetNextLevelHeadingText(std::string next_level_heading_text) {
  m_next_level_heading_text = next_level_heading_text;
}

//------------------------------------------------------------------------------
void Level_Transition_Controller::SetNextLevelNameText(std::string next_level_name_text) {
  m_next_level_name_text = next_level_name_text;
  if (next_level_name_text.empty()) {
    m_next_level_name_text = " ";
  }
}

//------------------------------------------------------------------------------
void Level_Transition_Controller::SetNextLevelBlurbText(std::string next_level_blurb_text) {
  m_next_level_blurb_text = next_level_blurb_text;
  if (m_next_level_blurb_text.empty()) {
    m_next_level_blurb_text = " ";
  }
}

//------------------------------------------------------------------------------
void Level_Transition_Controller::HandleEvent(Tunnelour::Component * const component) {
  if (component->GetType().compare("Camera_Component") == 0) {
    if (m_background != 0) {
      m_background->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y, m_z_bitmap_position);
    }
    if (m_level_complete_heading != 0) {
      m_level_complete_heading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_level_complete_heading_y_offset, m_z_text_position);
    }
    if (m_next_level_heading != 0) {
      m_next_level_heading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_heading_y_offset, m_z_text_position);
    }
    if (m_next_level_name != 0) {
      m_next_level_name->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_name_y_offset, m_z_text_position);
    }
    if (m_next_level_blurb != 0) {
      m_next_level_blurb->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_next_level_blurb_y_offset, m_z_text_position);
    }
    if (m_looking_key != 0) {
      m_looking_key->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_looking_key_y_offset, m_z_text_position);
    }
    if (m_loading != 0) {
      m_loading->SetPosition(m_camera->GetPosition().x, m_camera->GetPosition().y + m_loading_y_offset, m_z_text_position);
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
void Level_Transition_Controller::LoadTilesetMetadata() {
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
Tileset_Helper::Tileset_Metadata Level_Transition_Controller::GetNamedTileset(std::string name) {
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
Tileset_Helper::Subset Level_Transition_Controller::GetCurrentForegroundSubset() {
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
Tile_Bitmap* Level_Transition_Controller::CreateTile(float base_tile_size) {
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
Tileset_Helper::Line Level_Transition_Controller::GetCurrentSizedLine(float size) {
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
bool Level_Transition_Controller::StartTimer() {
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
void Level_Transition_Controller::IsItTimeToAnimateAFrame() {
  int milliseconds_per_frame = static_cast<int>(1000/2);

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

} // Tunnelour