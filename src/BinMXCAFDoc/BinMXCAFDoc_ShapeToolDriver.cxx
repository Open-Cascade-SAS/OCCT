// File:	BinMXCAFDoc_ShapeToolDriver.cxx
// Created:	Tue May 17 13:20:19 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <BinMXCAFDoc_ShapeToolDriver.ixx>
#include <XCAFDoc_ShapeTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_ShapeToolDriver::BinMXCAFDoc_ShapeToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_ShapeTool)->Name())
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_ShapeToolDriver::NewEmpty() const {
  return new XCAFDoc_ShapeTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_ShapeToolDriver::Paste(const BinObjMgt_Persistent& /*theSource*/,
						    const Handle(TDF_Attribute)& /*theTarget*/,
						    BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_ShapeToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
					BinObjMgt_Persistent& /*theTarget*/,
					BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
}

