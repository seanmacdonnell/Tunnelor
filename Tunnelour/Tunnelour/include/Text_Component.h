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
    std::string text;
    std::string font_csv_file;
  };

  struct Raw_Character_Frame {
    Raw_Character_Frame() : id(-1),
                            x(-1),
                            y(-1),
                            width(-1),
                            height(-1),
                            xoffset(-1),
                            yoffset(-1),
                            xadvance(-1) {
    }
    double id;
    double x;
    double y;
    double width;
    double height;
    double xoffset;
    double yoffset;
    double xadvance;
  };

  struct Font {
    double image_width;
    double image_height;
    // This is the distance in pixels between each line of text.
    double line_height;
    std::string font_name;
    std::wstring font_texture_name;
    double character_widths[256];
    Raw_Character_Frame raw_character_frames[256];
    Vertex_Type character_frames[256][6];
    D3DXCOLOR font_color;
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

  //---------------------------------------------------------------------------
  // Description : Retrives a pointer to the font struct for this compoent
  //---------------------------------------------------------------------------
  Font* GetFont();

  //---------------------------------------------------------------------------
  // Description : Returns a pointer to the text structure.
  //---------------------------------------------------------------------------
  Text* GetText();

 protected:
  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  Text m_text;
  Font m_font;
 private:
  std::wstring CharToWChar(const char* pstrSrc);

  void Load_Font_Struct();
  void Load_Character_Frames();
  //---------------------------------------------------------------------------
  // Description : Inits this components frame stucture
  //---------------------------------------------------------------------------
  void Create_String_Frame();
  void Load_Font_Texture();
};  // class Text_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_TEXT_COMPONENT_H_
