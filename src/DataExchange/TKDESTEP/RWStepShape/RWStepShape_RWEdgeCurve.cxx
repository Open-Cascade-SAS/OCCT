// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include "RWStepShape_RWEdgeCurve.pxx"

#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Precision.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Curve.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexPoint.hxx>

namespace
{
// ================================================================
// Function : GetSharing
// Purpose  : Returns a step entity of a type StepShapeType that is
//            sharing theStepEntity. If no sharing entity of
//            required type is found, returns nullptr.
// ================================================================
template <typename StepShapeType>
occ::handle<StepShapeType> GetSharing(const occ::handle<Standard_Transient>& theStepEntity,
                                 const Interface_ShareTool&        theShareTool)
{
  Interface_EntityIterator aSharedEntitiesIt = theShareTool.Sharings(theStepEntity);
  aSharedEntitiesIt.SelectType(STANDARD_TYPE(StepShapeType), true);
  return aSharedEntitiesIt.NbEntities() == 0
           ? occ::handle<StepShapeType>{}
           : occ::down_cast<StepShapeType>(aSharedEntitiesIt.Value());
}

// ================================================================
// Function : GetFaceBoundOrientation
// Purpose  : Returns an orientation of face bound sharing
//            theOrientedEdge. If face bound cannot be found,
//            returns true (to preserve pre-refactoring behavior).
// ================================================================
bool GetFaceBoundOrientation(const occ::handle<StepShape_OrientedEdge>& theOrientedEdge,
                                         const Interface_ShareTool&            theShareTool)
{
  if (!theShareTool.IsShared(theOrientedEdge))
  {
    return true;
  }

  const occ::handle<StepShape_EdgeLoop> anEdgeLoop =
    GetSharing<StepShape_EdgeLoop>(theOrientedEdge, theShareTool);
  if (!theShareTool.IsShared(anEdgeLoop))
  {
    return true;
  }

  const occ::handle<StepShape_FaceBound> aFaceBound =
    GetSharing<StepShape_FaceBound>(anEdgeLoop, theShareTool);
  return aFaceBound.IsNull() ? true : aFaceBound->Orientation();
}

// ================================================================
// Function : GetFaceBoundOrientation
// Purpose  : Returns true if start and end points of theEdgeCurve
//            are valid points of the different vertices and are
//            equal to each other within Precision::Confusion().
// ================================================================
bool AreEndsMatch(const occ::handle<StepShape_EdgeCurve>& theEdgeCurve)
{
  occ::handle<StepShape_VertexPoint> aStartVertex =
    occ::down_cast<StepShape_VertexPoint>(theEdgeCurve->EdgeStart());
  occ::handle<StepShape_VertexPoint> anEndVertex =
    occ::down_cast<StepShape_VertexPoint>(theEdgeCurve->EdgeEnd());
  if (aStartVertex == anEndVertex)
  {
    return false;
  }

  occ::handle<StepGeom_CartesianPoint> aStartPoint =
    occ::down_cast<StepGeom_CartesianPoint>(aStartVertex->VertexGeometry());
  occ::handle<StepGeom_CartesianPoint> anEndPoint =
    occ::down_cast<StepGeom_CartesianPoint>(anEndVertex->VertexGeometry());
  if (aStartPoint.IsNull() || anEndPoint.IsNull())
  {
    return false;
  }

  const double aDistance =
    std::sqrt((aStartPoint->CoordinatesValue(1) - anEndPoint->CoordinatesValue(1))
                * (aStartPoint->CoordinatesValue(1) - anEndPoint->CoordinatesValue(1))
              + (aStartPoint->CoordinatesValue(2) - anEndPoint->CoordinatesValue(2))
                  * (aStartPoint->CoordinatesValue(2) - anEndPoint->CoordinatesValue(2))
              + (aStartPoint->CoordinatesValue(3) - anEndPoint->CoordinatesValue(3))
                  * (aStartPoint->CoordinatesValue(3) - anEndPoint->CoordinatesValue(3)));
  return aDistance < Precision::Confusion();
}
} // namespace

//=================================================================================================

void RWStepShape_RWEdgeCurve::ReadStep(const occ::handle<StepData_StepReaderData>& theStepData,
                                       const int                 theRecordID,
                                       occ::handle<Interface_Check>&               theMessageTool,
                                       const occ::handle<StepShape_EdgeCurve>&     theEdgeCurve) const
{
  // --- Number of Parameter Control ---
  if (!theStepData->CheckNbParams(theRecordID, 5, theMessageTool, "edge_curve"))
  {
    return;
  }

  // --- inherited field : name ---
  occ::handle<TCollection_HAsciiString> aName;
  theStepData->ReadString(theRecordID, 1, "name", theMessageTool, aName);

  // --- inherited field : edgeStart ---
  occ::handle<StepShape_Vertex> anEdgeStart;
  theStepData->ReadEntity(theRecordID,
                          2,
                          "edge_start",
                          theMessageTool,
                          STANDARD_TYPE(StepShape_Vertex),
                          anEdgeStart);

  // --- inherited field : edgeEnd ---
  occ::handle<StepShape_Vertex> anEdgeEnd;
  theStepData->ReadEntity(theRecordID,
                          3,
                          "edge_end",
                          theMessageTool,
                          STANDARD_TYPE(StepShape_Vertex),
                          anEdgeEnd);

  // --- own field : edgeGeometry ---
  occ::handle<StepGeom_Curve> anEdgeGeometry;
  theStepData->ReadEntity(theRecordID,
                          4,
                          "edge_geometry",
                          theMessageTool,
                          STANDARD_TYPE(StepGeom_Curve),
                          anEdgeGeometry);

  // --- own field : sameSense ---
  bool aSameSense;
  theStepData->ReadBoolean(theRecordID, 5, "same_sense", theMessageTool, aSameSense);

  //--- Initialisation of the read entity ---
  theEdgeCurve->Init(aName, anEdgeStart, anEdgeEnd, anEdgeGeometry, aSameSense);
}

//=================================================================================================

void RWStepShape_RWEdgeCurve::WriteStep(StepData_StepWriter&               theStepWriter,
                                        const occ::handle<StepShape_EdgeCurve>& theEdgeCurve) const
{
  // --- inherited field name ---
  theStepWriter.Send(theEdgeCurve->Name());

  // --- inherited field edgeStart ---
  theStepWriter.Send(theEdgeCurve->EdgeStart());

  // --- inherited field edgeEnd ---
  theStepWriter.Send(theEdgeCurve->EdgeEnd());

  // --- own field : edgeGeometry ---
  theStepWriter.Send(theEdgeCurve->EdgeGeometry());

  // --- own field : sameSense ---
  theStepWriter.SendBoolean(theEdgeCurve->SameSense());
}

//=================================================================================================

void RWStepShape_RWEdgeCurve::Share(const occ::handle<StepShape_EdgeCurve>& theEdgeCurve,
                                    Interface_EntityIterator&          theSharedEntitiesIt) const
{
  theSharedEntitiesIt.GetOneItem(theEdgeCurve->EdgeStart());

  theSharedEntitiesIt.GetOneItem(theEdgeCurve->EdgeEnd());

  theSharedEntitiesIt.GetOneItem(theEdgeCurve->EdgeGeometry());
}

//=================================================================================================

void RWStepShape_RWEdgeCurve::Check(const occ::handle<StepShape_EdgeCurve>& theEdgeCurve,
                                    const Interface_ShareTool&         theShareTool,
                                    occ::handle<Interface_Check>&           theMessageTool) const
{
  // 1- First Vertex != LastVertex but First VertexPoint == Last VertexPoint
  // Remark: time consuming process but useful.
  // If this append, we can drop one of the two vertices and replace it
  // everywhere it is referenced. Side effect: tolerance problem.
  if (AreEndsMatch(theEdgeCurve))
  {
    theMessageTool->AddWarning(
      "Two instances of Vertex have equal (within uncertainty) coordinates");
  }

  if (!theShareTool.IsShared(theEdgeCurve))
  {
    theMessageTool->AddFail("ERROR: EdgeCurve not referenced");
    return;
  }

  // 2- Two-Manifold Topology
  Interface_EntityIterator aSharedEntitiesIt = theShareTool.Sharings(theEdgeCurve);
  aSharedEntitiesIt.SelectType(STANDARD_TYPE(StepShape_OrientedEdge), true);
  if (aSharedEntitiesIt.NbEntities() != 2)
  {
    return;
  }

  const occ::handle<StepShape_OrientedEdge> anOrientedEdge1 =
    occ::down_cast<StepShape_OrientedEdge>(aSharedEntitiesIt.Value());
  const bool aFaceBoundOrientation1 =
    GetFaceBoundOrientation(anOrientedEdge1, theShareTool);
  const bool anIsCumulated1 = aFaceBoundOrientation1 != anOrientedEdge1->Orientation();

  aSharedEntitiesIt.Next();

  const occ::handle<StepShape_OrientedEdge> anOrientedEdge2 =
    occ::down_cast<StepShape_OrientedEdge>(aSharedEntitiesIt.Value());
  const bool aFaceBoundOrientation2 =
    GetFaceBoundOrientation(anOrientedEdge2, theShareTool);
  const bool anIsCumulated2 = aFaceBoundOrientation2 != anOrientedEdge2->Orientation();

  // the orientation of the OrientedEdges must be opposite
  if (anIsCumulated1 == anIsCumulated2)
  {
    theMessageTool->AddFail("ERROR: non 2-manifold topology");
  }
}
