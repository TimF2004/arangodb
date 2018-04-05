
////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2018 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Christoph Uhde
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_SSL_SSL_FIX_H
#define ARANGODB_SSL_SSL_FIX_H 1

#ifdef _WIN32
  #ifdef U_STATIC_IMPLEMENTATION
    #include <windows.h>
    // https://stackoverflow.com/questions/30450042/unresolved-external-symbol-imp-iob-func-referenced-in-function-openssldie/33830712#33830712
    // FIX for openssl lib that searches for deprecated symbols
    FILE _iob[] = { *stdin, *stdout, *stderr };
    extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
  #endif
#endif

#endif
