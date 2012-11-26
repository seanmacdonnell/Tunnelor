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

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Text_Component::Text_Component(): Bitmap_Component() {
  m_text = new Tunnelour::Text_Component::Text();
  m_text->text = 0;
  m_text->font_csv_file = 0;

  m_font = new Tunnelour::Text_Component::Font();
  m_font->image_width = 0;
  m_font->image_height = 0;
  m_font->font_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

  m_texture->texture_path = L"resource\\";

  m_type = "Text_Component";
}

//------------------------------------------------------------------------------
Text_Component::~Text_Component()  {
  // Release the resources.
  if (m_text)  {
    delete m_text->text;
    delete m_text->font_csv_file;
    delete m_text;
  }
}

//------------------------------------------------------------------------------
void Text_Component::Init(ID3D11Device * const d3d11device) {
  Bitmap_Component::Init(d3d11device);

  Load_Font_Struct();

  Load_Character_Frames();

  Create_String_Frame();

  Load_Font_Texture();

  m_is_initialised = true;
}

//------------------------------------------------------------------------------
Text_Component::Font* Text_Component::GetFont() {
  return m_font;
}

//------------------------------------------------------------------------------
Text_Component::Text* Text_Component::GetText() {
  return m_text;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
std::wstring Text_Component::CharToWChar(const char* pstrSrc) {
    size_t origsize = strlen(pstrSrc) + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs_s(&convertedChars, wcstring, origsize, pstrSrc, _TRUNCATE);
    return std::wstring(wcstring);
}
//------------------------------------------------------------------------------
void Text_Component::Load_Font_Struct() {
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  // Open Font File as a text file
  pFile = fopen(m_text->font_csv_file,"r");
  if (pFile == NULL) {
    throw Tunnelour::Exceptions::init_error("Open font CSV file Failed!");
  }

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // Get the first line
	// Example line: "info face="Ariel" size=32 bold=0 italic=0 charset="" unicode=1 stretchH=100 smooth=1 aa=2 padding=0,0,0,0 spacing=1,1 outline=0"
  char line[225];
  fgets(line, 225, pFile);
  if (line != NULL) {
  char * token = strtok(line," ");
  if (strcmp(token,"info") == 0) 	{
		  while (token != NULL) {
        if (strcmp(token,"face") == 0) {
            token = strtok(NULL, " =\"");
            m_font->font_name = token;
        }
        token = strtok(NULL, " =\"");
		  }
    }
  } else {
    throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
  }

  // Get the second line
  // Example line: "common lineHeight=32 base=26 scaleW=512 scaleH=512 pages=1 packed=0 alphaChnl=1 redChnl=0 greenChnl=0 blueChnl=0"
  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"common") == 0) {
		  while (token != NULL) {
        if (strcmp(token,"lineHeight") == 0) {
            token = strtok(NULL, " =\"");
            m_font->line_height = atoi(token);
        }
        if (strcmp(token,"scaleW") == 0) {
            token = strtok(NULL, " =\"");
            m_font->image_width = atoi(token);
        }
        if (strcmp(token,"scaleH") == 0) {
            token = strtok(NULL, " =\"");
            m_font->image_height = atoi(token);
        }
        token = strtok(NULL, " =\"");
		  }
    }
  } else {
    throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
  }

  // Get the Third Line
  // Example line: "page id=0 file="Arial_0.dds""
  fgets(line, 225, pFile);
  if (line != NULL) {
    char * token = strtok(line," ");
    if (strcmp(token,"page") == 0) {
		    while (token != NULL) {
        if (strcmp(token,"file") == 0) {
            token = strtok(NULL, " =\"");
            m_font->font_texture_name = CharToWChar(token);
            m_texture->texture_path.append(m_font->font_texture_name);
        }
        token = strtok(NULL, " =\"");
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
    char * token = strtok(line," ");
    if (strcmp(token,"chars") == 0) {
		    while (token != NULL) {
        if (strcmp(token,"count") == 0) {
            token = strtok(NULL, " =\"");
            number_of_chars = atoi(token);
        }
        token = strtok(NULL, " =\"");
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
      char * token = strtok(line," ");
      if (strcmp(token,"char") == 0) {
        int id = 0;
	       while (token != NULL) {
          if (strcmp(token,"id") == 0) {
              token = strtok(NULL, " =\"");
              id = atoi(token);
              m_font->raw_character_frames[id].id = id;
          }
          if (strcmp(token,"x") == 0) {
              token = strtok(NULL, " =\"");
              m_font->raw_character_frames[id].x = atoi(token);
          }
          if (strcmp(token,"y") == 0) {
              token = strtok(NULL, " =\"");
              m_font->raw_character_frames[id].y = atoi(token);
          }
          if (strcmp(token,"width") == 0) {
              token = strtok(NULL, " =\"");
              m_font->raw_character_frames[id].width = atoi(token);
          }
          if (strcmp(token,"height") == 0) {
              token = strtok(NULL, " =\"");
              m_font->raw_character_frames[id].height = atoi(token);
          }
          if (strcmp(token,"xoffset") == 0) {
              token = strtok(NULL, " =\"");
              m_font->raw_character_frames[id].xoffset = atoi(token);
          }
          if (strcmp(token,"yoffset") == 0) {
              token = strtok(NULL, " =\"");
              m_font->raw_character_frames[id].yoffset = atoi(token);
          }
          if (strcmp(token,"xadvance") == 0) {
              token = strtok(NULL, " =\"");
              m_font->raw_character_frames[id].xadvance = atoi(token);
          }
          token = strtok(NULL, " =\"");
	       }
      }

    } else {
      throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
    }
  }

  fclose(pFile);
}

//------------------------------------------------------------------------------
// This is for debugging
//------------------------------------------------------------------------------
void Text_Component::Load_Character_Frames() {
  float position_left, position_right, position_top, position_bottom;
  float texture_left, texture_right, texture_top, texture_bottom;

  // Load the Character Frame Struct Array from the Raw Character Frame Struct Array
  for (int index = 0; index < 256; index++) {
    if (m_font->raw_character_frames[index].id != -1) {
      if (m_font->raw_character_frames[index].id == 55) {
        m_font->raw_character_frames[index].id = 55;
      }

      D3DXVECTOR2 size = D3DXVECTOR2(m_font->raw_character_frames[index].width, m_font->raw_character_frames[index].height);
      D3DXVECTOR2 position = D3DXVECTOR2(0, 0);
/*
      // Calculate the screen coordinates of the left side of the bitmap.
      position_left = position.x - (size.x / 2.0f);
      texture_left = m_font->raw_character_frames[index].x / m_font->image_width;

      // Calculate the screen coordinates of the right side of the bitmap.
      position_right = position.x + (size.x / 2.0f);
      texture_right = (m_font->raw_character_frames[index].x + size.x) / m_font->image_width;

      // Calculate the screen coordinates of the top of the bitmap.
      position_top = position.y + (size.y / 2.0f);
      texture_top = m_font->raw_character_frames[index].y / m_font->image_height;

      // Calculate the screen coordinates of the bottom of the bitmap.
      position_bottom = position.y - (size.y / 2.0f);
      texture_bottom = (m_font->raw_character_frames[index].y + size.y) / m_font->image_height;
*/
      // Calculate the screen coordinates of the left side of the bitmap.
      position_left = position.x;
      texture_left = m_font->raw_character_frames[index].x / m_font->image_width;

      // Calculate the screen coordinates of the right side of the bitmap.
      position_right = position.x + size.x;
      texture_right = (m_font->raw_character_frames[index].x + size.x) / m_font->image_width;

      // Calculate the screen coordinates of the top of the bitmap.
      position_top = position.y;
      texture_top = m_font->raw_character_frames[index].y / m_font->image_height;

      // Calculate the screen coordinates of the bottom of the bitmap.
      position_bottom = position.y - size.y;
      texture_bottom = (m_font->raw_character_frames[index].y + size.y) / m_font->image_height;

		    // First triangle in quad.
      // Top left.
		    m_font->character_frames[index][0].position = D3DXVECTOR3(position_left, position_top, 0.0f);
      m_font->character_frames[index][0].texture = D3DXVECTOR2(texture_left, texture_top);
      // Bottom right.
		    m_font->character_frames[index][1].position = D3DXVECTOR3(position_right, position_bottom, 0.0f);
		    m_font->character_frames[index][1].texture = D3DXVECTOR2(texture_right, texture_bottom);
      // Bottom left.
		    m_font->character_frames[index][2].position = D3DXVECTOR3(position_left, position_bottom, 0.0f);
		    m_font->character_frames[index][2].texture = D3DXVECTOR2(texture_left, texture_bottom);

		    // Second triangle in quad.
      // Top left.
		    m_font->character_frames[index][3].position = m_font->character_frames[index][0].position;
		    m_font->character_frames[index][3].texture = m_font->character_frames[index][0].texture;
      // Top right.
		    m_font->character_frames[index][4].position = D3DXVECTOR3(position_right, position_top, 0.0f);
		    m_font->character_frames[index][4].texture = D3DXVECTOR2(texture_right, texture_top);
      // Bottom right.
		    m_font->character_frames[index][5].position = m_font->character_frames[index][1].position;
		    m_font->character_frames[index][5].texture = m_font->character_frames[index][1].texture;
    }
  }
}


//------------------------------------------------------------------------------
void Text_Component::Create_String_Frame() {
  D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData, indexData;
  
    // Set Frame Size
  m_size = D3DXVECTOR2(0, 0);
  for (int i = 0; i < m_text->text->size(); i++) {
    char character_index = m_text->text->c_str()[i];
    m_size.x = m_size.x + m_font->raw_character_frames[character_index].width;
    if (m_size.y < m_font->raw_character_frames[character_index].height) {
      m_size.y = m_font->raw_character_frames[character_index].height;
    }
  }

  // Set the number of vertices in the vertex array.
  m_frame->vertex_count = 6 * m_text->text->size();

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
  float offset = 0;
  for (int i = 0; i < m_text->text->size(); i++) {
    char character_index = m_text->text->c_str()[i];
    for (int frame_index = 0; frame_index < 6; frame_index++) {
      m_frame->vertices[vertex_index] = m_font->character_frames[character_index][frame_index];
      m_frame->vertices[vertex_index].position.x += offset;
      vertex_index++;
    }
    offset += m_font->raw_character_frames[character_index].xadvance;
  }

  // Set up the description of the static vertex buffer.
  vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  vertexBufferDesc.ByteWidth = sizeof(Vertex_Type) * m_frame->vertex_count;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  // Give the subresource structure a pointer to the vertex data.
  vertexData.pSysMem = m_frame->vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  // Now create the vertex buffer.
  if (FAILED(m_d3d11device->CreateBuffer(&vertexBufferDesc,
                                         &vertexData,
                                         &(m_frame->vertex_buffer)))) {
    throw Tunnelour::Exceptions::init_error("CreateBuffer (vertex_buffer) Failed!");
  }

  // Set up the description of the static index buffer.
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_frame->index_count;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;
  indexBufferDesc.StructureByteStride = 0;

  // Give the subresource structure a pointer to the index data.
  indexData.pSysMem = m_frame->indices;
  indexData.SysMemPitch = 0;
  indexData.SysMemSlicePitch = 0;

  // Create the index buffer.
  if (FAILED(m_d3d11device->CreateBuffer(&indexBufferDesc,
                                         &indexData,
                                         &(m_frame->index_buffer)))) {
    throw Tunnelour::Exceptions::init_error("CreateBuffer (index_buffer) Failed!");
  }
}

//------------------------------------------------------------------------------
void Text_Component::Load_Font_Texture() {
  if (FAILED(D3DX11CreateShaderResourceViewFromFile(m_d3d11device,
                                                    m_texture->texture_path.c_str(),
                                                    NULL,
                                                    NULL,
                                                    &m_texture->texture,
                                                    NULL)))  {
    throw Tunnelour::Exceptions::init_error("D3DX11CreateShaderResourceViewFromFile Failed!");
  }
}
//------------------------------------------------------------------------------
// For Debugging
//------------------------------------------------------------------------------
/*void Text_Component::Load_Font_Texture() {
  if (FAILED(D3DX11CreateShaderResourceViewFromFile(m_d3d11device,
                                                    L"resource/Debug_Texture_001.dds",
                                                    NULL,
                                                    NULL,
                                                    &m_texture->texture,
                                                    NULL)))  {
    throw Tunnelour::Exceptions::init_error("D3DX11CreateShaderResourceViewFromFile Failed!");
  }
}*/

}  // namespace Tunnelour
