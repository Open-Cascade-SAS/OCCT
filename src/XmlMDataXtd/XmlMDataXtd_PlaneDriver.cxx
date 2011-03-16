// File:        XmlMDataXtd_PlaneDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataXtd_PlaneDriver.ixx>
#include <TDataXtd_Plane.hxx>

//=======================================================================
//function : XmlMDataXtd_PlaneDriver
//purpose  : Constructor
//=======================================================================

XmlMDataXtd_PlaneDriver::XmlMDataXtd_PlaneDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XmlMDataXtd_PlaneDriver::NewEmpty() const
{
  return (new TDataXtd_Plane());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 Standard_Boolean XmlMDataXtd_PlaneDriver::Paste
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

 void XmlMDataXtd_PlaneDriver::Paste (const Handle(TDF_Attribute)&,
                                      XmlObjMgt_Persistent&,
                                      XmlObjMgt_SRelocationTable&  ) const
{
}
