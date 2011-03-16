// File:        BinMDataXtd_GeometryDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:
// modified     13.04.2009 Sergey Zaritchny

#include <BinMDataXtd_GeometryDriver.ixx>
#include <TDataXtd_Geometry.hxx>

//=======================================================================
//function : BinMDataXtd_GeometryDriver
//purpose  : Constructor
//=======================================================================
BinMDataXtd_GeometryDriver::BinMDataXtd_GeometryDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataXtd_GeometryDriver::NewEmpty() const
{
  return (new TDataXtd_Geometry());
}

//=======================================================================
//function : Paste
//purpose  : P -> T
//=======================================================================
Standard_Boolean BinMDataXtd_GeometryDriver::Paste
                                        (const BinObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataXtd_Geometry) aT = 
    Handle(TDataXtd_Geometry)::DownCast (theTarget);

  Standard_Integer aType;
  Standard_Boolean ok = theSource >> aType;
  if (ok)
    aT->SetType ((TDataXtd_GeometryEnum) aType);

  return ok;
}

//=======================================================================
//function : Paste
//purpose  : T -> P
//=======================================================================
void BinMDataXtd_GeometryDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                        BinObjMgt_Persistent&        theTarget,
                                        BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataXtd_Geometry) aG = Handle(TDataXtd_Geometry)::DownCast(theSource);
  theTarget << (Standard_Integer) aG->GetType();
}
