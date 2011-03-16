// File:	BinMDataXtd_PlacementDriver.cxx
// Created:	Thu May 13 12:27:38 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004
// modified     13.04.2009 Sergey Zaritchny

#include <BinMDataXtd_PlacementDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TDataXtd_Placement.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
//=======================================================================
//function : BinMDataXtd_PlacementDriver
//purpose  : 
//=======================================================================

BinMDataXtd_PlacementDriver::BinMDataXtd_PlacementDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataXtd_Placement)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataXtd_PlacementDriver::NewEmpty() const
{
  return new TDataXtd_Placement();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataXtd_PlacementDriver::Paste
                                (const BinObjMgt_Persistent&,
                                 const Handle(TDF_Attribute)&,
                                 BinObjMgt_RRelocationTable& ) const
{return Standard_True;}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataXtd_PlacementDriver::Paste (const Handle(TDF_Attribute)&,
                                       BinObjMgt_Persistent&,
                                       BinObjMgt_SRelocationTable&  ) const
{}


