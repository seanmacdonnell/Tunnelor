//  Copyright 2011 Sean MacDonnell
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

#ifndef TUNNELOUR_DIRECT3D11_VIEW_MUTATOR_H_
#define TUNNELOUR_DIRECT3D11_VIEW_MUTATOR_H_

#include "Component.h"

namespace Tunnelour {
class Direct3D11_View_Init_Mutator: public Tunnelour::Component::Component_Mutator  {
 public:
  //-------------------------------------------------------------------------
  // Description : Constructor
  //-------------------------------------------------------------------------
  Direct3D11_View_Init_Mutator();

  //-------------------------------------------------------------------------
  // Description : Deconstructor
  //-------------------------------------------------------------------------
  virtual ~Direct3D11_View_Init_Mutator();

  //-------------------------------------------------------------------------
  // Description : Mutator function, pass it a component to mutate.
  //-------------------------------------------------------------------------
  void Mutate(Tunnelour::Component *component);

  float m_back_buffer_color[4];
  bool m_found_back_buffer_color;
  };
}  // namespace Tunnelour
#endif  // TUNNELOUR_DIRECT3D11_VIEW_MUTATOR_H_
