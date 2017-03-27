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


#include <BinMXCAFDoc_ColorDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_Color.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_ColorDriver,BinMDF_ADriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_ColorDriver::BinMXCAFDoc_ColorDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Color)->Name()) {
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_ColorDriver::NewEmpty() const {
  return new XCAFDoc_Color();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_ColorDriver::Paste(const BinObjMgt_Persistent& theSource,
						const Handle(TDF_Attribute)& theTarget,
						BinObjMgt_RRelocationTable& /*theRelocTable*/) const 
{
  Handle(XCAFDoc_Color) anAtt = Handle(XCAFDoc_Color)::DownCast(theTarget);
  Standard_Real R, G, B;
  Standard_ShortReal alpha;
  Standard_Boolean isOk = theSource >> R >> G >> B;
  if(isOk) {
    Standard_Boolean isRGBA = theSource >> alpha;
    if (!isRGBA)
      alpha = 1.0;
    anAtt->Set(R, G, B, alpha);
  }
  return isOk;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_ColorDriver::Paste(const Handle(TDF_Attribute)& theSource,
				    BinObjMgt_Persistent& theTarget,
				    BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Color) anAtt = Handle(XCAFDoc_Color)::DownCast(theSource);
  Standard_Real R, G, B;
  Standard_ShortReal alpha;
  anAtt->GetRGB(R, G, B);
  alpha = anAtt->GetAlpha();
  theTarget << R << G << B << alpha;
}

