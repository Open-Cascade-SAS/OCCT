// Created on: 1997-03-06
// Created by: Mister rmi
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Plugin.ixx>
#include <Plugin_MapOfFunctions.hxx>
#include <OSD_SharedLibrary.hxx>
#include <Resource_Manager.hxx>
#include <Plugin_Failure.hxx>

#include <TCollection_AsciiString.hxx>

static Standard_Character tc[1000];
static Standard_PCharacter thePluginId = tc;


//=======================================================================
//function : Load
//purpose  : 
//=======================================================================
Handle(Standard_Transient) Plugin::Load(const Standard_GUID& aGUID) 
{
  
  aGUID.ToCString(thePluginId);
  TCollection_AsciiString pid(thePluginId);
  static Plugin_MapOfFunctions theMapOfFunctions;
  OSD_Function f;

  if(!theMapOfFunctions.IsBound(pid)) {
   
    Handle(Resource_Manager) PluginResource = new Resource_Manager("Plugin");
    TCollection_AsciiString theResource(thePluginId);
    theResource += ".Location";

    if(!PluginResource->Find(theResource.ToCString())) {
      Standard_SStream aMsg; aMsg << "could not find the resource:";
      aMsg << theResource.ToCString()<< endl;
      cout << "could not find the resource:"<<theResource.ToCString()<< endl;
      Plugin_Failure::Raise(aMsg);
    }
    
    TCollection_AsciiString thePluginLibrary("");
#ifndef WNT
    thePluginLibrary += "lib";
#endif
    thePluginLibrary +=  PluginResource->Value(theResource.ToCString());
#ifdef WNT
    thePluginLibrary += ".dll";
#elif defined(__APPLE__)
    thePluginLibrary += ".dylib";
#elif defined (HPUX) || defined(_hpux)
    thePluginLibrary += ".sl";
#else
    thePluginLibrary += ".so";
#endif  
    OSD_SharedLibrary theSharedLibrary(thePluginLibrary.ToCString());
    if(!theSharedLibrary.DlOpen(OSD_RTLD_LAZY)) {
      TCollection_AsciiString error(theSharedLibrary.DlError());
      Standard_SStream aMsg; aMsg << "could not open:";
      aMsg << PluginResource->Value(theResource.ToCString());
      aMsg << "; reason:";
      aMsg << error.ToCString();
      cout << "could not open: "  << PluginResource->Value(theResource.ToCString())<< " ; reason: "<< error.ToCString() << endl;
      Plugin_Failure::Raise(aMsg);
    }
    f = theSharedLibrary.DlSymb("PLUGINFACTORY");
    if( f == NULL ) {
      TCollection_AsciiString error(theSharedLibrary.DlError());
      Standard_SStream aMsg; aMsg << "could not find the factory in:";
      aMsg << PluginResource->Value(theResource.ToCString());
      aMsg << error.ToCString();
      Plugin_Failure::Raise(aMsg);
    }
    theMapOfFunctions.Bind(pid,f);
  }
  else
    f = theMapOfFunctions(pid);
  
  Handle(Standard_Transient) (*fp) (const Standard_GUID&) = NULL;
  fp = (Handle(Standard_Transient) (*)(const Standard_GUID&)) f;
  Handle(Standard_Transient) theServiceFactory = (*fp) (aGUID);
  return theServiceFactory;
  
}

