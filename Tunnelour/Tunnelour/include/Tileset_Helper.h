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

#ifndef TUNNELOUR_TILESET_HELPER_H_
#define TUNNELOUR_TILESET_HELPER_H_

#include <string>
#include <vector>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This is a helper class for accessing parsing tilesets and
//              : modifying tileset objects.
//-----------------------------------------------------------------------------
class Tileset_Helper {
 public:
   struct Avatar_Collision_Block {
     std::string id;
     bool is_contacting;
     float top_left_x, top_left_y;
     float size_x, size_y;

     Avatar_Collision_Block() {
       id = "";
       is_contacting = false;
       top_left_x = 0;
       top_left_y = 0;
       size_x = 0;
       size_y = 0;
     }
   };

  struct Frame_Metadata {
    unsigned int id;
    unsigned int number_of_avatar_collision_blocks;
    std::vector<Avatar_Collision_Block> avatar_collision_blocks;

    Frame_Metadata() {
      id = 0;
      number_of_avatar_collision_blocks = 0;
    }
  };

  struct Animation_Subset {
    std::string name;
    std::string type;
    float top_left_x, top_left_y;
    float size_x, size_y;
    float tile_size_x, tile_size_y;
    unsigned int number_of_frames;
    unsigned int frames_per_second;
    bool is_repeatable;
    std::vector<Frame_Metadata> frames;

    Animation_Subset() {
      name = "";
      type = "";
      top_left_x = 0;
      top_left_y = 0;
      size_x = 0;
      size_y = 0;
      tile_size_x = 0;
      tile_size_y = 0;
      number_of_frames = 0;
      frames_per_second = 0;
      is_repeatable = false;
    }
  };

  struct Animation_Tileset_Metadata {
    std::string name;
    std::string type;
    std::string filename;
    float top_left_x;
    float top_left_y;
    float size_x;
    float size_y;
    int number_of_subsets;
    std::vector<Animation_Subset> subsets;

    Animation_Tileset_Metadata() {
      name = "";
      type + "";
      filename + "";
      top_left_x = 0;
      top_left_y = 0;
      size_x = 0;
      size_y = 0;
      number_of_subsets = 0;
    }
  };

  struct Line {
    int line_number;
    float top_left_x, top_left_y;
    float tile_size_x, tile_size_y;
    int number_of_tiles;

    Line() {
      line_number = 0;
      top_left_x = 0;
      top_left_y = 0;
      tile_size_x = 0;
      tile_size_y = 0;
      number_of_tiles = 0;;
    }
  };

  struct Subset {
    std::string type;
    float top_left_x, top_left_y;
    float top_left_x_offset, top_left_y_offset;
    float size_x, size_y;
    int number_of_lines;
    std::vector<Line> lines;

    Subset() {
      type = "";
      top_left_x = 0;
      top_left_y = 0;
      top_left_x_offset = 0;
      top_left_y_offset = 0;
      size_x = 0;
      size_y = 0;
      number_of_lines = 0;
    }
  };

  struct Tileset_Metadata {
    std::string name;
    std::string type;
    std::string filename;
    float top_left_x, top_left_y;
    float size_x, size_y;
    int number_of_subsets;
    std::vector<Subset> tilesets;

    Tileset_Metadata() {
      name = "";
      type = "";
      filename = "";
      top_left_x = 0;
      top_left_y = 0;
      size_x = 0;
      size_y = 0;
      number_of_subsets = 0;
    }
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Tileset_Helper();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Tileset_Helper();

  //---------------------------------------------------------------------------
  // Description : Loads the animation tileset metadata into a struct.
  //---------------------------------------------------------------------------
  static bool LoadAnimationTilesetMetadataIntoStruct(std::string metadata_file, Tileset_Helper::Animation_Tileset_Metadata *out_metadata);

  //---------------------------------------------------------------------------
  // Description : Loads the tileset metadata into a struct.
  //---------------------------------------------------------------------------
  static bool LoadTilesetMetadataIntoStruct(std::string metadata_file, Tileset_Helper::Tileset_Metadata *out_metadata);

 protected:

 private:
  
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_TILESET_HELPER_H_
