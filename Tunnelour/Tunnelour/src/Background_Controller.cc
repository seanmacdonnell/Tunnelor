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

}  // namespace Tunnelour
