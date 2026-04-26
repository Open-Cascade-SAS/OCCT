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

#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <gp_Pnt.hxx>

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetPoint(const BRepGraph_VertexId theVertex,
                                                const gp_Pnt&            thePoint)
{
  myGraph->myData->myIncStorage.ChangeVertex(theVertex).Point = thePoint;
  myGraph->markModified(theVertex);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetPoint(
  BRepGraph_MutGuard<BRepGraphInc::VertexDef>& theMut,
  const gp_Pnt&                                thePoint)
{
  theMut.Internal().Point = thePoint;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetTolerance(const BRepGraph_EdgeId theEdge,
                                                  double                 theTolerance)
{
  myGraph->myData->myIncStorage.ChangeEdge(theEdge).Tolerance = theTolerance;
  myGraph->markModified(theEdge);
}

//=================================================================================================

void BRepGraph::EditorView::EdgeOps::SetTolerance(
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
  double                                     theTolerance)
{
  theMut.Internal().Tolerance = theTolerance;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetParamRange(const BRepGraph_CoEdgeId theCoEdge,
                                                     double                   theFirst,
                                                     double                   theLast)
{
  BRepGraphInc::CoEdgeDef& aDef = myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge);
  aDef.ParamFirst               = theFirst;
  aDef.ParamLast                = theLast;
  myGraph->markModified(theCoEdge);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetParamRange(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
  double                                        theFirst,
  double                                        theLast)
{
  theMut.Internal().ParamFirst = theFirst;
  theMut.Internal().ParamLast  = theLast;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetIsClosed(const BRepGraph_WireId theWire,
                                                 bool                   theIsClosed)
{
  myGraph->myData->myIncStorage.ChangeWire(theWire).IsClosed = theIsClosed;
  myGraph->markModified(theWire);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetIsClosed(
  BRepGraph_MutGuard<BRepGraphInc::WireDef>& theMut,
  bool                                        theIsClosed)
{
  theMut.Internal().IsClosed = theIsClosed;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTolerance(const BRepGraph_FaceId theFace,
                                                  double                 theTolerance)
{
  myGraph->myData->myIncStorage.ChangeFace(theFace).Tolerance = theTolerance;
  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTolerance(
  BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
  double                                     theTolerance)
{
  theMut.Internal().Tolerance = theTolerance;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetNaturalRestriction(const BRepGraph_FaceId theFace,
                                                           bool theNaturalRestriction)
{
  myGraph->myData->myIncStorage.ChangeFace(theFace).NaturalRestriction = theNaturalRestriction;
  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetNaturalRestriction(
  BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
  bool                                        theNaturalRestriction)
{
  theMut.Internal().NaturalRestriction = theNaturalRestriction;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetTriangulationRep(
  const BRepGraph_FaceId             theFace,
  const BRepGraph_TriangulationRepId theRep)
{
  myGraph->myData->myIncStorage.ChangeFace(theFace).TriangulationRepId = theRep;
  myGraph->markModified(theFace);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefOrientation(const BRepGraph_FaceRefId theFaceRef,
                                                       TopAbs_Orientation        theOrientation)
{
  myGraph->myData->myIncStorage.ChangeFaceRef(theFaceRef).Orientation = theOrientation;
  myGraph->markRefModified(theFaceRef);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefOrientation(
  BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
  TopAbs_Orientation                         theOrientation)
{
  theMut.Internal().Orientation = theOrientation;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefLocalLocation(const BRepGraph_FaceRefId theFaceRef,
                                                         const TopLoc_Location&    theLoc)
{
  myGraph->myData->myIncStorage.ChangeFaceRef(theFaceRef).LocalLocation = theLoc;
  myGraph->markRefModified(theFaceRef);
}

//=================================================================================================

void BRepGraph::EditorView::FaceOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
  const TopLoc_Location&                     theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetRefLocalLocation(
  const BRepGraph_OccurrenceRefId theOccurrenceRef,
  const TopLoc_Location&          theLoc)
{
  myGraph->myData->myIncStorage.ChangeOccurrenceRef(theOccurrenceRef).LocalLocation = theLoc;
  myGraph->markRefModified(theOccurrenceRef);
}

//=================================================================================================

void BRepGraph::EditorView::OccurrenceOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>& theMut,
  const TopLoc_Location&                           theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetRefLocalLocation(
  const BRepGraph_VertexRefId theVertexRef,
  const TopLoc_Location&      theLoc)
{
  myGraph->myData->myIncStorage.ChangeVertexRef(theVertexRef).LocalLocation = theLoc;
  myGraph->markRefModified(theVertexRef);
}

//=================================================================================================

void BRepGraph::EditorView::VertexOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
  const TopLoc_Location&                       theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetRefLocalLocation(
  const BRepGraph_CoEdgeRefId theCoEdgeRef,
  const TopLoc_Location&      theLoc)
{
  myGraph->myData->myIncStorage.ChangeCoEdgeRef(theCoEdgeRef).LocalLocation = theLoc;
  myGraph->markRefModified(theCoEdgeRef);
}

//=================================================================================================

void BRepGraph::EditorView::CoEdgeOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef>& theMut,
  const TopLoc_Location&                       theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefLocalLocation(const BRepGraph_WireRefId theWireRef,
                                                         const TopLoc_Location&    theLoc)
{
  myGraph->myData->myIncStorage.ChangeWireRef(theWireRef).LocalLocation = theLoc;
  myGraph->markRefModified(theWireRef);
}

//=================================================================================================

void BRepGraph::EditorView::WireOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
  const TopLoc_Location&                     theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefLocalLocation(const BRepGraph_ShellRefId theShellRef,
                                                          const TopLoc_Location&     theLoc)
{
  myGraph->myData->myIncStorage.ChangeShellRef(theShellRef).LocalLocation = theLoc;
  myGraph->markRefModified(theShellRef);
}

//=================================================================================================

void BRepGraph::EditorView::ShellOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
  const TopLoc_Location&                      theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefLocalLocation(const BRepGraph_SolidRefId theSolidRef,
                                                          const TopLoc_Location&     theLoc)
{
  myGraph->myData->myIncStorage.ChangeSolidRef(theSolidRef).LocalLocation = theLoc;
  myGraph->markRefModified(theSolidRef);
}

//=================================================================================================

void BRepGraph::EditorView::SolidOps::SetRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
  const TopLoc_Location&                      theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
  theMut.MarkDirty();
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefLocalLocation(
  const BRepGraph_ChildRefId theChildRef,
  const TopLoc_Location&     theLoc)
{
  myGraph->myData->myIncStorage.ChangeChildRef(theChildRef).LocalLocation = theLoc;
  myGraph->markRefModified(theChildRef);
}

//=================================================================================================

void BRepGraph::EditorView::GenOps::SetChildRefLocalLocation(
  BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
  const TopLoc_Location&                      theLoc)
{
  theMut.Internal().LocalLocation = theLoc;
  theMut.MarkDirty();
}
