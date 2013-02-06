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
#include "Background_Color_Component.h"
#include "Exceptions.h"
#include <ctime>

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Background_Controller::Background_Controller() : Controller() {
  m_bitmap = NULL;
  std::srand(std::time(0));
}

//------------------------------------------------------------------------------
Background_Controller::~Background_Controller() {
}

//------------------------------------------------------------------------------
void Background_Controller::Init(Tunnelour::Component_Composite * const model) {
  Tunnelour::Controller::Init(model);

  Tunnelour::Component *background = 0;
  background = m_model->Add(new Tunnelour::Background_Color_Component());
  background->Init();

  //Load the Tileset Data
  Load_Tilset_Metadata();

  //Generate Random Tile

  //Position Tile 128x128
  //Resoltion is 1280x720
  //That is 10x128 x
  //and 6x128 y
  //56.5 Tiles Total
  //Top Left is (640,-360)
  int start_x = -512;
  int current_x = start_x;
  int current_y = 232;
  Tunnelour::Tile_Bitmap* tile;
  for (int y = 0; y < 6 ; y++) {
    for (int x = 0; x < 10 ; x++) {
       tile = Create_Tile();
       tile->SetPosition(new D3DXVECTOR3(current_x - (tile->GetSize()->x/2),
                                         current_y + (tile->GetSize()->y/2), 0));
       current_x += tile->GetSize()->x;
       m_model->Add(tile);
    }
    current_y -= tile->GetSize()->y;
    current_x = start_x;
  }
}

//------------------------------------------------------------------------------
void Background_Controller::Run() {

  
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
  long lSize;
  char * buffer;
  size_t result;

  // Open Font File as a text file
  pFile = fopen("resource\\tilesets\\Dirt_Tileset_4_0.txt","r");
  if (pFile == NULL) {
    throw Tunnelour::Exceptions::init_error("Open Tileset Metadata Failed!");
  }

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  char line[225];
  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"Name") == 0) 	{
      token = strtok(NULL, " =\"");
      m_metadata.name = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"Type") == 0) 	{
      token = strtok(NULL, " =\"");
      m_metadata.type = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"FileName") == 0) 	{
      token = strtok(NULL, " =\"");
      m_metadata.filename = CharToWChar(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"TopLeftX") == 0) 	{
      token = strtok(NULL, " =\"");
      m_metadata.top_left_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"TopLeftY") == 0) 	{
      token = strtok(NULL, " =\"");
      m_metadata.top_left_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"SizeX") == 0) 	{
      token = strtok(NULL, " =\"");
      m_metadata.size_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"SizeY") == 0) 	{
      token = strtok(NULL, " =\"");
      m_metadata.size_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"NumOfSubSets") == 0) 	{
      token = strtok(NULL, " =\"");
      m_metadata.number_of_subsets = atoi(token);
    }
  }

  if (m_metadata.number_of_subsets != 0) {
    for (int i = 0; i <  m_metadata.number_of_subsets; i++) {
      Tileset temp_tileset;

      fgets(line, 225, pFile);
      fgets(line, 225, pFile);
      if (line != NULL) {
        char * token = strtok(line," ");
        if (strcmp(token,"Type") == 0) 	{
          token = strtok(NULL, " =\"");
          temp_tileset.type = token;
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        char * token = strtok(line," ");
        if (strcmp(token,"TopLeftX") == 0) 	{
          token = strtok(NULL, " =\"");
          temp_tileset.top_left_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        char * token = strtok(line," ");
        if (strcmp(token,"TopLeftY") == 0) 	{
          token = strtok(NULL, " =\"");
          temp_tileset.top_left_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        char * token = strtok(line," ");
        if (strcmp(token,"SizeX") == 0) 	{
          token = strtok(NULL, " =\"");
          temp_tileset.size_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        char * token = strtok(line," ");
        if (strcmp(token,"SizeY") == 0) 	{
          token = strtok(NULL, " =\"");
          temp_tileset.size_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        char * token = strtok(line," ");
        if (strcmp(token,"NumOfLines") == 0) 	{
          token = strtok(NULL, " =\"");
          temp_tileset.number_of_lines = atoi(token);
        }
      }

      if (temp_tileset.number_of_lines != 0) {
        for (int i = 0; i < temp_tileset.number_of_lines; i++) {
          Line temp_line;

          fgets(line, 225, pFile);
          if (line != NULL) {
            char * token = strtok(line," ");
            if (strcmp(token,"Line") == 0) 	{
              token = strtok(NULL, " =\"");
              temp_line.line_number = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            char * token = strtok(line," ");
            if (strcmp(token,"TopLeftX") == 0) 	{
              token = strtok(NULL, " =\"");
              temp_line.top_left_x = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            char * token = strtok(line," ");
            if (strcmp(token,"TopLeftY") == 0) 	{
              token = strtok(NULL, " =\"");
              temp_line.top_left_y = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            char * token = strtok(line," ");
            if (strcmp(token,"SizeX") == 0) 	{
              token = strtok(NULL, " =\"");
              temp_line.size_x = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            char * token = strtok(line," ");
            if (strcmp(token,"SizeY") == 0) 	{
              token = strtok(NULL, " =\"");
              temp_line.size_y = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            char * token = strtok(line," ");
            if (strcmp(token,"NumOfTiles") == 0) 	{
              token = strtok(NULL, " =\"");
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
    if (line->size_x == 32 && line->size_y == 32) {
      background_32x32_line = *line;
    }
  }

  Tunnelour::Tile_Bitmap* tile = new Tunnelour::Tile_Bitmap();  
  tile->SetPosition(new D3DXVECTOR3(0, 0, 0));
  tile->GetTexture()->transparency = 1.0f;
  tile->GetTexture()->texture_path = L"resource\\tilesets\\Dirt_Tileset_4_0.dds";
  tile->GetTexture()->texture_size = D3DXVECTOR2(m_metadata.size_x, m_metadata.size_y);
  tile->GetTexture()->tile_size = D3DXVECTOR2(background_32x32_line.size_x, background_32x32_line.size_y);

  int random_variable = (rand() % ((background_32x32_line.number_of_tiles)));

  tile->GetTexture()->top_left_position = D3DXVECTOR2(random_variable*32, background_32x32_line.top_left_y);

  tile->SetSize(new D3DXVECTOR2(128, 128));

  return tile;
}

//------------------------------------------------------------------------------
std::wstring Background_Controller::CharToWChar(const char* pstrSrc) {
    size_t origsize = strlen(pstrSrc) + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs_s(&convertedChars, wcstring, origsize, pstrSrc, _TRUNCATE);
    return std::wstring(wcstring);
}

}  // namespace Tunnelour
