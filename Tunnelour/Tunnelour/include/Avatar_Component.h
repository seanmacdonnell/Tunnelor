//  Copyright 2014 Sean MacDonnell
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

     bool operator==(const Avatar_Collision_Block& rhs) const {
       if (id.compare(rhs.id) != 0) { return false; }
       if (is_contacting != rhs.is_contacting) { return false; }
       if (offset_from_avatar_centre != rhs.offset_from_avatar_centre) { return false; }
       if (size != rhs.size) { return false; } 
       return true;
     }
   };

  struct Avatar_State {
    std::string parent_state;
    std::string state;
    unsigned int state_index;
    int max_state_index;
    std::string direction;
    std::vector<Avatar_Collision_Block> avatar_collision_blocks;

    bool operator==(const Avatar_State& rhs) const {
      if (parent_state.compare(rhs.parent_state) != 0) { return false; }
      if (state.compare(rhs.state) != 0) { return false; }
      if (state_index != rhs.state_index) { return false; }
      if (max_state_index != rhs.max_state_index) { return false; }
      if (direction.compare(rhs.direction) != 0) { return false; }
      if (avatar_collision_blocks != rhs.avatar_collision_blocks) { return false; }
      return true;
    }

    bool operator!=(const Avatar_State& rhs) const {
      if (parent_state.compare(rhs.parent_state) != 0) { return true; }
      if (state.compare(rhs.state) != 0) { return true; }
      if (state_index != rhs.state_index) { return true; }
      if (max_state_index != rhs.max_state_index) { return true; }
      if (direction.compare(rhs.direction) != 0) { return true; }
      if (avatar_collision_blocks != rhs.avatar_collision_blocks) { return true; }
      return false;
    }
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

  Avatar_State GetLastRenderedState();
  void SetLastRenderedState(Avatar_Component::Avatar_State last_state);

  Avatar_State GetCommand();
  void SetCommand(Avatar_State current_command);

  

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Inits this components frame stucture
  //---------------------------------------------------------------------------
  void Init_Frame();

  Avatar_State m_state;
  Avatar_State m_last_rendered_state;
  Avatar_State m_initial_state;
  Avatar_State m_command;
};  // class Avatar_Component
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_COMPONENT_H_
