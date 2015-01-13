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

#ifndef TUNNELOUR_AVATAR_STATE_CONTROLLER_H_
#define TUNNELOUR_AVATAR_STATE_CONTROLLER_H_

#include <vector>
#include <string>

#include "Component_Composite.h"
#include "Avatar_Component.h"
#include "Tile_Bitmap.h"
#include "Avatar_Helper.h"
#include "Controller.h"

#include "Game_Settings_Component.h"
#include "World_Settings_Component.h"


namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Avatar_State_Controller is a base type for controllers, they are designed
//                to read and modify the Composite_Component class (the model)
//-----------------------------------------------------------------------------
class Avatar_State_Controller: public Controller,
                               public Component_Composite::Component_Composite_Type_Observer {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Avatar_State_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Avatar_State_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Avatar_State_Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite *const model);

  //---------------------------------------------------------------------------
  // Description : Avatar_State_Controller Runner
  //---------------------------------------------------------------------------
  bool Run();

  //---------------------------------------------------------------------------
  // Description : Virtual function in order to run the avatar state
  //---------------------------------------------------------------------------
  virtual void Run_Avatar_State();

  //---------------------------------------------------------------------------
  // Description : Returns whether the avatars parent state has changed
  //---------------------------------------------------------------------------
  bool HasAvatarStateChanged();

  //---------------------------------------------------------------------------
  // Description : Sets the Avatar_Controller pointer
  //---------------------------------------------------------------------------
  void SetAvatarComponent(Avatar_Component *avatar_component);

  void SetFloorTiles(std::vector<Tile_Bitmap*> floor_tiles);

  void SetWallTiles(std::vector<Tile_Bitmap*> wall_tiles);

  void SetLedgeTiles(std::vector<Tile_Bitmap*> ledge_tiles);

  void SetGameSettings(Game_Settings_Component* game_settings);

  void SetAnimationMetadata(std::vector<Tileset_Helper::Animation_Tileset_Metadata> *animation_metadata);

  void SetCurrentAnimationSubset(Tileset_Helper::Animation_Subset *current_animation_subset);

  void SetCurrentlyAdjacentWallTile(Avatar_Helper::Tile_Collision *currently_adjacent_wall_tile);

  void SetCurrentMetadataFilePath(std::string *current_metadata_file_path);

  void SetCurrentMetadata(Tileset_Helper::Animation_Tileset_Metadata *current_metadata);

  void SetWorldSettings(World_Settings_Component *world_settings);

  void SetLastFrameTime(int *last_frame_time);

  void SetYFallen(float *y_fallen);

  void SetDistanceTraveled(float *distance_traveled);

  void SetIsMovingContinuously(bool *is_moving_continuously);

  void SetCurrentlyGrabbedTile(Bitmap_Component *& currently_grabbed_tile);

  //---------------------------------------------------------------------------
  // Description : Called when handling an "Add" call in the model
  //---------------------------------------------------------------------------
  virtual void HandleEventAdd(Tunnelour::Component * const component);

  //---------------------------------------------------------------------------
  // Description : Called when handling a "Remove" call in the model
  //---------------------------------------------------------------------------
  virtual void HandleEventRemove(Tunnelour::Component * const component);

 protected:
  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  Game_Settings_Component* m_game_settings;
  Avatar_Helper::Avatar_Stored_State m_initial_state;
  Avatar_Component *m_avatar;
  std::vector<Tile_Bitmap*> m_floor_tiles;
  std::vector<Tile_Bitmap*> m_wall_tiles;
  std::vector<Tile_Bitmap*> m_ledge_tiles;
  std::vector<Tileset_Helper::Animation_Tileset_Metadata> *m_animation_metadata;
  Tileset_Helper::Animation_Subset *m_current_animation_subset;
  Avatar_Helper::Tile_Collision *m_adjacent_wall;
  std::string *m_current_metadata_file_path;
  Tileset_Helper::Animation_Tileset_Metadata *m_current_metadata;
  World_Settings_Component *m_world_settings;
  int *m_last_frame_time;
  float *m_y_fallen;
  float *m_distance_traveled;
  bool *m_is_moving_continuously;
  Bitmap_Component ** m_currently_grabbed_tile;
 private:
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_STATE_CONTROLLER_H_
