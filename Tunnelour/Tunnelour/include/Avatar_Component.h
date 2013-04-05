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

#ifndef TUNNELOUR_AVATAR_COMPONENT_H_
#define TUNNELOUR_AVATAR_COMPONENT_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11tex.h>
#include "Bitmap_Component.h"
#include <string>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Avatar_Component is a component for a renderable 2D Sprite
//-----------------------------------------------------------------------------
class Avatar_Component: public Tunnelour::Bitmap_Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Avatar_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Avatar_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  std::string GetState();
  void SetState(std::string state);

  unsigned int GetStateIndex();
  void SetStateIndex(unsigned int state_index);

  std::string GetLastCommand();
  void SetLastCommand(std::string last_command);

  std::string GetCurrentCommand();
  void SetCurrentCommand(std::string current_command);

  std::string GetNextCommand();
  void SetNextCommand(std::string next_command);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Inits this components frame stucture
  //---------------------------------------------------------------------------
  void Init_Frame();

  std::string m_state;
  unsigned int m_state_index;
  std::string m_last_command;
  std::string m_current_command;
  std::string m_next_command;
};  // class Avatar_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_COMPONENT_H_
