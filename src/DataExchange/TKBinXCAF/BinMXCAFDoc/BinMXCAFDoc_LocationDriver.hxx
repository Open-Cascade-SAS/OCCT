// Created on: 2005-05-17
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

#ifndef _BinMXCAFDoc_LocationDriver_HeaderFile
#define _BinMXCAFDoc_LocationDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BinMNaming_NamedShapeDriver.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
class Message_Messenger;
class TDF_Attribute;
class BinObjMgt_Persistent;
class TopLoc_Location;

class BinMXCAFDoc_LocationDriver;
DEFINE_STANDARD_HANDLE(BinMXCAFDoc_LocationDriver, BinMDF_ADriver)

class BinMXCAFDoc_LocationDriver : public BinMDF_ADriver
{

public:
  Standard_EXPORT BinMXCAFDoc_LocationDriver(const Handle(Message_Messenger)& theMsgDriver);

  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean Paste(const BinObjMgt_Persistent&  theSource,
                                                 const Handle(TDF_Attribute)& theTarget,
                                                 BinObjMgt_RRelocationTable&  theRelocTable) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)& theSource,
                                     BinObjMgt_Persistent&        theTarget,
                                     BinObjMgt_SRelocationTable&  theRelocTable) const
    Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean Translate(const BinObjMgt_Persistent& theSource,
                                             TopLoc_Location&            theLoc,
                                             BinObjMgt_RRelocationTable& theMap) const;

  //! Translate transient location to storable
  Standard_EXPORT void Translate(const TopLoc_Location&      theLoc,
                                 BinObjMgt_Persistent&       theTarget,
                                 BinObjMgt_SRelocationTable& theMap) const;

  void SetNSDriver(const Handle(BinMNaming_NamedShapeDriver)& theNSDriver)
  {
    myNSDriver = theNSDriver;
  }

  DEFINE_STANDARD_RTTIEXT(BinMXCAFDoc_LocationDriver, BinMDF_ADriver)

protected:
private:
  Handle(BinMNaming_NamedShapeDriver) myNSDriver;
};

#endif // _BinMXCAFDoc_LocationDriver_HeaderFile
