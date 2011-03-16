// File:	XmlMXCAFDoc_DocumentToolDriver.cxx
// Created:	Tue May 17 11:32:57 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <XmlMXCAFDoc_DocumentToolDriver.ixx>
#include <XCAFDoc_DocumentTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_DocumentToolDriver::XmlMXCAFDoc_DocumentToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "DocumentTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
 Handle(TDF_Attribute) XmlMXCAFDoc_DocumentToolDriver::NewEmpty() const {
   return new XCAFDoc_DocumentTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
 Standard_Boolean XmlMXCAFDoc_DocumentToolDriver::Paste(const XmlObjMgt_Persistent& /*theSource*/,
							const Handle(TDF_Attribute)& theTarget,
							XmlObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_DocumentTool) T = Handle(XCAFDoc_DocumentTool)::DownCast(theTarget);
  T->Init();
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_DocumentToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
					   XmlObjMgt_Persistent& /*theTarget*/,
					   XmlObjMgt_SRelocationTable& /*theRelocTable*/) const
{
}

