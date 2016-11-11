// Created on: 2001-08-27
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// modified     20.04.2009 Sergey Zaritchny

#include <CDM_MessageDriver.hxx>
#include <XmlMDataXtd.hxx>
#include <XmlMDataXtd_AxisDriver.hxx>
#include <XmlMDataXtd_ConstraintDriver.hxx>
#include <XmlMDataXtd_GeometryDriver.hxx>
#include <XmlMDataXtd_PatternStdDriver.hxx>
#include <XmlMDataXtd_PlacementDriver.hxx>
#include <XmlMDataXtd_PlaneDriver.hxx>
#include <XmlMDataXtd_PointDriver.hxx>
#include <XmlMDataXtd_ShapeDriver.hxx>
#include <XmlMDataXtd_TriangulationDriver.hxx>
#include <XmlMDF_ADriverTable.hxx>

#include <XmlMDataXtd_PresentationDriver.hxx>
#include <XmlMDataXtd_PositionDriver.hxx>

static Standard_Integer myDocumentVersion = -1;
//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlMDataXtd::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                              const Handle(CDM_MessageDriver)&   anMsgDrv)
{
  aDriverTable->AddDriver(new XmlMDataXtd_ShapeDriver         (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_PointDriver         (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_AxisDriver          (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_PlaneDriver         (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_GeometryDriver      (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_ConstraintDriver    (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_PlacementDriver     (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_PatternStdDriver    (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_TriangulationDriver (anMsgDrv));

  aDriverTable->AddDriver(new XmlMDataXtd_PresentationDriver  (anMsgDrv));
  aDriverTable->AddDriver(new XmlMDataXtd_PositionDriver      (anMsgDrv));
}

//=======================================================================
//function : SetDocumentVersion
//purpose  : Sets current document version
//=======================================================================
void XmlMDataXtd::SetDocumentVersion(const Standard_Integer theVersion)
{
  myDocumentVersion = theVersion;
}
//=======================================================================
//function : DocumentVersion
//purpose  : Retrieved document version
//=======================================================================
Standard_Integer XmlMDataXtd::DocumentVersion()
{
  return myDocumentVersion;
}
