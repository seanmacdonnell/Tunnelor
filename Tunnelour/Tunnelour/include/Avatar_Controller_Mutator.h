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

#ifndef TUNNELOUR_AVATAR_CONTROLLER_MUTATOR_H_
#define TUNNELOUR_AVATAR_CONTROLLER_MUTATOR_H_

#include <list>

#include "Component.h"
#include "Game_Settings_Component.h"
#include "Tile_Bitmap.h"
#include "World_Settings_Component.h"

namespace Tunnelour {
class Avatar_Controller_Mutator: public Component::Component_Mutator  {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Avatar_Controller_Mutator is a mutator run on the model
//                which collects the game settings component.
//-----------------------------------------------------------------------------
 public:
  //-------------------------------------------------------------------------
  // Description : Constructor
  //-------------------------------------------------------------------------
  Avatar_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Deconstructor
  //-------------------------------------------------------------------------
  virtual ~Avatar_Controller_Mutator();

  //-------------------------------------------------------------------------
  // Description : Mutator function, pass it a component to modify.
  //-------------------------------------------------------------------------
  void Mutate(Component * const component);

  //-------------------------------------------------------------------------
  // Description : Accessors for the Game_Settings_Component
  //-------------------------------------------------------------------------
  Game_Settings_Component* const GetGameSettings();

  //-------------------------------------------------------------------------
  // Description : Accessor for the Floor Tiles
  //-------------------------------------------------------------------------
  std::list<Tile_Bitmap*> GetFloorTiles();

  //-------------------------------------------------------------------------
  // Description : Accessor for the Wall Tiles
  //-------------------------------------------------------------------------
  std::list<Tile_Bitmap*> GetWallTiles();

  //-------------------------------------------------------------------------
  // Description : Accessor for the World_Settings_Component
  //-------------------------------------------------------------------------
  World_Settings_Component* GetWorldSettings();

  //-------------------------------------------------------------------------
  // Description : Has this mutator completed successfully?
  //-------------------------------------------------------------------------
  bool WasSuccessful();

 private:
  bool m_found_game_settings;
  bool m_found_floor_tiles;
  bool m_found_world_settings;
  bool m_found_wall_tiles;
  Game_Settings_Component *m_game_settings;
  std::list<Tile_Bitmap*> m_floor_tiles;
  std::list<Tile_Bitmap*> m_wall_tiles;
  World_Settings_Component *m_world_settings;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_AVATAR_CONTROLLER_MUTATOR_H_
