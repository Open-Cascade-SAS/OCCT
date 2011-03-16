// File:      XmlMXCAFDoc_DimTolToolDriver.cxx
// Created:   10.12.08 15:03:14
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <XmlMXCAFDoc_DimTolToolDriver.ixx>
#include <XCAFDoc_DimTolTool.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_DimTolToolDriver::XmlMXCAFDoc_DimTolToolDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, "xcaf", "DimTolTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_DimTolToolDriver::NewEmpty() const
{
  return new XCAFDoc_DimTolTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_DimTolToolDriver::Paste(const XmlObjMgt_Persistent& ,
                                                     const Handle(TDF_Attribute)& ,
                                                     XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_DimTolToolDriver::Paste(const Handle(TDF_Attribute)& ,
                                         XmlObjMgt_Persistent& ,
                                         XmlObjMgt_SRelocationTable& ) const
{
}
