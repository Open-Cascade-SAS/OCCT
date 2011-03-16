// File:	XmlMXCAFDoc_ShapeToolDriver.cxx
// Created:	Tue May 17 11:42:20 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005

#include <XmlMXCAFDoc_ShapeToolDriver.ixx>
#include <XCAFDoc_ShapeTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_ShapeToolDriver::XmlMXCAFDoc_ShapeToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "ShapeTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_ShapeToolDriver::NewEmpty() const {
  return new XCAFDoc_ShapeTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_ShapeToolDriver::Paste(const XmlObjMgt_Persistent& /*theSource*/,
						    const Handle(TDF_Attribute)& /*theTarget*/,
						    XmlObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_ShapeToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
					XmlObjMgt_Persistent& /*theTarget*/,
					XmlObjMgt_SRelocationTable& /*theRelocTable*/) const
{
}

