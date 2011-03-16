// File:	BinMDataXtd_ShapeDriver.cxx
// Created:	Thu May 13 11:31:23 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004
// modified     13.04.2009 Sergey Zaritchny

#include <BinMDataXtd_ShapeDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TDataXtd_Shape.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
//=======================================================================
//function : BinMDataXtd_ShapeDriver
//purpose  : 
//=======================================================================

BinMDataXtd_ShapeDriver::BinMDataXtd_ShapeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataXtd_Shape)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataXtd_ShapeDriver::NewEmpty() const
{
  return new TDataXtd_Shape();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataXtd_ShapeDriver::Paste
                                (const BinObjMgt_Persistent&,
                                 const Handle(TDF_Attribute)&,
                                 BinObjMgt_RRelocationTable& ) const
{return Standard_True;}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataXtd_ShapeDriver::Paste (const Handle(TDF_Attribute)&,
                                       BinObjMgt_Persistent&,
                                       BinObjMgt_SRelocationTable&  ) const
{}
