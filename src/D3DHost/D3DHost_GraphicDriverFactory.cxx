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

#include <D3DHost_GraphicDriverFactory.hxx>

#include <D3DHost_GraphicDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(D3DHost_GraphicDriverFactory, OpenGl_GraphicDriverFactory)

// =======================================================================
// function : D3DHost_GraphicDriverFactory
// purpose  :
// =======================================================================
D3DHost_GraphicDriverFactory::D3DHost_GraphicDriverFactory()
{
  myName = "TKD3DHost";
}

// =======================================================================
// function : CreateDriver
// purpose  :
// =======================================================================
Handle(Graphic3d_GraphicDriver) D3DHost_GraphicDriverFactory::CreateDriver(
  const Handle(Aspect_DisplayConnection)&)
{
  Handle(D3DHost_GraphicDriver) aDriver = new D3DHost_GraphicDriver();
  aDriver->ChangeOptions()              = *myDefaultCaps;
  aDriver->InitContext();
  return aDriver;
}
