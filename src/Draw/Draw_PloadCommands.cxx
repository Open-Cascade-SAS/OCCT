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

#include <TCollection_AsciiString.hxx>
#include <OSD_Path.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Environment.hxx>
#include <OSD_SharedLibrary.hxx>
#include <Resource_Manager.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_MapOfAsciiString.hxx>
#include <Draw.hxx>

static Handle(Resource_Manager) myResources;

//=======================================================================
//function : FindPluginFile
//purpose  : Searches for the existence of the plugin file according to its name thePluginName:
//           - if thePluginName is empty then it defaults to DrawPlugin
//           - the search directory is defined according to the variable
//             CSF_<filename>Defaults (if it is omitted then it defaults to
//             $CASROOT/src/DrawResources)
//           - finally existence of the file is verified in the search directory
//           - if the file exists but corresponding variable (CSF_...) has not been
//             explicitly set, it is forced to (for further reuse by Resource_Manager)
//           Returns True if the file exists, otherwise - False.
//=======================================================================

#define FAILSTR "Failed to load plugin: "
     
//static Standard_Boolean FindPluginFile (TCollection_AsciiString& thePluginName)
static Standard_Boolean FindPluginFile (TCollection_AsciiString& thePluginName, TCollection_AsciiString& aPluginDir)
{
  Standard_Boolean aResult = Standard_True;
  
  // check if the file name has been specified and use default value if not
  if (thePluginName.IsEmpty()) {
    thePluginName += "DrawPlugin";
#ifdef OCCT_DEBUG
    cout << "Plugin file name has not been specified. Defaults to " << thePluginName.ToCString() << endl;
#endif
  }

  //TCollection_AsciiString aPluginDir; // the search directory
  Standard_Boolean aDirFound = Standard_True, aToSetCSFVariable = Standard_False;
  
  // the order of search : by CSF_<PluginFileName>Defaults and then by CASROOT
  TCollection_AsciiString aCSFVariable = TCollection_AsciiString ("CSF_") + thePluginName + "Defaults";
  aPluginDir = getenv (aCSFVariable.ToCString());

  if (aPluginDir.IsEmpty()) {
    // now try by CASROOT
    aPluginDir = getenv("CASROOT");

    if ( !aPluginDir.IsEmpty() ) {
      aPluginDir +="/src/DrawResources" ;
      aToSetCSFVariable = Standard_True; //CSF variable to be set later
    } else {
      aResult = aDirFound = Standard_False;
      cout << FAILSTR "Neither " << aCSFVariable.ToCString() << ", nor CASROOT variables have been set" << endl;
    }
  }
  
  if (aDirFound) {
    // search directory name has been constructed, now check whether it and the file exist
    
    TCollection_AsciiString aPluginFileName = aPluginDir + "/" + thePluginName;
    OSD_File PluginFile ( aPluginFileName );
    if ( PluginFile.Exists() ) {
      if (aToSetCSFVariable) {
        OSD_Environment aCSFVarEnv ( aCSFVariable, aPluginDir );
        aCSFVarEnv.Build();
#ifdef OCCT_DEBUG
        cout << "Variable " << aCSFVariable.ToCString() << " has not been explicitly defined. Set to " << aPluginDir.ToCString() << endl;
#endif
        if ( aCSFVarEnv.Failed() ) {
          aResult = Standard_False;
          cout << FAILSTR "Failed to initialize " << aCSFVariable.ToCString() << " with " << aPluginDir.ToCString() << endl;
        }
      }
    } else {
      aResult = Standard_False;
      cout << FAILSTR "File " << aPluginFileName.ToCString() << " not found" << endl;
    }
  }
  
  return aResult;
}

//=======================================================================
//function : Parse
//purpose  : Parse the input keys to atomic keys (<key> --> <akey>[<akey> ..])
//=======================================================================

static void Parse (Draw_MapOfAsciiString& theMap)
{
  Draw_MapOfAsciiString aMap, aMap2;
  Standard_Integer j, k;
  Standard_Integer aMapExtent, aMap2Extent;
  aMapExtent = theMap.Extent();
  for(j = 1; j <= aMapExtent; j++) {
    if (!myResources.IsNull()) {
      const TCollection_AsciiString& aKey = theMap.FindKey(j);
      TCollection_AsciiString aResource = aKey;
      if(myResources->Find(aResource.ToCString())) {
#ifdef OCCT_DEBUG
	cout << "Parse Value ==> " << myResources->Value(aResource.ToCString()) << endl;
#endif
	TCollection_AsciiString aValue(myResources->Value(aResource.ToCString()));
	// parse aValue string
	Standard_Integer i=1;
	for(;;) {
	  TCollection_AsciiString aCurKey = aValue.Token(" \t,", i++);
#ifdef OCCT_DEBUG
	  cout << "Parse aCurKey = " << aCurKey.ToCString() << endl;
#endif
	  if(aCurKey.IsEmpty()) break;
	  if(!myResources->Find(aCurKey.ToCString())) {
	    // It is toolkit
	    aMap.Add(aResource);
	  }
	  else
	    aMap2.Add(aCurKey);
	}
      } else
	cout <<"Pload : Resource = " << aResource << " is not found" << endl;
      if(!aMap2.IsEmpty())
	Parse(aMap2);
      //
      aMap2Extent = aMap2.Extent();
      for(k = 1; k <= aMap2Extent; k++) {
	aMap.Add(aMap2.FindKey(k));
      }

    }
  }

  theMap.Assign(aMap);
}

//=======================================================================
//function : Pload
//purpose  : 
//=======================================================================

static Standard_Integer Pload (Draw_Interpretor& di,
                               Standard_Integer  n,
                               const char**      argv)
{
  char adef[] = "-";
  TCollection_AsciiString aPluginFileName("");
  TCollection_AsciiString aPluginDir(""), aPluginDir2("");
  Standard_Integer aStart = 0;
  Standard_Integer aFinish = n - 1;

  if (n == 1) {
    // Load DEFAULT key
    aStart = 0;
  } else {
    if(argv[1][0] == adef[0]) {
      aPluginFileName = argv[1];
      aPluginFileName.Remove(1,1);
      if (n == 2) {
	// Load DEFAULT key from aPluginFileName file
	aStart = 0;
	aFinish = n - 2;
      } else {
	aStart = 2;
      }
    } else {
      aStart = 1;
    }
  }

  //if ( !FindPluginFile (aPluginFileName) ) {
  if ( !FindPluginFile (aPluginFileName, aPluginDir) ) {
    return 1;
  } 

  Draw_MapOfAsciiString aMap;
  TCollection_AsciiString aDEFAULT("DEFAULT");
  //for(Standard_Integer i = aStart; i < n; i++) 
  for(Standard_Integer i = aStart; i <= aFinish; i++) 
    if (i == 0) {
      // Load DEFAULT key
      aMap.Add(aDEFAULT);
    } else {
      TCollection_AsciiString aTK(argv[i]);
      aMap.Add(aTK);
    }
  
  //myResources = new Resource_Manager(aPluginFileName.ToCString());
  myResources = new Resource_Manager(aPluginFileName.ToCString(), aPluginDir, aPluginDir2, Standard_False);

  Parse(aMap);
  Standard_Integer j;
  Standard_Integer aMapExtent;
  aMapExtent = aMap.Extent();
  for(j = 1; j <= aMapExtent; j++) {
    const TCollection_AsciiString& aKey = aMap.FindKey(j);
    TCollection_AsciiString aResource = aKey;
#ifdef OCCT_DEBUG
      cout << "aResource = " << aResource << endl;
#endif
    if(myResources->Find(aResource.ToCString())) {
      const TCollection_AsciiString& aValue = myResources->Value(aResource.ToCString()); 
#ifdef OCCT_DEBUG
      cout << "Value ==> " << aValue << endl;
#endif
	
      //Draw::Load(di, aKey, aPluginFileName);
      Draw::Load(di, aKey, aPluginFileName, aPluginDir, aPluginDir2, Standard_False);

      // Load TclScript
      TCollection_AsciiString aCSFVariable ("CSF_DrawPluginTclDir");
      TCollection_AsciiString aTclScriptDir;
      aTclScriptDir = getenv (aCSFVariable.ToCString());
      TCollection_AsciiString aTclScriptFileName;
      TCollection_AsciiString aTclScriptFileNameDefaults;
      aTclScriptFileName = aTclScriptDir + "/" + aValue + ".tcl";
      aTclScriptFileNameDefaults = aPluginDir + "/" + aValue + ".tcl";
      OSD_File aTclScriptFile ( aTclScriptFileName );
      OSD_File aTclScriptFileDefaults ( aTclScriptFileNameDefaults );
      if (!aTclScriptDir.IsEmpty() && aTclScriptFile.Exists()) {
#ifdef OCCT_DEBUG
	cout << "Load " << aTclScriptFileName << " TclScript" << endl;
#endif
	di.EvalFile( aTclScriptFileName.ToCString() );
      } else if (!aPluginDir.IsEmpty() && aTclScriptFileDefaults.Exists()) {
#ifdef OCCT_DEBUG
	cout << "Load " << aTclScriptFileNameDefaults << " TclScript" << endl;
#endif
	di.EvalFile( aTclScriptFileNameDefaults.ToCString() );
      }
  
    } else 
      cout <<"Pload : Resource = " << aResource << " is not found" << endl;
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
    cout << "Error: specify path to library to be loaded" << endl;
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
