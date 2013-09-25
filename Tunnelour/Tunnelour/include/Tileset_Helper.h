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

#ifndef TUNNELOUR_TILESET_HELPER_H_
#define TUNNELOUR_TILESET_HELPER_H_

#include <string>
#include <list>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This is a helper class for accessing parsing tilesets and
//              : modifying tileset objects.
//-----------------------------------------------------------------------------
class Tileset_Helper {
 public:
   struct Collision_Block {
     std::string id;
     bool is_contacting;
     float top_left_x, top_left_y;
     float size_x, size_y;
   };

  struct Frame_Metadata {
    unsigned int id;
    unsigned int number_of_collision_blocks;
    std::list<Collision_Block> collision_blocks;
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
    std::list<Frame_Metadata> frames;
  };

  struct Animation_Tileset_Metadata {
    std::string name;
    std::string type;
    std::string filename;
    float top_left_x, top_left_y;
    float size_x, size_y;
    int number_of_subsets;
    std::list<Animation_Subset> subsets;
  };

  struct Line {
    int line_number;
    float top_left_x, top_left_y;
    float tile_size_x, tile_size_y;
    int number_of_tiles;
  };

  struct Subset {
    std::string type;
    float top_left_x, top_left_y;
    float size_x, size_y;
    int number_of_lines;
    std::list<Line> lines;
  };

  struct Tileset_Metadata {
    std::string name;
    std::string type;
    std::string filename;
    float top_left_x, top_left_y;
    float size_x, size_y;
    int number_of_subsets;
    std::list<Subset> tilesets;
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
