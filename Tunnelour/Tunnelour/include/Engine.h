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

#ifndef TUNNELOUR_ENGINE_H_
#define TUNNELOUR_ENGINE_H_

#include "Component_Composite.h"
#include "Controller_Composite.h"
#include "View_Composite.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Engine is the main engine class for this game engine.
//                It maintains a controller, view and model - and runs
//                the main game loop.
//-----------------------------------------------------------------------------
class Engine {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Engine();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Engine();

  //--------------------------------------------------------------------------
  // Description : Initialises this engine
  //--------------------------------------------------------------------------
  void Init(bool include_view, bool include_controller);

  //--------------------------------------------------------------------------
  // Description : Accessor for if the Model has been initialised
  //--------------------------------------------------------------------------
  bool IsModelInit();

  //--------------------------------------------------------------------------
  // Description : Accessor for if the View has been initialised
  //--------------------------------------------------------------------------
  bool IsViewInit();

  //--------------------------------------------------------------------------
  // Description : Accessor for if the Controller has been initialised
  //--------------------------------------------------------------------------
  bool IsControllerInit();

  //--------------------------------------------------------------------------
  // Description : Starts the Tunnelour Game Loop
  //--------------------------------------------------------------------------
  int Start();

 protected:
  Engine::Component_Composite *m_model;
  Engine::View_Composite *m_view;
  Engine::Controller_Composite *m_controller;

 private:
  //--------------------------------------------------------------------------
  // Description : The game loop, loops untill a WM_QUIT message is recieved.
  //               Or the Quit flag has been set.
  //--------------------------------------------------------------------------
  int Loop();
};
}  // namespace Engine

#endif  // TUNNELOUR_ENGINE_H_
