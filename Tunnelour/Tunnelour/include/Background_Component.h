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

#ifndef TUNNELOUR_BACKGROUND_COMPONENT_H_
#define TUNNELOUR_BACKGROUND_COMPONENT_H_


#include "Component.h"

namespace Tunnelour {
class Background_Component: public Tunnelour::Component {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Background_Component();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Background_Component();

  //---------------------------------------------------------------------------
  // Description : Initialise this Component
  //---------------------------------------------------------------------------
  virtual void Init();

  //---------------------------------------------------------------------------
  // Description : Accessor and Mutator for the Alpha
  //---------------------------------------------------------------------------
  float GetAlpha();
  void SetAlpha(float alpha);

  //---------------------------------------------------------------------------
  // Description : Accessor and Mutator for the intensity of the color Blue
  //---------------------------------------------------------------------------
  float GetBlue();
  void SetBlue(float intensity);

  //---------------------------------------------------------------------------
  // Description : Accessor and Mutator for the intensity of the color Green
  //---------------------------------------------------------------------------
  float GetGreen();
  void SetGreen(float intensity);

  //---------------------------------------------------------------------------
  // Description : Accessor and Mutator for the intensity of the color Red
  //---------------------------------------------------------------------------
  float GetRed();
  void SetRed(float intensity);

 protected:

 private:
   float m_alpha, m_blue, m_green, m_red;

};
}  // namespace Tunnelour
#endif  // TUNNELOUR_BACKGROUND_COMPONENT_H_
