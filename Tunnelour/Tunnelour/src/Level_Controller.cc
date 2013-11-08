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

#include "Level_Controller.h"
#include "String_Helper.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Level_Controller::Level_Controller() : Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_level_name_heading = 0;
  m_level_blurb = 0;
  m_camera = 0;
}

//------------------------------------------------------------------------------
Level_Controller::~Level_Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_level_name_heading = 0;
  m_level_blurb = 0;
  m_camera = 0;
}

//------------------------------------------------------------------------------
bool Level_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  Level_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_game_settings = mutator.GetGameSettings();
    m_camera = mutator.GetCamera();
    if (m_level == 0) {
      m_level = new Level_Component();
      m_model->Add(m_level);
      m_level->Init();
      LoadLevelMetadata();
    }
    m_font_path = "resource\\tilesets\\Ariel.fnt";
    m_has_been_initialised = true;
  } else {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool Level_Controller::Run() {
  if (m_has_been_initialised) {
    if (m_level_name_heading == 0 && m_level->GetCurrentLevel().level_name.size() != 0) {
      m_level_name_heading = new Text_Component();
      m_level_name_heading->GetText()->font_csv_file = m_font_path;
      m_level_name_heading->GetText()->text = m_level->GetCurrentLevel().level_name;
      m_level_name_heading->GetTexture()->transparency = 1.0f;
      m_model->Add(m_level_name_heading);
    }
    if (m_level_blurb == 0) {
      m_level_blurb = new Text_Component();
      m_level_blurb->GetText()->font_csv_file = m_font_path;
      m_level_blurb->GetText()->text = m_level->GetCurrentLevel().blurb;
      m_level_blurb->GetTexture()->transparency = 1.0f;
      m_model->Add(m_level_blurb);
      m_level_blurb->Init();
    }
    if (m_level_name_heading->GetTexture()->transparency != 0.0) {
      float transparency = m_level_name_heading->GetTexture()->transparency;
      transparency = transparency - 0.01f;
      if (transparency < 0.0) { transparency = 0.0f; }
      m_level_name_heading->GetTexture()->transparency = transparency;
      m_level_blurb->GetTexture()->transparency = transparency;
    }

    m_level_name_heading->SetPosition(m_camera->GetPosition().x,
                                      m_camera->GetPosition().y + m_level_name_heading->GetSize().y, -3);
    m_level_name_heading->Init();

    m_level_blurb->SetPosition(m_camera->GetPosition().x,
                               m_camera->GetPosition().y - m_level_name_heading->GetSize().y, -3);
    m_level_blurb->Init();
  } else {
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------
void Level_Controller::LoadLevelMetadata() {
  Level_Component::Level_Metadata level_0_metadata;
  std::string level_0_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_3.txt");
  level_0_metadata = LoadLevelMetadataIntoStruct(level_0_metadata_file_path);
  std::string level_0_csv_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_3.csv");
  LoadLevelCSVIntoStruct(level_0_csv_file_path, &level_0_metadata);
  m_level->SetCurrentLevel(level_0_metadata);
}

//------------------------------------------------------------------------------
Level_Component::Level_Metadata Level_Controller::LoadLevelMetadataIntoStruct(std::string metadata_path) {
  Level_Component::Level_Metadata level_metadata;

  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_path.c_str(), "r") != 0) {
    std::string error = "Open Tileset Metadata Failed! " +  metadata_path;
    throw Tunnelour::Exceptions::init_error(error);
  }

  // obtain file size:
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  char * token;
  char * next_token;

  char line[255];
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_Name") == 0)   {
      token = strtok_s(NULL, "\"", &next_token);
      level_metadata.level_name = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Name");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_Blurb") == 0)   {
      token = strtok_s(NULL, "\"", &next_token);
      level_metadata.blurb = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Blurb");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_CSV_Filename") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.filename = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_CSV_Filename");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TilesetName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tileset_name = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_TilesetName");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TunnelStartTopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tunnel_top_left_x = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Tileset_TopLeftX");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TunnelStartTopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tunnel_top_left_y = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Tileset_TopLeftX");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_AvatarStartTopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.avatar_top_left_x = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Tileset_TopLeftX");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_AvatarStartTopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.avatar_top_left_y = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Tileset_TopLeftX");
    }
  }
  fclose(pFile);

  return level_metadata;
}

//------------------------------------------------------------------------------
void Level_Controller::LoadLevelCSVIntoStruct(std::string metadata_path, Level_Component::Level_Metadata *out_metadata) {
  if (out_metadata == 0) { throw Tunnelour::Exceptions::run_error("Need an initialized strut"); }
  if (out_metadata->filename.compare("") == 0) { throw Tunnelour::Exceptions::run_error("Need a struct with a filename"); }
  Level_Component::Level_Metadata level_metadata;

  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_path.c_str(), "r") != 0) {
    std::string error = "Open Tileset Metadata Failed! " +  metadata_path;
    throw Tunnelour::Exceptions::init_error(error);
  }

  // obtain file size:
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  char * token;
  char * next_token;

  //This is string::max_size
  unsigned int max_size = 10000;
  char line[10000];
  while (fgets(line, max_size, pFile) != NULL) {
    std::vector<Level_Component::Tile_Metadata> line_metadata;
    std::vector<std::string> split_line = String_Helper::Split(line, ',');
    std::vector<std::string>::iterator line_tile;
    for (line_tile = split_line.begin(); line_tile != split_line.end(); line_tile++) {
      Level_Component::Tile_Metadata tile_metadata;
      std::vector<std::string> quote_stripper = String_Helper::Split((*line_tile), '\'');
      std::vector<std::string> split_tile = String_Helper::Split(quote_stripper[0], ';');
      std::vector<std::string>::iterator line_tile_data;
      for (line_tile_data = split_tile.begin(); line_tile_data != split_tile.end(); line_tile_data++) {
        std::vector<std::string> split_tile_data = String_Helper::Split((*line_tile_data), ' ');
        if (split_tile_data.begin()->compare("Size") == 0) {
          tile_metadata.size = static_cast<float>(atof(split_tile_data[1].c_str()));
        } else if (split_tile_data.begin()->compare("Type") == 0) {
          tile_metadata.type = split_tile_data[1];
        }
      }
      line_metadata.push_back(tile_metadata);
    }
    out_metadata->level.push_back(line_metadata);
  }

  fclose (pFile);
}

//---------------------------------------------------------------------------
Avatar_Component::Collision_Block Level_Controller::GetNamedCollisionBlock(std::string id, std::list<Avatar_Component::Collision_Block> collision_blocks) {
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
