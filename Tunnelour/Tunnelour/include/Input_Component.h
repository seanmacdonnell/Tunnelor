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

#ifndef TUNNELOUR_INPUT_COMPONENT_H_
#define TUNNELOUR_INPUT_COMPONENT_H_

#include "Component.h"
#include <vector>

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Input_Component contains all the data for the current 
//              : game level and the past game level.
//-----------------------------------------------------------------------------
class Input_Component: public Tunnelour::Component {
 public:
   struct Key_Input {
     bool IsSpace;
     bool IsEsc;
     bool IsRight;
     bool IsLeft;
     bool IsDown;
     bool IsUp;

     Key_Input() {
       IsSpace = false;
       IsEsc = false;
       IsRight = false;
       IsLeft = false;
       IsDown = false;
       IsUp = false;
     }
   };
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Input_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  ~Input_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  void Init();

  Key_Input GetCurrentKeyInput();
  void SetCurrentKeyInput(Key_Input key_input);
  Key_Input GetLastKeyInput();
  
 protected:

 private:
 Key_Input m_current_key_input;
 Key_Input m_last_key_input;

};
}  // namespace Tunnelour
#endif  //  TUNNELOUR_INPUT_COMPONENT_H_
