// File:        XmlMDataXtd_ShapeDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataXtd_ShapeDriver.ixx>
#include <TDataXtd_Shape.hxx>

//=======================================================================
//function : XmlMDataXtd_ShapeDriver
//purpose  : Constructor
//=======================================================================

XmlMDataXtd_ShapeDriver::XmlMDataXtd_ShapeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XmlMDataXtd_ShapeDriver::NewEmpty() const
{
  return (new TDataXtd_Shape());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 Standard_Boolean XmlMDataXtd_ShapeDriver::Paste
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

 void XmlMDataXtd_ShapeDriver::Paste (const Handle(TDF_Attribute)&,
                                      XmlObjMgt_Persistent&,
                                      XmlObjMgt_SRelocationTable&  ) const
{
}
