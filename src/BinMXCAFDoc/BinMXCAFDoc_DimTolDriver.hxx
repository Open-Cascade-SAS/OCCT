// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _BinMXCAFDoc_DimTolDriver_HeaderFile
#define _BinMXCAFDoc_DimTolDriver_HeaderFile

#include <Standard.hxx>

#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
class Message_Messenger;
class TDF_Attribute;
class BinObjMgt_Persistent;

class BinMXCAFDoc_DimTolDriver;
DEFINE_STANDARD_HANDLE(BinMXCAFDoc_DimTolDriver, BinMDF_ADriver)

class BinMXCAFDoc_DimTolDriver : public BinMDF_ADriver
{

public:
  Standard_EXPORT BinMXCAFDoc_DimTolDriver(const Handle(Message_Messenger)& theMsgDriver);

  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean Paste(const BinObjMgt_Persistent&  theSource,
                                                 const Handle(TDF_Attribute)& theTarget,
                                                 BinObjMgt_RRelocationTable&  theRelocTable) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)& theSource,
                                     BinObjMgt_Persistent&        theTarget,
                                     BinObjMgt_SRelocationTable&  theRelocTable) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BinMXCAFDoc_DimTolDriver, BinMDF_ADriver)

protected:
private:
};

#endif // _BinMXCAFDoc_DimTolDriver_HeaderFile
