// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _BinMDataStd_BooleanArrayDriver_HeaderFile
#define _BinMDataStd_BooleanArrayDriver_HeaderFile

#include <Standard.hxx>

#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
class Message_Messenger;
class TDF_Attribute;
class BinObjMgt_Persistent;

class BinMDataStd_BooleanArrayDriver;
DEFINE_STANDARD_HANDLE(BinMDataStd_BooleanArrayDriver, BinMDF_ADriver)

class BinMDataStd_BooleanArrayDriver : public BinMDF_ADriver
{

public:
  Standard_EXPORT BinMDataStd_BooleanArrayDriver(const Handle(Message_Messenger)& theMessageDriver);

  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean Paste(const BinObjMgt_Persistent&  Source,
                                                 const Handle(TDF_Attribute)& Target,
                                                 BinObjMgt_RRelocationTable&  RelocTable) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)& Source,
                                     BinObjMgt_Persistent&        Target,
                                     BinObjMgt_SRelocationTable&  RelocTable) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BinMDataStd_BooleanArrayDriver, BinMDF_ADriver)

protected:
private:
};

#endif // _BinMDataStd_BooleanArrayDriver_HeaderFile
