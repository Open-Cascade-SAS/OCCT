// Created on: 2005-04-18
// Created by: Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _BinMXCAFDoc_HeaderFile
#define _BinMXCAFDoc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class BinMDF_ADriverTable;
class Message_Messenger;

class BinMXCAFDoc
{
public:
  DEFINE_STANDARD_ALLOC

  //! Adds the attribute drivers to <theDriverTable>.
  Standard_EXPORT static void AddDrivers(const Handle(BinMDF_ADriverTable)& theDriverTable,
                                         const Handle(Message_Messenger)&   theMsgDrv);
};

#endif // _BinMXCAFDoc_HeaderFile
