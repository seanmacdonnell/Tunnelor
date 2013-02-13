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

#ifndef TUNNELOUR_STRING_HELPER_H_
#define TUNNELOUR_STRING_HELPER_H_

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

namespace String_Helper {

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Converts a char to a wchar
//-----------------------------------------------------------------------------
inline std::wstring CharToWChar(const char* pstrSrc) {
    size_t origsize = strlen(pstrSrc) + 1;
    size_t convertedChars = 0;
    wchar_t wcstring[100];
    mbstowcs_s(&convertedChars, wcstring, origsize, pstrSrc, _TRUNCATE);
    return std::wstring(wcstring);
}

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Converts a string to a wstring
//-----------------------------------------------------------------------------
inline std::wstring StringToWString(const std::string& string) {
    int len;
    int slength = static_cast<int>(string.length() + 1);
    len = MultiByteToWideChar(CP_ACP, 0, string.c_str(), slength, 0, 0);
    std::wstring r(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, string.c_str(), slength, &r[0], len);
    return r;
}

//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : Convert wstring to string
//-----------------------------------------------------------------------------
inline std::string WStringToString(const std::wstring& wstring) {
    int len;
    int slength = static_cast<int>(wstring.length() + 1);
    len = WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), slength, 0, 0, 0, 0);
    std::string r(len, '\0');
    WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), slength, &r[0], len, 0, 0);
    return r;
}


}  // namespace String_Helper
#endif  // TUNNELOUR_STRING_HELPER_H_
