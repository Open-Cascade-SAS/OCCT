// Created on: 1997-03-06
// Created by: Mister rmi
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

