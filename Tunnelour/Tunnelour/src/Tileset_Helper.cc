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
bool Tileset_Helper::Load_Tileset_Metadata_Into_Struct(std::string metadata_file, Tileset_Helper::Animation_Tileset_Metadata &out_metadata) {
  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_file.c_str(), "r") != 0) {
    throw Tunnelour::Exceptions::init_error("Open Tileset Metadata Failed!");
    return false;
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
      out_metadata.name = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_Type") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.type = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_FileName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.filename = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.top_left_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.top_left_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.size_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.size_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_NumOfSubSets") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.number_of_subsets = atoi(token);
    }
  }

  if (out_metadata.number_of_subsets != 0) {
    for (int i = 0; i <  out_metadata.number_of_subsets; i++) {
      Animation_Subset temp_subset;

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Name") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.name = token;
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Type") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.type = token;
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.top_left_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.top_left_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.size_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.size_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Tile_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.tile_size_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Tile_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.tile_size_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_NumOfFrames") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.number_of_frames = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_FramesPerSecond") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.frames_per_second = atoi(token);
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
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_CollisionBlock_NumOfBlocks") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.number_of_collision_blocks = atoi(token);
          }
        }

        for (unsigned int i = 0; i <  temp_frame_metadata.number_of_collision_blocks; i++) {
          Collision_Block temp_collision_block;

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_ID") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_collision_block.id = token;
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_TopLeftX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_collision_block.top_left_x = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_TopLeftY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_collision_block.top_left_y = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_SizeX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_collision_block.size_x = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_SizeY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_collision_block.size_y = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Frame_CollisionBlock_Contact") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              if (strcmp(token,"Y") == 0) {
                temp_collision_block.is_contacting = true;
              } else {
                temp_collision_block.is_contacting = false;
              }
            }
          }

          temp_frame_metadata.collision_blocks.push_back(temp_collision_block);
        }

        temp_subset.frames.push_back(temp_frame_metadata);
      }
    
      out_metadata.subsets.push_back(temp_subset);
    }
  }

  return true;
}

} // Tunnelour