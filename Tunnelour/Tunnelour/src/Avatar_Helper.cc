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

#include "Avatar_Helper.h"
#include "Bitmap_Helper.h"
#include "String_Helper.h"
#include "Geometry_Helper.h"
#include "Exceptions.h"


namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Avatar_Helper::Avatar_Helper() {
}

//------------------------------------------------------------------------------
Avatar_Helper::~Avatar_Helper() {
}

//------------------------------------------------------------------------------
bool Avatar_Helper::IsAvatarFloorAdjacent(Avatar_Component *m_avatar, std::vector<Tile_Bitmap*> *adjacent_tiles, std::vector<Tile_Bitmap*> *floor_tiles) {
  if (adjacent_tiles != 0) {
    adjacent_tiles->clear();
  }

  // Going to deal only with gravity only
  // Also only dealing with the lowest foot (Lowest collision Block).
  if (!floor_tiles->empty()) {
    // Find the lowest contact block
    // Get the lowest block most right/left block
    Avatar_Component::Avatar_Collision_Block avatar_avatar_collision_block;
    if (m_avatar->GetState().state.compare("Stopping") == 0) {
      avatar_avatar_collision_block = GetNamedCollisionBlock("Right_Foot", m_avatar->GetState().avatar_collision_blocks);
    } else {
      avatar_avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks);
    }
    Bitmap_Component *avatar_avatar_collision_block_bitmap;
    avatar_avatar_collision_block_bitmap = CollisionBlockToBitmapComponent(avatar_avatar_collision_block, (*m_avatar->GetPosition()));

    // Create a list of floor tiles which are adjacent with the collision block
    std::vector<Tile_Bitmap*>::iterator floor_tile;
    for (floor_tile = floor_tiles->begin(); floor_tile != floor_tiles->end(); floor_tile++) {
      if (Bitmap_Helper::DoTheseTilesXCollide(*floor_tile, avatar_avatar_collision_block_bitmap)) {
        float floor_top = (*floor_tile)->GetTopLeftPostion().y;
        float avatar_bottom = avatar_avatar_collision_block_bitmap->GetBottomRightPostion().y;
        if (floor_top == avatar_bottom) {
          adjacent_tiles->push_back(*floor_tile);
        }
      }
    }
    delete avatar_avatar_collision_block_bitmap;

    if (adjacent_tiles->empty()) {
      return false;
    }
  } else {
    return false;
  }

  // Adjacent tiles!
  return true;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_Collision_Block Avatar_Helper::GetNamedCollisionBlock(std::string id, std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks) {
  Avatar_Component::Avatar_Collision_Block found_avatar_collision_block;
  found_avatar_collision_block.id = "";

  std::vector<Avatar_Component::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = avatar_collision_blocks.begin(); avatar_collision_block != avatar_collision_blocks.end(); avatar_collision_block++) {
    if (avatar_collision_block->id.compare(id) == 0) {
      found_avatar_collision_block = (*avatar_collision_block);
    }
  }

  return found_avatar_collision_block;
}

//------------------------------------------------------------------------------
Bitmap_Component* Avatar_Helper::CollisionBlockToBitmapComponent(Avatar_Component::Avatar_Collision_Block avatar_collision_block, D3DXVECTOR3 position) {
  Bitmap_Component* collision_bitmap = new Bitmap_Component();

  D3DXVECTOR3 collision_bitmap_position;
  collision_bitmap_position.x = position.x + avatar_collision_block.offset_from_avatar_centre.x;
  collision_bitmap_position.y = position.y + avatar_collision_block.offset_from_avatar_centre.y;
  collision_bitmap_position.z = position.z;

  collision_bitmap->SetPosition(collision_bitmap_position);

  collision_bitmap->SetSize(avatar_collision_block.size.x, avatar_collision_block.size.y);

  return collision_bitmap;
}

//------------------------------------------------------------------------------
void Avatar_Helper::SetAvatarState(Avatar_Component *m_avatar, std::wstring tileset_path, std::vector<Tileset_Helper::Animation_Tileset_Metadata> *animation_metadata, std::string new_state_parent_name, std::string new_state_name, std::string direction, std::string *current_metadata_file_path, Tileset_Helper::Animation_Tileset_Metadata *current_metadata, Tileset_Helper::Animation_Subset *current_animation_subset) {
  Avatar_Component::Avatar_State new_state;
  Tileset_Helper::Animation_Tileset_Metadata new_state_metadata;
  Tileset_Helper::Animation_Subset new_animation_subset;

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  std::vector<Tileset_Helper::Animation_Tileset_Metadata>::iterator metadata;
  for (metadata = animation_metadata->begin(); metadata != animation_metadata->end(); metadata++) {
    if (metadata->name.compare(new_state_parent_name) == 0) {
      std::vector<Tileset_Helper::Animation_Subset>::iterator tileset;
      for (tileset = (*metadata).subsets.begin(); tileset != (*metadata).subsets.end(); tileset++) {
        if (tileset->name.compare(new_state_name) == 0) {
          new_animation_subset = *tileset;
          new_state_metadata = (*metadata);
          new_state.direction = direction;
          new_state.state = new_state_name;
          new_state.parent_state = metadata->name;
          new_state.max_state_index = tileset->number_of_frames;
        }
      }
    }
  }

  if (new_state_metadata.name.compare("") == 0) {
    std::string error;
    error = "Animation " + new_state_name;
    error += " not found in State " + new_state_parent_name + " Metadata";
    throw Exceptions::init_error(error);
  }

  new_state.state_index = 0;
  std::wstring texture_path = tileset_path;
  texture_path += String_Helper::StringToWString(new_state_metadata.filename);

  m_avatar->GetTexture()->texture_path = texture_path;
  m_avatar->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(new_state_metadata.size_x),
                                                     static_cast<float>(new_state_metadata.size_y));
  m_avatar->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(new_animation_subset.tile_size_x),
                                                  static_cast<float>(new_animation_subset.tile_size_y));
  m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(new_animation_subset.top_left_x),
                                                          static_cast<float>((new_animation_subset.top_left_y) * -1));
  m_avatar->SetSize(D3DXVECTOR2(static_cast<float>(new_animation_subset.tile_size_x),
                                static_cast<float>(new_animation_subset.tile_size_y)));

  // Find the initial frame of the new tile set state
  Tileset_Helper::Frame_Metadata initial_frame;
  std::vector<Tileset_Helper::Frame_Metadata>::iterator frame;
  for (frame = new_animation_subset.frames.begin(); frame != new_animation_subset.frames.end(); frame++) {
    if (frame->id == 1) {
      initial_frame = (*frame);
    }
  }

    // Create new collision block from the initial frame collision block
  std::vector<Tileset_Helper::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = initial_frame.avatar_collision_blocks.begin(); avatar_collision_block != initial_frame.avatar_collision_blocks.end(); avatar_collision_block++) {
    Avatar_Component::Avatar_Collision_Block new_avatar_collision_block;
    new_avatar_collision_block = TilesetCollisionBlockToAvatarCollisionBlock((*avatar_collision_block),
                                                                             new_animation_subset.top_left_y,
                                                                             new_state.state_index,
                                                                             direction);
    new_state.avatar_collision_blocks.push_back(new_avatar_collision_block);
  }

  m_avatar->SetState(new_state);

  m_avatar->GetTexture()->texture = 0;
  m_avatar->GetFrame()->vertex_buffer = 0;
  m_avatar->GetFrame()->index_buffer = 0;
  m_avatar->Init();

  *current_metadata_file_path = new_state_metadata.filename;
  *current_metadata = new_state_metadata;
  *current_animation_subset = new_animation_subset;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_Collision_Block Avatar_Helper::TilesetCollisionBlockToAvatarCollisionBlock(Tileset_Helper::Avatar_Collision_Block tileset_avatar_collision_block, float tileset_animation_top_left_y, int state_index, std::string direction) {
  Avatar_Component::Avatar_Collision_Block new_avatar_collision_block;

  // Create new collision block from the initial frame collision block
  new_avatar_collision_block.id = tileset_avatar_collision_block.id;
  new_avatar_collision_block.is_contacting = tileset_avatar_collision_block.is_contacting;
  new_avatar_collision_block.size.x = static_cast<float>(tileset_avatar_collision_block.size_x);
  new_avatar_collision_block.size.y = static_cast<float>(tileset_avatar_collision_block.size_y);

  // Need to change the coordinate information from the "animation sheet" to
  // the coordinate system in the game world.
  // Create a frame for the avatar foot
  Frame_Component avatar_contact_foot;
  D3DXVECTOR3 avatar_collision_block_tilesheet_centre;
  avatar_collision_block_tilesheet_centre.x = static_cast<float>(tileset_avatar_collision_block.top_left_x + (tileset_avatar_collision_block.size_x /2));
  avatar_collision_block_tilesheet_centre.y = static_cast<float>(tileset_avatar_collision_block.top_left_y - (tileset_avatar_collision_block.size_y /2));

  // Need to account for different positions in the frame and the avatar
  // Work out the center of the avatar frame (128x128 block) int the Tileset
  // frame is 128x128 so get the frame # and times it by 128/2 for y
  D3DXVECTOR3 animation_frame_centre;
  animation_frame_centre.x = static_cast<float>(((state_index + 1) * 128) - (128 / 2));
  // and 128/2 for x
  animation_frame_centre.y = static_cast<float>(((tileset_animation_top_left_y) - (128 / 2)));
  animation_frame_centre.z = static_cast<float>(-2);

  // store the distance from x and y to the center of the animation frame
  new_avatar_collision_block.offset_from_avatar_centre.x = avatar_collision_block_tilesheet_centre.x - animation_frame_centre.x;
  new_avatar_collision_block.offset_from_avatar_centre.y = avatar_collision_block_tilesheet_centre.y - animation_frame_centre.y;

  if (direction.compare("Left") == 0) {
    // We need to reverse the x on the collision block.
//    if (new_avatar_collision_block.id.compare("Left_Foot") == 0) {
      //new_avatar_collision_block.id = "Right_Foot";
      new_avatar_collision_block.offset_from_avatar_centre.x = (new_avatar_collision_block.offset_from_avatar_centre.x * -1);
//    } else if (new_avatar_collision_block.id.compare("Right_Foot") == 0) {
      //new_avatar_collision_block.id = "Left_Foot";
//      new_avatar_collision_block.offset_from_avatar_centre.x = (new_avatar_collision_block.offset_from_avatar_centre.x * -1);
//    }
  }

  return new_avatar_collision_block;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLastContactingFoot(Avatar_Component *avatar) {
  if (avatar->GetLastState().state.compare("") == 0) { return; }

  Avatar_Component::Avatar_Collision_Block last_right_foot;
  Avatar_Component::Avatar_Collision_Block last_left_foot;
  last_right_foot = GetNamedCollisionBlock("Right_Foot", avatar->GetLastRenderedState().avatar_collision_blocks);
  last_left_foot =  GetNamedCollisionBlock("Left_Foot", avatar->GetLastRenderedState().avatar_collision_blocks);

  Avatar_Component::Avatar_Collision_Block current_right_foot;
  Avatar_Component::Avatar_Collision_Block current_left_foot;
  current_right_foot = GetNamedCollisionBlock("Right_Foot", avatar->GetState().avatar_collision_blocks);
  current_left_foot =  GetNamedCollisionBlock("Left_Foot", avatar->GetState().avatar_collision_blocks);

  if (last_right_foot.is_contacting && current_right_foot.is_contacting) {
    Avatar_Helper::AlignAvatarOnRightFoot(avatar);
  } else if (last_left_foot.is_contacting && current_left_foot.is_contacting) {
    Avatar_Helper::AlignAvatarOnLeftFoot(avatar);
  } else {
    if (avatar->GetState().direction.compare("Right") == 0) {
      Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(avatar);
    } else {
      Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(avatar);
    }
  }
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnRightFoot(Avatar_Component *avatar) {
  if (avatar->GetLastRenderedState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Right_Foot", avatar->GetState().avatar_collision_blocks);
  if (avatar->GetState().direction.compare(avatar->GetLastRenderedState().direction) != 0) {
    current_avatar_collision_block.offset_from_avatar_centre.x = (current_avatar_collision_block.offset_from_avatar_centre.x * -1);
  }
  Bitmap_Component *current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  D3DXVECTOR3 current_position;
  current_position = current_collision_bitmap->GetBottomRightPostion();

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Right_Foot", avatar->GetLastRenderedState().avatar_collision_blocks);
  if (avatar->GetState().direction.compare(avatar->GetLastRenderedState().direction) != 0) {
    last_avatar_collision_block.offset_from_avatar_centre.x = (last_avatar_collision_block.offset_from_avatar_centre.x * -1);
  }
  Bitmap_Component *last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, avatar->GetLastRenderedPosition());
  D3DXVECTOR3 last_position;
  last_position = last_collision_bitmap->GetBottomRightPostion();

  if (current_position != last_position) {
    D3DXVECTOR3 difference;
    difference.x = last_position.x - current_position.x;
    difference.y = last_position.y - current_position.y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
    new_avatar_position += difference;

    avatar->SetPosition(new_avatar_position);
  }

  current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  current_position = current_collision_bitmap->GetBottomRightPostion();
  if (current_position != last_position) {
    throw Exceptions::run_error("AlignAvatarOnRightFoot: Failed to set avatar position correctly!");
  }

  delete current_collision_bitmap;
  delete last_collision_bitmap;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLeftFoot(Avatar_Component *avatar) {
  if (avatar->GetLastRenderedState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Left_Foot", avatar->GetState().avatar_collision_blocks);
  Bitmap_Component *current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  D3DXVECTOR3 current_bottom_right = current_collision_bitmap->GetBottomRightPostion();

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Left_Foot", avatar->GetLastRenderedState().avatar_collision_blocks);
  Bitmap_Component *last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, avatar->GetLastRenderedPosition());
  D3DXVECTOR3 last_bottom_right = last_collision_bitmap->GetBottomRightPostion();

  if (current_bottom_right != last_bottom_right) {
    D3DXVECTOR3 difference;
    difference.x = last_bottom_right.x - current_bottom_right.x;
    difference.y = last_bottom_right.y - current_bottom_right.y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
    new_avatar_position += difference;

    avatar->SetPosition(new_avatar_position);
  }

  current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  current_bottom_right = current_collision_bitmap->GetBottomRightPostion();
  if (current_bottom_right != last_bottom_right) {
    throw Exceptions::run_error("AlignAvatarOnRightFoot: Failed to set avatar position correctly!");
  }

  delete current_collision_bitmap;
  delete last_collision_bitmap;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(Avatar_Component *avatar) {
  if (avatar->GetLastRenderedState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks);
  Bitmap_Component *current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  D3DXVECTOR3 current_bottom_right = current_collision_bitmap->GetBottomRightPostion();

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetLastRenderedState().avatar_collision_blocks);
  // This fixes a bug where the GetNamedCollisionBlock uses the current avatars direction to determine if the
  // x offsets should be reversed (because the avatar would be facing left, and all the offsets are using right facing sprites
  // but if the last state was facing in the opposite direction then the alignment calculation would be off, so
  // this detects and fixes this alignment issue.
  if (avatar->GetLastRenderedState().direction.compare(avatar->GetState().direction) != 0) {
    last_avatar_collision_block.offset_from_avatar_centre.x = (last_avatar_collision_block.offset_from_avatar_centre.x * -1);
  }
  Bitmap_Component *last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, *(avatar->GetPosition()));//m_avatar->GetLastRenderedPosition() );//
  D3DXVECTOR3 last_bottom_right = last_collision_bitmap->GetBottomRightPostion();

  if (current_bottom_right != last_bottom_right) {
    D3DXVECTOR3 difference;
    difference.x = last_bottom_right.x - current_bottom_right.x;
    difference.y = last_bottom_right.y - current_bottom_right.y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
    new_avatar_position += difference;

    avatar->SetPosition(new_avatar_position);
  }

  current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  current_bottom_right = current_collision_bitmap->GetBottomRightPostion();
  if (current_bottom_right != last_bottom_right) {
    throw Exceptions::run_error("AlignAvatarOnLastAvatarCollisionBlockRightBottom: Failed to set avatar position correctly!");
  }

  delete current_collision_bitmap;
  delete last_collision_bitmap;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(Avatar_Component *avatar) {
  if (avatar->GetLastRenderedState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks);
  Bitmap_Component *current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  D3DXVECTOR3 current_bottom_right = current_collision_bitmap->GetBottomRightPostion();
  D3DXVECTOR3 current_top_left = current_collision_bitmap->GetTopLeftPostion();
  D3DXVECTOR3 current_bottom_left = D3DXVECTOR3(current_top_left.x, current_bottom_right.y, 0);

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetLastRenderedState().avatar_collision_blocks);
  Bitmap_Component *last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, *(avatar->GetPosition()));
  D3DXVECTOR3 last_bottom_right = last_collision_bitmap->GetBottomRightPostion();
  D3DXVECTOR3 last_top_left = last_collision_bitmap->GetTopLeftPostion();
  D3DXVECTOR3 last_bottom_left = D3DXVECTOR3(last_top_left.x, last_bottom_right.y, 0);

  if (current_bottom_left != last_bottom_left) {
    D3DXVECTOR3 difference;
    difference.x = last_bottom_left.x - current_bottom_left.x;
    difference.y = last_bottom_left.y - current_bottom_left.y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
    new_avatar_position += difference;

    avatar->SetPosition(new_avatar_position);
  }

  current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  current_bottom_right = current_collision_bitmap->GetBottomRightPostion();
  current_top_left = current_collision_bitmap->GetTopLeftPostion();
  current_bottom_left = D3DXVECTOR3(current_top_left.x, current_bottom_right.y, 0);
  if (current_bottom_left != last_bottom_left) {
    throw Exceptions::run_error("AlignAvatarOnLastAvatarCollisionBlockLeftBottom: Failed to set avatar position correctly!");
  }

  delete current_collision_bitmap;
  delete last_collision_bitmap;
}

//------------------------------------------------------------------------------
void Avatar_Helper::MoveAvatarTileAdjacent(Avatar_Component *avatar, std::string direction, Bitmap_Component* tile) {
  D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
  if (direction.compare("Right") == 0) {
    D3DXVECTOR3 tile_position = tile->GetBottomRightPostion();
    float foot_x_offset = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks).offset_from_avatar_centre.x - (GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks).size.x / 2);
    new_avatar_position.x = tile_position.x - foot_x_offset;
  } else if (direction.compare("Left") == 0) {
    D3DXVECTOR3 tile_position = tile->GetTopLeftPostion();
    float foot_x_offset = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks).offset_from_avatar_centre.x + (GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks).size.x / 2);
    new_avatar_position.x = tile_position.x - foot_x_offset;
  } else if (direction.compare("Top") == 0) {
    D3DXVECTOR3 tile_position = tile->GetTopLeftPostion();
    float foot_y_offset = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks).offset_from_avatar_centre.y - (GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks).size.y / 2);
    new_avatar_position.y = tile_position.y - foot_y_offset;
  }
  avatar->SetPosition(new_avatar_position);
}


//------------------------------------------------------------------------------
void Avatar_Helper::SetAvatarStateAnimationFrame(Avatar_Component *avatar, unsigned int new_state_index, Tileset_Helper::Animation_Subset *current_animation_subset) {
  Avatar_Component::Avatar_State incremented_state;

  incremented_state.state = avatar->GetState().state;
  incremented_state.direction = avatar->GetState().direction;
  incremented_state.state_index = new_state_index;
  incremented_state.parent_state = avatar->GetState().parent_state;

  // Set new bitmap frame location on the Tileset
  avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(current_animation_subset->top_left_x + (new_state_index * current_animation_subset->tile_size_x)),
                                                        static_cast<float>(current_animation_subset->top_left_y * -1));

  // Find the new frame of the current tile set
  Tileset_Helper::Frame_Metadata new_frame;
  std::vector<Tileset_Helper::Frame_Metadata>::iterator frame;
  for (frame = current_animation_subset->frames.begin(); frame != current_animation_subset->frames.end(); frame++) {
    if (frame->id == (new_state_index + 1)) {
      new_frame = (*frame);
    }
  }

  // Create new collision block from the initial frame collision block
  std::vector<Tileset_Helper::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = new_frame.avatar_collision_blocks.begin(); avatar_collision_block != new_frame.avatar_collision_blocks.end(); avatar_collision_block++) {
    Avatar_Component::Avatar_Collision_Block new_avatar_collision_block;
    new_avatar_collision_block = Avatar_Helper::TilesetCollisionBlockToAvatarCollisionBlock((*avatar_collision_block),
                                                                                              current_animation_subset->top_left_y,
                                                                                              new_state_index,
                                                                                              avatar->GetState().direction);
    incremented_state.avatar_collision_blocks.push_back(new_avatar_collision_block);
  }

  avatar->SetState(incremented_state);

  avatar->GetTexture()->texture = 0;
  avatar->GetFrame()->vertex_buffer = 0;
  avatar->GetFrame()->index_buffer = 0;
  avatar->Init();
}

//------------------------------------------------------------------------------
bool Avatar_Helper::IsAvatarWallColliding(Avatar_Component *avatar, std::vector<Tile_Collision> *out_collisions, std::vector<Tile_Bitmap*> *wall_tiles) {
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot
  if (!wall_tiles->empty()) {
    // Get the lowest block
    Avatar_Component::Avatar_Collision_Block avatar_collision_block;
    avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks);

    // Make a bitmap for the lowest avatar collision block
    Bitmap_Component *out_avatar_collision_block = CollisionBlockToBitmapComponent(avatar_collision_block, *(avatar->GetPosition()));

    // Create a list of floor tiles which are colliding with the collision block
    std::vector<Tile_Bitmap*>::iterator border_tile;
    for (border_tile = wall_tiles->begin(); border_tile != wall_tiles->end(); border_tile++) {
      if ((*border_tile)->IsRightWall() && avatar->GetBottomRightPostion().x >= (*border_tile)->GetTopLeftPostion().x ||
          (*border_tile)->IsLeftWall() && avatar->GetTopLeftPostion().x <= (*border_tile)->GetBottomRightPostion().x) {
        D3DXVECTOR2 Tile_Top_Left;
        Tile_Top_Left.x = (*border_tile)->GetTopLeftPostion().x;
        Tile_Top_Left.y = (*border_tile)->GetTopLeftPostion().y;
        D3DXVECTOR2 Tile_Top_Right;
        Tile_Top_Right.x = (*border_tile)->GetBottomRightPostion().x;
        Tile_Top_Right.y = (*border_tile)->GetTopLeftPostion().y;
        D3DXVECTOR2 Tile_Bottom_Left;
        Tile_Bottom_Left.x = (*border_tile)->GetTopLeftPostion().x;
        Tile_Bottom_Left.y = (*border_tile)->GetBottomRightPostion().y;
        D3DXVECTOR2 Tile_Bottom_Right;
        Tile_Bottom_Right.x = (*border_tile)->GetBottomRightPostion().x;
        Tile_Bottom_Right.y = (*border_tile)->GetBottomRightPostion().y;

        D3DXVECTOR2 Current_Avatar_Position;
        D3DXVECTOR3 velocity = avatar->GetVelocity();
        std::string avatar_direction;
        if (velocity.x == 0) {
          avatar_direction = avatar->GetState().direction;
          if (avatar_direction.compare("Right") == 0) {
            Current_Avatar_Position.x = out_avatar_collision_block->GetBottomRightPostion().x;
          } else {
            Current_Avatar_Position.x = out_avatar_collision_block->GetTopLeftPostion().x;
          }
        } else {
          if (velocity.x > 0) {
            Current_Avatar_Position.x = out_avatar_collision_block->GetBottomRightPostion().x;
          } else {
            Current_Avatar_Position.x = out_avatar_collision_block->GetTopLeftPostion().x;
          }
        }
        Current_Avatar_Position.y = out_avatar_collision_block->GetBottomRightPostion().y;

        Avatar_Component::Avatar_Collision_Block last_collision_block;
        last_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetLastRenderedState().avatar_collision_blocks);
        Bitmap_Component *lowest_last_collision_bitmap = CollisionBlockToBitmapComponent(last_collision_block, avatar->GetLastRenderedPosition());
        D3DXVECTOR2 Last_Avatar_Position;
        Last_Avatar_Position.x = lowest_last_collision_bitmap->GetPosition()->x;
        if (velocity.x == 0) {
          avatar_direction = avatar->GetState().direction;
          if (avatar_direction.compare("Right") == 0) {
            Last_Avatar_Position.x = lowest_last_collision_bitmap->GetBottomRightPostion().x;
          } else {
            Last_Avatar_Position.x = lowest_last_collision_bitmap->GetTopLeftPostion().x;
          }
        } else {
          if (velocity.x > 0) {
            Last_Avatar_Position.x = lowest_last_collision_bitmap->GetBottomRightPostion().x;
          } else {
            Last_Avatar_Position.x = lowest_last_collision_bitmap->GetTopLeftPostion().x;
          }
        }
        Last_Avatar_Position.y = lowest_last_collision_bitmap->GetBottomRightPostion().y;


        D3DXVECTOR2 floor_intersection;
        bool isthereafloor_intersection = Geometry_Helper::DoTheseLinesIntersect(Tile_Top_Left, Tile_Top_Right, Current_Avatar_Position, Last_Avatar_Position, &floor_intersection);

        D3DXVECTOR2 left_wall_intersection;
        bool istherealeft_wall_intersection = false;
        // This ensures that x adjacent tiles are not counted as wall collisions
        if (Tile_Top_Left.y != Current_Avatar_Position.y && Tile_Top_Left.y != Last_Avatar_Position.y) {
          istherealeft_wall_intersection = Geometry_Helper::DoTheseLinesIntersect(Tile_Top_Left, Tile_Bottom_Left, Current_Avatar_Position, Last_Avatar_Position, &left_wall_intersection);
        }

        D3DXVECTOR2 right_wall_intersection;
        bool istherearight_wall_intersection = false;
        // This ensures that x adjacent tiles are not counted as wall collisions
        if (Tile_Top_Right.y != Current_Avatar_Position.y && Tile_Top_Right.y != Last_Avatar_Position.y) {
          istherearight_wall_intersection = Geometry_Helper::DoTheseLinesIntersect(Tile_Top_Right, Tile_Bottom_Right, Current_Avatar_Position, Last_Avatar_Position, &right_wall_intersection);
        }

        if (istherealeft_wall_intersection && velocity.z >= 0) {
          if (isthereafloor_intersection) {
            if (left_wall_intersection < floor_intersection) {
              Tile_Collision collision;
              collision.colliding_tile = *border_tile;
              std::string collision_side = "Left";
              collision.collision_side = collision_side;
              out_collisions->push_back(collision);
            }
          } else {
            Tile_Collision collision;
            collision.colliding_tile = *border_tile;
            std::string collision_side = "Left";
            collision.collision_side = collision_side;
            out_collisions->push_back(collision);
          }
        }
        if (istherearight_wall_intersection && velocity.x <= 0) {
          if (isthereafloor_intersection) {
            if (right_wall_intersection < floor_intersection) {
              Tile_Collision collision;
              collision.colliding_tile = *border_tile;
              std::string collision_side = "Right";
              collision.collision_side = collision_side;
              out_collisions->push_back(collision);
            }
          } else {
            Tile_Collision collision;
            collision.colliding_tile = *border_tile;
            std::string collision_side = "Right";
            collision.collision_side = collision_side;
            out_collisions->push_back(collision);
          }
        }
        delete lowest_last_collision_bitmap;
      }
    }

    delete out_avatar_collision_block;

    // If colliding tiles is not empty
    // This means the avatar is colliding with a tile.
    if (!out_collisions->empty()) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(Avatar_Component *avatar) {
  if (avatar->GetLastState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks);
  Bitmap_Component *current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetLastState().avatar_collision_blocks);
  Bitmap_Component *last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, avatar->GetLastRenderedPosition());

  if (current_collision_bitmap->GetBottomRightPostion().y != last_collision_bitmap->GetBottomRightPostion().y) {
    D3DXVECTOR3 right_foot_offset;
    right_foot_offset.x = static_cast<float>(last_avatar_collision_block.size.x -
                                             current_avatar_collision_block.size.x);
    if (avatar->GetState().direction.compare("Left") == 0) { right_foot_offset.x = right_foot_offset.x * -1; }
    right_foot_offset.y = static_cast<float>(last_avatar_collision_block.size.y -
                                             current_avatar_collision_block.size.y);
    right_foot_offset.z = 0;


    D3DXVECTOR3 current_avatar_position = *avatar->GetPosition();
    new_avatar_position.x = current_avatar_position.x - (right_foot_offset.x / 2);
    new_avatar_position.y = current_avatar_position.y - (right_foot_offset.y / 2);
    new_avatar_position.z = current_avatar_position.z;

    avatar->SetPosition(new_avatar_position);

    D3DXVECTOR3 avatar_position_offset;
    avatar_position_offset.x = current_avatar_collision_block.offset_from_avatar_centre.x;
    avatar_position_offset.y = current_avatar_collision_block.offset_from_avatar_centre.y;
    avatar_position_offset.z = 0;

    current_avatar_position = *avatar->GetPosition();
    new_avatar_position.x = current_avatar_position.x + avatar_position_offset.x;
    new_avatar_position.y = current_avatar_position.y - avatar_position_offset.y;
    new_avatar_position.z = current_avatar_position.z;

    avatar->SetPosition(new_avatar_position);
  }

  delete current_collision_bitmap;
  delete last_collision_bitmap;
}

//------------------------------------------------------------------------------
bool Avatar_Helper::IsAvatarFloorColliding(Avatar_Component *avatar, std::vector<Avatar_Helper::Tile_Collision> *out_collisions, std::vector<Tile_Bitmap*> *floor_tiles) {
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot
  if (!floor_tiles->empty()) {
    // Get the lowest block
    Avatar_Component::Avatar_Collision_Block lowest_avatar_collision_block;
    lowest_avatar_collision_block = Avatar_Helper::GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks);

    // Make a bitmap for the lowest avatar collision block
    Bitmap_Component *out_avatar_collision_block = CollisionBlockToBitmapComponent(lowest_avatar_collision_block, *(avatar->GetPosition()));

    // Create a list of floor tiles which are colliding with the collision block
    std::vector<Tile_Bitmap*>::iterator border_tile;
    for (border_tile = floor_tiles->begin(); border_tile != floor_tiles->end(); border_tile++) {
      if (out_avatar_collision_block->GetBottomRightPostion().y <= (*border_tile)->GetTopLeftPostion().y) {
        std::vector<std::string> directons;
        directons.push_back("Right");
        directons.push_back("Left");
        std::vector<std::string>::iterator direction;
        for (direction = directons.begin(); direction != directons.end(); direction++) {
          D3DXVECTOR2 Tile_Top_Left;
          Tile_Top_Left.x = (*border_tile)->GetTopLeftPostion().x;
          Tile_Top_Left.y = (*border_tile)->GetTopLeftPostion().y;
          D3DXVECTOR2 Tile_Top_Right;
          Tile_Top_Right.x = (*border_tile)->GetBottomRightPostion().x;
          Tile_Top_Right.y = (*border_tile)->GetTopLeftPostion().y;
          D3DXVECTOR2 Tile_Bottom_Left;
          Tile_Bottom_Left.x = (*border_tile)->GetTopLeftPostion().x;
          Tile_Bottom_Left.y = (*border_tile)->GetBottomRightPostion().y;
          D3DXVECTOR2 Tile_Bottom_Right;
          Tile_Bottom_Right.x = (*border_tile)->GetBottomRightPostion().x;
          Tile_Bottom_Right.y = (*border_tile)->GetBottomRightPostion().y;

          D3DXVECTOR2 Current_Avatar_Position;
          if (direction->compare("Right") == 0) {
            Current_Avatar_Position.x = out_avatar_collision_block->GetBottomRightPostion().x;
          } else {
            Current_Avatar_Position.x = out_avatar_collision_block->GetTopLeftPostion().x;
          }
          Current_Avatar_Position.y = out_avatar_collision_block->GetBottomRightPostion().y;

          Avatar_Component::Avatar_Collision_Block lowest_last_collision_block;
          lowest_last_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetLastRenderedState().avatar_collision_blocks);
          Bitmap_Component *lowest_last_collision_bitmap = CollisionBlockToBitmapComponent(lowest_last_collision_block, avatar->GetLastRenderedPosition());
          D3DXVECTOR2 Last_Avatar_Position;
          if (direction->compare("Right") == 0) {
            Last_Avatar_Position.x = lowest_last_collision_bitmap->GetBottomRightPostion().x;
          } else {
            Last_Avatar_Position.x = lowest_last_collision_bitmap->GetTopLeftPostion().x;
          }
          Last_Avatar_Position.y = lowest_last_collision_bitmap->GetBottomRightPostion().y;

          D3DXVECTOR2 floor_intersection;
          bool isthereafloor_intersection = Geometry_Helper::DoTheseLinesIntersect(Tile_Top_Left, Tile_Top_Right, Current_Avatar_Position, Last_Avatar_Position, &floor_intersection);
          double floor_intersection_disance = Geometry_Helper::WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, floor_intersection);

          D3DXVECTOR2 left_wall_intersection;
          bool istherealeft_wall_intersection = false;
          double left_wall_intersection_disance = 0;
          if ((*border_tile)->IsWall()) {
            istherealeft_wall_intersection = Geometry_Helper::DoTheseLinesIntersect(Tile_Top_Left, Tile_Bottom_Left, Current_Avatar_Position, Last_Avatar_Position, &left_wall_intersection);
            left_wall_intersection_disance = Geometry_Helper::WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, left_wall_intersection);
          }

          D3DXVECTOR2 right_wall_intersection;
          bool istherearight_wall_intersection = false;
          double right_wall_intersection_distance = 0;
          if ((*border_tile)->IsWall()) {
            istherearight_wall_intersection = Geometry_Helper::DoTheseLinesIntersect(Tile_Top_Right, Tile_Bottom_Right, Current_Avatar_Position, Last_Avatar_Position, &right_wall_intersection);
            right_wall_intersection_distance = Geometry_Helper::WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, right_wall_intersection);
          }

           if (isthereafloor_intersection) {
            if (istherealeft_wall_intersection) {
              if (floor_intersection_disance < left_wall_intersection_disance) {
                Avatar_Helper::Tile_Collision collision;
                collision.colliding_tile = *border_tile;
                std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheTopOrBottom(*border_tile, out_avatar_collision_block);
                collision.collision_side = collision_side;
                out_collisions->push_back(collision);
              }
            } else if (istherearight_wall_intersection) {
              if (floor_intersection_disance < right_wall_intersection_distance) {
                Avatar_Helper::Tile_Collision collision;
                collision.colliding_tile = *border_tile;
                std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheTopOrBottom(*border_tile, out_avatar_collision_block);
                collision.collision_side = collision_side;
                out_collisions->push_back(collision);
              }
            } else {
              Avatar_Helper::Tile_Collision collision;
              collision.colliding_tile = *border_tile;
              std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheRightOrLeft(*border_tile, out_avatar_collision_block);
              collision.collision_side = collision_side;
              out_collisions->push_back(collision);
            }
          }

          delete lowest_last_collision_bitmap;
        }
    }
    }


    delete out_avatar_collision_block;

    // If colliding tiles is not empty
    // This means the avatar is colliding with a tile.
    if (!out_collisions->empty()) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
bool Avatar_Helper::CanAvatarGrabALedge(Avatar_Component *avatar, std::vector<Avatar_Helper::Tile_Collision> *out_collisions, std::vector<Tile_Bitmap*> *ledge_tiles) {
  // Magic number
  double avatar_grab_range = 60;

  if (!ledge_tiles->empty()) {
    Avatar_Component::Avatar_Collision_Block avatar_hand;
    avatar_hand = GetNamedCollisionBlock("Arm", avatar->GetState().avatar_collision_blocks);
    if (avatar_hand.id.compare("") == 0) { return false; }
    Bitmap_Component *avatar_hand_bitmap = Avatar_Helper::CollisionBlockToBitmapComponent(avatar_hand, *(avatar->GetPosition()));

    // Create a list of floor tiles which are colliding with the collision block
    std::vector<Tile_Bitmap*>::iterator ledge_tile;
    for (ledge_tile = ledge_tiles->begin(); ledge_tile != ledge_tiles->end(); ledge_tile++) {
      D3DXVECTOR2 grab_point;
      grab_point.y = (*ledge_tile)->GetTopLeftPostion().y;
      bool is_right_wall = (*ledge_tile)->IsRightWall();
      bool is_left_wall = (*ledge_tile)->IsLeftWall();
      if (is_right_wall && is_left_wall) {
        if (avatar->GetState().direction.compare("Right") == 0) {
          grab_point.x = (*ledge_tile)->GetTopLeftPostion().x;
        } else {
          grab_point.x = (*ledge_tile)->GetBottomRightPostion().x;
        }
      } else if (is_right_wall) {
        grab_point.x = (*ledge_tile)->GetTopLeftPostion().x;
      } else if (is_left_wall) {
        grab_point.x = (*ledge_tile)->GetBottomRightPostion().x;
      }

      D3DXVECTOR2 hand_point;
      hand_point.x = avatar_hand_bitmap->GetPosition()->x;
      hand_point.y = avatar_hand_bitmap->GetPosition()->y;

      double hand_grab_point_distance = 0;
      hand_grab_point_distance = Geometry_Helper::WhatsTheDistanceBetweenThesePoints(hand_point, grab_point);

      if (hand_grab_point_distance <= avatar_grab_range) {
        if ((avatar->GetState().direction.compare("Right") == 0 && (*ledge_tile)->IsRightWall() && avatar->GetCommand().direction.compare("Right") == 0) ||
            (avatar->GetState().direction.compare("Left") == 0 && (*ledge_tile)->IsLeftWall())&& avatar->GetCommand().direction.compare("Left") == 0) {
          Avatar_Helper::Tile_Collision collision;
          collision.colliding_tile = *ledge_tile;
          if (is_right_wall && is_left_wall) {
            if (avatar->GetState().direction.compare("Right") == 0) {
              collision.collision_side = "Left";
            } else {
              collision.collision_side = "Right";
            }
          } else if ((*ledge_tile)->IsRightWall()) {
            collision.collision_side = "Left";
          } else if ((*ledge_tile)->IsLeftWall()) {
            collision.collision_side = "Right";
          }
          out_collisions->push_back(collision);
        }
      }
    }

    // If colliding tiles is not empty
    // This means the avatar is colliding with a tile.
    if (!out_collisions->empty()) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLastLedgeEdge(Avatar_Component *avatar, Avatar_Helper::Tile_Collision ledge) {
  if (avatar->GetLastState().state.compare("") == 0) { return; }

  Avatar_Component::Avatar_Collision_Block avatar_hand;
  avatar_hand = Avatar_Helper::GetNamedCollisionBlock("Hand", avatar->GetState().avatar_collision_blocks);
  Bitmap_Component *avatar_hand_bitmap = Avatar_Helper::CollisionBlockToBitmapComponent(avatar_hand, *(avatar->GetPosition()));

  D3DXVECTOR3 grab_point;
  grab_point.y = ledge.colliding_tile->GetTopLeftPostion().y;
  if (ledge.collision_side.compare("Left") == 0) {
    grab_point.x = ledge.colliding_tile->GetTopLeftPostion().x;
  } else if (ledge.collision_side.compare("Right") == 0) {
    grab_point.x = ledge.colliding_tile->GetBottomRightPostion().x;
  }
  grab_point.z = 0;

  D3DXVECTOR3 hand_point;
  hand_point.x = avatar_hand_bitmap->GetPosition()->x;
  hand_point.y = avatar_hand_bitmap->GetPosition()->y;
  hand_point.z = 0;

  if (grab_point != hand_point) {
    D3DXVECTOR3 difference;
    difference.x = grab_point.x - hand_point.x;
    difference.y = grab_point.y - hand_point.y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
    new_avatar_position += difference;

    avatar->SetPosition(new_avatar_position);
  }

  avatar_hand_bitmap = CollisionBlockToBitmapComponent(avatar_hand, *(avatar->GetPosition()));
  hand_point.x = avatar_hand_bitmap->GetPosition()->x;
  hand_point.y = avatar_hand_bitmap->GetPosition()->y;
  if (grab_point != hand_point) {
    throw Exceptions::run_error("AlignAvatarOnLastLedgeEdge: Failed to set avatar position correctly!");
  }

  delete avatar_hand_bitmap;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLastHand(Avatar_Component *avatar) {
  if (avatar->GetLastState().state.compare("") == 0) { return; }

  Avatar_Component::Avatar_Collision_Block last_hand;
  last_hand = Avatar_Helper::GetNamedCollisionBlock("Hand", avatar->GetLastRenderedState().avatar_collision_blocks);
  if (last_hand.id.compare("") == 0) {
    Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(avatar);
    return;
  }
  Bitmap_Component *last_hand_bitmap = Avatar_Helper::CollisionBlockToBitmapComponent(last_hand, avatar->GetLastRenderedPosition());
  D3DXVECTOR3 last_hand_position = *(last_hand_bitmap->GetPosition());

  Avatar_Component::Avatar_Collision_Block current_hand;
  current_hand = Avatar_Helper::GetNamedCollisionBlock("Hand", avatar->GetState().avatar_collision_blocks);
  if (current_hand.id.compare("") == 0) {
    Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(avatar);
    return;
  }
  Bitmap_Component *current_hand_bitmap = Avatar_Helper::CollisionBlockToBitmapComponent(current_hand, *(avatar->GetPosition()));
  D3DXVECTOR3 current_hand_position = *(current_hand_bitmap->GetPosition());

  if (current_hand_position != last_hand_position) {
    D3DXVECTOR3 difference;
    difference.x = last_hand_position.x - current_hand_position.x;
    difference.y = last_hand_position.y - current_hand_position.y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
    new_avatar_position += difference;

    avatar->SetPosition(new_avatar_position);
  }

  delete current_hand_bitmap;
  current_hand_bitmap = Avatar_Helper::CollisionBlockToBitmapComponent(current_hand, *(avatar->GetPosition()));
  current_hand_position = *(current_hand_bitmap->GetPosition());
  if (current_hand_position != last_hand_position) {
    throw Exceptions::run_error("AlignAvatarOnLastHand: Failed to set avatar position correctly!");
  }

  delete last_hand_bitmap;
  delete current_hand_bitmap;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightTop(Avatar_Component *avatar) {
  if (avatar->GetLastRenderedState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks);
  Bitmap_Component *current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  D3DXVECTOR3 current_top_right;
  current_top_right.y = current_collision_bitmap->GetTopLeftPostion().y;
  current_top_right.x = current_collision_bitmap->GetBottomRightPostion().x;
  current_top_right.z = 0;

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetLastRenderedState().avatar_collision_blocks);
  // This fixes a bug where the GetNamedCollisionBlock uses the current avatars direction to determine if the
  // x offsets should be reversed (because the avatar would be facing left, and all the offsets are using right facing sprites
  // but if the last state was facing in the opposite direction then the alignment calculation would be off, so
  // this detects and fixes this alignment issue.
  if (avatar->GetLastRenderedState().direction.compare(avatar->GetState().direction) != 0) {
    last_avatar_collision_block.offset_from_avatar_centre.x = (last_avatar_collision_block.offset_from_avatar_centre.x * -1);
  }
  Bitmap_Component *last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, avatar->GetLastRenderedPosition());
  D3DXVECTOR3 last_top_right;
  last_top_right.y = last_collision_bitmap->GetTopLeftPostion().y;
  last_top_right.x = last_collision_bitmap->GetBottomRightPostion().x;
  last_top_right.z = 0;

  if (current_top_right != last_top_right) {
    D3DXVECTOR3 difference;
    difference.x = last_top_right.x - current_top_right.x;
    difference.y = last_top_right.y - current_top_right.y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
    new_avatar_position += difference;

    avatar->SetPosition(new_avatar_position);
  }

  current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  current_top_right.y = current_collision_bitmap->GetTopLeftPostion().y;
  current_top_right.x = current_collision_bitmap->GetBottomRightPostion().x;
  current_top_right.z = 0;
  if (current_top_right != last_top_right) {
    throw Exceptions::run_error("AlignAvatarOnLastAvatarCollisionBlockRightTop: Failed to set avatar position correctly!");
  }

  delete current_collision_bitmap;
  delete last_collision_bitmap;
}

//------------------------------------------------------------------------------
void Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftTop(Avatar_Component *avatar) {
  if (avatar->GetLastRenderedState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetState().avatar_collision_blocks);
  Bitmap_Component *current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  D3DXVECTOR3 current_top_left = current_collision_bitmap->GetTopLeftPostion();

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Avatar", avatar->GetLastRenderedState().avatar_collision_blocks);
  // This fixes a bug where the GetNamedCollisionBlock uses the current avatars direction to determine if the
  // x offsets should be reversed (because the avatar would be facing left, and all the offsets are using right facing sprites
  // but if the last state was facing in the opposite direction then the alignment calculation would be off, so
  // this detects and fixes this alignment issue.
  if (avatar->GetLastRenderedState().direction.compare(avatar->GetState().direction) != 0) {
    last_avatar_collision_block.offset_from_avatar_centre.x = (last_avatar_collision_block.offset_from_avatar_centre.x * -1);
  }
  Bitmap_Component *last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, avatar->GetLastRenderedPosition());
  D3DXVECTOR3 last_top_left = last_collision_bitmap->GetTopLeftPostion();

  if (current_top_left != last_top_left) {
    D3DXVECTOR3 difference;
    difference.x = last_top_left.x - current_top_left.x;
    difference.y = last_top_left.y - current_top_left.y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *avatar->GetPosition();
    new_avatar_position += difference;

    avatar->SetPosition(new_avatar_position);
  }

  current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(avatar->GetPosition()));
  current_top_left = current_collision_bitmap->GetTopLeftPostion();
  if (current_top_left != last_top_left) {
    throw Exceptions::run_error("AlignAvatarOnLastAvatarCollisionBlockLeftTop: Failed to set avatar position correctly!");
  }

  delete current_collision_bitmap;
  delete last_collision_bitmap;
}

} // Tunnelour