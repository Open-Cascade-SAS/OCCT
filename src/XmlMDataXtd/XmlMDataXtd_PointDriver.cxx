// File:        XmlMDataXtd_PointDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataXtd_PointDriver.ixx>
#include <TDataXtd_Point.hxx>

//=======================================================================
//function : XmlMDataXtd_PointDriver
//purpose  : Constructor
//=======================================================================

XmlMDataXtd_PointDriver::XmlMDataXtd_PointDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XmlMDataXtd_PointDriver::NewEmpty() const
{
  return (new TDataXtd_Point());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 Standard_Boolean XmlMDataXtd_PointDriver::Paste
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

 void XmlMDataXtd_PointDriver::Paste (const Handle(TDF_Attribute)&,
                                      XmlObjMgt_Persistent&,
                                      XmlObjMgt_SRelocationTable&  ) const
{
}
