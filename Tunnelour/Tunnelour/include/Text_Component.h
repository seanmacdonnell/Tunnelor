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
    char * font_file;
  };

  struct Font {
    int image_width;
    int image_height;
    int cell_width;
    int cell_height;
    int base_character_offset;
    char * font_name;
    int font_height;
    int font_width;
    int * character_widths;
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
};  // class Text_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_TEXT_COMPONENT_H_
