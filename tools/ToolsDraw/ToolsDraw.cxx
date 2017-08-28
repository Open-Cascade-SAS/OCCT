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

#include <inspector/ToolsDraw.hxx>
#include <inspector/ToolsDraw.hxx>

#include <AIS_InteractiveContext.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Stream.hxx>
#include <TDocStd_Application.hxx>
#include <inspector/TInspector_Communicator.hxx>
#include <TopoDS_Shape.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>

#if ! defined(_WIN32)
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
#else
Standard_EXPORT ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
#endif

static TInspector_Communicator* MyCommunicator;

// =======================================================================
// function : convertToPluginName
// purpose  : defines plugin library name by the command argument
// =======================================================================
Standard_Boolean convertToPluginName (const TCollection_AsciiString& theArgument,
                                      TCollection_AsciiString& thePluginName)
{
  TCollection_AsciiString anArgument = theArgument;
  anArgument.LowerCase();

  if (anArgument == "dfbrowser")       { thePluginName = "TKDFBrowser"; return Standard_True; }
  else if (anArgument == "shapeview")  { thePluginName = "TKShapeView"; return Standard_True; }
  else if (anArgument == "vinspector") { thePluginName = "TKVInspector"; return Standard_True; }

  return Standard_False;
}

// =======================================================================
// function : getArgumentPlugins
// purpose  : fills container of plugin names by the next following plugin names
// =======================================================================
void getArgumentPlugins (Standard_Integer theArgsNb, const char** theArgs, Standard_Integer& theIt,
                         NCollection_List<TCollection_AsciiString>& thePlugins)
{
  while (theIt != theArgsNb)
  {
    TCollection_AsciiString aPluginName;
    if (convertToPluginName (theArgs[theIt], aPluginName))
    {
      if (!thePlugins.Contains (aPluginName))
        thePlugins.Append (aPluginName);
    }
    else
    {
      break;
    }
    theIt++;
  }
  theIt--; // the last not processed parameter is the next argument
}

// =======================================================================
// function : tinspector
// purpose  : 
// =======================================================================
static int tinspector (Draw_Interpretor&/* di*/, Standard_Integer theArgsNb, const char** theArgs)
{
  if (theArgsNb < 1)
  {
    std::cout << "Error: wrong number of arguments.\n";
    return 1;
  }

  // parse command arguments
  NCollection_List<TCollection_AsciiString> aPlugins;
  NCollection_DataMap<TCollection_AsciiString, NCollection_List<Handle(Standard_Transient)> > aParameters;
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString > anOpenFileParameters;
  TCollection_AsciiString aPluginNameToActivate;
  Standard_Boolean aNeedToUpdateContent = Standard_False,
                   aNeedToHideInspector = Standard_False,
                   aNeedToShowInspector = Standard_False;

  NCollection_List<Handle(Standard_Transient)> aDefaultParameters;
  TCollection_AsciiString aDefaultOpenFileParameter;

  NCollection_List<Handle(Standard_Transient)> anObjectsToSelect;
  NCollection_List<TCollection_AsciiString> anItemNamesToSelect;

  for (Standard_Integer anIt = 1; anIt < theArgsNb; ++anIt)
  {
    TCollection_AsciiString aParam (theArgs[anIt]);
    aParam.LowerCase();

    if (aParam.IsEqual ("-plugins")) // [-plugins {name1 [name2] ... [name3] | all}]
    {
      anIt++;
      getArgumentPlugins(theArgsNb, theArgs, anIt, aPlugins);
    }
    else if (aParam.IsEqual ("-activate")) // [-activate name]
    {
      anIt++;
      if (anIt == theArgsNb)
      {
        cout << "Empty argument of '" << aParam << "'.\n";
        return 1;
      }
      TCollection_AsciiString aPluginName;
      if (convertToPluginName (theArgs[anIt], aPluginName))
        aPluginNameToActivate = aPluginName;
    }
    else if (aParam.IsEqual ("-shape")) // [-shape object [name1] ... [nameN]]
    {
      anIt++;
      if (anIt == theArgsNb)
      {
        cout << "Empty argument of '" << aParam << "'.\n";
        return 1;
      }
      TopoDS_Shape aShape = DBRep::Get (theArgs[anIt]);
      anIt++;
      if (aShape.IsNull())
      {
        cout << "Wrong shape name: " << aParam << ".\n";
        return 1;
      }
      NCollection_List<TCollection_AsciiString> anArgPlugins;
      getArgumentPlugins(theArgsNb, theArgs, anIt, anArgPlugins);
      if (anArgPlugins.IsEmpty())
        aDefaultParameters.Append(aShape.TShape());
      else
      {
        for (NCollection_List<TCollection_AsciiString>::Iterator anArgIt (anArgPlugins);
          anArgIt.More(); anArgIt.Next())
        {
          NCollection_List<Handle(Standard_Transient)> aPluginParameters;
          aParameters.Find(anArgIt.Value(), aPluginParameters);
          aPluginParameters.Append(aShape.TShape());
          aParameters.Bind (anArgIt.Value(), aPluginParameters);
        }
      }
    }
    else if (aParam.IsEqual ("-open")) // [-open file_name [name1] ... [nameN]]
    {
      anIt++;
      if (anIt == theArgsNb)
      {
        cout << "Empty argument of '" << aParam << "'.\n";
        return 1;
      }
      TCollection_AsciiString aFileName (theArgs[anIt]);
      anIt++;

      NCollection_List<TCollection_AsciiString> anArgPlugins;
      getArgumentPlugins(theArgsNb, theArgs, anIt, anArgPlugins);
      if (anArgPlugins.IsEmpty())
        aDefaultOpenFileParameter = aFileName;
      else
      {
        for (NCollection_List<TCollection_AsciiString>::Iterator anArgIt (anArgPlugins);
          anArgIt.More(); anArgIt.Next())
        {
          NCollection_List<Handle(Standard_Transient)> aPluginParameters;
          aParameters.Find(anArgIt.Value(), aPluginParameters);
          anOpenFileParameters.Bind(anArgIt.Value(), aFileName);
        }
      }
    }
    else if (aParam.IsEqual ("-update")) // [-update]
    {
      aNeedToUpdateContent = Standard_True;
    }
    else if (aParam.IsEqual ("-select")) // [-select {name|object}]
    {
      anIt++;
      if (anIt == theArgsNb)
      {
        cout << "Empty argument of '" << aParam << "'.\n";
        return 1;
      }
      // search shape with given name
      TopoDS_Shape aShape = DBRep::Get (theArgs[anIt]);
      if (!aShape.IsNull())
      {
        anObjectsToSelect.Append(aShape.TShape());
      }
      // search prsentations with given name
      if (GetMapOfAIS().IsBound2(theArgs[anIt]))
      {
        Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast
          (GetMapOfAIS().Find2 (theArgs[anIt]));
        if (!anIO.IsNull())
        anObjectsToSelect.Append(anIO);
      }
      // give parameters as a container of names
      aParam = TCollection_AsciiString (theArgs[anIt]);
      while (!aParam.StartsWith ("-"))
      {
        anItemNamesToSelect.Append (aParam);
        anIt++;
        if (anIt >= theArgsNb)
          break;
        aParam = theArgs[anIt];
      }
      anIt--;
    }
    else if (aParam.IsEqual ("-show")) // [-show {0|1} = 1]
    {
      anIt++;
      if (anIt == theArgsNb)
      {
        cout << "Empty argument of '" << aParam << "'.\n";
        return 1;
      }
      aNeedToHideInspector = Draw::Atoi (theArgs[anIt]) == 0;
      aNeedToShowInspector = Draw::Atoi (theArgs[anIt]) > 0;
    }
    else
    {
      cout << "Wrong argument of command: " << aParam.ToCString() << "\n";
      return 1;
    }
  }

  // start inspector
  Standard_Boolean isTInspectorCreation = !MyCommunicator;
  if (!MyCommunicator)
    MyCommunicator = new TInspector_Communicator();

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (!aContext.IsNull())
    aDefaultParameters.Append (aContext);

  // Sets OCAF application into DFBrowser
  const Handle(TDocStd_Application)& anApplication = DDocStd::GetApplication();
  // Initialize standard document formats at creation - they should
  // be available even if this DRAW plugin is not loaded by pload command
  if (!anApplication.IsNull())
  {
    NCollection_List<Handle(Standard_Transient)> aDFBrowserParameters;
    aParameters.Find("TKDFBrowser", aDFBrowserParameters);
    aDFBrowserParameters.Append(anApplication);
    aParameters.Bind ("TKDFBrowser", aDFBrowserParameters);
  }

  // by starting, if the plugns were not defined, register all
  if (isTInspectorCreation)
  {
    if (aPlugins.IsEmpty())
    {
      aPlugins.Append("TKDFBrowser");
      aPlugins.Append("TKShapeView");
      aPlugins.Append("TKVInspector");
    }
    aPluginNameToActivate = !aPluginNameToActivate.IsEmpty() ? aPluginNameToActivate : aPlugins.First();
  }

  // register plugin from parameters
  for (NCollection_List<TCollection_AsciiString>::Iterator aPluginNameIt (aPlugins);
       aPluginNameIt.More(); aPluginNameIt.Next())
    MyCommunicator->RegisterPlugin (aPluginNameIt.Value());

  // init all registered plugins with the default and parameters values
  NCollection_List<TCollection_AsciiString> aRegisteredPlugins = MyCommunicator->RegisteredPlugins();
  for (NCollection_List<TCollection_AsciiString>::Iterator anIterator (aRegisteredPlugins);
    anIterator.More(); anIterator.Next())
  {
    TCollection_AsciiString aPluginName = anIterator.Value();
    NCollection_List<Handle(Standard_Transient)> aParameterValues;
    aParameters.Find (aPluginName, aParameterValues);

    for (NCollection_List<Handle(Standard_Transient)>::Iterator aDefIt(aDefaultParameters);
         aDefIt.More(); aDefIt.Next())
      aParameterValues.Append (aDefIt.Value());
    MyCommunicator->Init (aPluginName, aParameterValues, Standard_True);
  }

  if (!aPluginNameToActivate.IsEmpty())
    MyCommunicator->Activate (!aPluginNameToActivate.IsEmpty() ? aPluginNameToActivate : aPlugins.First());

  if (!anOpenFileParameters.IsEmpty())
  {
    for (NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString >::Iterator anOpenIt
      (anOpenFileParameters); anOpenIt.More(); anOpenIt.Next())
      MyCommunicator->OpenFile(anOpenIt.Key(), anOpenIt.Value());
  }
  else if (!aDefaultOpenFileParameter.IsEmpty()) // open file in active plugin
    MyCommunicator->OpenFile("", aDefaultOpenFileParameter);

  if (!anObjectsToSelect.IsEmpty())
    MyCommunicator->SetSelected(anObjectsToSelect);

  if (!anItemNamesToSelect.IsEmpty())
    MyCommunicator->SetSelected(anItemNamesToSelect);

  if (aNeedToUpdateContent)
    MyCommunicator->UpdateContent();

  if (isTInspectorCreation || aNeedToShowInspector)
    MyCommunicator->SetVisible (true);

  if (aNeedToHideInspector)
    MyCommunicator->SetVisible (false);

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
    "tinspector [-plugins {name1 ... [nameN] | all}]"
    "\n\t\t:            [-activate name]"
    "\n\t\t:            [-shape object [name1] ... [nameN]]"
    "\n\t\t:            [-open file_name [name1] ... [nameN]]"
    "\n\t\t:            [-update]"
    "\n\t\t:            [-select {object | name1 ... [nameN]}]"
    "\n\t\t:            [-show {0|1} = 1]"
    "\n\t\t: Starts tool of inspection."
    "\n\t\t: Options:"
    "\n\t\t:  -plugins enters plugins that should be added in the inspector."
    "\n\t\t:           Available names are: dfbrowser, vinspector and shapeview."
    "\n\t\t:           Plugins order will be the same as defined in arguments."
    "\n\t\t:           'all' adds all available plugins in the order:"
    "\n\t\t:                 DFBrowser, VInspector and ShapeView."
    "\n\t\t:           If at the first call this option is not used, 'all' option is applyed;"
    "\n\t\t:  -activate activates the plugin in the tool view."
    "\n\t\t:           If at the first call this option is not used, the first plugin is activated;"
    "\n\t\t:  -shape initializes plugin/s by the shape object. If 'name' is empty, initializes all plugins;"
    "\n\t\t:  -open gives the file to the plugin/s. If the plugin is active, after open, update content will be done;"
    "\n\t\t:  -update updates content of the active plugin;"
    "\n\t\t:  -select sets the parameter that should be selected in an active tool view."
    "\n\t\t:          Depending on active tool the parameter is:"
    "\n\t\t:          ShapeView: 'object' is an instance of TopoDS_Shape TShape,"
    "\n\t\t:          DFBrowser: 'name' is an entry of TDF_Label and name2(optionaly) for TDF_Attribute type name,"
    "\n\t\t:          VInspector: 'object' is an instance of AIS_InteractiveObject;"
    "\n\t\t:  -show sets Inspector view visible or hidden. The first call of this command will show it.",
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
