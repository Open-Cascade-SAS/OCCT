// File:	XmlMXCAFDoc_LayerToolDriver.cxx
// Created:	Tue May 17 11:39:40 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <XmlMXCAFDoc_LayerToolDriver.ixx>
#include <XCAFDoc_LayerTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_LayerToolDriver::XmlMXCAFDoc_LayerToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "LayerTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_LayerToolDriver::NewEmpty() const {
  return new XCAFDoc_LayerTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_LayerToolDriver::Paste(const XmlObjMgt_Persistent& /*theSource*/,
						    const Handle(TDF_Attribute)& /*theTarget*/,
						    XmlObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_LayerToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
					XmlObjMgt_Persistent& /*theTarget*/,
					XmlObjMgt_SRelocationTable& /*theRelocTable*/) const
{
}


