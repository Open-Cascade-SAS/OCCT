// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StdLDrivers_HeaderFile
#define _StdLDrivers_HeaderFile

#include <Standard_Handle.hxx>

class Standard_Transient;
class Standard_GUID;
class StdObjMgt_MapOfInstantiators;
class TDocStd_Application;

class StdLDrivers
{
public:
  //! Depending from the ID, returns a list of storage
  //! or retrieval attribute drivers. Used for plugin
  Standard_EXPORT static Handle(Standard_Transient) Factory (const Standard_GUID& aGUID);

  //! Defines format "OCC-StdLite" and registers its retrieval driver
  //! in the specified application
  Standard_EXPORT static void DefineFormat (const Handle(TDocStd_Application)& theApp);

  //! Register types.
  Standard_EXPORT static void BindTypes (StdObjMgt_MapOfInstantiators& theMap);
};

#endif // _StdLDrivers_HeaderFile
