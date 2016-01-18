// Created on: 
// Created by: 
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


#include <BinMXCAFDoc_GeomToleranceDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_GeomTolerance.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_GeomToleranceDriver,BinMDF_ADriver)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BinMXCAFDoc_GeomToleranceDriver::BinMXCAFDoc_GeomToleranceDriver (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_GeomTolerance)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_GeomToleranceDriver::NewEmpty() const
{
  return new XCAFDoc_GeomTolerance();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_GeomToleranceDriver::Paste (const BinObjMgt_Persistent&  /*theSource*/,
                                                         const Handle(TDF_Attribute)& /*theTarget*/,
                                                         BinObjMgt_RRelocationTable&  /*theRelocTable*/) const
{
  //Handle(XCAFDoc_GeomTolerance) anAtt = Handle(XCAFDoc_GeomTolerance)::DownCast(theTarget);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMXCAFDoc_GeomToleranceDriver::Paste (const Handle(TDF_Attribute)&  /*theSource*/,
                                             BinObjMgt_Persistent&         /*theTarget*/,
                                             BinObjMgt_SRelocationTable&   /*theRelocTable*/) const
{
  //Handle(XCAFDoc_GeomTolerance) anAtt = Handle(XCAFDoc_GeomTolerance)::DownCast(theSource);
}
