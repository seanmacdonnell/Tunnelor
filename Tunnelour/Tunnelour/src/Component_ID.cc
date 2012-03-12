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

#include "Component_ID.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Component_ID::~Component_ID() {
  if (m_instance != 0) {
      delete m_instance;
  }
}

//------------------------------------------------------------------------------
Component_ID* Component_ID::GetInstance() {
  // If this instance hasn't been initialized yet:
  if (m_instance == 0) {
    // Create a new pointer to exampleClass
    m_instance = new Component_ID();
  }
  // return the current instance
  return m_instance;
}

//------------------------------------------------------------------------------
int Component_ID::Next() {
  return m_id++;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------
Component_ID::Component_ID() {
  m_id = 0;
}


//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
Component_ID* Component_ID::m_instance = 0;

}  // namespace Tunnelour
