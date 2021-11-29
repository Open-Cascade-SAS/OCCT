// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlXCAFDrivers_HeaderFile
#define _XmlXCAFDrivers_HeaderFile

#include <Standard_Handle.hxx>

class Standard_Transient;
class Standard_GUID;
class TDocStd_Application;

class XmlXCAFDrivers 
{
public:
  
  //! Depending from the  ID, returns a list of  storage
  //! or retrieval attribute drivers. Used for plugin.
  //!
  //! Standard data model drivers
  //! ===========================
  //! 47b0b826-d931-11d1-b5da-00a0c9064368 Transient-Persistent
  //! 47b0b827-d931-11d1-b5da-00a0c9064368 Persistent-Transient
  //!
  //! XCAF data model drivers
  //! =================================
  //! ed8793f8-3142-11d4-b9b5-0060b0ee281b Transient-Persistent
  //! ed8793f9-3142-11d4-b9b5-0060b0ee281b Persistent-Transient
  //! ed8793fa-3142-11d4-b9b5-0060b0ee281b XCAFSchema
  Standard_EXPORT static const Handle(Standard_Transient)& Factory (const Standard_GUID& aGUID);

  //! Defines format "XmlXCAF" and registers its read and write drivers
  //! in the specified application
  Standard_EXPORT static void DefineFormat (const Handle(TDocStd_Application)& theApp);
};

#endif // _XmlXCAFDrivers_HeaderFile
