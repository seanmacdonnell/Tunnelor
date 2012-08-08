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

  m_texture->texture_path = L"resource\\";

  m_type = "Bitmap_Component";
}

//------------------------------------------------------------------------------
Text_Component::~Text_Component()  {
  // Release the resources.
  if (m_text)  {
    delete m_text->text;
    delete m_text->font_csv_file;
    delete m_text;
  }

  if (m_font)  {
    delete m_font->font_name;
    delete m_font;
  }
}

//------------------------------------------------------------------------------
void Text_Component::Init(ID3D11Device * const d3d11device) {
  Bitmap_Component::Init(d3d11device);
  // Turning String + font into bitmap texture here
  m_text->font_csv_file = "resource\\Arial.fnt";
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

  // Load font texture
  if (FAILED(D3DX11CreateShaderResourceViewFromFile(m_d3d11device,
                                                    m_texture->texture_path.c_str(),
                                                    NULL,
                                                    NULL,
                                                    &m_texture->texture,
                                                    NULL)))  {
    throw Tunnelour::Exceptions::init_error("D3DX11CreateShaderResourceViewFromFile Failed!");
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
              m_font->character_frames[id].id = id;
          }
          if (strcmp(token,"x") == 0) {
              token = strtok(NULL, " =\"");
              m_font->character_frames[id].x = atoi(token);
          }
          if (strcmp(token,"y") == 0) {
              token = strtok(NULL, " =\"");
              m_font->character_frames[id].y = atoi(token);
          }
          if (strcmp(token,"width") == 0) {
              token = strtok(NULL, " =\"");
              m_font->character_frames[id].width = atoi(token);
          }
          if (strcmp(token,"height") == 0) {
              token = strtok(NULL, " =\"");
              m_font->character_frames[id].height = atoi(token);
          }
          if (strcmp(token,"xoffset") == 0) {
              token = strtok(NULL, " =\"");
              m_font->character_frames[id].xoffset = atoi(token);
          }
          if (strcmp(token,"yoffset") == 0) {
              token = strtok(NULL, " =\"");
              m_font->character_frames[id].yoffset = atoi(token);
          }
          if (strcmp(token,"xadvance") == 0) {
              token = strtok(NULL, " =\"");
              m_font->character_frames[id].xadvance = atoi(token);
          }
          token = strtok(NULL, " =\"");
	       }
      }

    } else {
      throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
    }
  }



  // Load Font Map
  //for (int index = 0; index <= 256; index++) {
    /*
			// First triangle in quad.
			m_font->character_frames[0].position = D3DXVECTOR3(m_font->character_widths[index]/2, m_font->character_widths[index]/2, 0.0f);  // Top left.
			m_font->character_frames[0].texture =  D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_Font[letter].size + 1.0f;
      */
  //}

  // Read String

  // Create a Frame for each character

  // Load Texture for each frame

  fclose(pFile);
  m_is_initialised = true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
wchar_t* Text_Component::CharToWChar(const char* pstrSrc) {
    size_t convertedChars = 0;
    int nLen = strlen(pstrSrc)+1;
    wchar_t* pwstr = (LPWSTR) malloc(sizeof(wchar_t )* nLen);
    
    mbstowcs(pwstr, pstrSrc, nLen);
    return pwstr;
}

}  // namespace Tunnelour
