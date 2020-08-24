// Created on: 2003-10-09
// Created by: Mikhail KUZMITCHEV
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

#include <Draw_Interpretor.hxx>
#include <Draw_MapOfAsciiString.hxx>
#include <Draw.hxx>
#include <Message.hxx>
#include <OSD_Path.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Environment.hxx>
#include <OSD_SharedLibrary.hxx>
#include <Resource_Manager.hxx>
#include <TCollection_AsciiString.hxx>

//! Searches for the existence of the plugin file according to its name thePluginName:
//! - if thePluginName is empty then it defaults to DrawPlugin
//! - the search directory is defined according to the variable
//!   CSF_<filename>Defaults (if it is omitted then it defaults to
//!   $CASROOT/src/DrawResources)
//! - finally existence of the file is verified in the search directory
//! - if the file exists but corresponding variable (CSF_...) has not been
//!   explicitly set, it is forced to (for further reuse by Resource_Manager)
//! @return TRUE if the file exists, otherwise - False
static Standard_Boolean findPluginFile (TCollection_AsciiString& thePluginName,
                                        TCollection_AsciiString& thePluginDir)
{
  // check if the file name has been specified and use default value if not
  if (thePluginName.IsEmpty())
  {
    thePluginName += "DrawPlugin";
#ifdef OCCT_DEBUG
    std::cout << "Plugin file name has not been specified. Defaults to " << thePluginName.ToCString() << std::endl;
#endif
  }

  Standard_Boolean aToSetCSFVariable = Standard_False;
  
  // the order of search : by CSF_<PluginFileName>Defaults and then by CASROOT
  const TCollection_AsciiString aCSFVariable = TCollection_AsciiString ("CSF_") + thePluginName + "Defaults";
  thePluginDir = OSD_Environment (aCSFVariable).Value();
  if (thePluginDir.IsEmpty())
  {
    thePluginDir = OSD_Environment ("DRAWHOME").Value();
    if (!thePluginDir.IsEmpty())
    {
      aToSetCSFVariable = Standard_True; //CSF variable to be set later
    }
    else
    {
      // now try by CASROOT
      thePluginDir = OSD_Environment ("CASROOT").Value();
      if (!thePluginDir.IsEmpty())
      {
        thePluginDir += "/src/DrawResources";
        aToSetCSFVariable = Standard_True; //CSF variable to be set later
      }
      else
      {
        Message::SendFail() << "Failed to load plugin: Neither " << aCSFVariable << ", nor CASROOT variables have been set";
        return Standard_False;
      }
    }
  }

  // search directory name has been constructed, now check whether it and the file exist
  const TCollection_AsciiString aPluginFileName = thePluginDir + "/" + thePluginName;
  OSD_File aPluginFile (aPluginFileName);
  if (!aPluginFile.Exists())
  {
    Message::SendFail() << "Failed to load plugin: File " << aPluginFileName << " not found";
    return Standard_False;
  }

  if (aToSetCSFVariable)
  {
    OSD_Environment aCSFVarEnv (aCSFVariable, thePluginDir);
    aCSFVarEnv.Build();
#ifdef OCCT_DEBUG
    std::cout << "Variable " << aCSFVariable << " has not been explicitly defined. Set to " << thePluginDir << std::endl;
#endif
    if (aCSFVarEnv.Failed())
    {
      Message::SendFail() << "Failed to load plugin: Failed to initialize " << aCSFVariable << " with " << thePluginDir;
      return Standard_False;
    }
  }
  
  return Standard_True;
}

//! Resolve keys within input map (groups, aliases and toolkits) to the list of destination toolkits (plugins to load).
//! @param theMap [in] [out] map to resolve (will be rewritten)
//! @param theResMgr [in] resource manager to resolve keys
static void resolveKeys (Draw_MapOfAsciiString& theMap,
                         const Handle(Resource_Manager)& theResMgr)
{
  if (theResMgr.IsNull())
  {
    return;
  }

  Draw_MapOfAsciiString aMap, aMap2;
  const Standard_Integer aMapExtent = theMap.Extent();
  for (Standard_Integer j = 1; j <= aMapExtent; ++j)
  {
    TCollection_AsciiString aValue;
    const TCollection_AsciiString aResource = theMap.FindKey (j);
    if (theResMgr->Find (aResource, aValue))
    {
    #ifdef OCCT_DEBUG
      std::cout << "Parse Value ==> " << aValue << std::endl;
    #endif
      for (Standard_Integer aKeyIter = 1;; ++aKeyIter)
      {
        const TCollection_AsciiString aCurKey = aValue.Token (" \t,", aKeyIter);
      #ifdef OCCT_DEBUG
        std::cout << "Parse aCurKey = " << aCurKey << std::endl;
      #endif
        if (aCurKey.IsEmpty())
        {
          break;
        }

        if (theResMgr->Find (aCurKey.ToCString()))
        {
          aMap2.Add (aCurKey);
        }
        else
        {
          aMap.Add (aResource); // It is toolkit
        }
      }
    }
    else
    {
      Message::SendFail() << "Pload : Resource = " << aResource << " is not found";
    }

    if (!aMap2.IsEmpty())
    {
      resolveKeys (aMap2, theResMgr);
    }

    //
    const Standard_Integer aMap2Extent = aMap2.Extent();
    for (Standard_Integer k = 1; k <= aMap2Extent; ++k)
    {
      aMap.Add (aMap2.FindKey (k));
    }
  }

  theMap.Assign (aMap);
}

//=======================================================================
//function : Pload
//purpose  : 
//=======================================================================

static Standard_Integer Pload (Draw_Interpretor& theDI,
                               Standard_Integer  theNbArgs,
                               const char**      theArgVec)
{
  Draw_MapOfAsciiString aMap;
  TCollection_AsciiString aPluginFileName;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    const TCollection_AsciiString aTK (theArgVec[anArgIter]);
    if (anArgIter == 1
     && aTK.Value (1) == '-')
    {
      aPluginFileName = aTK.SubString (2, aTK.Length());
    }
    else
    {
      aMap.Add (aTK);
    }
  }
  if (aMap.IsEmpty())
  {
    aMap.Add ("DEFAULT"); // Load DEFAULT key
  }

  TCollection_AsciiString aPluginDir, aPluginDir2;
  if (!findPluginFile (aPluginFileName, aPluginDir))
  {
    return 1;
  }

  Handle(Resource_Manager) aResMgr = new Resource_Manager (aPluginFileName.ToCString(), aPluginDir, aPluginDir2, Standard_False);
  resolveKeys (aMap, aResMgr);

  const Standard_Integer aMapExtent = aMap.Extent();
  for (Standard_Integer aResIter = 1; aResIter <= aMapExtent; ++aResIter)
  {
    const TCollection_AsciiString aResource = aMap.FindKey (aResIter);
  #ifdef OCCT_DEBUG
    std::cout << "aResource = " << aResource << std::endl;
  #endif
    TCollection_AsciiString aValue;
    if (!aResMgr->Find (aResource, aValue))
    {
      Message::SendWarning() <<"Pload : Resource = " << aResource << " is not found";
      continue;
    }

  #ifdef OCCT_DEBUG
    std::cout << "Value ==> " << aValue << std::endl;
  #endif

    Draw::Load (theDI, aResource, aPluginFileName, aPluginDir, aPluginDir2, Standard_False);

    // Load TclScript
    const TCollection_AsciiString aTclScriptDir = OSD_Environment ("CSF_DrawPluginTclDir").Value();
    const TCollection_AsciiString aTclScriptFileName         = aTclScriptDir + "/" + aValue + ".tcl";
    const TCollection_AsciiString aTclScriptFileNameDefaults = aPluginDir    + "/" + aValue + ".tcl";
    OSD_File aTclScriptFile (aTclScriptFileName);
    OSD_File aTclScriptFileDefaults (aTclScriptFileNameDefaults);
    if (!aTclScriptDir.IsEmpty()
      && aTclScriptFile.Exists())
    {
    #ifdef OCCT_DEBUG
      std::cout << "Load " << aTclScriptFileName << " TclScript" << std::endl;
    #endif
      theDI.EvalFile (aTclScriptFileName.ToCString());
    }
    else if (!aPluginDir.IsEmpty()
           && aTclScriptFileDefaults.Exists())
    {
    #ifdef OCCT_DEBUG
      std::cout << "Load " << aTclScriptFileNameDefaults << " TclScript" << std::endl;
    #endif
      theDI.EvalFile (aTclScriptFileNameDefaults.ToCString());
    }
  }
  return 0;
}

//=======================================================================
//function : dtryload
//purpose  : 
//=======================================================================

static Standard_Integer dtryload (Draw_Interpretor& di, Standard_Integer n, const char** argv)
{
  if (n != 2)
  {
    Message::SendFail() << "Error: specify path to library to be loaded";
    return 1;
  }

  OSD_SharedLibrary aLib(argv[1]);
  if (aLib.DlOpen(OSD_RTLD_NOW))
  {
    di << "Loading " << argv[1] << " successful";
    aLib.DlClose();
  }
  else 
  {
    di << "Loading " << argv[1] << " failed: " << aLib.DlError();
  }
  return 0;
}

//=======================================================================
//function : PloadCommands
//purpose  : 
//=======================================================================

void Draw::PloadCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean Done = Standard_False;
  if (Done) return;
  Done = Standard_True;

  const char* g = "Draw Plugin";
  
  theCommands.Add("pload" , "pload [-PluginFilename] [[Key1] [Key2] ...]: Loads Draw plugins " ,
		  __FILE__, Pload, g);
  theCommands.Add("dtryload" , "dtryload path : load and unload specified dynamic loaded library" ,
		  __FILE__, dtryload, g);
}
