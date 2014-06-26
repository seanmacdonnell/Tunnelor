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

#include "Game_Settings_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Game_Settings_Component::Game_Settings_Component(): Component() {
  m_resolution = D3DXVECTOR2(0, 0);
  m_is_full_screen = false;
  m_vsync_enabled = false;
  m_screen_depth = 0.0f;
  m_screen_near = 0.0f;
  //m_color = D3DXCOLOR(1, 1, 1, 1);
  m_color = D3DXCOLOR(0.043137254901960784, 0.0196078431372549, 0.058823529411764705, 1);
  m_tileset_path = L"";
  m_type = "Game_Settings_Component";
  m_hinstance = 0;
  m_hwnd = 0;
  m_is_camera_following = false;
}

//------------------------------------------------------------------------------
Game_Settings_Component::~Game_Settings_Component() {
}

//------------------------------------------------------------------------------
void Game_Settings_Component::Init() {
  //Default Values
  
  //m_resolution = D3DXVECTOR2(640, 480);
  m_resolution = D3DXVECTOR2(1280, 720);
  m_is_full_screen = false;
  m_vsync_enabled = false;
  m_screen_depth = 1000.0f;
  m_screen_near = 0.1f;
  m_tileset_path = L"resource\\tilesets\\";
  m_level_path = L"resource\\levels\\";
  m_is_debug_mode = false;
  m_is_camera_following = true;
}

//------------------------------------------------------------------------------
D3DXVECTOR2 Game_Settings_Component::GetResolution() {
  return m_resolution;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetResolution(int x, int y) {
  m_resolution = D3DXVECTOR2(static_cast<float>(x) , static_cast<float>(y));
}

//------------------------------------------------------------------------------
bool Game_Settings_Component::IsFullScreen() {
  return m_is_full_screen;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetIsFullScreen(bool is_full_screen) {
  m_is_full_screen = is_full_screen;
}

//------------------------------------------------------------------------------
bool Game_Settings_Component::IsVSyncEnabled() {
  return m_vsync_enabled;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetIsVSyncEnabled(bool is_vsync_enabled) {
  m_vsync_enabled = is_vsync_enabled;
}

//------------------------------------------------------------------------------
float Game_Settings_Component::GetScreenDepth() {
  return m_screen_depth;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetScreenDepth(float screen_depth) {
  m_screen_depth = screen_depth;
}

//------------------------------------------------------------------------------
float Game_Settings_Component::GetScreenNear() {
  return m_screen_near;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetScreenNear(float screen_near) {
  m_screen_near = screen_near;
}

//------------------------------------------------------------------------------
D3DXCOLOR Game_Settings_Component::GetColor() {
  return m_color;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetColor(D3DXCOLOR & color) {
  m_color = color;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetColor(float red, float green, float blue, float alpha) {
  m_color = D3DXCOLOR(red, green, blue, alpha);
}

//------------------------------------------------------------------------------
std::wstring Game_Settings_Component::GetLevelPath() {
  return m_level_path;
}

//------------------------------------------------------------------------------
std::wstring Game_Settings_Component::GetTilesetPath() {
  return m_tileset_path;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetTilesetPath(std::wstring tileset_path) {
  m_tileset_path = tileset_path;
}

//------------------------------------------------------------------------------
HINSTANCE& Game_Settings_Component::GetHInstance() {
  return m_hinstance;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetHInstance(HINSTANCE hinstance) {
  m_hinstance = hinstance;
}

//------------------------------------------------------------------------------
HWND& Game_Settings_Component::GetHWnd() {
  return m_hwnd;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetHWnd(HWND hwnd) {
  m_hwnd = hwnd;
}

//------------------------------------------------------------------------------
bool Game_Settings_Component::IsDebugMode() {
  return m_is_debug_mode;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetIsDebugMode(bool is_debug_mode) {
  m_is_debug_mode = is_debug_mode;
}

//------------------------------------------------------------------------------
bool Game_Settings_Component::IsCameraFollowing() {
  return m_is_camera_following;
}

//------------------------------------------------------------------------------
void Game_Settings_Component::SetIsCameraFollowing(bool is_camera_following) {
  m_is_camera_following = is_camera_following;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
