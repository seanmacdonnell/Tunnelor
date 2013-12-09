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
#include <vector>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Avatar_Component is a component for a renderable 2D Sprite
//-----------------------------------------------------------------------------
class Avatar_Component: public Tunnelour::Bitmap_Component {
 public:
   struct Avatar_Collision_Block {
     std::string id;
     bool is_contacting;
     D3DXVECTOR3 offset_from_avatar_centre;
     D3DXVECTOR2 size;
   };

  struct Avatar_State {
    std::string parent_state;
    std::string state;
    unsigned int state_index;
    unsigned int max_state_index;
    std::string direction;
    std::vector<Avatar_Collision_Block> avatar_collision_blocks;
  };

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

  Avatar_State GetState();
  void SetState(Avatar_State state);

  Avatar_State GetLastState();

  Avatar_State GetCommand();
  void SetCommand(Avatar_State current_command);

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Inits this components frame stucture
  //---------------------------------------------------------------------------
  void Init_Frame();

  Avatar_State m_state;
  Avatar_State m_last_state;
  Avatar_State m_command;
};  // class Avatar_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_COMPONENT_H_
