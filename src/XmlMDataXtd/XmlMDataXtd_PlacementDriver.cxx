// File:        XmlMDataXtd_PlacementDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataXtd_PlacementDriver.ixx>
#include <TDataXtd_Placement.hxx>

//=======================================================================
//function : XmlMDataXtd_PlacementDriver
//purpose  : Constructor
//=======================================================================

XmlMDataXtd_PlacementDriver::XmlMDataXtd_PlacementDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XmlMDataXtd_PlacementDriver::NewEmpty() const
{
  return (new TDataXtd_Placement());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 Standard_Boolean XmlMDataXtd_PlacementDriver::Paste
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

 void XmlMDataXtd_PlacementDriver::Paste (const Handle(TDF_Attribute)&,
                                          XmlObjMgt_Persistent&,
                                          XmlObjMgt_SRelocationTable&  ) const
{
}
