//  Copyright 2011 Sean MacDonnell
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

#ifndef TUNNELOUR_DIRECT3D11_VIEW_H_
#define TUNNELOUR_DIRECT3D11_VIEW_H_

#include "Component_Composite.h"
#include "View.h"
#include <windows.h>

namespace Tunnelour {
//-----------------------------------------------------------------------------
// Most of the code here was copied and pasted from the tutorials at
// http://rastertek.com/dx11tut02.html
//-----------------------------------------------------------------------------
class Direct3D11_View : public Tunnelour::View {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Direct3D11_View();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Direct3D11_View();

  //---------------------------------------------------------------------------
  // Description : Initialises this model for Running.
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite *model);

  //---------------------------------------------------------------------------
  // Description : View Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:
  Tunnelour::Component_Composite *m_model;

 private:
  LPCWSTR m_application_name;
  HINSTANCE m_hinstance;
  HWND m_hwnd;
  int m_screen_width;
  int m_screen_height;
  bool m_is_window_created;
  bool m_is_full_screen;

  void Create_Window();
};
}  // namespace Tunnelour

#endif  // TUNNELOUR_DIRECT3D11_VIEW_H_
