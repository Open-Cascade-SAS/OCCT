// File:      BinMDF.cxx
// Created:   29.10.02 20:01:51
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#include <BinMDF.ixx>
#include <BinMDF_ReferenceDriver.hxx>
#include <BinMDF_TagSourceDriver.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================

void BinMDF::AddDrivers (const Handle(BinMDF_ADriverTable)& aDriverTable,
                         const Handle(CDM_MessageDriver)&   aMsgDrv)
{
  aDriverTable->AddDriver (new BinMDF_ReferenceDriver (aMsgDrv) );
  aDriverTable->AddDriver (new BinMDF_TagSourceDriver (aMsgDrv) );
}
