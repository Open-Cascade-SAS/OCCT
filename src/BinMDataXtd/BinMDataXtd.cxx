// Created on: 2002-10-30
// Created by: Michael SAZONOV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// modified   13.04.2009 Sergey Zaritchny

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
