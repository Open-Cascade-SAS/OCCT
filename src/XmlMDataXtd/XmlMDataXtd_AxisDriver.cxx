// File:        XmlMDataXtd_AxisDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataXtd_AxisDriver.ixx>
#include <TDataXtd_Axis.hxx>

//=======================================================================
//function : XmlMDataXtd_AxisDriver
//purpose  : Constructor
//=======================================================================

XmlMDataXtd_AxisDriver::XmlMDataXtd_AxisDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XmlMDataXtd_AxisDriver::NewEmpty() const
{
  return (new TDataXtd_Axis());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 Standard_Boolean XmlMDataXtd_AxisDriver::Paste
                                        (const XmlObjMgt_Persistent&,
                                         const Handle(TDF_Attribute)&,
                                         XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void XmlMDataXtd_AxisDriver::Paste (const Handle(TDF_Attribute)&,
                                     XmlObjMgt_Persistent&,
                                     XmlObjMgt_SRelocationTable&  ) const
{
}
