// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinDrivers_HeaderFile
#define _BinDrivers_HeaderFile

#include <Standard_Handle.hxx>

class Standard_Transient;
class Standard_GUID;
class BinMDF_ADriverTable;
class Message_Messenger;
class TDocStd_Application;

class BinDrivers
{
public:
  Standard_EXPORT static const Handle(Standard_Transient)& Factory(const Standard_GUID& theGUID);

  //! Defines format "BinOcaf" and registers its read and write drivers
  //! in the specified application
  Standard_EXPORT static void DefineFormat(const Handle(TDocStd_Application)& theApp);

  //! Creates the table of drivers of types supported
  Standard_EXPORT static Handle(BinMDF_ADriverTable) AttributeDrivers(
    const Handle(Message_Messenger)& MsgDrv);
};

#endif // _BinDrivers_HeaderFile
