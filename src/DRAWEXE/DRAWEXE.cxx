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
#include <Message_PrinterOStream.hxx>
#include <Message_PrinterSystemLog.hxx>
#include <NCollection_IndexedMap.hxx>
#include <OSD.hxx>
#include <OSD_Thread.hxx>
#include <Standard_ErrorHandler.hxx>

#ifdef OCCT_NO_PLUGINS
  #include <BOPTest.hxx>
  #include <DPrsStd.hxx>
  #if defined(HAVE_OPENGL) || defined(HAVE_GLES2)
  #include <OpenGlTest.hxx>
  #endif
  #include <TObjDRAW.hxx>
  #include <ViewerTest.hxx>
  #include <XSDRAW.hxx>
  #include <XDEDRAW.hxx>
  #include <XSDRAWSTEP.hxx>
  #include <XSDRAWIGES.hxx>
  #include <XSDRAWGLTF.hxx>
  #include <XSDRAWOBJ.hxx>
  #include <XSDRAWPLY.hxx>
  #include <XSDRAWVRML.hxx>
  #include <XSDRAWSTL.hxx>
#endif

Standard_IMPORT Standard_Boolean Draw_Interprete (const char* theCommand);

#if defined(__EMSCRIPTEN__)
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/threading.h>

//! Signal async command completion to Module.evalAsyncCompleted callback.
EM_JS(void, occJSEvalAsyncCompleted, (int theResult), {
  if (Module.evalAsyncCompleted != undefined) {
    Module.evalAsyncCompleted (theResult);
  } else {
    console.error ("Module.evalAsyncCompleted() is undefined");
  }
});

//! Draw Harness interface for JavaScript.
class DRAWEXE
{
public:
  //! Evaluate Tcl command.
  static int eval (const std::string& theCommand)
  {
    int aRes = 0;
    try
    {
      OCC_CATCH_SIGNALS
      //aRes = Draw::GetInterpretor().Eval (theCommand.c_str());
      aRes = Draw_Interprete (theCommand.c_str()) ? 1 : 0;
    }
    catch (Standard_Failure& anExcept)
    {
      std::cout << "Failed to evaluate command: " << anExcept.GetMessageString() << std::endl;
    }
    return aRes;
  }

  //! Check if Tcl command is complete.
  static bool isComplete (const std::string& theCommand)
  {
    return Draw::GetInterpretor().Complete (theCommand.c_str());
  }

  //! Evaluate Tcl command asynchronously.
  static void evalAsync (const std::string& theCommand)
  {
  #if defined(__EMSCRIPTEN_PTHREADS__)
    std::string* aCmdPtr = new std::string (theCommand);
    OSD_Thread aThread (&evalAsyncEntry);
    aThread.Run (aCmdPtr);
  #else
    // fallback synchronous implementation
    int aRes = eval (theCommand);
    occJSEvalAsyncCompleted (aRes);
  #endif
  }

#if defined(__EMSCRIPTEN_PTHREADS__)
private:
  //! Thread entry for async command execution.
  static Standard_Address evalAsyncEntry (Standard_Address theData)
  {
    OSD::SetSignal (false);
    std::string* aCmdPtr = (std::string* )theData;
    const std::string aCmd = *aCmdPtr;
    delete aCmdPtr;
    int aRes = eval (aCmd);
    emscripten_async_run_in_main_runtime_thread (EM_FUNC_SIG_VI, evalAsyncCompletedEntry, aRes);
    return 0;
  }

  //! Notify Module.evalAsyncCompleted about async cmd completion.
  static void evalAsyncCompletedEntry (int theResult)
  {
    occJSEvalAsyncCompleted (theResult);
  }
#endif
};

//! Print message to Module.printMessage callback.
EM_JS(void, occJSPrintMessage, (const char* theStr, int theGravity), {
  if (Module.printMessage != undefined && Module.printMessage != null) {
    Module.printMessage (UTF8ToString(theStr), theGravity);
  } else if (Module.print != undefined && Module.print != null) {
    Module.print (UTF8ToString(theStr));
  } else {
    //console.info (UTF8ToString(theStr));
  }
});

//! Auxiliary printer to a Module.printMessage callback accepting text and gravity.
class DRAWEXE_WasmModulePrinter : public Message_Printer
{
  DEFINE_STANDARD_RTTI_INLINE(DRAWEXE_WasmModulePrinter, Message_Printer)
public:

  //! Main constructor.
  DRAWEXE_WasmModulePrinter (const Message_Gravity theTraceLevel = Message_Info)
  {
    SetTraceLevel (theTraceLevel);
  }

  //! Destructor.
  virtual ~DRAWEXE_WasmModulePrinter() {}

protected:

  //! Puts a message.
  virtual void send (const TCollection_AsciiString& theString,
                     const Message_Gravity theGravity) const Standard_OVERRIDE
  {
    if (theGravity >= myTraceLevel)
    {
      occJSPrintMessage (theString.ToCString(), (int )theGravity);
    }
  }
};

EMSCRIPTEN_BINDINGS(DRAWEXE) {
  emscripten::function("eval",       &DRAWEXE::eval);
  emscripten::function("evalAsync",  &DRAWEXE::evalAsync);
  emscripten::function("isComplete", &DRAWEXE::isComplete);
}
#endif

#ifdef OCCT_NO_PLUGINS
//! Mimic pload command by loading pre-defined set of statically linked plugins.
static Standard_Integer Pload (Draw_Interpretor& theDI,
                               Standard_Integer  theNbArgs,
                               const char**      theArgVec)
{
  NCollection_IndexedMap<TCollection_AsciiString> aPlugins;
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
      aPlugins.Add ("XSDRAWSTEP");
      aPlugins.Add ("XSDRAWIGES");
      aPlugins.Add ("XSDRAWGLTF");
      aPlugins.Add ("XSDRAWOBJ");
      aPlugins.Add ("XSDRAWPLY");
      aPlugins.Add ("XSDRAWVRML");
      aPlugins.Add ("XSDRAWSTL");
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
      aPlugins.Add ("XSDRAWSTEP");
      aPlugins.Add ("XSDRAWIGES");
      aPlugins.Add ("XSDRAWGLTF");
      aPlugins.Add ("XSDRAWOBJ");
      aPlugins.Add ("XSDRAWPLY");
      aPlugins.Add ("XSDRAWVRML");
      aPlugins.Add ("XSDRAWSTL");
    }
    else
    {
      aPlugins.Add (anArg);
    }
  }

  for (NCollection_IndexedMap<TCollection_AsciiString>::Iterator aPluginIter (aPlugins);
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
      XSDRAW::Factory (theDI);
    }
    else if (aPlugin == "XDEDRAW")
    {
      XDEDRAW::Factory (theDI);
    }
    else if (aPlugin == "STEP")
    {
      XSDRAWSTEP::Factory (theDI);
    }
    else if (aPlugin == "IGES")
    {
      XSDRAWIGES::Factory (theDI);
    }
    else if (aPlugin == "PLY")
    {
      XSDRAWPLY::Factory (theDI);
    }
    else if (aPlugin == "GLTF")
    {
      XSDRAWGLTF::Factory (theDI);
    }
    else if (aPlugin == "VRML")
    {
      XSDRAWVRML::Factory (theDI);
    }
    else if (aPlugin == "STL")
    {
      XSDRAWSTL::Factory (theDI);
    }
    else if (aPlugin == "OBJ")
    {
      XSDRAWOBJ::Factory (theDI);
    }
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
  Message_Gravity aGravity = Message_Info;
  Handle(Message_PrinterSystemLog) aJSConsolePrinter = new Message_PrinterSystemLog ("DRAWEXE", aGravity);
  Message::DefaultMessenger()->AddPrinter (aJSConsolePrinter);
  // replace printer into std::cout by a printer into a custom callback Module.printMessage accepting message gravity
  Message::DefaultMessenger()->RemovePrinters (STANDARD_TYPE(Message_PrinterOStream));
  Handle(DRAWEXE_WasmModulePrinter) aJSModulePrinter = new DRAWEXE_WasmModulePrinter (aGravity);
  Message::DefaultMessenger()->AddPrinter (aJSModulePrinter);
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
