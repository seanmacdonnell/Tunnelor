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

#ifndef TUNNELOUR_LEVEL_CONTROLLER_H_
#define TUNNELOUR_LEVEL_CONTROLLER_H_

#include "Game_Settings_Component.h"
#include "Level_Controller_Mutator.h"
#include "Avatar_Component.h"
#include "Controller.h"
#include "Level_Component.h"
#include "Text_Component.h"
#include "Camera_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation
//              : and movement of the camera_component.
//-----------------------------------------------------------------------------
class Level_Controller: public Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Level_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Level_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

 protected:

 private:
  Avatar_Component *m_avatar;
  Camera_Component *m_camera;
  Game_Settings_Component *m_game_settings;
  Level_Component *m_level;

  //---------------------------------------------------------------------------
  // Description : Creates a Bitmap Component using the given collision block
  //--------------------------------------------------------------------------
  void LoadLevelMetadata();
  Level_Component::Level_Metadata LoadLevelMetadataIntoStruct(std::string metadata_path);
  void LoadLevelCSVIntoStruct(std::string metadata_path, Level_Component::Level_Metadata *out_struct);

  Avatar_Component::Collision_Block GetNamedCollisionBlock(std::string id, std::list<Avatar_Component::Collision_Block> collision_blocks);
  
  Text_Component *m_level_name_heading;
  Text_Component *m_level_blurb;
  std::string m_font_path;

};
}  // namespace Tunnelour
#endif  // TUNNELOUR_LEVEL_CONTROLLER_H_
