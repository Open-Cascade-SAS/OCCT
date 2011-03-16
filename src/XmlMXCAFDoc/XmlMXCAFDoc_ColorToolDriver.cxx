// File:	XmlMXCAFDoc_ColorToolDriver.cxx
// Created:	Tue May 17 11:10:36 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <XmlMXCAFDoc_ColorToolDriver.ixx>
#include <XCAFDoc_ColorTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_ColorToolDriver::XmlMXCAFDoc_ColorToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "ColorTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
 Handle(TDF_Attribute) XmlMXCAFDoc_ColorToolDriver::NewEmpty() const
{
  return new XCAFDoc_ColorTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_ColorToolDriver::Paste(const XmlObjMgt_Persistent& ,
						    const Handle(TDF_Attribute)& ,
						    XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_ColorToolDriver::Paste(const Handle(TDF_Attribute)& ,
					XmlObjMgt_Persistent& ,
					XmlObjMgt_SRelocationTable& ) const
{
}

