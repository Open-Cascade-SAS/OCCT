// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#include <inspector/TInspectorAPI_Communicator.hxx>

#include <cstdio>
#include <map>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#define LIB_HANDLE HINSTANCE

// =======================================================================
// function :  LoadPluginLibrary
// purpose :
// =======================================================================
TInspectorAPI_Communicator* TInspectorAPI_Communicator::LoadPluginLibrary
                                         (const TCollection_AsciiString& thePluginName)
{
  if (thePluginName.IsEmpty())
    std::cout << thePluginName.ToCString() << "%s plugin could not be loaded." << std::endl;
  TCollection_AsciiString aPluginLibraryName = thePluginName;
#ifdef _WIN32
  aPluginLibraryName += ".dll";
#else
  aPluginLibraryName.Prepend ("lib");
  aPluginLibraryName += ".so";
#endif

  COMMUNICATOR_INSTANCE crtInst = 0;
#ifdef _WIN32
  HINSTANCE modLib = ::LoadLibraryA((LPCSTR)aPluginLibraryName.ToCString());
#else
  void* modLib = dlopen(aPluginLibraryName.ToCString(), RTLD_LAZY | RTLD_GLOBAL);
#endif

  if (!modLib)
    std::cout << "Failed to load plugin." << aPluginLibraryName.ToCString() << std::endl;
  else
  {
#ifdef _WIN32
    crtInst = (COMMUNICATOR_INSTANCE)::GetProcAddress(modLib, CREATE_COMMUNICATOR_FUNCTION_NAME);
#else
    crtInst = (COMMUNICATOR_INSTANCE)dlsym(modLib, CREATE_COMMUNICATOR_FUNCTION_NAME);
#endif
    if (!crtInst)
      std::cout << "Failed to find " << CREATE_COMMUNICATOR_FUNCTION_NAME << " function." << std::endl;
  }
  TInspectorAPI_Communicator* aModule = crtInst ? crtInst() : 0;
  return aModule;
}
