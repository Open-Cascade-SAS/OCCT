// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <STEPControl_ActorRead.hxx>
#include <STEPControl_Controller.hxx>
#include <BRep_Tool.hxx>
#include <Interface_HGraph.hxx>
#include <NCollection_HArray1.hxx>
#include <StepAP214_Protocol.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepRepr_ConstructiveGeometryRepresentation.hxx>
#include <StepRepr_ConstructiveGeometryRepresentationRelationship.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <TransferBRep.hxx>
#include <Transfer_TransientProcess.hxx>

#include <gtest/gtest.h>

namespace
{
//==================================================================================================

occ::handle<StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext> makeContext(
  const bool theMillimeters)
{
  const occ::handle<StepBasic_SiUnitAndLengthUnit> aUnit = new StepBasic_SiUnitAndLengthUnit;
  aUnit->Init(theMillimeters, StepBasic_spMilli, StepBasic_sunMetre);
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>> aUnits =
    new NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>(1, 1);
  aUnits->SetValue(1, aUnit);
  const occ::handle<StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext> aContext =
    new StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext;
  aContext->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), 3, aUnits);
  return aContext;
}
} // namespace

TEST(STEPControl_ActorReadTest, ConstructivePoints_UseTheirRepresentationUnitsAndReadSetting)
{
  STEPControl_Controller::Init();
  for (const bool toReadConstructive : {false, true})
  {
    const occ::handle<StepData_StepModel> aModel = new StepData_StepModel;
    aModel->SetProtocol(new StepAP214_Protocol);
    aModel->SetLocalLengthUnit(1.0);
    aModel->InternalParameters.ReadRelationship          = true;
    aModel->InternalParameters.ReadConstrRelation        = toReadConstructive;
    const occ::handle<TCollection_HAsciiString> aName    = new TCollection_HAsciiString();
    const occ::handle<StepGeom_CartesianPoint>  anOrigin = new StepGeom_CartesianPoint;
    anOrigin->Init3D(aName, 0, 0, 0);
    const occ::handle<StepGeom_Axis2Placement3d> anAxis = new StepGeom_Axis2Placement3d;
    anAxis->Init(aName, anOrigin, false, nullptr, false, nullptr);
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aRootItems =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
    aRootItems->SetValue(1, anAxis);
    const occ::handle<StepShape_ShapeRepresentation> aRoot = new StepShape_ShapeRepresentation;
    aRoot->Init(aName, aRootItems, makeContext(true));
    const occ::handle<StepRepr_RepresentationMap> aMap = new StepRepr_RepresentationMap;
    aMap->Init(anAxis, aRoot);
    const occ::handle<StepRepr_MappedItem> aMapped = new StepRepr_MappedItem;
    aMapped->Init(aName, aMap, anAxis);
    aModel->AddWithRefs(aMapped);
    occ::handle<StepGeom_CartesianPoint> aPoints[2];
    for (int anIndex = 0; anIndex < 2; ++anIndex)
    {
      aPoints[anIndex] = new StepGeom_CartesianPoint;
      aPoints[anIndex]->Init3D(aName, anIndex + 1, 0, 0);
      const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItems =
        new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
      anItems->SetValue(1, aPoints[anIndex]);
      const occ::handle<StepRepr_ConstructiveGeometryRepresentation> aRepresentation =
        new StepRepr_ConstructiveGeometryRepresentation;
      aRepresentation->Init(aName, anItems, makeContext(anIndex == 1));
      const occ::handle<StepRepr_ConstructiveGeometryRepresentationRelationship> aRelation =
        new StepRepr_ConstructiveGeometryRepresentationRelationship;
      aRelation->Init(aName, aName, aRoot, aRepresentation);
      aModel->AddWithRefs(aRelation);
    }
    const occ::handle<Transfer_TransientProcess> aProcess = new Transfer_TransientProcess;
    aProcess->SetGraph(new Interface_HGraph(aModel));
    const occ::handle<STEPControl_ActorRead> anActor = new STEPControl_ActorRead(aModel);
    anActor->TransferShape(aMapped, aProcess, StepData_Factors());
    for (int anIndex = 0; anIndex < 2; ++anIndex)
    {
      const TopoDS_Shape aShape = TransferBRep::ShapeResult(aProcess, aPoints[anIndex]);
      if (!toReadConstructive)
      {
        EXPECT_TRUE(aShape.IsNull());
        continue;
      }
      ASSERT_FALSE(aShape.IsNull());
      ASSERT_EQ(aShape.ShapeType(), TopAbs_VERTEX);
      EXPECT_NEAR(BRep_Tool::Pnt(TopoDS::Vertex(aShape)).X(), anIndex == 0 ? 1000.0 : 2.0, 1.e-10);
    }
  }
}

//==================================================================================================

TEST(STEPControl_ActorReadTest, StandalonePoint_UsesExplicitFactors)
{
  STEPControl_Controller::Init();
  const occ::handle<StepData_StepModel> aModel = new StepData_StepModel;
  aModel->SetProtocol(new StepAP214_Protocol);
  const occ::handle<StepGeom_CartesianPoint> aPoint = new StepGeom_CartesianPoint;
  aPoint->Init3D(new TCollection_HAsciiString(), 2, 3, 4);
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItems =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
  anItems->SetValue(1, aPoint);
  const occ::handle<StepRepr_ConstructiveGeometryRepresentation> aRepresentation =
    new StepRepr_ConstructiveGeometryRepresentation;
  aRepresentation->Init(new TCollection_HAsciiString(), anItems, makeContext(false));
  aModel->AddWithRefs(aRepresentation);
  const occ::handle<Transfer_TransientProcess> aProcess = new Transfer_TransientProcess;
  aProcess->SetGraph(new Interface_HGraph(aModel));
  const occ::handle<STEPControl_ActorRead> anActor = new STEPControl_ActorRead(aModel);
  StepData_Factors                         aFactors;
  anActor->PrepareUnits(aRepresentation, aProcess, aFactors);
  const TopoDS_Shape aShape =
    TransferBRep::ShapeResult(anActor->TransferShape(aPoint, aProcess, aFactors));
  ASSERT_FALSE(aShape.IsNull());
  ASSERT_EQ(aShape.ShapeType(), TopAbs_VERTEX);
  EXPECT_LT(BRep_Tool::Pnt(TopoDS::Vertex(aShape)).Distance(gp_Pnt(2000, 3000, 4000)), 1.e-10);
}
