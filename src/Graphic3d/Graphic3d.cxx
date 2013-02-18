// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
