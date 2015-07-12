// Created on: 1997-02-28
// Created by: Jean-Louis Frenkel
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

#ifndef _Plugin_HeaderFile
#define _Plugin_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class Standard_Transient;
class Standard_GUID;
class Resource_Manager;



class Plugin 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Handle(Standard_Transient) Load (const Standard_GUID& aGUID, const Standard_Boolean theVerbose = Standard_True);
  
  //! Returns a global map of {guid, plugin_library} pairs.
  //! The Load() method will use this map to search for plugins if and only if
  //! the GUID is not found in the Plugin file specified by the CSF_PluginDefaults
  //! (or CSF_PluginUserDefaults) environment variable, or if they are not defined.
  //!
  //! This allows to populate this additional resource manager
  //! in run-time and to avoid using the above environment variables.
  //! This map must be populated (using Resource_Manager::SetResource() method)
  //! following syntax conventions of the Plugin file, for instance:
  //! const Handle(Resource_Manager)& aPluginMap = Plugin::AdditionalPluginMap();
  //! aPluginMap->SetResource ("ad696000-5b34-11d1-b5ba-00a0c9064368.Location", "TKStdSchema");
  Standard_EXPORT static const Handle(Resource_Manager)& AdditionalPluginMap();




protected:





private:





};







#endif // _Plugin_HeaderFile
