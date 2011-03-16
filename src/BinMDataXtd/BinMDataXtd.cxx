// File:      BinMDataXtd.cxx
// Created:   30.10.02 15:16:19
// Author:    Michael SAZONOV
// modified   13.04.2009 Sergey Zaritchny
// Copyright: Open CASCADE 2002

#include <BinMDataXtd.ixx>
#include <BinMDataXtd_ConstraintDriver.hxx>
#include <BinMDataXtd_GeometryDriver.hxx>
#include <BinMDataXtd_PatternStdDriver.hxx>
#include <BinMDataXtd_ShapeDriver.hxx>
#include <BinMDataXtd_PointDriver.hxx>
#include <BinMDataXtd_AxisDriver.hxx>
#include <BinMDataXtd_PlaneDriver.hxx>
#include <BinMDataXtd_PlacementDriver.hxx>

static Standard_Integer myDocumentVersion = -1;
//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================

void BinMDataXtd::AddDrivers (const Handle(BinMDF_ADriverTable)& theDriverTable,
                              const Handle(CDM_MessageDriver)&   theMsgDriver)
{
  theDriverTable->AddDriver (new BinMDataXtd_ConstraintDriver  (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataXtd_GeometryDriver    (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataXtd_PatternStdDriver  (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataXtd_ShapeDriver       (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataXtd_PointDriver       (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataXtd_AxisDriver        (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataXtd_PlaneDriver       (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataXtd_PlacementDriver   (theMsgDriver) );

}

//=======================================================================
//function : SetDocumentVersion
//purpose  : Sets current document version
//=======================================================================
void BinMDataXtd::SetDocumentVersion(const Standard_Integer theVersion)
{
  myDocumentVersion = theVersion;
}
//=======================================================================
//function : DocumentVersion
//purpose  : Retrieved document version
//=======================================================================
Standard_Integer BinMDataXtd::DocumentVersion()
{
  return myDocumentVersion;
}
