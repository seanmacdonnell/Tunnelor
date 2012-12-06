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

#include "Init_Controller.h"
#include "Background_Component.h"
#include "Camera_Component.h"
#include "Debug_Bitmap.h"
#include "Text_Component.h"
#include "Debug_Text.h"
#include "Bitmap_Component.h"
#include "SeaFloorSquare.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Init_Controller::Init_Controller() : Controller() {
}

//------------------------------------------------------------------------------
Init_Controller::~Init_Controller() {
}

//------------------------------------------------------------------------------
void Init_Controller::Init(Tunnelour::Component_Composite * const model) {
  Tunnelour::Controller::Init(model);
}

//------------------------------------------------------------------------------
void Init_Controller::Run() {
  // DO NOT DELETE THESE. I AM PASSING CONTROL TO THE MODEL FOR THESE.
  // IT SHOULD DO ALL THE CLEANUP AND DELETION.

  Tunnelour::Component *background = 0;
  background = m_model->Add(new Tunnelour::Background_Component());
  background->Init();

  Tunnelour::Component *camera = 0;
  camera = m_model->Add(new Tunnelour::Camera_Component());
  camera->Init();
  
  Tunnelour::Component *bitmap = 0;
  bitmap = m_model->Add(new Tunnelour::Debug_Bitmap());
  static_cast<Tunnelour::Debug_Bitmap*>(bitmap)->SetPosition(new D3DXVECTOR3(0, 0, -1));
  Tunnelour::Component *text = 0;
  //text = m_model->Add(new Tunnelour::Debug_Text());
  //static_cast<Tunnelour::Debug_Text*>(text)->GetText()->text = new std::string("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
  //static_cast<Tunnelour::Debug_Text*>(text)->SetPosition(new D3DXVECTOR3(0, 0, 0));
  text = m_model->Add(new Tunnelour::Debug_Text());
  static_cast<Tunnelour::Debug_Text*>(text)->GetText()->text = new std::string("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
  static_cast<Tunnelour::Debug_Text*>(text)->SetPosition(new D3DXVECTOR3(0, 0, -2));
  /*
  Tunnelour::Component *bitmap = 0;
  bitmap = m_model->Add(new Tunnelour::Debug_Bitmap());
  static_cast<Tunnelour::Debug_Bitmap*>(bitmap)->SetPosition(new D3DXVECTOR3(50, 50, 0));  
  bitmap = m_model->Add(new Tunnelour::Debug_Bitmap());
  static_cast<Tunnelour::Debug_Bitmap*>(bitmap)->GetTexture()->transparency = 0.8f;
  static_cast<Tunnelour::Debug_Bitmap*>(bitmap)->SetPosition(new D3DXVECTOR3(0, 0, -0.1));
  Tunnelour::Component *text = 0;
  text = m_model->Add(new Tunnelour::Debug_Text());
  static_cast<Tunnelour::Debug_Text*>(text)->SetPosition(new D3DXVECTOR3(10, 10, -1.2));
  text = m_model->Add(new Tunnelour::Debug_Text());
  static_cast<Tunnelour::Debug_Text*>(text)->SetPosition(new D3DXVECTOR3(0, 0, -1.3));
  static_cast<Tunnelour::Debug_Text*>(text)->GetText()->text = new std::string("TWO");
  */
  m_is_finished = true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
