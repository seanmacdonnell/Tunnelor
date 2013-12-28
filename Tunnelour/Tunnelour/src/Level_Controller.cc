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

#include "Level_Controller.h"
#include "String_Helper.h"
#include "Exceptions.h"
#include "Get_Avatar_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Level_Controller::Level_Controller() : Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_level_name_heading = 0;
  m_level_blurb = 0;
  m_camera = 0;
  m_z_position = -3;
  m_background_controller = 0;
  m_middleground_controller = 0;
  m_level_tile_controller = 0;
  m_splash_screen_component = 0;
  m_level_transition_controller = 0;
  m_avatar_controller = 0; 
}

//------------------------------------------------------------------------------
Level_Controller::~Level_Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_level_name_heading = 0;
  m_level_blurb = 0;
  m_camera = 0;
}

//------------------------------------------------------------------------------
bool Level_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);

  if (m_level == 0) {
    m_level = new Level_Component();
    m_model->Add(m_level);
    m_level->Init();
  }
  Level_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    if (m_game_settings == 0) {
      m_game_settings = mutator.GetGameSettings();
    }
    if (m_camera == 0) {
      m_camera = mutator.GetCamera();
    }
    if (m_splash_screen_component == 0) {
      m_splash_screen_component = mutator.GetSplashScreen();
    }

    if (m_splash_screen_component->IsLoading()) {
      LoadLevelMetadata();
      m_font_path = "resource\\tilesets\\Ariel.fnt";

      if (m_level_tile_controller == 0) {
        m_level_tile_controller = new Level_Tile_Controller();
        m_level_tile_controller->Init(m_model);
        m_level_tile_controller->CreateLevel();
        m_level_tile_controller->AddLevelToModel();
      }

      if (m_avatar_controller == 0) {
        m_avatar_controller = new Tunnelour::Avatar_Controller();
        m_avatar_controller->Init(m_model);
      }

      m_has_been_initialised = true;
      m_splash_screen_component->SetIsLoading(false);
    }
  } else {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool Level_Controller::Run() {
  if (m_has_been_initialised) {
    if (m_splash_screen_component->HasSpaceBeenPressed()) {
      m_level_tile_controller->ShowLevel();
    }
    /*
    if (m_level_name_heading == 0 && m_level->GetCurrentLevel().level_name.size() != 0) {
      m_level_name_heading = new Text_Component();
      m_level_name_heading->GetText()->font_csv_file = m_font_path;
      m_level_name_heading->GetText()->text = m_level->GetCurrentLevel().level_name;
      m_level_name_heading->GetTexture()->transparency = 1.0f;
      m_level_name_heading->SetPosition(0, 0, m_z_position);
      m_model->Add(m_level_name_heading);
    }
    if (m_level_blurb == 0) {
      m_level_blurb = new Text_Component();
      m_level_blurb->GetText()->font_csv_file = m_font_path;
      m_level_blurb->GetText()->text = m_level->GetCurrentLevel().blurb;
      m_level_blurb->GetTexture()->transparency = 1.0f;
      m_level_blurb->SetPosition(0, 0, m_z_position);
      m_model->Add(m_level_blurb);
      m_level_blurb->Init();
    }
    if (m_level_name_heading->GetTexture()->transparency != 0.0) {
      float transparency = m_level_name_heading->GetTexture()->transparency;
      transparency = transparency - 0.01f;
      if (transparency < 0.0) { transparency = 0.0f; }
      m_level_name_heading->GetTexture()->transparency = transparency;
      m_level_blurb->GetTexture()->transparency = transparency;
    }
    */
    m_avatar_controller->Run();
    if (m_avatar == 0) {
      Get_Avatar_Mutator mutator;
      m_model->Apply(&mutator);
      if (mutator.WasSuccessful()) {
        m_avatar = mutator.GetAvatarComponent();
      }
    }
    /*
    m_level_name_heading->SetPosition(m_camera->GetPosition().x,
                                      m_camera->GetPosition().y + m_level_name_heading->GetSize().y, m_z_position);
    m_level_name_heading->Init();

    m_level_blurb->SetPosition(m_camera->GetPosition().x,
                               m_camera->GetPosition().y - m_level_name_heading->GetSize().y, m_z_position);
    m_level_blurb->Init();
    */
    // Check if the level end conditions are met
    if (m_avatar != 0) {
      if (m_level->GetCurrentLevel().end_conditions.size() != 0) {
        std::vector<Level_Component::End_Condition*> end_conditions = m_level->GetCurrentLevel().end_conditions;
        std::vector<Level_Component::End_Condition*>::iterator end_condition;
        for (end_condition = end_conditions.begin(); end_condition != end_conditions.end(); end_condition++) {
          bool and = true;
          bool or = false;
          std::vector<Level_Component::Condition*>::iterator condition;
          for (condition = (*end_condition)->conditions.begin(); condition != (*end_condition)->conditions.end(); condition++) {
            if ((*end_condition)->and) {
              if ((*condition)->type.compare("State") == 0) {
                Level_Component::End_Condition_Avatar_State *state_end_condition = static_cast<Level_Component::End_Condition_Avatar_State*>((*condition));
                if ((m_avatar->GetLastState().state.compare(state_end_condition->avatar_state) == 0 && m_avatar->GetLastState().direction.compare(state_end_condition->avatar_direction) != 0) || (*condition)->has_been_met == true) {
                  if (m_avatar->GetLastState().state.compare(m_avatar->GetState().state) != 0 || m_avatar->GetLastState().direction.compare(m_avatar->GetState().direction) != 0) {
                    (*condition)->has_been_met = true;
                  } else {
                    and = false;
                  }
                } else {
                  and = false;
                }
              } else {
                throw Tunnelour::Exceptions::run_error("Level_Controller: Unhandled State");
              }
            } else if ((*end_condition)->or) {
              if ((*condition)->type.compare("State") == 0) {
                Level_Component::End_Condition_Avatar_State *state_end_condition = static_cast<Level_Component::End_Condition_Avatar_State*>((*condition));
                if (m_avatar->GetLastState().state.compare(state_end_condition->avatar_state) == 0) {
                  if (m_avatar->GetLastState().direction.compare(state_end_condition->avatar_direction) == 0 || (*condition)->has_been_met == true) {
                    (*condition)->has_been_met = true;
                    or = true;
                  }
                }
              }
            }
          }
          if ((*end_condition)->and && and) {
            //m_model->Remove(m_level_name_heading);
            //m_level_name_heading = 0;
            //m_model->Remove(m_level_blurb);
            //m_level_blurb = 0;
            if ((*end_condition)->next_level.compare("QUIT") != 0) {
              //m_level->SetCurrentLevel(GetNamedLevel((*end_condition)->next_level));
              if (m_level_transition_controller == 0) {
                m_level_transition_controller = new Level_Transition_Controller();
                m_level_transition_controller->Init(m_model);
                m_level_transition_controller->Run();
              }
              m_level->SetIsComplete(true);
            } else {
              ///throw Tunnelour::Exceptions::run_error("QUIT");
              PostQuitMessage(0);
            }
          } else if ((*end_condition)->or && or) {
            //m_model->Remove(m_level_name_heading);
            //m_level_name_heading = 0;
            //m_model->Remove(m_level_blurb);
            //m_level_blurb = 0;
            if ((*end_condition)->next_level.compare("QUIT") != 0) {
              //m_level->SetCurrentLevel(GetNamedLevel((*end_condition)->next_level));
              //m_splash_screen_component->SetIsLoading(true);
              m_level->SetIsComplete(true);
            } else {
              //throw Tunnelour::Exceptions::run_error("QUIT");
              PostQuitMessage(0);
            }
          }
        }
      }
    }
    //m_background_controller->Run();
    //m_middleground_controller->Run();
    
    m_level_tile_controller->Run();
    
    if (m_level_transition_controller != 0) {
      m_level_transition_controller->Run();
    }
    
  } else {
    return false;
  }



  return true;
}

//---------------------------------------------------------------------------
void Level_Controller::LoadLevelMetadata() {
  Level_Component::Level_Metadata level_metadata;
  std::string level_metadata_file_path;
  level_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_0.txt");
  level_metadata = LoadLevelMetadataIntoStruct(level_metadata_file_path);
  std::string level_csv_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_0.csv");
  LoadLevelCSVIntoStruct(level_csv_file_path, &level_metadata);
  m_level->SetCurrentLevel(level_metadata);
  m_levels.push_back(level_metadata);
  level_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_1.txt");
  level_metadata = LoadLevelMetadataIntoStruct(level_metadata_file_path);
  level_csv_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_1.csv");
  LoadLevelCSVIntoStruct(level_csv_file_path, &level_metadata);
  m_levels.push_back(level_metadata);
  level_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_2.txt");
  level_metadata = LoadLevelMetadataIntoStruct(level_metadata_file_path);
  level_csv_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_2.csv");
  LoadLevelCSVIntoStruct(level_csv_file_path, &level_metadata);
  m_levels.push_back(level_metadata);
  level_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_3.txt");
  level_metadata = LoadLevelMetadataIntoStruct(level_metadata_file_path);
  level_csv_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_3.csv");
  LoadLevelCSVIntoStruct(level_csv_file_path, &level_metadata);
  m_levels.push_back(level_metadata);
  level_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_4.txt");
  level_metadata = LoadLevelMetadataIntoStruct(level_metadata_file_path);
  level_csv_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_4.csv");
  LoadLevelCSVIntoStruct(level_csv_file_path, &level_metadata);
  m_levels.push_back(level_metadata);
}

//------------------------------------------------------------------------------
Level_Component::Level_Metadata Level_Controller::LoadLevelMetadataIntoStruct(std::string metadata_path) {
  Level_Component::Level_Metadata level_metadata;

  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_path.c_str(), "r") != 0) {
    std::string error = "Open Tileset Metadata Failed! " +  metadata_path;
    throw Tunnelour::Exceptions::init_error(error);
  }

  // obtain file size:
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  char * token;
  char * next_token;

  char line[255];
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_Name") == 0)   {
      token = strtok_s(NULL, "\"", &next_token);
      level_metadata.level_name = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Name");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_Blurb") == 0)   {
      token = strtok_s(NULL, "\"", &next_token);
      level_metadata.blurb = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Blurb");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_CSV_Filename") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.filename = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_CSV_Filename");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TilesetName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tileset_name = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_TilesetName");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TunnelStartTopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tunnel_top_left_x = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_TunnelStartTopLeftX");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TunnelStartTopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tunnel_top_left_y = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_TunnelStartTopLeftY");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_Start_AvatarX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.start_avatar_top_left_x = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Start_AvatarX");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_Start_AvatarY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.start_avatar_top_left_y = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Start_AvatarY");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_Start_AvatarState") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.start_avatar_state = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Start_AvatarState");
    }
  }
  Level_Component::End_Condition *conditions = 0;
  conditions = new Level_Component::End_Condition();
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_EndCondition_ID") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      conditions->ID = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_ID");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_EndCondition_IsAnd") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      if (strcmp(token, "True") == 0) {
        conditions->and = true;
      } else {
        conditions->and = false;
      }
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_IsAnd");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_EndCondition_IsOr") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      if (strcmp(token, "True") == 0) {
        conditions->or = true;
      } else {
        conditions->or = false;
      }
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_IsOr");
    }
  }
  int NoOfConditions = 0;
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_EndCondition_NoOfConditions") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      NoOfConditions = static_cast<int>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_NoOfConditions");
    }
  }
  for (int i = 0; i < NoOfConditions; i++) {
    Level_Component::Condition *condition = 0;
    fgets(line, 225, pFile);
    if (line != NULL) {
      token = strtok_s(line, " ", &next_token);
      if (strcmp(token, "Level_EndCondition_Type") == 0)   {
        token = strtok_s(NULL, " =\"", &next_token);
        if (strcmp(token,"State") == 0) {
          condition = new Level_Component::End_Condition_Avatar_State();
        } else {
           throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Level_EndCondition_Type type was not recognized");
        }
        condition->type = token;
      } else {
        throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_Type");
      }
    }
    if (condition->type.compare("State") == 0) {
      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "Level_EndCondition_WhenAvatarState") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          static_cast<Level_Component::End_Condition_Avatar_State*>(condition)->avatar_state = token;
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_WhenAvatarState");
        }
      }
      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "Level_EndCondition_WhenAvatarStateComplete") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          if (strcmp(token, "True") == 0) {
            static_cast<Level_Component::End_Condition_Avatar_State*>(condition)->when_state_complete = true;
          } else {
            static_cast<Level_Component::End_Condition_Avatar_State*>(condition)->when_state_complete = false;
          }
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_WhenAvatarStateComplete");
        }
      }
      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "Level_EndCondition_WhenAvatarDirection") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          static_cast<Level_Component::End_Condition_Avatar_State*>(condition)->avatar_direction = token;
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_WhenAvatarDirection");
        }
      }
      condition->has_been_met = false;
      conditions->conditions.push_back(condition);
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_EndCondition_NextLevel") == 0)   {
      token = strtok_s(NULL, "\"", &next_token);
      conditions->next_level = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_EndCondition_NextLevel");
    }
  }
  level_metadata.end_conditions.push_back(conditions);

  fclose(pFile);

  return level_metadata;
}

//------------------------------------------------------------------------------
void Level_Controller::LoadLevelCSVIntoStruct(std::string metadata_path, Level_Component::Level_Metadata *out_metadata) {
  if (out_metadata == 0) { throw Tunnelour::Exceptions::run_error("Need an initialized strut"); }
  if (out_metadata->filename.compare("") == 0) { throw Tunnelour::Exceptions::run_error("Need a struct with a filename"); }
  Level_Component::Level_Metadata level_metadata;

  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_path.c_str(), "r") != 0) {
    std::string error = "Open Tileset Metadata Failed! " +  metadata_path;
    throw Tunnelour::Exceptions::init_error(error);
  }

  // obtain file size:
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  char * token;
  char * next_token;

  //This is string::max_size
  unsigned int max_size = 10000;
  char line[10000];
  while (fgets(line, max_size, pFile) != NULL) {
    std::vector<Level_Component::Tile_Metadata> line_metadata;
    std::vector<std::string> split_line = String_Helper::Split(line, ',');
    std::vector<std::string>::iterator line_tile;
    for (line_tile = split_line.begin(); line_tile != split_line.end(); line_tile++) {
      Level_Component::Tile_Metadata tile_metadata;
      std::vector<std::string> quote_stripper = String_Helper::Split((*line_tile), '\'');
      std::vector<std::string> split_tile = String_Helper::Split(quote_stripper[0], ';');
      std::vector<std::string>::iterator line_tile_data;
      for (line_tile_data = split_tile.begin(); line_tile_data != split_tile.end(); line_tile_data++) {
        std::vector<std::string> split_tile_data = String_Helper::Split((*line_tile_data), ' ');
        if (split_tile_data.begin()->compare("Size") == 0) {
          tile_metadata.size = static_cast<float>(atof(split_tile_data[1].c_str()));
        } else if (split_tile_data.begin()->compare("Type") == 0) {
          tile_metadata.type = split_tile_data[1];
        }
      }
      line_metadata.push_back(tile_metadata);
    }
    out_metadata->level.push_back(line_metadata);
  }

  fclose (pFile);
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_Collision_Block Level_Controller::GetNamedCollisionBlock(std::string id, std::list<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks) {
  Avatar_Component::Avatar_Collision_Block found_avatar_collision_block;

  Avatar_Component::Avatar_Collision_Block* current_right_foot_avatar_collision_block = 0;

  std::list<Avatar_Component::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = avatar_collision_blocks.begin(); avatar_collision_block != avatar_collision_blocks.end(); avatar_collision_block++) {
    if (avatar_collision_block->id.compare(id) == 0) {
      found_avatar_collision_block = (*avatar_collision_block);
    }
  }

  return found_avatar_collision_block;
}

//---------------------------------------------------------------------------
Level_Component::Level_Metadata Level_Controller::GetNamedLevel(std::string level_name) {
  Level_Component::Level_Metadata found_level;

  std::vector<Level_Component::Level_Metadata>::iterator level;
  for (level = m_levels.begin(); level != m_levels.end(); level++) {
    if (level->level_name.compare(level_name) == 0) {
      found_level = *level;
    }
  }

  return found_level;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // Tunnelour
