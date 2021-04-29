// Created on: 2003-08-11
// Created by: Sergey ZARITCHNY
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <Draw.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <Message.hxx>
#include <Message_PrinterSystemLog.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard_ErrorHandler.hxx>

#ifdef OCCT_NO_PLUGINS
  #include <BOPTest.hxx>
  #include <DPrsStd.hxx>
  #if defined(HAVE_OPENGL) || defined(HAVE_GLES2)
  #include <OpenGlTest.hxx>
  #endif
  #include <TObjDRAW.hxx>
  #include <ViewerTest.hxx>
  #include <XSDRAWSTLVRML.hxx>
  #include <XDEDRAW.hxx>
#endif

#ifdef OCCT_NO_PLUGINS
//! Mimic pload command by loading pre-defined set of statically linked plugins.
static Standard_Integer Pload (Draw_Interpretor& theDI,
                               Standard_Integer  theNbArgs,
                               const char**      theArgVec)
{
  NCollection_IndexedMap<TCollection_AsciiString, TCollection_AsciiString> aPlugins;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.UpperCase();
    if (anArg == "DEFAULT")
    {
      aPlugins.Add ("TOPTEST");
    }
    else if (anArg == "MODELING")
    {
      aPlugins.Add ("TOPTEST");
    }
    else if (anArg == "VISUALIZATION")
    {
      aPlugins.Add ("AISV");
    }
    else if (anArg == "OCAFKERNEL")
    {
      aPlugins.Add ("DCAF");
    }
    else if (anArg == "DATAEXCHANGEKERNEL")
    {
      aPlugins.Add ("XSDRAW");
    }
    else if (anArg == "OCAF")
    {
      aPlugins.Add ("AISV");
      aPlugins.Add ("DCAF");
    }
    else if (anArg == "DATAEXCHANGE")
    {
      aPlugins.Add ("XSDRAW");
      aPlugins.Add ("XDEDRAW");
      aPlugins.Add ("AISV");
    }
    else if (anArg == "XDE")
    {
      aPlugins.Add ("XSDRAW");
      aPlugins.Add ("XDEDRAW");
    }
    else if (anArg == "ALL")
    {
      aPlugins.Add ("TOPTEST");
      aPlugins.Add ("DCAF");
      aPlugins.Add ("XSDRAW");
      aPlugins.Add ("XDEDRAW");
      aPlugins.Add ("AISV");
    }
    else
    {
      aPlugins.Add (anArg);
    }
  }

  for (NCollection_IndexedMap<TCollection_AsciiString, TCollection_AsciiString>::Iterator aPluginIter (aPlugins);
       aPluginIter.More(); aPluginIter.Next())
  {
    const TCollection_AsciiString& aPlugin = aPluginIter.Value();
    if (aPlugin == "TOPTEST")
    {
      BOPTest::Factory (theDI);
    }
    else if (aPlugin == "DCAF")
    {
      DPrsStd::Factory (theDI);
    }
    else if (aPlugin == "AISV")
    {
      ViewerTest::Factory (theDI);
    }
  #if defined(HAVE_OPENGL)
    else if (aPlugin == "GL"
          || aPlugin == "OPENGL")
    {
      OpenGlTest::Factory (theDI);
    }
  #endif
  #if defined(HAVE_GLES2)
    else if (aPlugin == "GLES"
          || aPlugin == "OPENGLES")
    {
      OpenGlTest::Factory (theDI);
    }
  #endif
    else if (aPlugin == "XSDRAW")
    {
      XSDRAWSTLVRML::Factory (theDI);
    }
    else if (aPlugin == "XDEDRAW")
    {
      XDEDRAW::Factory (theDI);
    }
    //else if (aPlugin == "TOBJ")       { TObjDRAW::Factory (theDI); }
    //else if (aPlugin == "QACOMMANDS") { QADraw::Factory (theDI); }
    else
    {
      theDI << "Error: unknown plugin '" << aPlugin << "'";
      return 1;
    }
  }

  return 0;
}
#endif

//=======================================================================
//function : Draw_InitAppli
//purpose  : 
//=======================================================================

void Draw_InitAppli (Draw_Interpretor& theDI)
{
#if defined(__EMSCRIPTEN__)
  // open JavaScript console within the Browser to see this output
  Handle(Message_PrinterSystemLog) aJSConsolePrinter = new Message_PrinterSystemLog ("DRAWEXE");
  Message::DefaultMessenger()->AddPrinter (aJSConsolePrinter);
#endif

  Draw::Commands (theDI);
  DBRep::BasicCommands (theDI);
  DrawTrSurf::BasicCommands (theDI);

#ifdef OCCT_NO_PLUGINS
  theDI.Add ("pload" , "pload [[Key1] [Key2] ...]: Loads Draw plugins",
             __FILE__, Pload, "Draw Plugin");
#endif
}

#include <Draw_Main.hxx>
DRAW_MAIN
