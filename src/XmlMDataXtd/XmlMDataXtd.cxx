// Created on: 2001-08-27
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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

// modified     20.04.2009 Sergey Zaritchny

#include <XmlMDataXtd.ixx>

#include <XmlMDataXtd_ShapeDriver.hxx>
#include <XmlMDataXtd_PointDriver.hxx>
#include <XmlMDataXtd_AxisDriver.hxx>
#include <XmlMDataXtd_PlaneDriver.hxx>
#include <XmlMDataXtd_GeometryDriver.hxx>
#include <XmlMDataXtd_ConstraintDriver.hxx>
#include <XmlMDataXtd_PlacementDriver.hxx>
#include <XmlMDataXtd_PatternStdDriver.hxx>

static Standard_Integer myDocumentVersion = -1;
//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlMDataXtd::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                              const Handle(CDM_MessageDriver)&   anMsgDrv)
{
  aDriverTable-> AddDriver (new XmlMDataXtd_ShapeDriver         (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataXtd_PointDriver         (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataXtd_AxisDriver          (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataXtd_PlaneDriver         (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataXtd_GeometryDriver      (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataXtd_ConstraintDriver    (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataXtd_PlacementDriver     (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataXtd_PatternStdDriver    (anMsgDrv));
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
