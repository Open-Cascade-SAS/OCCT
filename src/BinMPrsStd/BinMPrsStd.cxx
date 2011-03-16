// File:	BinMPrsStd.cxx
// Created:	Mon May 17 11:52:51 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004


#include <BinMPrsStd.ixx>
#include <CDM_MessageDriver.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinMPrsStd_AISPresentationDriver.hxx>
#include <BinMPrsStd_PositionDriver.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================

void BinMPrsStd::AddDrivers (const Handle(BinMDF_ADriverTable)& aDriverTable,
				    const Handle(CDM_MessageDriver)&   aMessageDriver)
{
  aDriverTable->AddDriver (new BinMPrsStd_AISPresentationDriver(aMessageDriver));
  aDriverTable->AddDriver (new BinMPrsStd_PositionDriver(aMessageDriver));
}
