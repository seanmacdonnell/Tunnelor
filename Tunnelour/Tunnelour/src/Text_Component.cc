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

#include "Text_Component.h"
#include <stdio.h>
#include "Exceptions.h"
#include "String_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Text_Component::Text_Component(): Bitmap_Component() {
  m_text.text = "";
  m_text.font_csv_file = "";

  m_font.image_width = 0;
  m_font.image_height = 0;
  m_font.font_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

  m_texture->texture_path = L"resource\\tilesets\\";

  m_type = "Text_Component";
  m_has_font_been_loaded = false;
}

//------------------------------------------------------------------------------
Text_Component::~Text_Component()  {
  m_text.text = "";
  m_text.font_csv_file = "";
  m_font.image_width = 0;
  m_font.image_height = 0;
  m_font.font_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
  m_texture->texture_path = L"";
  m_type = "";
  m_has_font_been_loaded = false;
}

//------------------------------------------------------------------------------
void Text_Component::Init() {
  Bitmap_Component::Init();

  if (!m_has_font_been_loaded) { 
    Load_Font_Struct(); 
    Load_Character_Frames();
  }

  Create_String_Frame();

  m_is_initialised = true;
}

//------------------------------------------------------------------------------
Text_Component::Font* Text_Component::GetFont() {
  return &m_font;
}

//------------------------------------------------------------------------------
Text_Component::Text* Text_Component::GetText() {
  return &m_text;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Text_Component::Load_Font_Struct() {
  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, m_text.font_csv_file.c_str(), "r") != 0) {
    throw Tunnelour::Exceptions::init_error("Open font CSV file Failed!");
  }

  // obtain file size:
  fseek(pFile , 0 , SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  char * token;
  char * next_token;

  // Get the first line
  // Example line: "info face="Ariel" size=32 bold=0 italic=0 charset="" unicode=1 stretchH=100 smooth=1 aa=2 padding=0,0,0,0 spacing=1,1 outline=0"
  char line[225];
  fgets(line, 225, pFile);
  if (line != NULL) {
  token = strtok_s(line, " ", &next_token);
  if (strcmp(token, "info") == 0) {
      while (token != NULL) {
        if (strcmp(token, "face") == 0) {
            token = strtok_s(NULL, " =\"", &next_token);
            m_font.font_name = token;
        }
        token = strtok_s(NULL, " =\"", &next_token);
      }
    }
  } else {
    throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
  }

  // Get the second line
  // Example line: "common lineHeight=32 base=26 scaleW=512 scaleH=512 pages=1 packed=0 alphaChnl=1 redChnl=0 greenChnl=0 blueChnl=0"
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "common") == 0) {
      while (token != NULL) {
        if (strcmp(token, "lineHeight") == 0) {
            token = strtok_s(NULL, " =\"", &next_token);
            m_font.line_height = atof(token);
        }
        if (strcmp(token, "scaleW") == 0) {
            token = strtok_s(NULL, " =\"", &next_token);
            m_font.image_width = atoi(token);
        }
        if (strcmp(token, "scaleH") == 0) {
            token = strtok_s(NULL, " =\"", &next_token);
            m_font.image_height = atoi(token);
        }
        token = strtok_s(NULL, " =\"", &next_token);
      }
    }
  } else {
    throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
  }

  // Get the Third Line
  // Example line: "page id=0 file="Arial_0.dds""
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "page") == 0) {
        while (token != NULL) {
        if (strcmp(token, "file") == 0) {
            token = strtok_s(NULL, " =\"", &next_token);
            m_font.font_texture_name = String_Helper::CharToWChar(token);
            m_texture->texture_path.append(m_font.font_texture_name);
        }
        token = strtok_s(NULL, " =\"", &next_token);
        }
    }
  } else {
    throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
  }

  // Get the fourth line
  // Example line: chars count=95
  fgets(line, 225, pFile);
  int number_of_chars = 0;
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "chars") == 0) {
        while (token != NULL) {
        if (strcmp(token, "count") == 0) {
            token = strtok_s(NULL, " =\"", &next_token);
            number_of_chars = atoi(token);
        }
        token = strtok_s(NULL, " =\"", &next_token);
        }
    }
  } else {
    throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
  }

  // Get all character lines
  // Example line: char id=32   x=88    y=25    width=1     height=1     xoffset=0     yoffset=0     xadvance=7     page=0  chnl=15
  for (int line_count = 0; line_count <= number_of_chars; line_count++) {
    fgets(line, 225, pFile);
    int number_of_chars = 0;
    if (line != NULL) {
      token = strtok_s(line, " ", &next_token);
      if (strcmp(token, "char") == 0) {
        int id = 0;
         while (token != NULL) {
          if (strcmp(token, "id") == 0) {
              token = strtok_s(NULL, " =\"", &next_token);
              id = atoi(token);
              m_font.raw_char_frames[id].id = id;
          }
          if (strcmp(token, "x") == 0) {
              token = strtok_s(NULL, " =\"", &next_token);
              m_font.raw_char_frames[id].x = atoi(token);
          }
          if (strcmp(token, "y") == 0) {
              token = strtok_s(NULL, " =\"", &next_token);
              m_font.raw_char_frames[id].y = atoi(token);
          }
          if (strcmp(token, "width") == 0) {
              token = strtok_s(NULL, " =\"", &next_token);
              m_font.raw_char_frames[id].width = atoi(token);
          }
          if (strcmp(token, "height") == 0) {
              token = strtok_s(NULL, " =\"", &next_token);
              m_font.raw_char_frames[id].height = atoi(token);
          }
          if (strcmp(token, "xoffset") == 0) {
              token = strtok_s(NULL, " =\"", &next_token);
              m_font.raw_char_frames[id].xoffset = atoi(token);
          }
          if (strcmp(token, "yoffset") == 0) {
              token = strtok_s(NULL, " =\"", &next_token);
              m_font.raw_char_frames[id].yoffset = atoi(token);
          }
          if (strcmp(token, "xadvance") == 0) {
              token = strtok_s(NULL, " =\"", &next_token);
              m_font.raw_char_frames[id].xadvance = atoi(token);
          }
          token = strtok_s(NULL, " =\"", &next_token);
         }
      }

    } else {
      throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
    }
  }

  fclose(pFile);
  m_has_font_been_loaded = true;
}

//------------------------------------------------------------------------------
// This is for debugging
//------------------------------------------------------------------------------
void Text_Component::Load_Character_Frames() {
  double position_left, position_right, position_top, position_bottom;
  double texture_left, texture_right, texture_top, texture_bottom;

  // Load the Character Frame Struct Array from the
  // Raw Character Frame Struct Array
  for (int index = 0; index < 256; index++) {
    if (m_font.raw_char_frames[index].id != -1) {
      D3DXVECTOR2 size = D3DXVECTOR2(static_cast<float>(m_font.raw_char_frames[index].width),
                                     static_cast<float>(m_font.raw_char_frames[index].height));
      D3DXVECTOR2 position = D3DXVECTOR2(0, 0);

      // Calculate the screen coordinates of the left side of the bitmap
      position_left = position.x;
      texture_left = m_font.raw_char_frames[index].x / m_font.image_width;

      // Calculate the screen coordinates of the right side of the bitmap
      position_right = position.x + size.x;
      texture_right = (m_font.raw_char_frames[index].x + size.x) / m_font.image_width;

      // Calculate the screen coordinates of the top of the bitmap
      position_top = position.y;
      texture_top = m_font.raw_char_frames[index].y / m_font.image_height;

      // Calculate the screen coordinates of the bottom of the bitmap
      position_bottom = position.y - size.y;
      texture_bottom = (m_font.raw_char_frames[index].y + size.y) / m_font.image_height;

      // First triangle in quad.
      // Top left
      m_font.character_frames[index][0].position = D3DXVECTOR3(static_cast<float>(position_left),
                                                               static_cast<float>(position_top),
                                                               0.0f);
      m_font.character_frames[index][0].texture = D3DXVECTOR2(static_cast<float>(texture_left),
                                                              static_cast<float>(texture_top));
      // Bottom right
      m_font.character_frames[index][1].position = D3DXVECTOR3(static_cast<float>(position_right),
                                                               static_cast<float>(position_bottom),
                                                               0.0f);
      m_font.character_frames[index][1].texture = D3DXVECTOR2(static_cast<float>(texture_right),
                                                              static_cast<float>(texture_bottom));
      // Bottom left
      m_font.character_frames[index][2].position = D3DXVECTOR3(static_cast<float>(position_left),
                                                               static_cast<float>(position_bottom),
                                                               0.0f);
      m_font.character_frames[index][2].texture = D3DXVECTOR2(static_cast<float>(texture_left),
                                                              static_cast<float>(texture_bottom));

      // Second triangle in quad.
      // Top left
      m_font.character_frames[index][3].position = m_font.character_frames[index][0].position;
      m_font.character_frames[index][3].texture = m_font.character_frames[index][0].texture;
      // Top right
      m_font.character_frames[index][4].position = D3DXVECTOR3(static_cast<float>(position_right),
                                                               static_cast<float>(position_top),
                                                               0.0f);
      m_font.character_frames[index][4].texture = D3DXVECTOR2(static_cast<float>(texture_right),
                                                              static_cast<float>(texture_top));
      // Bottom right
      m_font.character_frames[index][5].position = m_font.character_frames[index][1].position;
      m_font.character_frames[index][5].texture = m_font.character_frames[index][1].texture;
    }
  }
}


//------------------------------------------------------------------------------
void Text_Component::Create_String_Frame() {

  // Set Frame Size
  m_size = D3DXVECTOR2(0, 0);
  for (std::string::size_type i = 0; i < m_text.text.size(); i++) {
    char character_index = m_text.text.c_str()[i];
    m_size.x = static_cast<float>(m_size.x + m_font.raw_char_frames[character_index].width);
    if (m_size.y < m_font.raw_char_frames[character_index].height) {
      m_size.y = static_cast<float>(m_font.raw_char_frames[character_index].height);
    }
  }

  // Set the number of vertices in the vertex array.
  m_frame->vertex_count = 6 * m_text.text.size();

  // Set the number of indices in the index array.
  m_frame->index_count = m_frame->vertex_count;

  // Create the vertex array.
  m_frame->vertices = new Vertex_Type[m_frame->vertex_count];
  if (!m_frame->vertices) {
    throw Tunnelour::Exceptions::init_error("Creating the vertex array Failed!");
  }

  // Create the index array.
  m_frame->indices = new unsigned int[m_frame->index_count];
  if (!m_frame->indices) {
    throw Tunnelour::Exceptions::init_error("Creating the index array Failed!");
  }

  // Initialize vertex array to zeros at first.
  memset(m_frame->vertices, 0, (sizeof(Vertex_Type) * m_frame->vertex_count));

  // Load the index array with data.
  for (int i = 0; i < m_frame->index_count; i++)  {
    m_frame->indices[i] = i;
  }

  // Load the vertex array with data.
  int vertex_index = 0;
  double offset = 0;
  for (std::string::size_type i = 0; i < m_text.text.size(); i++) {
    char character_index = m_text.text.c_str()[i];
    for (int frame_index = 0; frame_index < 6; frame_index++) {
      m_frame->vertices[vertex_index] = m_font.character_frames[character_index][frame_index];
      m_frame->vertices[vertex_index].position.x += static_cast<float>(offset);

      if (i != 0) {
        m_frame->vertices[vertex_index].position.y -= (m_font.raw_char_frames[m_text.text.c_str()[0]].height - m_font.raw_char_frames[m_text.text.c_str()[i]].height);
      }

      if (character_index == 103 || character_index == 112 || character_index == 113 ) {
        if (m_text.text.c_str()[0] != 97) {
          m_frame->vertices[vertex_index].position.y += (m_font.raw_char_frames[97].height - m_font.raw_char_frames[m_text.text.c_str()[i]].height);
        }
      }
      vertex_index++;
    }
    offset += m_font.raw_char_frames[character_index].xadvance;
  }

  m_centre = D3DXVECTOR3(0, 0, 0);

  for (int i = 0; i < m_frame->vertex_count; i++) {
    m_centre.x += m_frame->vertices[i].position.x;
    m_centre.y += m_frame->vertices[i].position.y;
    m_centre.z += m_frame->vertices[i].position.z;
  }

  m_centre.x = m_centre.x / m_frame->vertex_count;
  m_centre.y = m_centre.y / m_frame->vertex_count;
  m_centre.z = m_centre.z / m_frame->vertex_count;
}


}  // namespace Tunnelour
