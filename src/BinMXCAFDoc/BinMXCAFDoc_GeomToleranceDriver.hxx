// Created on: 
// Created by: 
// Copyright (c) 20015 OPEN CASCADE SAS
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

#ifndef _BinMXCAFDoc_GeomToleranceDriver_HeaderFile
#define _BinMXCAFDoc_GeomToleranceDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BinMDF_ADriver.hxx>
#include <Standard_Boolean.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>

class CDM_MessageDriver;
class TDF_Attribute;
class BinObjMgt_Persistent;

class BinMXCAFDoc_GeomToleranceDriver;
DEFINE_STANDARD_HANDLE(BinMXCAFDoc_GeomToleranceDriver, BinMDF_ADriver)

class BinMXCAFDoc_GeomToleranceDriver : public BinMDF_ADriver
{

public:

  
  Standard_EXPORT BinMXCAFDoc_GeomToleranceDriver (const Handle(CDM_MessageDriver)& theMsgDriver);
  
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean Paste (const BinObjMgt_Persistent& theSource,
                                                  const Handle(TDF_Attribute)& theTarget,
                                                  BinObjMgt_RRelocationTable& theRelocTable) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& theSource,
                                      BinObjMgt_Persistent& theTarget,
                                      BinObjMgt_SRelocationTable& theRelocTable) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BinMXCAFDoc_GeomToleranceDriver,BinMDF_ADriver)

};

#endif // _BinMXCAFDoc_GeomToleranceDriver_HeaderFile
