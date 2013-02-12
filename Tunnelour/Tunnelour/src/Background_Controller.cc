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

#include "Background_Controller.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include "Exceptions.h"
#include "Background_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Background_Controller::Background_Controller() : Controller() {
  m_bitmap = NULL;
  std::srand(static_cast<unsigned int>(std::time(0)));
  m_game_settings = 0;
  m_has_init_background_been_generated = false;
}

//------------------------------------------------------------------------------
Background_Controller::~Background_Controller() {
}

//------------------------------------------------------------------------------
void Background_Controller::Init(Tunnelour::Component_Composite * const model) {
    Tunnelour::Controller::Init(model);
}

//------------------------------------------------------------------------------
void Background_Controller::Run() {
  // Generate an initial Random Tile Background
  if (!m_has_init_background_been_generated) {
    // Get game settings component from the model with the Mutator.
    Tunnelour::Background_Controller_Mutator mutator;

    m_model->Apply(&mutator);
    if (!mutator.FoundGameSettings())  {
      throw Tunnelour::Exceptions::run_error("Can't find a game settings component!");
    } else {
      m_game_settings = mutator.GetGameSettings();
    }

    // Load the Tileset Data
    Load_Tilset_Metadata();

    int top_left_window_x = static_cast<int>(-1*(m_game_settings->GetResolution().x/2));
    int top_left_window_y = static_cast<int>(m_game_settings->GetResolution().y/2);
    int current_x = top_left_window_x;
    int current_y = top_left_window_y;
    Tunnelour::Tile_Bitmap* tile;
    for (int y = 0; y < 6 ; y++) {
      std::vector<Tunnelour::Tile_Bitmap*> tile_line;
      for (int x = 0; x < 10 ; x++) {
         tile = Create_Tile();
         tile->SetPosition(new D3DXVECTOR3(current_x + (tile->GetSize()->x/2),
                                           current_y - (tile->GetSize()->y/2), 0));
         current_x += static_cast<int>(tile->GetSize()->x);

         tile_line.push_back(tile);
      }
      current_y -= static_cast<int>(tile->GetSize()->y);
      current_x = static_cast<int>(top_left_window_x);
      m_background_tiles.push_back(tile_line);
    }

    // Add tiles to the model
    for (std::vector<std::vector<Tunnelour::Tile_Bitmap*>>::iterator row = m_background_tiles.begin(); row != m_background_tiles.end(); ++row) {
      for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = row->begin() ; tile != row->end(); ++tile) {
        m_model->Add(*tile);
      }
    }

    m_has_init_background_been_generated = true;
  }
  m_is_finished = true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Background_Controller::Load_Tilset_Metadata() {
  FILE * pFile;
  int lSize;

  std::wstring wtileset_path = m_game_settings->GetTilesetPath();
  m_metadata_file_path = WStringToString(wtileset_path + L"Dirt_Tileset_4_0.txt");

  // Open Font File as a text file
  if (fopen_s(&pFile, m_metadata_file_path.c_str(), "r") != 0) {
    throw Tunnelour::Exceptions::init_error("Open Tileset Metadata Failed!");
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
    if (strcmp(token, "Tileset_Name") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.name = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_Type") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.type = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_FileName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.filename = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.top_left_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.top_left_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.size_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.size_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_NumOfSubSets") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.number_of_subsets = atoi(token);
    }
  }

  if (m_metadata.number_of_subsets != 0) {
    for (int i = 0; i <  m_metadata.number_of_subsets; i++) {
      Tileset temp_tileset;

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Name") == 0)   {
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Type") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.type = token;
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.top_left_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.top_left_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.size_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.size_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_NumOfLines") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.number_of_lines = atoi(token);
        }
      }

      if (temp_tileset.number_of_lines != 0) {
        for (int i = 0; i < temp_tileset.number_of_lines; i++) {
          Line temp_line;

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_Name") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.line_number = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_TopLeftX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.top_left_x = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_TopLeftY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.top_left_y = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Tile_SizeX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.tile_size_x = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Tile_SizeY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.tile_size_y = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_NumOfTiles") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.number_of_tiles = atoi(token);
            }
          }
          temp_tileset.lines.push_back(temp_line);
        }
      }

      m_metadata.tilesets.push_back(temp_tileset);
    }
  }
}

//------------------------------------------------------------------------------
Tunnelour::Tile_Bitmap* Background_Controller::Create_Tile() {
  Tileset background_tileset;
  std::list<Tileset>::iterator tileset;
  for (tileset = m_metadata.tilesets.begin(); tileset != m_metadata.tilesets.end(); tileset++) {
    if (tileset->type.compare("Background")) {
      background_tileset = *tileset;
    }
  }

  Line background_32x32_line;
  std::list<Line>::iterator line;
  for (line = background_tileset.lines.begin(); line != background_tileset.lines.end(); line++) {
    if (line->tile_size_x == 32 && line->tile_size_y == 32) {
      background_32x32_line = *line;
    }
  }

  Tunnelour::Tile_Bitmap* tile = new Tunnelour::Tile_Bitmap();
  tile->SetPosition(new D3DXVECTOR3(0, 0, 0));
  tile->GetTexture()->transparency = 1.0f;
  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += StringToWString(m_metadata.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_metadata.size_x),
                                                 static_cast<float>(m_metadata.size_y));
  tile->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(background_32x32_line.tile_size_x),
                                              static_cast<float>(background_32x32_line.tile_size_y));

  int random_variable = rand() % background_32x32_line.number_of_tiles;

  tile->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(random_variable*(background_32x32_line.tile_size_x)),
                                                      static_cast<float>(background_32x32_line.top_left_y));

  tile->SetSize(new D3DXVECTOR2(128, 128));

  return tile;
}

//------------------------------------------------------------------------------
std::wstring Background_Controller::CharToWChar(const char* pstrSrc) {
    size_t origsize = strlen(pstrSrc) + 1;
    size_t convertedChars = 0;
    wchar_t wcstring[100];
    mbstowcs_s(&convertedChars, wcstring, origsize, pstrSrc, _TRUNCATE);
    return std::wstring(wcstring);
}

//------------------------------------------------------------------------------
std::wstring Background_Controller::StringToWString(const std::string& string) {
    int len;
    int slength = static_cast<int>(string.length() + 1);
    len = MultiByteToWideChar(CP_ACP, 0, string.c_str(), slength, 0, 0);
    std::wstring r(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, string.c_str(), slength, &r[0], len);
    return r;
}

//------------------------------------------------------------------------------
std::string Background_Controller::WStringToString(const std::wstring& wstring) {
    int len;
    int slength = static_cast<int>(wstring.length() + 1);
    len = WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), slength, 0, 0, 0, 0);
    std::string r(len, '\0');
    WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), slength, &r[0], len, 0, 0);
    return r;
}

}  // namespace Tunnelour
