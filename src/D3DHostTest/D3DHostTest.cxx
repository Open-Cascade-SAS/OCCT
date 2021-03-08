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

#include <D3DHostTest.hxx>

#include <Draw_PluginMacro.hxx>
#include <D3DHost_GraphicDriverFactory.hxx>

// ======================================================================
// function : Factory
// purpose  :
// ======================================================================
void D3DHostTest::Factory (Draw_Interpretor& )
{
  static const Handle(D3DHost_GraphicDriverFactory) aFactory = new D3DHost_GraphicDriverFactory();
  Graphic3d_GraphicDriverFactory::RegisterFactory (aFactory);
#ifdef DEB
  theDI << "Draw Plugin : D3DHost commands are loaded.\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(D3DHostTest)
