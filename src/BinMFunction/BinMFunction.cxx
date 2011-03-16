// File:	BinMFunction.cxx
// Created:	Thu May 13 15:16:24 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004

#include <BinMFunction.ixx>
#include <BinMDF_ADriverTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <BinMFunction_FunctionDriver.hxx>
#include <BinMFunction_GraphNodeDriver.hxx>
#include <BinMFunction_ScopeDriver.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================

void BinMFunction::AddDrivers (const Handle(BinMDF_ADriverTable)& theDriverTable,
			       const Handle(CDM_MessageDriver)&   theMsgDriver)
{
  theDriverTable->AddDriver (new BinMFunction_FunctionDriver  (theMsgDriver) );
  theDriverTable->AddDriver (new BinMFunction_GraphNodeDriver (theMsgDriver) );
  theDriverTable->AddDriver (new BinMFunction_ScopeDriver     (theMsgDriver) );
}
