// File:        XmlMDataXtd.cxx
// Created:     Mon Aug 27 17:33:16 2001
// Author:      Alexander GRIGORIEV
// Copyright:   Open Cascade 2001
// History:
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
