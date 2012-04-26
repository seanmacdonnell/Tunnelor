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
//  Author(s)  : Sean MacDonnell
//  Description: Exceptions contains the definitions of the different types
//               of custom exceptions used by the Game_Engine.
//

#ifndef TUNNELOUR_EXCEPTIONS_H_
#define TUNNELOUR_EXCEPTIONS_H_

#include <string>

namespace Tunnelour {
namespace Exceptions {

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Exception for when an object has not been initialised
//-----------------------------------------------------------------------------
class uninit_error : public std::runtime_error {
 public:
  explicit uninit_error(std::string message) : std::runtime_error(message) { }
};

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Exception for when an object has not been finshed
//-----------------------------------------------------------------------------
class unfinished_error : public std::runtime_error {
 public:
  explicit unfinished_error(std::string message) : std::runtime_error(message) { }
};

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Exception for when an object is initialising and fails.
//-----------------------------------------------------------------------------
class init_error : public std::runtime_error {
 public:
  explicit init_error(std::string message) : std::runtime_error(message) { }
};

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Exception for when an object throws an excepton in run
//-----------------------------------------------------------------------------
class run_error : public std::runtime_error {
 public:
  explicit run_error(std::string message) : std::runtime_error(message) { }
};

}  // namespace Exceptions
}  // namespace Tunnelour
#endif  // TUNNELOUR_EXCEPTIONS_H_
