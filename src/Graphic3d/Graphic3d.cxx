// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Graphic3d.ixx>

#include <Aspect_DisplayConnection.hxx>
#include <Aspect_DriverDefinitionError.hxx>
#include <OSD_Environment.hxx>

//=======================================================================
//function : InitGraphicDriver
//purpose  :
//=======================================================================
Handle(Graphic3d_GraphicDriver) Graphic3d::InitGraphicDriver (const Handle(Aspect_DisplayConnection)& theDisplayConnection)
{
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (theDisplayConnection.IsNull())
  {
    Aspect_DriverDefinitionError::Raise ("Null display connection.");
  }
#endif

  TCollection_AsciiString aGraphicLibName;

  // Setting the library name. Depends on the platform.
#if defined(_WIN32) || defined(__WIN32__)
  aGraphicLibName = "TKOpenGl.dll";
#elif defined(__hpux) || defined(HPUX)
  aGraphicLibName = "libTKOpenGl.sl";
#elif defined(__APPLE__)
  aGraphicLibName = "libTKOpenGl.dylib";
#else
  aGraphicLibName = "libTKOpenGl.so";
#endif

  // Loading the library.
  OSD_SharedLibrary aSharedLibrary (aGraphicLibName.ToCString());
  if (!aSharedLibrary.DlOpen (OSD_RTLD_LAZY))
  {
    Aspect_DriverDefinitionError::Raise (aSharedLibrary.DlError());
  }

  // Retrieving factory function pointer.
  typedef Handle(Graphic3d_GraphicDriver) (*GraphicDriverFactoryPointer) (Standard_CString);
  GraphicDriverFactoryPointer aGraphicDriverConstructor = (GraphicDriverFactoryPointer )aSharedLibrary.DlSymb ("MetaGraphicDriverFactory");
  if (aGraphicDriverConstructor == NULL)
  {
    Aspect_DriverDefinitionError::Raise (aSharedLibrary.DlError());
  }

  // Creating driver instance.
  Handle(Graphic3d_GraphicDriver) aGraphicDriver = aGraphicDriverConstructor (aSharedLibrary.Name());

  // Management of traces.
  OSD_Environment aTraceEnv ("CSF_GraphicTrace");
  TCollection_AsciiString aTrace = aTraceEnv.Value();
  if (aTrace.IsIntegerValue())
  {
    aGraphicDriver->SetTrace (aTrace.IntegerValue());
  }

  // Starting graphic driver.
  if (!aGraphicDriver->Begin (theDisplayConnection))
  {
    Aspect_DriverDefinitionError::Raise ("Cannot connect to graphic library.");
  }

  return aGraphicDriver;
}
