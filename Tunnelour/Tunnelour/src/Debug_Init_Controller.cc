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

#include "Debug_Init_Controller.h"
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
Debug_Init_Controller::Debug_Init_Controller() : Controller() {
}

//------------------------------------------------------------------------------
Debug_Init_Controller::~Debug_Init_Controller() {
}

//------------------------------------------------------------------------------
void Debug_Init_Controller::Init(Tunnelour::Component_Composite * const model) {
  Tunnelour::Controller::Init(model);
}

//------------------------------------------------------------------------------
void Debug_Init_Controller::Run() {
  // DO NOT DELETE THESE. I AM PASSING CONTROL TO THE MODEL FOR THESE.
  // IT SHOULD DO ALL THE CLEANUP AND DELETION.

  Tunnelour::Component *camera = 0;
  camera = m_model->Add(new Tunnelour::Camera_Component());
  camera->Init();

  Tunnelour::Component *bitmap = 0;
  bitmap = m_model->Add(new Tunnelour::Debug_Bitmap());
  static_cast<Tunnelour::Debug_Bitmap*>(bitmap)->SetPosition(new D3DXVECTOR3(50, 50, -1));
  static_cast<Tunnelour::Debug_Bitmap*>(bitmap)->GetTexture()->transparency = 0.8f;
  bitmap = m_model->Add(new Tunnelour::Debug_Bitmap());
  static_cast<Tunnelour::Debug_Bitmap*>(bitmap)->SetPosition(new D3DXVECTOR3(-50, -50, 0));
  static_cast<Tunnelour::Debug_Bitmap*>(bitmap)->GetTexture()->transparency = 1.0f;

  Tunnelour::Component *text = 0;
  text = m_model->Add(new Tunnelour::Debug_Text());
  static_cast<Tunnelour::Debug_Text*>(text)->GetText()->text = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
  static_cast<Tunnelour::Debug_Text*>(text)->SetPosition(new D3DXVECTOR3(0, 0, 0));
  text = m_model->Add(new Tunnelour::Debug_Text());
  static_cast<Tunnelour::Debug_Text*>(text)->GetText()->text = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
  static_cast<Tunnelour::Debug_Text*>(text)->SetPosition(new D3DXVECTOR3(0, 0, -2));
  text = m_model->Add(new Tunnelour::Debug_Text());
  static_cast<Tunnelour::Debug_Text*>(text)->SetPosition(new D3DXVECTOR3(10.0f, 10.0f, -1.2f));
  text = m_model->Add(new Tunnelour::Debug_Text());
  static_cast<Tunnelour::Debug_Text*>(text)->SetPosition(new D3DXVECTOR3(0.0f, 0.0f, -1.3f));
  static_cast<Tunnelour::Debug_Text*>(text)->GetText()->text = "111111111111111111111111111111111111111111111111111111111111111111111";

  m_is_finished = true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
