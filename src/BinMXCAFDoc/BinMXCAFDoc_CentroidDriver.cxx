// Created on: 2005-05-17
// Created by: Eugeny NAPALKOV
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


#include <BinMXCAFDoc_CentroidDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_Centroid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_CentroidDriver,BinMDF_ADriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_CentroidDriver::BinMXCAFDoc_CentroidDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Centroid)->Name()) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_CentroidDriver::NewEmpty() const {
  return new XCAFDoc_Centroid();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_CentroidDriver::Paste(const BinObjMgt_Persistent& theSource,
						   const Handle(TDF_Attribute)& theTarget,
						   BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Centroid) anAtt = Handle(XCAFDoc_Centroid)::DownCast(theTarget);
  Standard_Real x, y, z;
  Standard_Boolean isOk = theSource >> x >> y >> z;
  if(isOk) {
    gp_Pnt aPnt(x, y, z);
    anAtt->Set(aPnt);
  }
  return isOk;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_CentroidDriver::Paste(const Handle(TDF_Attribute)& theSource,
				       BinObjMgt_Persistent& theTarget,
				       BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Centroid) anAtt = Handle(XCAFDoc_Centroid)::DownCast(theSource);
  gp_Pnt aPnt = anAtt->Get();
  theTarget << aPnt.X() << aPnt.Y() << aPnt.Z();
}

