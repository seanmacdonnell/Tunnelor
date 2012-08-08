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

#ifndef TUNNELOUR_TEXT_COMPONENT_H_
#define TUNNELOUR_TEXT_COMPONENT_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11tex.h>
#include <string>
#include "Bitmap_Component.h"


namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Text_Component is a component for a renderable 2D Sprite
//-----------------------------------------------------------------------------
class Text_Component: public Tunnelour::Bitmap_Component {
 public:
  struct Text {
    std::string * text;
    char * font_csv_file;
  };

  struct Character_Frame {
    int id;
    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int xadvance;
  };

  struct Font {
    int image_width;
    int image_height;
    // This is the distance in pixels between each line of text.
    int line_height;
    char * font_name;
    std::wstring font_texture_name;
    int character_widths[256];
    Character_Frame character_frames[256];
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Text_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Text_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init(ID3D11Device * const d3d11device);

 protected:
  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  Text * m_text;
  Font * m_font;
 private:
   wchar_t* CharToWChar(const char* pstrSrc);

};  // class Text_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_TEXT_COMPONENT_H_
