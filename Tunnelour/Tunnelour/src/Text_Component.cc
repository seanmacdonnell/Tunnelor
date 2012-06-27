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
  m_text->font_file = 0;


  m_font = new Tunnelour::Text_Component::Font();
  m_font->image_width = 0;
  m_font->image_height = 0;
  m_font->cell_width = 0;
  m_font->cell_height = 0;
  m_font->base_character_offset = 0;
  m_font->font_name = "";
  m_font->font_height = 0;
  m_font->font_width = 0;
  m_font->character_widths = new int[276];

  m_type = "Bitmap_Component";
}

//------------------------------------------------------------------------------
Text_Component::~Text_Component()  {
  // Release the resources.
  if (m_text)  {
    delete m_text->text;
    delete m_text->font_file;
    delete m_text;
  }

  if (m_font)  {
    delete m_font->font_name;
    delete m_font->character_widths;
    delete m_font;
  }
}

//------------------------------------------------------------------------------
void Text_Component::Init(ID3D11Device * const d3d11device) {
  Bitmap_Component::Init(d3d11device);
  // Turning String + font into bitmap texture here
  m_text->font_file = "resource\\Khmer_UI.csv";
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  // Open Font File as a binary file
  pFile = fopen(m_text->font_file,"r");
  if (pFile == NULL) {
    throw Tunnelour::Exceptions::init_error("Open font CSV file Failed!");
  }

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // Get Image Width
  char line[100];
  fgets(line, 100, pFile);
  if (line != NULL) {
    char * pch = strtok(line,",");
    m_font->image_width = atoi(strtok(NULL,","));
  } else {
    throw Tunnelour::Exceptions::init_error("Unexpected end of file!");
  }

  // Get Image Height
  // Get Cell Width
  // Get Cell Height
  // Get Base Character Offset
  // Get Font Name
  // Get Font Height
  // Get Font Width
  // Get Char Widths

  // Load font texture
  /*
  HRESULT result_memory = D3DX11CreateShaderResourceViewFromMemory(m_d3d11device,
                                                                   (LPCVOID)img,
                                                                   img_size,
                                                                   NULL,
                                                                   NULL,
                                                                   &m_texture->texture,
                                                                   NULL);
  if (FAILED(result_memory)) {
    throw Tunnelour::Exceptions::init_error("D3DX11CreateShaderResourceViewFromMemory Failed!");
  }
  */
  // Load Font Map

  // Read String
  // Create a Frame for each character
  // Load Texture for each frame

  m_is_initialised = true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
