// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <OpenGl_GraphicDriverFactory.hxx>

#include <OpenGl_GraphicDriver.hxx>

#ifdef HAVE_GLES2
  #define OpenGl_DRIVER_NAME "TKOpenGles"
#else
  #define OpenGl_DRIVER_NAME "TKOpenGl"
#endif

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_GraphicDriverFactory, Graphic3d_GraphicDriverFactory)

// =======================================================================
// function : OpenGl_GraphicDriverFactory
// purpose  :
// =======================================================================
OpenGl_GraphicDriverFactory::OpenGl_GraphicDriverFactory()
    : Graphic3d_GraphicDriverFactory(OpenGl_DRIVER_NAME),
      myDefaultCaps(new OpenGl_Caps())
{
  //
}

// =======================================================================
// function : CreateDriver
// purpose  :
// =======================================================================
Handle(Graphic3d_GraphicDriver) OpenGl_GraphicDriverFactory::CreateDriver(
  const Handle(Aspect_DisplayConnection)& theDisp)
{
  Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver(theDisp, false);
  aDriver->ChangeOptions()             = *myDefaultCaps;
  aDriver->InitContext();
  return aDriver;
}
