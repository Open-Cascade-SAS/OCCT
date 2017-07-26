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

#include <ToolsDraw.hxx>

#include <AIS_InteractiveContext.hxx>
#include <DDocStd.hxx>
#include <Draw_PluginMacro.hxx>
#include <Standard_Stream.hxx>
#include <TDocStd_Application.hxx>
#include <TInspector_Communicator.hxx>
#include <ViewerTest.hxx>
#include <DDocStd.hxx>

static TInspector_Communicator* MyCommunicator;

// =======================================================================
// function : tinspector
// purpose  : 
// =======================================================================
static int tinspector (Draw_Interpretor&/* di*/, Standard_Integer theArgsNb, const char** theArgs)
{
  if (theArgsNb < 1)
  {
    std::cerr << "Error: wrong number of arguments.\n";
    return 1;
  }

  if (!MyCommunicator)
    MyCommunicator = new TInspector_Communicator();
  NCollection_List<Handle(Standard_Transient)> aParameters;

  NCollection_List<TCollection_AsciiString> aPlugins;
  bool aNeedToUpdateContent = false, aNeedToActivateAllPlugins = false;
  for (Standard_Integer anIt = 1; anIt < theArgsNb; ++anIt)
  {
    TCollection_AsciiString aParam (theArgs[anIt]);
    aParam.LowerCase();

    TCollection_AsciiString aPluginName;
    if (aParam == "-dfbrowser")
      aPluginName = "TKDFBrowser";
    else if (aParam == "-shapeview")
      aPluginName = "TKShapeView";
    else if (aParam == "-vinspector")
      aPluginName = "TKVInspector";
    else if (aParam == "-all")
      aNeedToActivateAllPlugins = true;
    else if (aParam == "-update")
      aNeedToUpdateContent = true;

    if (!aPluginName.IsEmpty() && !aPlugins.Contains (aPluginName))
      aPlugins.Append (aPluginName);
  }

  // DFBrowser only parameter
  const Handle(TDocStd_Application)& anApplication = DDocStd::GetApplication();
  // Initialize standard document formats at creation - they should
  // be available even if this DRAW plugin is not loaded by pload command
  if (!anApplication.IsNull())
    aParameters.Append (anApplication);

  const Handle(AIS_InteractiveContext)& aContext = ViewerTest::GetAISContext();
  if (!aContext.IsNull())
    aParameters.Append (aContext);

  if (aNeedToActivateAllPlugins)
  {
    MyCommunicator->RegisterPlugin ("TKDFBrowser");
    MyCommunicator->RegisterPlugin ("TKShapeView");
    MyCommunicator->RegisterPlugin ("TKVInspector");

    TCollection_AsciiString aLastPluginName = "TKDFBrowser";
    MyCommunicator->Init (aParameters);
    MyCommunicator->Activate (aLastPluginName);
  }

  if (!aNeedToActivateAllPlugins)
  {
    TCollection_AsciiString aLastPluginName;
    for (NCollection_List<TCollection_AsciiString>::Iterator aPluginNameIt (aPlugins);
         aPluginNameIt.More(); aPluginNameIt.Next())
    {
      aLastPluginName = aPluginNameIt.Value();
      MyCommunicator->RegisterPlugin (aLastPluginName);
      MyCommunicator->Init (aLastPluginName, aParameters);
    }
    MyCommunicator->Activate (aLastPluginName);
  }

  if (aNeedToUpdateContent)
    MyCommunicator->UpdateContent();

  MyCommunicator->SetVisible (true);
  return 0;
}

// =======================================================================
// function : Commands
// purpose  : 
// =======================================================================
void ToolsDraw::Commands(Draw_Interpretor& theCommands)
{
  const char *group = "Tools";

  // display
  theCommands.Add ("tinspector",
    "tinspector\n"
    "[-all]\n"
    "[-tinspector]\n"
    "[-dfbrowser]\n"
    "[-shaperview]\n"
    "[-vinspector]\n"
    "[-update]\n"
    "\tStarts tool of inspection.\n",
      __FILE__, tinspector, group);
}

// =======================================================================
// function : Factory
// purpose  : 
// =======================================================================
void ToolsDraw::Factory (Draw_Interpretor& theDI)
{
  // definition of Tools Command
  ToolsDraw::Commands (theDI);

#ifdef OCCT_DEBUG
      theDI << "Draw Plugin : OCC Tools commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN (ToolsDraw)
