// File:	BinMDataXtd_PointDriver.cxx
// Created:	Thu May 13 11:51:10 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004
// modified     13.04.2009 Sergey Zaritchny

#include <BinMDataXtd_PointDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TDataXtd_Point.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
//=======================================================================
//function : BinMDataXtd_PointDriver
//purpose  : 
//=======================================================================

BinMDataXtd_PointDriver::BinMDataXtd_PointDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataXtd_Point)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataXtd_PointDriver::NewEmpty() const
{
  return new TDataXtd_Point();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataXtd_PointDriver::Paste
                                (const BinObjMgt_Persistent&,
                                 const Handle(TDF_Attribute)&,
                                 BinObjMgt_RRelocationTable& ) const
{return Standard_True;}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataXtd_PointDriver::Paste (const Handle(TDF_Attribute)&,
                                       BinObjMgt_Persistent&,
                                       BinObjMgt_SRelocationTable&  ) const
{}


