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

#include <OpenGlTest.hxx>

#include <Draw_PluginMacro.hxx>
#include <OpenGl_GraphicDriverFactory.hxx>

// ======================================================================
// function : Factory
// purpose  :
// ======================================================================
void OpenGlTest::Factory(Draw_Interpretor& theDI)
{
  static const Handle(OpenGl_GraphicDriverFactory) aFactory = new OpenGl_GraphicDriverFactory();
  Graphic3d_GraphicDriverFactory::RegisterFactory(aFactory);
  OpenGlTest::Commands(theDI);
#ifdef DEB
  theDI << "Draw Plugin : OpenGL commands are loaded.\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(OpenGlTest)
