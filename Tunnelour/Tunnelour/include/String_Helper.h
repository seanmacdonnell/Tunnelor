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

#ifndef STRING_HELPER_H_
#define STRING_HELPER_H_

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <iostream>

namespace String_Helper {

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Converts a char to a wchar
//-----------------------------------------------------------------------------
inline std::wstring CharToWChar(const char* pstrSrc) {
  /*
    size_t origsize = strlen(pstrSrc) + 1;
    size_t convertedChars = 0;
    wchar_t wcstring[100];
    mbstowcs_s(&convertedChars, wcstring, origsize, pstrSrc, _TRUNCATE);
    return std::wstring(wcstring);
    */


    const size_t cSize = strlen(pstrSrc)+1;
    std::wstring wc( cSize, L'#' );
    mbstowcs( &wc[0], pstrSrc, cSize );

    return wc;
}

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Converts a string to a wstring
//-----------------------------------------------------------------------------
inline std::wstring StringToWString(const std::string& string) {
  int len;
  int slength = (int)string.length() + 1;
  len = MultiByteToWideChar(CP_ACP, 0, string.c_str(), slength, 0, 0); 
  wchar_t* buf = new wchar_t[len];
  MultiByteToWideChar(CP_ACP, 0, string.c_str(), slength, buf, len);
  std::wstring r(buf);
  delete[] buf;
  return r;
}

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Convert wstring to string
//-----------------------------------------------------------------------------
inline std::string WStringToString(const std::wstring& wstring) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  return converter.to_bytes( wstring );
}

//-----------------------------------------------------------------------------
//  Author(s)   : http://stackoverflow.com/a/6328211
//  Description : Splits a string into elements on a given delimiter and
//              : adds them to the provided vector
//-----------------------------------------------------------------------------    
inline std::vector<std::string> Split( std::string const& source, char delim ) {
    std::vector<std::string> results;
    std::string::const_iterator curr = source.begin();
    std::string::const_iterator end = source.end();
    std::string::const_iterator next = std::find( curr, end, delim );
    while ( next != end ) {
      std::string curr_next = std::string( curr, next );
      if (!curr_next.empty()) {
        results.push_back(curr_next);
      }
      curr = next + 1;
      next = std::find( curr, end, delim );
    }
    std::string last = std::string( curr, next );
    if (strcmp(last.c_str(),"\n") != 0 && !last.empty()) {
      results.push_back(last);
    }
    return results;
}

//-----------------------------------------------------------------------------
inline std::string To_String(int a) {
  std::stringstream ss;
  ss << a;
  std::string str = ss.str();
  return str;
}

//-----------------------------------------------------------------------------
inline std::string To_String(float a) {
  std::stringstream ss;
  ss << a;
  std::string str = ss.str();
  return str;
}

}  // namespace String_Helper
#endif  // STRING_HELPER_H_
