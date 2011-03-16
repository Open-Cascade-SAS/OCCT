// File:	BinMXCAFDoc_ColorToolDriver.cxx
// Created:	Tue May 17 12:29:31 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <BinMXCAFDoc_ColorToolDriver.ixx>
#include <XCAFDoc_ColorTool.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BinMXCAFDoc_ColorToolDriver::BinMXCAFDoc_ColorToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_ColorTool)->Name()) {
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_ColorToolDriver::NewEmpty() const {
  return new XCAFDoc_ColorTool();
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_ColorToolDriver::Paste(const BinObjMgt_Persistent& /*theSource*/,
						    const Handle(TDF_Attribute)& /*theTarget*/,
						    BinObjMgt_RRelocationTable& /*theRelocTable*/) const 
{
  return Standard_True;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void BinMXCAFDoc_ColorToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
					BinObjMgt_Persistent& /*theTarget*/,
					BinObjMgt_SRelocationTable& /*theRelocTable*/) const {
}

