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

#ifndef TUNNELOUR_LEVEL_COMPONENT_H_
#define TUNNELOUR_LEVEL_COMPONENT_H_

#include "Component.h"
#include <vector>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Level_Component contains all the data for the current 
//              : game level and the past game level.
//-----------------------------------------------------------------------------
class Level_Component: public Tunnelour::Component {
 public:
  /* Example
  Size 128;Type Middleground,Size 128;Type Tunnel,Size 128;Type Tunnel
  Size 128;Type Middleground,Size 128;Type Tunnel,Size 128;Type Tunnel
  */
  struct Tile_Metadata {
    std::string type;
    float size;
  };

  struct Condition {
    std::string type;
    bool has_been_met;
  };

  struct End_Condition_Block_Intersect: public Condition {
    float avatar_intersecting_tile_top_left_x;
    float avatar_intersecting_tile_top_left_y;
    float avatar_intersecting_tile_size;
  };

  struct End_Condition_Avatar_State: public Condition {
    std::string avatar_state;
    bool when_state_complete;
    std::string avatar_direction;
  };

  struct End_Condition {
    std::string ID;
    bool is_and;
    bool is_or;
    std::vector<Condition*> conditions;
    std::string next_level;
  };

  /* Example
  Level_Name "Level 0"
  Level_Blurb "Testing Walking Animation. Walk Right and Left"
  Level_CSV_Filename "Level_0.csv"
  Level_TilesetName "Dirt"
  Level_TunnelStartTopLeftX "0"
  Level_TunnelStartTopLeftY "0"
  Level_Start_AvatarX "192"
  Level_Start_AvatarY "-448"
  Level_Start_AvatarState "Standing"
  Level_EndCondition_ID "1"
  Level_EndCondition_IsAnd "True"
  Level_EndCondition_IsOr "False"
  Level_EndCondition_NoOfConditions "2"
  Level_EndCondition_Type "State"
  Level_EndCondition_WhenAvatarState "Walking"
  Level_EndCondition_WhenAvatarStateComplete "True"
  Level_EndCondition_WhenAvatarDirection "Right"
  Level_EndCondition_Type "State"
  Level_EndCondition_WhenAvatarState "Walking"
  Level_EndCondition_WhenAvatarStateComplete "True"
  Level_EndCondition_WhenAvatarDirection "Left"
  Level_EndCondition_NextLevel "Level 1"
  */
  struct Level_Metadata {
    std::string level_name;
    std::string blurb;
    std::string filename;
    std::string tileset_name;
    float tunnel_top_left_x;
    float tunnel_top_left_y;
    float start_avatar_top_left_x;
    float start_avatar_top_left_y;
    std::string start_avatar_state;
    std::vector<End_Condition*> end_conditions;
    std::vector<std::vector<Tile_Metadata>> level;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Level_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  ~Level_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  void Init();

  Level_Metadata GetCurrentLevel();

  void SetCurrentLevel(Level_Metadata current_level);

  bool IsComplete();

  void SetIsComplete(bool is_complete);

 protected:

 private:
 Level_Metadata m_current_level_metadata;
 bool m_is_complete;
};
}  // namespace Tunnelour
#endif  //  TUNNELOUR_LEVEL_COMPONENT_H_
