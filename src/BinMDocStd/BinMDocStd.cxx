// File:	BinMDocStd.cxx
// Created:	Thu May 13 16:37:25 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004

#include <BinMDocStd.ixx>
#include <BinMDF_ADriverTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <BinMDocStd_XLinkDriver.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================

void BinMDocStd::AddDrivers (const Handle(BinMDF_ADriverTable)& theDriverTable,
                              const Handle(CDM_MessageDriver)&   theMsgDriver)
{
  theDriverTable->AddDriver (new BinMDocStd_XLinkDriver  (theMsgDriver) );
  
}
