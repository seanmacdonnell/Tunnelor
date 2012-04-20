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

#include "Background_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Background_Component::Background_Component() {
  m_alpha = 1.0;
  m_blue = 1.0;
  m_green = 1.0;
  m_red = 1.0;
}

//------------------------------------------------------------------------------
Background_Component::~Background_Component() {
}

//------------------------------------------------------------------------------
void Background_Component::Init() {
  // Set to Black
  m_alpha = 1.0;
  m_blue = 0.0;
  m_green = 0.0;
  m_red = 0.0;
}

//------------------------------------------------------------------------------
float Background_Component::GetAlpha() {
  return m_alpha;
}

//------------------------------------------------------------------------------
void Background_Component::SetAlpha(float alpha) {
  m_alpha = alpha;
}

//------------------------------------------------------------------------------
float Background_Component::GetBlue() {
  return m_blue;
}

//------------------------------------------------------------------------------
void Background_Component::SetBlue(float intensity) {
  m_blue = intensity;
}

//------------------------------------------------------------------------------
float Background_Component::GetGreen() {
  return m_green;
}

//------------------------------------------------------------------------------
void Background_Component::SetGreen(float intensity) {
  m_green = intensity;
}

//------------------------------------------------------------------------------
float Background_Component::GetRed() {
  return m_red;
}

//------------------------------------------------------------------------------
void Background_Component::SetRed(float intensity) {
  m_red = intensity;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
