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

#include "Debug_Text.h"
#include <stdio.h>
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Debug_Text::Debug_Text(): Text_Component() {
  m_text = new Tunnelour::Debug_Text::Text();
  m_text->text = 0;
  m_text->font_csv_file = 0;

  m_font = new Tunnelour::Debug_Text::Font();
  m_font->image_width = 0;
  m_font->image_height = 0;
  m_font->font_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

  m_position = D3DXVECTOR3(0, 0, 0);
  m_text->font_csv_file = "resource\\Arial_3.fnt";
  m_text->text = new std::string("ABCDEF");
}

//------------------------------------------------------------------------------
Debug_Text::~Debug_Text()  {
}

//------------------------------------------------------------------------------
void Debug_Text::Init(ID3D11Device * const d3d11device) {
  Text_Component::Init(d3d11device);
}

}  // namespace Tunnelour
