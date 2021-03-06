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

#include "Tileset_Helper.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include "Exceptions.h"
#include "String_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Tileset_Helper::Tileset_Helper() {
}

//------------------------------------------------------------------------------
Tileset_Helper::~Tileset_Helper() {
}

//------------------------------------------------------------------------------
bool Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(std::string metadata_file, Tileset_Helper::Animation_Tileset_Metadata *out_metadata) {
  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_file.c_str(), "r") != 0) {
    std::string error = "Open Tileset Metadata Failed! " +  metadata_file;
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
    if (strcmp(token, "Tileset_Name") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->name = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_Name");
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_Type") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->type = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_Type");
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_FileName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->filename = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_FileName");
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->top_left_x = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_TopLeftX");
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->top_left_y = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_TopLeftY");
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->size_x = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_SizeX");
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->size_y = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_SizeY");
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_NumOfSubSets") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->number_of_subsets = atoi(token);
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_NumOfSubSets");
    }
  }

  if (out_metadata->number_of_subsets != 0) {
    for (int i = 0; i <  out_metadata->number_of_subsets; i++) {
      Animation_Subset temp_subset;

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Name") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.name = token;
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_Name");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Type") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.type = token;
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_Type");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.top_left_x = static_cast<float>(atof(token));
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_TopLeftX");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.top_left_y = static_cast<float>(atof(token));
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: \"SubSet_TopLeftY\" got \"" + line + "\"");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.size_x = static_cast<float>(atof(token));
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_SizeX");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.size_y = static_cast<float>(atof(token));
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_SizeY");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Tile_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.tile_size_x = static_cast<float>(atof(token));
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_Tile_SizeX");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Tile_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.tile_size_y = static_cast<float>(atof(token));
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_Tile_SizeY");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_NumOfFrames") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.number_of_frames = atoi(token);
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_NumOfFrames");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_FramesPerSecond") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.frames_per_second = atoi(token);
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_FramesPerSecond");
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Repeating") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          if (strcmp(token,"Y") == 0) {
            temp_subset.is_repeatable = true;
          } else {
            temp_subset.is_repeatable = false;
          }
        } else {
          throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_Repeating");
        }
      }

      for (unsigned int i = 0; i <  temp_subset.number_of_frames; i++) {
        Frame_Metadata temp_frame_metadata;

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_ID") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.id = atoi(token);
          } else {
            throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: \"Frame_ID\" got \"" + line + "\"");
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_CollisionBlock_NumOfBlocks") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.number_of_avatar_collision_blocks = atoi(token);
          } else {
            throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Frame_CollisionBlock_NumOfBlocks");
          }
        }

        for (unsigned int i = 0; i <  temp_frame_metadata.number_of_avatar_collision_blocks; i++) {
          Avatar_Collision_Block temp_avatar_collision_block;

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_ID") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_avatar_collision_block.id = token;
            } else {
              throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Frame_CollisionBlock_ID");
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_TopLeftX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_avatar_collision_block.top_left_x = static_cast<float>(atof(token));
            } else {
              throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Frame_CollisionBlock_TopLeftX");
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_TopLeftY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_avatar_collision_block.top_left_y = static_cast<float>(atof(token));
            } else {
              throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Frame_CollisionBlock_TopLeftY");
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_SizeX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_avatar_collision_block.size_x = static_cast<float>(atof(token));
            } else {
              throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Frame_CollisionBlock_SizeX");
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_SizeY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_avatar_collision_block.size_y = static_cast<float>(atof(token));
            } else {
              throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Frame_CollisionBlock_SizeY");
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_Contact") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              if (strcmp(token,"Y") == 0) {
                temp_avatar_collision_block.is_contacting = true;
              } else {
                temp_avatar_collision_block.is_contacting = false;
              }
            } else {
              throw Tunnelour::Exceptions::init_error("Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Frame_CollisionBlock_Contact");
            }
          }

          temp_frame_metadata.avatar_collision_blocks.push_back(temp_avatar_collision_block);
        }

        temp_subset.frames.push_back(temp_frame_metadata);
      }
    
       out_metadata->subsets.push_back(temp_subset);
    }
  }

  fclose(pFile);
  return true;
}

//---------------------------------------------------------------------------
bool Tileset_Helper::LoadTilesetMetadataIntoStruct(std::string metadata_file, Tileset_Helper::Tileset_Metadata *out_metadata) {
  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_file.c_str(), "r") != 0) {
    std::string error = "Open Tileset Metadata Failed! " +  metadata_file;
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
    if (strcmp(token, "Tileset_Name") == 0)   {
      token = strtok_s(NULL, "\"", &next_token);
      out_metadata->name = token;
    } else {
      std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_Name, got: ";
      error += token;
      throw Tunnelour::Exceptions::run_error(error);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_Type") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->type = token;
    } else {
      std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_Type, got: ";
      error += token;
      throw Tunnelour::Exceptions::run_error(error);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_FileName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->filename = token;
    } else {
      std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_FileName, got: ";
      error += token;
      throw Tunnelour::Exceptions::run_error(error);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->top_left_x = static_cast<float>(atof(token));
    } else {
      std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_TopLeftX, got: ";
      error += token;
      throw Tunnelour::Exceptions::run_error(error);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->top_left_y = static_cast<float>(atof(token));
    } else {
      std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_TopLeftY, got: ";
      error += token;
      throw Tunnelour::Exceptions::run_error(error);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->size_x = static_cast<float>(atof(token));
    } else {
      std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_SizeX, got: ";
      error += token;
      throw Tunnelour::Exceptions::run_error(error);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->size_y = static_cast<float>(atof(token));
    } else {
      std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_SizeY, got: ";
      error += token;
      throw Tunnelour::Exceptions::run_error(error);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_NumOfSubSets") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata->number_of_subsets = atoi(token);
    } else {
      std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: Tileset_NumOfSubSets, got: ";
      error += token;
      throw Tunnelour::Exceptions::run_error(error);
    }
  }

  if (out_metadata->number_of_subsets != 0) {
    for (int i = 0; i <  out_metadata->number_of_subsets; i++) {
      Tileset_Helper::Subset temp_tileset;

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Name") == 0)   {
        } else {
          std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_Name, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Type") == 0)   {
          token = strtok_s(NULL, "\"", &next_token);
          temp_tileset.type = token;
        } else {
          std::string error = "Parse Tileset Metadata Failed! (" + metadata_file + ") Expected: SubSet_Type, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.top_left_x = static_cast<float>(atof(token));
        } else {
          std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: SubSet_TopLeftX, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.top_left_y = static_cast<float>(atof(token));
        } else {
          std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: SubSet_TopLeftY, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftXOffset") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.top_left_x_offset = static_cast<float>(atof(token));
        } else {
          std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: SubSet_TopLeftXOffset, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftYOffset") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.top_left_y_offset = static_cast<float>(atof(token));
        } else {
          std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: SubSet_TopLeftYOffset, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.size_x = static_cast<float>(atof(token));
        } else {
          std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: SubSet_SizeX, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.size_y = static_cast<float>(atof(token));
        } else {
          std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: SubSet_SizeY, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_NumOfLines") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.number_of_lines = atoi(token);
        } else {
          std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: SubSet_NumOfLines, got: ";
          error += token;
          throw Tunnelour::Exceptions::run_error(error);
        }
      }

      if (temp_tileset.number_of_lines != 0) {
        for (int i = 0; i < temp_tileset.number_of_lines; i++) {
          Tileset_Helper::Line temp_line;

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_Name") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.line_number = atoi(token);
            } else {
              std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: Line_Name, got: ";
              error += token;
              throw Tunnelour::Exceptions::run_error(error);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_TopLeftX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.top_left_x = static_cast<float>(atof(token));
            } else {
              std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: Line_TopLeftX, got: ";
              error += token;
              throw Tunnelour::Exceptions::run_error(error);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_TopLeftY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.top_left_y = static_cast<float>(atof(token));
            } else {
              std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: Line_TopLeftY, got: ";
              error += token;
              throw Tunnelour::Exceptions::run_error(error);
            }

          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Tile_SizeX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.tile_size_x = static_cast<float>(atof(token));
            } else {
              std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: Tile_SizeX, got: ";
              error += token;
              throw Tunnelour::Exceptions::run_error(error);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Tile_SizeY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.tile_size_y = static_cast<float>(atof(token));
            } else {
              std::string error = "Parse Tileset Metadata Failed!  (" + metadata_file + ") Expected: Tile_SizeY, got: ";
              error += token;
              throw Tunnelour::Exceptions::run_error(error);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_NumOfTiles") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.number_of_tiles = atoi(token);
            } else {
              std::string error = "Metadata file: " + metadata_file;
              error.append(" Failed Parsing! Expected: Line_NumOfTiles, got: ");
              error.append(token);
              throw Tunnelour::Exceptions::run_error(error);
            }
          }
          temp_tileset.lines.push_back(temp_line);
        }
      }

      out_metadata->tilesets.push_back(temp_tileset);
    }
  }

  fclose(pFile);
  return true;
}

//---------------------------------------------------------------------------
Tileset_Helper::Line Tileset_Helper::GetCurrentSizedLine(float size, Tileset_Helper::Subset &tileset_subset) {
  Tileset_Helper::Line middleground_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  for (line = tileset_subset.lines.begin(); line != tileset_subset.lines.end(); line++) {
    if (line->tile_size_x == size) {
      if (line->tile_size_y == size) {
        middleground_line = *line;
      }
    }
  }
  return middleground_line;
}

//---------------------------------------------------------------------------
Tileset_Helper::Tileset_Metadata Tileset_Helper::GetNamedTileset(std::string name, std::vector<Tileset_Helper::Tileset_Metadata> tilesets) {
  Tileset_Helper::Tileset_Metadata found_tileset_metadata;

  std::vector<Tileset_Helper::Tileset_Metadata>::iterator tileset_metadata;
  for (tileset_metadata = tilesets.begin(); tileset_metadata != tilesets.end(); tileset_metadata++) {
    if (tileset_metadata->name.compare(name) == 0) {
      found_tileset_metadata = (*tileset_metadata);
    }
  }

  return found_tileset_metadata;
}

//---------------------------------------------------------------------------
Tileset_Helper::Subset Tileset_Helper::GetForegroundSubset(Tileset_Helper::Tileset_Metadata tileset_metadata) {
  Tileset_Helper::Subset found_subset;

  std::vector<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = tileset_metadata.tilesets.begin(); tileset != tileset_metadata.tilesets.end(); tileset++) {
    if (tileset->type.compare("Foreground") == 0) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}
} // Tunnelour