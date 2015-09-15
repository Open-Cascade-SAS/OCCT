// Created on: 1998-06-29
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <AppStd_Application.hxx>
#include <DDataStd.hxx>
#include <DDF.hxx>
#include <DDocStd.hxx>
#include <DNaming.hxx>
#include <DPrsStd.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>

//#include <AppStdL_Application.hxx>
// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef _MSC_VER
#pragma warning(4:4190)
#endif
//static Handle(AppStdL_Application) stdApp;
static Handle(AppStd_Application) stdApp;
//=======================================================================
//function : AllComands
//purpose  : 
//=======================================================================

void DPrsStd::AllCommands (Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  // APPLICATION  
  if (stdApp.IsNull()) stdApp = new AppStd_Application ();//new AppStdL_Application ();

  DPrsStd::AISPresentationCommands(theCommands); 
  DPrsStd::AISViewerCommands(theCommands);  
  //DPrsStd::BasicCommands(theCommands);  
}

//==============================================================================
// Found standard CAF Plugin
//==============================================================================
static Standard_Boolean FoundCAFPlugin () {

 // Define Environment Variable 

  char *plugin, *casroot, *standard;
  plugin   = getenv("CSF_PluginDefaults");
  standard = getenv("CSF_StandardDefaults");
  casroot  = getenv("CASROOT");
  Standard_Boolean hasPlugin = Standard_False; 
  TCollection_AsciiString PluginName ;
  if ( !plugin ) { 
    if ( casroot ) {
      PluginName = TCollection_AsciiString (casroot);
      PluginName+="/src/StdResource" ;
      hasPlugin = Standard_True ;
    }
  } else {
    PluginName = TCollection_AsciiString (plugin);
    hasPlugin = Standard_True ;
  }
  if (  hasPlugin ) {
    OSD_Path aPath ( PluginName );
    OSD_Directory aDir(aPath);
    if ( aDir.Exists () ) {
      TCollection_AsciiString PluginFileName = PluginName + "/Plugin" ;
      OSD_File PluginFile ( PluginFileName );
      if ( PluginFile.Exists() ) {
	if (!plugin)   {
	  OSD_Environment PluginEnv   ( "CSF_PluginDefaults" , PluginName );
	  PluginEnv.Build();
	  if ( PluginEnv.Failed() ) {
	    cout << " Problem when initialise CSF_PluginDefaults whith " << PluginName.ToCString() << endl;
	  }
	}
	if (!standard) { 
	  OSD_Environment StandardEnv ( "CSF_StandardDefaults" , PluginName );
	  StandardEnv .Build();
	  if ( StandardEnv.Failed() ) {
	    cout << " Problem when initialise CSF_StandardDefaults whith " << PluginName.ToCString() << endl;
	  }
	}
      } else {
	hasPlugin = Standard_False; 
      }
    } else {
      hasPlugin = Standard_False; 
    }
  } 

  if ( !hasPlugin ) { 
    cout << " an environement variable named : CSF_PluginDefaults is mandatory to use OCAF " <<endl;
    Standard_Failure::Raise ( "an environement variable named : CSF_PluginDefaults is mandatory to use OCAF" );
  }  
  
  return hasPlugin ;
}

//==============================================================================
// DPrsStd::Factory
//==============================================================================
void DPrsStd::Factory(Draw_Interpretor& theDI)
{
  if(!FoundCAFPlugin()) 
    return;

  static Standard_Boolean DPrsStdFactoryDone = Standard_False;
  if (DPrsStdFactoryDone) return;
  DPrsStdFactoryDone = Standard_True;

  DDF::AllCommands(theDI);
  DNaming::AllCommands(theDI);
  DDataStd::AllCommands(theDI);  
  DPrsStd::AllCommands(theDI);
  DDocStd::AllCommands(theDI);
#ifdef OCCT_DEBUG
  cout << "Draw Plugin : All DF commands are loaded" << endl;
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(DPrsStd)
