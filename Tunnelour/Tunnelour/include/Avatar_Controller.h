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

#ifndef TUNNELOUR_AVATAR_CONTROLLER_H_
#define TUNNELOUR_AVATAR_CONTROLLER_H_

#include <windows.h>
#include <vector>
#include <string>
#include <vector>

#include "Avatar_Component.h"
#include "Avatar_Controller_Mutator.h"
#include "Component_Composite.h"
#include "Controller.h"
#include "Game_Settings_Component.h"
#include "Tile_Bitmap.h"
#include "Tileset_Helper.h"
#include "Level_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the creation and mutation
//                of the avatar_component.
//-----------------------------------------------------------------------------
class Avatar_Controller: public Controller,
                         public Component_Composite::Component_Composite_Type_Observer {
 public:
  struct Collision {
    Tile_Bitmap* a_tile;
    Tile_Bitmap* b_tile;
    D3DXVECTOR2 a_tile_collision_point;
    D3DXVECTOR2 b_tile_collision_point;
  };

  struct Wall_Collision {
    Bitmap_Component* colliding_tile;
    std::string collision_side;
  };

  struct Avatar_Last_State {
    Avatar_Component::Avatar_State state;
    D3DXVECTOR3 position;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Avatar_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Avatar_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

  //---------------------------------------------------------------------------
  // Description : Called when handling an "Add" call in the model
  //---------------------------------------------------------------------------
  virtual void HandleEventAdd(Tunnelour::Component * const component);

  //---------------------------------------------------------------------------
  // Description : Called when handling a "Remove" call in the model
  //---------------------------------------------------------------------------
  virtual void HandleEventRemove(Tunnelour::Component * const component);

  void HideAvatar();

  void ShowAvatar();

  //---------------------------------------------------------------------------
  // Description : Resets the Avatar_Component tile to level defaults
  //---------------------------------------------------------------------------
  void ResetAvatar();

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Generates the Avatar_Component tile
  //---------------------------------------------------------------------------
  void CreateAvatar();

  //---------------------------------------------------------------------------
  // Description : Creates the Avatar_Component and adds it to the model
  //---------------------------------------------------------------------------
  void GenerateAvatar();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the state of the avatar
  //---------------------------------------------------------------------------
  void RunAvatarState();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the standing state of the avatar
  //---------------------------------------------------------------------------
  void RunStandingState();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the walking state of the avatar
  //---------------------------------------------------------------------------
  void RunWalkingState();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the walking state of the avatar
  //---------------------------------------------------------------------------
  void RunRunningState();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the falling state of the avatar
  //---------------------------------------------------------------------------
  void RunFallingState();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the jumping state of the avatar
  //---------------------------------------------------------------------------
  void RunJumpingState();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the jumping state of the avatar
  //---------------------------------------------------------------------------
  void RunClimbingState();

  //---------------------------------------------------------------------------
  // Description : Changes and maintains the transitioning state of the avatar
  //---------------------------------------------------------------------------
  void RunLevelTransitioningState();

  //---------------------------------------------------------------------------
  // Description : Returns true if the avatar is standing on a floor
  //---------------------------------------------------------------------------
  bool IsAvatarFloorAdjacent(std::vector<Bitmap_Component*> *adjacent_tiles);

  //---------------------------------------------------------------------------
  // Description : Returns true if the avatar is colliding with a floor
  //---------------------------------------------------------------------------
  bool IsAvatarFloorColliding(std::vector<Wall_Collision> *out_collisions);

  //---------------------------------------------------------------------------
  // Description : Returns true if the avatar is colliding with a floor
  //---------------------------------------------------------------------------
  bool IsAvatarWallColliding(std::vector<Wall_Collision> *out_collisions);

  double Avatar_Controller::WhatsTheDistanceBetweenThesePoints(D3DXVECTOR2 point_1, D3DXVECTOR2 point_2);

  bool DoTheseLinesIntersect(D3DXVECTOR2 line_a_begin, D3DXVECTOR2 line_a_end, D3DXVECTOR2 line_b_begin, D3DXVECTOR2 line_b_end, D3DXVECTOR2 *out_intersecting_point); 

  //---------------------------------------------------------------------------
  // Description : Loads all the tile animations into the controller
  //---------------------------------------------------------------------------
  void LoadTilesets(std::wstring wtileset_path);

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with the right foot position
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  void AlignAvatarOnRightFoot();

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with the right foot position
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  void AlignAvatarOnLastContactingFoot();

  void AlignAvatarOnLastHand();

  //---------------------------------------------------------------------------
  // Description : 
  //---------------------------------------------------------------------------
  void AlignAvatarOnLastLedgeEdge(Wall_Collision ledge);

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with the left foot position
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  void AlignAvatarOnLeftFoot();

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with avatar collision block
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  void AlignAvatarOnLastAvatarCollisionBlock();

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with avatar collision block
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  void AlignAvatarOnLastAvatarCollisionBlockRightTop();

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with avatar collision block
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  void AlignAvatarOnLastAvatarCollisionBlockLeftTop();

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with avatar collision block
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  void AlignAvatarOnLastAvatarCollisionBlockRightBottom();

  //---------------------------------------------------------------------------
  // Description : Syncs the avatar tile position with avatar collision block
  //             : of the last avatar animation frame.
  //---------------------------------------------------------------------------
  void AlignAvatarOnLastAvatarCollisionBlockLeftBottom();

  //---------------------------------------------------------------------------
  // Description : Repositions the avatar so he is standing adjacent to
  //             : a colliding wall tile.
  //---------------------------------------------------------------------------
  void MoveAvatarTileAdjacent(std::string direction, Bitmap_Component* tile);

  //---------------------------------------------------------------------------
  // Description : Repositions the avatar so he is standing adjacent to
  //             : a colliding floor tile.
  //---------------------------------------------------------------------------
  void MoveAvatarFloorAdjacent();

  //---------------------------------------------------------------------------
  // Description : Initialises the timer used for the animation ticks
  //---------------------------------------------------------------------------
  bool InitTimer();

  //---------------------------------------------------------------------------
  // Description : Updates and determines if the animation needs to update
  //---------------------------------------------------------------------------
  void UpdateTimer();

  //---------------------------------------------------------------------------
  // Description : Changes the avatar animation and state and/or direction
  //---------------------------------------------------------------------------
  void SetAvatarState(std::string new_state_parent_name, std::string new_state_name, std::string direction);

  //---------------------------------------------------------------------------
  // Description : Updates the avatar state animation
  //---------------------------------------------------------------------------
  void SetAvatarStateAnimationFrame(int new_state_index);

  //---------------------------------------------------------------------------
  // Description : Returns the lowest collision block on the avatar
  //---------------------------------------------------------------------------
  Avatar_Component::Avatar_Collision_Block GetLowestCollisionBlock(std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks);

  //---------------------------------------------------------------------------
  // Description : Returns the lowest and most towards the direction 
  //             : collision block on the avatar
  //---------------------------------------------------------------------------
  Avatar_Component::Avatar_Collision_Block GetLowestMostForwardFootCollisionBlock(std::string direction);

  //---------------------------------------------------------------------------
  // Description : Creates a Bitmap Component using the given collision block
  //---------------------------------------------------------------------------
  Bitmap_Component* CollisionBlockToBitmapComponent(Avatar_Component::Avatar_Collision_Block avatar_collision_block, D3DXVECTOR3 position);

  //---------------------------------------------------------------------------
  // Description : Creates a Bitmap Component using the given collision block
  //---------------------------------------------------------------------------
  Avatar_Component::Avatar_Collision_Block GetNamedCollisionBlock(std::string id, std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks);

  //---------------------------------------------------------------------------
  // Description : Creates an Avatar_Component::Avatar_Collision_Block from a
  //             : Tileset_Helper::Avatar_Collision_Block
  //---------------------------------------------------------------------------
  Avatar_Component::Avatar_Collision_Block TilesetCollisionBlockToAvatarCollisionBlock(Tileset_Helper::Avatar_Collision_Block tileset_avatar_collision_block,
                                                                                float tileset_animation_top_left_y,
                                                                                int state_index);

  void ClearAvatarState();

  bool CanAvatarGrabALedge(std::vector<Wall_Collision> *out_collisions);

  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  Avatar_Component *m_avatar;
  Game_Settings_Component* m_game_settings;
  Level_Component *m_level;
  World_Settings_Component *m_world_settings;

  std::string m_running_metadata_file_path;
  std::string m_walking_metadata_file_path;
  std::string m_standing_metadata_file_path;
  std::string m_falling_metadata_file_path;
  std::string m_level_transitioning_metadata_file_path;
  std::string m_jumping_metadata_file_path;
  std::string m_climbing_metadata_file_path;
  std::string m_current_metadata_file_path;
  Tileset_Helper::Animation_Tileset_Metadata m_running_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_walking_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_standing_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_falling_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_level_transitioning_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_jumping_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_climbing_metadata;
  Tileset_Helper::Animation_Tileset_Metadata m_current_metadata;
  Tileset_Helper::Animation_Subset m_current_animation_subset;
  std::vector<Tileset_Helper::Animation_Tileset_Metadata> m_animation_metadata;

  INT64 m_frequency;
  float m_ticksPerMs;
  INT64 m_startTime;
  float m_frameTime;
  int m_last_frame_time;
  bool m_animation_tick;
  int m_current_animation_fps;
  
  std::vector<Tile_Bitmap*> m_floor_tiles;
  std::vector<Tile_Bitmap*> m_wall_tiles;
  std::vector<Tile_Bitmap*> m_ledge_tiles;

  float m_avatar_z_position;

  std::string m_current_level_name;

  int m_y_fallen;
  bool m_wall_impacting;
  Avatar_Last_State m_last_state;

  int m_distance_traveled;

  Bitmap_Component *m_currently_grabbed_tile;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_CONTROLLER_H_
