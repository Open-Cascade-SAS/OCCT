// File:	BinMNaming.cxx
// Created:	Tue Apr 20 11:18:57 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004

#include <BinMNaming.ixx>

#include <BinMNaming_NamedShapeDriver.hxx>
#include <BinMNaming_NamingDriver.hxx>

static Standard_Integer myDocumentVersion = -1;
//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void BinMNaming::AddDrivers (const Handle(BinMDF_ADriverTable)& aDriverTable,
                             const Handle(CDM_MessageDriver)&   aMessageDriver)
{
  aDriverTable->AddDriver (new BinMNaming_NamedShapeDriver(aMessageDriver));
  aDriverTable->AddDriver (new BinMNaming_NamingDriver(aMessageDriver));
}

//=======================================================================
//function : SetDocumentVersion
//purpose  : Sets current document version
//=======================================================================
void BinMNaming::SetDocumentVersion(const Standard_Integer theVersion)
{
  myDocumentVersion = theVersion;
}
//=======================================================================
//function : DocumentVersion
//purpose  : Retrieved document version
//=======================================================================
Standard_Integer BinMNaming::DocumentVersion()
{
  return myDocumentVersion;
}
