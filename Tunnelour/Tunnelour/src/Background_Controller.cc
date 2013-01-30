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
#include <iostream>
#include <fstream>

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Background_Controller::Background_Controller() : Controller() {
  m_bitmap = NULL;
}

//------------------------------------------------------------------------------
Background_Controller::~Background_Controller() {
}

//------------------------------------------------------------------------------
void Background_Controller::Init(Tunnelour::Component_Composite * const model) {
  Tunnelour::Controller::Init(model);

  Tunnelour::Component* bitmap = m_model->Add(new Tunnelour::Tile_Bitmap());
  m_bitmap = static_cast<Tunnelour::Tile_Bitmap*>(bitmap);  
  m_bitmap->SetPosition(new D3DXVECTOR3(50, 50, -1));  
  m_bitmap->GetTexture()->transparency = 1.0f;

  /*
  Tunnelour::Component* bitmap = m_model->Add(new Tunnelour::Bitmap_Component());
   m_bitmap = static_cast<Tunnelour::Bitmap_Component*>(bitmap);  
  
  Tunnelour::Bitmap_Component::Texture texture;
  texture.texture_path = L"resource/Tunnelor_Background_32x32.dds";
  texture.transparency = 1.0f;
  m_bitmap->SetTexture(texture);
  */

  Tunnelour::Component *background = 0;
  background = m_model->Add(new Tunnelour::Background_Color_Component());
  background->Init();

  Load_Tilset_Metadata();
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
  pFile = fopen("resource\\tilesets\\Dirt_Tileset.txt","r");
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
      m_metadata.filename = token;
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

}  // namespace Tunnelour
