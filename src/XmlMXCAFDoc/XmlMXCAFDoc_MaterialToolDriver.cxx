// File:      XmlMXCAFDoc_MaterialToolDriver.cxx
// Created:   10.12.08 15:03:14
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <XmlMXCAFDoc_MaterialToolDriver.ixx>
#include <XCAFDoc_MaterialTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_MaterialToolDriver::XmlMXCAFDoc_MaterialToolDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, "xcaf", "MaterialTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_MaterialToolDriver::NewEmpty() const
{
  return new XCAFDoc_MaterialTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_MaterialToolDriver::Paste(const XmlObjMgt_Persistent& ,
                                                       const Handle(TDF_Attribute)& ,
                                                       XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_MaterialToolDriver::Paste(const Handle(TDF_Attribute)& ,
                                           XmlObjMgt_Persistent& ,
                                           XmlObjMgt_SRelocationTable& ) const
{
}
