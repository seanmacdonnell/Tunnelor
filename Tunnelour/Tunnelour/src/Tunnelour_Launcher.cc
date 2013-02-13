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

#include "Tunnelour_Launcher.h"
#include "Engine.h"
#include "windows.h"
#include "String_Helper.h"

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {    
  bool result = 0;

  try {
    // Create Engine
    Tunnelour::Engine* engine;
    engine = new Tunnelour::Engine;

    // Init Engine
    engine->Init(true, true);

    // Run Engine
    engine->Start();

    // Kill Engine
    delete engine;
  }
  catch (const std::exception& e) {
    const char* raw_message = e.what();
    std::wstring converted_message = String_Helper::CharToWChar(raw_message);
    LPCTSTR message = converted_message.c_str();
    MessageBox( NULL, message, TEXT("Unhandled exception"), MB_OK );
    return EXIT_FAILURE;
  }
  catch (...)  {
    MessageBox( NULL, TEXT("Unknown error"), NULL, MB_OK );
    return EXIT_FAILURE;
  }

  return result;
}


