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

#ifndef TUNNELOUR_GAME_SETTINGS_COMPONENT_H_
#define TUNNELOUR_GAME_SETTINGS_COMPONENT_H_

//-----------------------------------------------------------------------------
// NOTE(Sean): d3d11 is not required for this class, I am including it as a
//             temporary fix for a compatibility error of including d3dx10math
//             before d3d11.
// TODO(Sean): Either replace d3dx10math with a different maths library or fix
//             the include order dependency.
//-----------------------------------------------------------------------------
#include <d3d11.h>
#include <d3dx10math.h>
#include "Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Game_Settings_Component contains all the information
//                for setting the game up.
//-----------------------------------------------------------------------------
class Game_Settings_Component: public Tunnelour::Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Game_Settings_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Game_Settings_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  //---------------------------------------------------------------------------
  // Description : Accessor for the resolution
  //---------------------------------------------------------------------------
  D3DXVECTOR2 GetResolution();

  //---------------------------------------------------------------------------
  // Description : Mutator for the resolution
  //---------------------------------------------------------------------------
  void SetResolution(int x, int y);

  //---------------------------------------------------------------------------
  // Description : Accessor for the full screen flag
  //---------------------------------------------------------------------------
  bool IsFullScreen();

  //---------------------------------------------------------------------------
  // Description : Mutator for the full screen flag
  //---------------------------------------------------------------------------
  void SetIsFullScreen(bool is_full_screen);

  //---------------------------------------------------------------------------
  // Description : Accessor for the vsync enabled flag
  //---------------------------------------------------------------------------
  bool IsVSyncEnabled();

  //---------------------------------------------------------------------------
  // Description : Mutator for the vsync enabled flag
  //---------------------------------------------------------------------------
  void SetIsVSyncEnabled(bool is_vsync_enabled);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Screen Depth
  //---------------------------------------------------------------------------
  float GetScreenDepth();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Screen Depth
  //---------------------------------------------------------------------------
  void SetScreenDepth(float screen_depth);

  //---------------------------------------------------------------------------
  // Description : Accessor for the Screen Near
  //---------------------------------------------------------------------------
  float GetScreenNear();

  //---------------------------------------------------------------------------
  // Description : Mutator for the Screen Near
  //---------------------------------------------------------------------------
  void SetScreenNear(float screen_near);

  //---------------------------------------------------------------------------
  // Description : Accessor for the color
  //---------------------------------------------------------------------------
  D3DXCOLOR GetColor();

  //---------------------------------------------------------------------------
  // Description : Mutator for the color
  //---------------------------------------------------------------------------
  void SetColor(D3DXCOLOR & color);
  void SetColor(float red, float green, float blue, float alpha);

  //---------------------------------------------------------------------------
  // Description : Accessor for the resource path
  //---------------------------------------------------------------------------
  std::wstring GetTilesetPath();

  //---------------------------------------------------------------------------
  // Description : Accessor for the resource path
  //---------------------------------------------------------------------------
  std::wstring GetLevelPath();

  //---------------------------------------------------------------------------
  // Description : Mutator for the color
  //---------------------------------------------------------------------------
  void SetTilesetPath(std::wstring tileset_path);

  //---------------------------------------------------------------------------
  // Description : Accessor for the HInstance
  //---------------------------------------------------------------------------
  HINSTANCE& GetHInstance();

  //---------------------------------------------------------------------------
  // Description : Accessor for the HInstance
  //---------------------------------------------------------------------------
  void SetHInstance(HINSTANCE hinstance);

  //---------------------------------------------------------------------------
  // Description : Accessor for the HWnd
  //---------------------------------------------------------------------------
  HWND& GetHWnd();

  //---------------------------------------------------------------------------
  // Description : Accessor for the HWnd
  //---------------------------------------------------------------------------
  void SetHWnd(HWND hwnd);

  //---------------------------------------------------------------------------
  // Description : Accessor for the debug mode flag
  //---------------------------------------------------------------------------
  bool IsDebugMode();

  //---------------------------------------------------------------------------
  // Description : Mutator for the debug mode flag
  //---------------------------------------------------------------------------
  void SetIsDebugMode(bool is_debug_mode);

  //---------------------------------------------------------------------------
  // Description : Accessor for the debug mode flag
  //---------------------------------------------------------------------------
  bool IsCameraFollowing();

  //---------------------------------------------------------------------------
  // Description : Mutator for the debug mode flag
  //---------------------------------------------------------------------------
  void SetIsCameraFollowing(bool is_camera_following);

  //---------------------------------------------------------------------------
  // Description : Accessor for the is using level file flag
  //---------------------------------------------------------------------------
  bool IsUsingLevelFile();

 protected:

 private:
  //---------------------------------------------------------------------------
  // Description : Class variables
  //---------------------------------------------------------------------------
  D3DXVECTOR2 m_resolution;
  bool m_is_full_screen;
  bool m_vsync_enabled;
  float m_screen_depth;
  float m_screen_near;
  D3DXCOLOR m_color;
  std::wstring m_tileset_path;
  std::wstring m_level_path;
  HINSTANCE m_hinstance;
  HWND m_hwnd;
  bool m_is_debug_mode;
  bool m_is_camera_following;
  bool m_is_using_level_file;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_BACKGROUND_COLOR_COMPONENT_H_
