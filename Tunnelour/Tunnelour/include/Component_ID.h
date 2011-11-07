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

#ifndef TUNNELOUR_COMPONENT_ID_H_
#define TUNNELOUR_COMPONENT_ID_H_

namespace Tunnelour {
class Component_ID {
 public:
  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Component_ID();

  //---------------------------------------------------------------------------
  // Description : Returns the instance of this Component_ID
  //---------------------------------------------------------------------------
  static Component_ID* GetInstance();

  //---------------------------------------------------------------------------
  // Description : Returns the next program scope unique id
  //---------------------------------------------------------------------------
  int Next();

 protected:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Component_ID();

 private:
  static Component_ID* m_instance;
  //---------------------------------------------------------------------------
  // Description : Integer ID - starts at 0 and increments by 1's.
  //---------------------------------------------------------------------------
  int m_id;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_COMPONENT_ID_H_
