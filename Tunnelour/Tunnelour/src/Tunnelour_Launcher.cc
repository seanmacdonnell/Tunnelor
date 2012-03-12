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

#include "Tunnelour_Launcher.h"
#include "Engine.h"

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
  Tunnelour::Engine* engine;
  bool result = 0;

  // Create Engine
  engine = new Tunnelour::Engine;

  // Init Engine
  engine->Init(true, true);

  // Run Engine
  engine->Start();

  // Kill Engine
  delete engine;

  return result;
}
