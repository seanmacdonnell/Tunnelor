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

#include "View.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
View::View() {
  m_model = NULL;
  m_is_initialised = false;
}

//------------------------------------------------------------------------------
View::~View() {
}

//------------------------------------------------------------------------------
void View::Init(Tunnelour::Component_Composite * const model) {
  m_model = model;
}

//------------------------------------------------------------------------------
void View::Run() {
}

//------------------------------------------------------------------------------
bool View::IsInitialised() {
  return m_is_initialised;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
