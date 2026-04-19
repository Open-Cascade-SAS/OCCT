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

#include <BRepGraphInc_Storage.hxx>

#include <type_traits>

namespace
{

template <typename T>
bool containsNodeIndex(const NCollection_Vector<T>* theVec, const int theIndex)
{
  if (theVec == nullptr)
  {
    return false;
  }
  for (const T& anElem : *theVec)
  {
    if (anElem.Index == theIndex)
    {
      return true;
    }
  }
  return false;
}

template <typename StoreT, typename TypeIdT>
const typename StoreT::ValueType* findInStore(const StoreT& theStore, const TypeIdT theId)
{
  return theId.IsValid(theStore.Nb()) ? &theStore.Get(theId) : nullptr;
}

template <typename StoreT, typename TypeIdT>
typename StoreT::ValueType* changeFindInStore(StoreT& theStore, const TypeIdT theId)
{
  return theId.IsValid(theStore.Nb()) ? &theStore.Change(theId) : nullptr;
}

} // namespace

//=================================================================================================

BRepGraphInc_Storage::BRepGraphInc_Storage(const occ::handle<NCollection_BaseAllocator>& theAlloc)
    : myVertices(256, theAlloc),
      myEdges(256, theAlloc),
      myCoEdges(256, theAlloc),
      myWires(256, theAlloc),
      myFaces(256, theAlloc),
      myShells(256, theAlloc),
      mySolids(256, theAlloc),
      myCompounds(256, theAlloc),
      myCompSolids(256, theAlloc),
      myProducts(256, theAlloc),
      myOccurrences(256, theAlloc),
      myShellRefs(256, theAlloc),
      myFaceRefs(256, theAlloc),
      myWireRefs(256, theAlloc),
      myCoEdgeRefs(256, theAlloc),
      myVertexRefs(256, theAlloc),
      mySolidRefs(256, theAlloc),
      myChildRefs(256, theAlloc),
      myOccurrenceRefs(256, theAlloc),
      mySurfaces(256, theAlloc),
      myCurves3D(256, theAlloc),
      myCurves2D(256, theAlloc),
      myTriangulationsRep(256, theAlloc),
      myPolygons3D(256, theAlloc),
      myPolygons2D(256, theAlloc),
      myPolygonsOnTri(256, theAlloc),
      myTShapeToNodeId(1, theAlloc),
      myOriginalShapes(1, theAlloc),
      myAllocator(theAlloc.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator() : theAlloc)
{
}

//=================================================================================================

const NCollection_Vector<BRepGraph_UID>& BRepGraphInc_Storage::UIDs(
  const BRepGraph_NodeId::Kind theKind) const
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return myVertices.UIDs;
    case BRepGraph_NodeId::Kind::Edge:
      return myEdges.UIDs;
    case BRepGraph_NodeId::Kind::CoEdge:
      return myCoEdges.UIDs;
    case BRepGraph_NodeId::Kind::Wire:
      return myWires.UIDs;
    case BRepGraph_NodeId::Kind::Face:
      return myFaces.UIDs;
    case BRepGraph_NodeId::Kind::Shell:
      return myShells.UIDs;
    case BRepGraph_NodeId::Kind::Solid:
      return mySolids.UIDs;
    case BRepGraph_NodeId::Kind::Compound:
      return myCompounds.UIDs;
    case BRepGraph_NodeId::Kind::CompSolid:
      return myCompSolids.UIDs;
    case BRepGraph_NodeId::Kind::Product:
      return myProducts.UIDs;
    case BRepGraph_NodeId::Kind::Occurrence:
      return myOccurrences.UIDs;
    default:
      break;
  }
  Standard_ASSERT_VOID(false, "UIDs: unhandled Kind");
  static const NCollection_Vector<BRepGraph_UID> THE_EMPTY;
  return THE_EMPTY;
}

//=================================================================================================

NCollection_Vector<BRepGraph_UID>& BRepGraphInc_Storage::ChangeUIDs(
  const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return myVertices.UIDs;
    case BRepGraph_NodeId::Kind::Edge:
      return myEdges.UIDs;
    case BRepGraph_NodeId::Kind::CoEdge:
      return myCoEdges.UIDs;
    case BRepGraph_NodeId::Kind::Wire:
      return myWires.UIDs;
    case BRepGraph_NodeId::Kind::Face:
      return myFaces.UIDs;
    case BRepGraph_NodeId::Kind::Shell:
      return myShells.UIDs;
    case BRepGraph_NodeId::Kind::Solid:
      return mySolids.UIDs;
    case BRepGraph_NodeId::Kind::Compound:
      return myCompounds.UIDs;
    case BRepGraph_NodeId::Kind::CompSolid:
      return myCompSolids.UIDs;
    case BRepGraph_NodeId::Kind::Product:
      return myProducts.UIDs;
    case BRepGraph_NodeId::Kind::Occurrence:
      return myOccurrences.UIDs;
  }
  Standard_ASSERT_RETURN(false, "ChangeUIDs: invalid Kind value", myVertices.UIDs);
  return myVertices.UIDs;
}

//=================================================================================================

void BRepGraphInc_Storage::ResetAllUIDs()
{
  myVertices.UIDs.Clear();
  myEdges.UIDs.Clear();
  myCoEdges.UIDs.Clear();
  myWires.UIDs.Clear();
  myFaces.UIDs.Clear();
  myShells.UIDs.Clear();
  mySolids.UIDs.Clear();
  myCompounds.UIDs.Clear();
  myCompSolids.UIDs.Clear();
  myProducts.UIDs.Clear();
  myOccurrences.UIDs.Clear();
}

//=================================================================================================

const NCollection_Vector<BRepGraph_RefUID>& BRepGraphInc_Storage::RefUIDs(
  const BRepGraph_RefId::Kind theKind) const
{
  switch (theKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myShellRefs.UIDs;
    case BRepGraph_RefId::Kind::Face:
      return myFaceRefs.UIDs;
    case BRepGraph_RefId::Kind::Wire:
      return myWireRefs.UIDs;
    case BRepGraph_RefId::Kind::CoEdge:
      return myCoEdgeRefs.UIDs;
    case BRepGraph_RefId::Kind::Vertex:
      return myVertexRefs.UIDs;
    case BRepGraph_RefId::Kind::Solid:
      return mySolidRefs.UIDs;
    case BRepGraph_RefId::Kind::Child:
      return myChildRefs.UIDs;
    case BRepGraph_RefId::Kind::Occurrence:
      return myOccurrenceRefs.UIDs;
    default:
      break;
  }
  Standard_ASSERT_VOID(false, "RefUIDs: unhandled Kind");
  static const NCollection_Vector<BRepGraph_RefUID> THE_EMPTY;
  return THE_EMPTY;
}

//=================================================================================================

NCollection_Vector<BRepGraph_RefUID>& BRepGraphInc_Storage::ChangeRefUIDs(
  const BRepGraph_RefId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myShellRefs.UIDs;
    case BRepGraph_RefId::Kind::Face:
      return myFaceRefs.UIDs;
    case BRepGraph_RefId::Kind::Wire:
      return myWireRefs.UIDs;
    case BRepGraph_RefId::Kind::CoEdge:
      return myCoEdgeRefs.UIDs;
    case BRepGraph_RefId::Kind::Vertex:
      return myVertexRefs.UIDs;
    case BRepGraph_RefId::Kind::Solid:
      return mySolidRefs.UIDs;
    case BRepGraph_RefId::Kind::Child:
      return myChildRefs.UIDs;
    case BRepGraph_RefId::Kind::Occurrence:
      return myOccurrenceRefs.UIDs;
    default:
      break;
  }
  Standard_ASSERT_RETURN(false, "ChangeRefUIDs: invalid Kind value", myShellRefs.UIDs);
  return myShellRefs.UIDs;
}

//=================================================================================================

void BRepGraphInc_Storage::ResetAllRefUIDs()
{
  myShellRefs.UIDs.Clear();
  myFaceRefs.UIDs.Clear();
  myWireRefs.UIDs.Clear();
  myCoEdgeRefs.UIDs.Clear();
  myVertexRefs.UIDs.Clear();
  mySolidRefs.UIDs.Clear();
  myChildRefs.UIDs.Clear();
  myOccurrenceRefs.UIDs.Clear();
}

//=================================================================================================

const BRepGraphInc::BaseRef& BRepGraphInc_Storage::BaseRef(const BRepGraph_RefId theRefId) const
{
  static const BRepGraphInc::BaseRef anInvalid;
  if (!theRefId.IsValid())
  {
    return anInvalid;
  }

  const auto aFindRef = [this](const auto theTypedId) -> const BRepGraphInc::BaseRef* {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_ShellRefId>)
      return findInStore(myShellRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceRefId>)
      return findInStore(myFaceRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireRefId>)
      return findInStore(myWireRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CoEdgeRefId>)
      return findInStore(myCoEdgeRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_VertexRefId>)
      return findInStore(myVertexRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidRefId>)
      return findInStore(mySolidRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ChildRefId>)
      return findInStore(myChildRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceRefId>)
      return findInStore(myOccurrenceRefs, theTypedId);

    Standard_ASSERT_RETURN(false, "BaseRef: unsupported ref id type", nullptr);
  };

  const BRepGraphInc::BaseRef* aRef = BRepGraph_RefId::Visit(theRefId, aFindRef);
  return aRef != nullptr ? *aRef : anInvalid;
}

//=================================================================================================

BRepGraphInc::BaseRef& BRepGraphInc_Storage::ChangeBaseRef(const BRepGraph_RefId theRefId)
{
  static BRepGraphInc::BaseRef anInvalid;
  if (!theRefId.IsValid())
  {
    return anInvalid;
  }

  const auto aChangeRef = [this](const auto theTypedId) -> BRepGraphInc::BaseRef* {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_ShellRefId>)
      return changeFindInStore(myShellRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceRefId>)
      return changeFindInStore(myFaceRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireRefId>)
      return changeFindInStore(myWireRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CoEdgeRefId>)
      return changeFindInStore(myCoEdgeRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_VertexRefId>)
      return changeFindInStore(myVertexRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidRefId>)
      return changeFindInStore(mySolidRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ChildRefId>)
      return changeFindInStore(myChildRefs, theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceRefId>)
      return changeFindInStore(myOccurrenceRefs, theTypedId);

    Standard_ASSERT_RETURN(false, "ChangeBaseRef: unsupported ref id type", nullptr);
  };

  BRepGraphInc::BaseRef* aRef = BRepGraph_RefId::Visit(theRefId, aChangeRef);
  return aRef != nullptr ? *aRef : anInvalid;
}

//=================================================================================================

void BRepGraphInc_Storage::Clear()
{
  myVertices.Clear();
  myEdges.Clear();
  myCoEdges.Clear();
  myWires.Clear();
  myFaces.Clear();
  myShells.Clear();
  mySolids.Clear();
  myCompounds.Clear();
  myCompSolids.Clear();
  myProducts.Clear();
  myOccurrences.Clear();
  myShellRefs.Clear();
  myFaceRefs.Clear();
  myWireRefs.Clear();
  myCoEdgeRefs.Clear();
  myVertexRefs.Clear();
  mySolidRefs.Clear();
  myChildRefs.Clear();
  myOccurrenceRefs.Clear();
  mySurfaces.Clear();
  myCurves3D.Clear();
  myCurves2D.Clear();
  myTriangulationsRep.Clear();
  myPolygons3D.Clear();
  myPolygons2D.Clear();
  myPolygonsOnTri.Clear();
  myReverseIdx.Clear();
  myTShapeToNodeId.Clear();
  myOriginalShapes.Clear();
  myIsDone = false;
}

//=================================================================================================

bool BRepGraphInc_Storage::MarkRemoved(const BRepGraph_NodeId theNodeId)
{
  const auto aMarkRemoved = [this](const auto theTypedId) -> bool {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_VertexId>)
      return myVertices.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_EdgeId>)
      return myEdges.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CoEdgeId>)
      return myCoEdges.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireId>)
      return myWires.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceId>)
      return myFaces.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ShellId>)
      return myShells.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidId>)
      return mySolids.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CompoundId>)
      return myCompounds.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CompSolidId>)
      return myCompSolids.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ProductId>)
      return myProducts.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceId>)
      return myOccurrences.MarkRemoved(theTypedId);

    Standard_ASSERT_RETURN(false, "MarkRemoved: unsupported node id type", false);
  };

  return theNodeId.IsValid() ? BRepGraph_NodeId::Visit(theNodeId, aMarkRemoved) : false;
}

//=================================================================================================

bool BRepGraphInc_Storage::MarkRemovedRef(const BRepGraph_RefId theRefId)
{
  const auto aMarkRemoved = [this](const auto theTypedId) -> bool {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_ShellRefId>)
      return myShellRefs.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceRefId>)
      return myFaceRefs.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireRefId>)
      return myWireRefs.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CoEdgeRefId>)
      return myCoEdgeRefs.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_VertexRefId>)
      return myVertexRefs.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidRefId>)
      return mySolidRefs.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ChildRefId>)
      return myChildRefs.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceRefId>)
      return myOccurrenceRefs.MarkRemoved(theTypedId);

    Standard_ASSERT_RETURN(false, "MarkRemovedRef: unsupported ref id type", false);
  };

  return theRefId.IsValid() ? BRepGraph_RefId::Visit(theRefId, aMarkRemoved) : false;
}

//=================================================================================================

bool BRepGraphInc_Storage::MarkRemovedRep(const BRepGraph_RepId theRepId)
{
  const auto aMarkRemoved = [this](const auto theTypedId) -> bool {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_SurfaceRepId>)
      return mySurfaces.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_Curve3DRepId>)
      return myCurves3D.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_Curve2DRepId>)
      return myCurves2D.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_TriangulationRepId>)
      return myTriangulationsRep.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_Polygon3DRepId>)
      return myPolygons3D.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_Polygon2DRepId>)
      return myPolygons2D.MarkRemoved(theTypedId);
    else if constexpr (std::is_same_v<TypeId, BRepGraph_PolygonOnTriRepId>)
      return myPolygonsOnTri.MarkRemoved(theTypedId);

    Standard_ASSERT_RETURN(false, "MarkRemovedRep: unsupported rep id type", false);
  };

  return theRepId.IsValid() ? BRepGraph_RepId::Visit(theRepId, aMarkRemoved) : false;
}

//=================================================================================================

void BRepGraphInc_Storage::BuildReverseIndex()
{
  myReverseIdx.SetAllocator(myAllocator);
  myReverseIdx.Build(*this);
  myReverseIdx.BuildProductOccurrences(myOccurrences.Entities, myProducts.Nb());

  // Recount active entities to sync counters after Build.
  // Populate may have set IsRemoved on some entities without going through RemoveNode.
  myVertices.NbActive          = 0;
  myEdges.NbActive             = 0;
  myCoEdges.NbActive           = 0;
  myWires.NbActive             = 0;
  myFaces.NbActive             = 0;
  myShells.NbActive            = 0;
  mySolids.NbActive            = 0;
  myCompounds.NbActive         = 0;
  myCompSolids.NbActive        = 0;
  myProducts.NbActive          = 0;
  myOccurrences.NbActive       = 0;
  mySurfaces.NbActive          = 0;
  myCurves3D.NbActive          = 0;
  myCurves2D.NbActive          = 0;
  myTriangulationsRep.NbActive = 0;
  myPolygons3D.NbActive        = 0;
  myPolygons2D.NbActive        = 0;
  myPolygonsOnTri.NbActive     = 0;
  const int aNbVertices        = myVertices.Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
    if (!myVertices.Get(aVertexId).IsRemoved)
      ++myVertices.NbActive;
  const int aNbEdges = myEdges.Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
    if (!myEdges.Get(anEdgeId).IsRemoved)
      ++myEdges.NbActive;
  const int aNbCoEdges = myCoEdges.Nb();
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
    if (!myCoEdges.Get(aCoEdgeId).IsRemoved)
      ++myCoEdges.NbActive;
  const int aNbWires = myWires.Nb();
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
    if (!myWires.Get(aWireId).IsRemoved)
      ++myWires.NbActive;
  const int aNbFaces = myFaces.Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
    if (!myFaces.Get(aFaceId).IsRemoved)
      ++myFaces.NbActive;
  const int aNbShells = myShells.Nb();
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbShells); ++aShellId)
    if (!myShells.Get(aShellId).IsRemoved)
      ++myShells.NbActive;
  const int aNbSolids = mySolids.Nb();
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aNbSolids); ++aSolidId)
    if (!mySolids.Get(aSolidId).IsRemoved)
      ++mySolids.NbActive;
  const int aNbCompounds = myCompounds.Nb();
  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(aNbCompounds); ++aCompoundId)
    if (!myCompounds.Get(aCompoundId).IsRemoved)
      ++myCompounds.NbActive;
  const int aNbCompSolids = myCompSolids.Nb();
  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(aNbCompSolids); ++aCompSolidId)
    if (!myCompSolids.Get(aCompSolidId).IsRemoved)
      ++myCompSolids.NbActive;
  const int aNbProducts = myProducts.Nb();
  for (BRepGraph_ProductId aProductId(0); aProductId.IsValid(aNbProducts); ++aProductId)
    if (!myProducts.Get(aProductId).IsRemoved)
      ++myProducts.NbActive;
  const int aNbOccurrences = myOccurrences.Nb();
  for (BRepGraph_OccurrenceId anOccurrenceId(0); anOccurrenceId.IsValid(aNbOccurrences);
       ++anOccurrenceId)
    if (!myOccurrences.Get(anOccurrenceId).IsRemoved)
      ++myOccurrences.NbActive;
  const int aNbSurfaces = mySurfaces.Nb();
  for (BRepGraph_SurfaceRepId aSurfaceRepId(0); aSurfaceRepId.IsValid(aNbSurfaces); ++aSurfaceRepId)
    if (!mySurfaces.Get(aSurfaceRepId).IsRemoved)
      ++mySurfaces.NbActive;
  const int aNbCurves3D = myCurves3D.Nb();
  for (BRepGraph_Curve3DRepId aCurve3DRepId(0); aCurve3DRepId.IsValid(aNbCurves3D); ++aCurve3DRepId)
    if (!myCurves3D.Get(aCurve3DRepId).IsRemoved)
      ++myCurves3D.NbActive;
  const int aNbCurves2D = myCurves2D.Nb();
  for (BRepGraph_Curve2DRepId aCurve2DRepId(0); aCurve2DRepId.IsValid(aNbCurves2D); ++aCurve2DRepId)
    if (!myCurves2D.Get(aCurve2DRepId).IsRemoved)
      ++myCurves2D.NbActive;
  const int aNbTriangulations = myTriangulationsRep.Nb();
  for (BRepGraph_TriangulationRepId aTriangulationRepId(0);
       aTriangulationRepId.IsValid(aNbTriangulations);
       ++aTriangulationRepId)
    if (!myTriangulationsRep.Get(aTriangulationRepId).IsRemoved)
      ++myTriangulationsRep.NbActive;
  const int aNbPolygons3D = myPolygons3D.Nb();
  for (BRepGraph_Polygon3DRepId aPolygon3DRepId(0); aPolygon3DRepId.IsValid(aNbPolygons3D);
       ++aPolygon3DRepId)
    if (!myPolygons3D.Get(aPolygon3DRepId).IsRemoved)
      ++myPolygons3D.NbActive;
  const int aNbPolygons2D = myPolygons2D.Nb();
  for (BRepGraph_Polygon2DRepId aPolygon2DRepId(0); aPolygon2DRepId.IsValid(aNbPolygons2D);
       ++aPolygon2DRepId)
    if (!myPolygons2D.Get(aPolygon2DRepId).IsRemoved)
      ++myPolygons2D.NbActive;
  const int aNbPolygonsOnTri = myPolygonsOnTri.Nb();
  for (BRepGraph_PolygonOnTriRepId aPolygonOnTriRepId(0);
       aPolygonOnTriRepId.IsValid(aNbPolygonsOnTri);
       ++aPolygonOnTriRepId)
    if (!myPolygonsOnTri.Get(aPolygonOnTriRepId).IsRemoved)
      ++myPolygonsOnTri.NbActive;
}

//=================================================================================================

void BRepGraphInc_Storage::BuildDeltaReverseIndex(const int theOldNbEdges,
                                                  const int theOldNbWires,
                                                  const int theOldNbFaces,
                                                  const int theOldNbShells,
                                                  const int theOldNbSolids,
                                                  const int theOldNbCompounds,
                                                  const int theOldNbCompSolids,
                                                  const int theOldNbChildRefs,
                                                  const int theOldNbSolidRefs)
{
  // Ensure allocator is set for reverse index inner vectors.
  // BuildReverseIndex() always calls SetAllocator(), but when
  // AppendFlattened/AppendFull is the first operation on a fresh
  // graph (no prior BRepGraph_Builder::Perform()), the allocator has not been set yet.
  myReverseIdx.SetAllocator(myAllocator);
  myReverseIdx.BuildDelta(myVertices.Entities,
                          myEdges.Entities,
                          myCoEdges.Entities,
                          myWires.Entities,
                          myFaces.Entities,
                          myShells.Entities,
                          mySolids.Entities,
                          myCompounds.Entities,
                          myCompSolids.Entities,
                          myShellRefs.Refs,
                          myFaceRefs.Refs,
                          myWireRefs.Refs,
                          myCoEdgeRefs.Refs,
                          mySolidRefs.Refs,
                          myChildRefs.Refs,
                          myVertexRefs.Refs,
                          theOldNbEdges,
                          theOldNbWires,
                          theOldNbFaces,
                          theOldNbShells,
                          theOldNbSolids,
                          theOldNbCompounds,
                          theOldNbCompSolids,
                          theOldNbChildRefs,
                          theOldNbSolidRefs);
}

//=================================================================================================

bool BRepGraphInc_Storage::ValidateReverseIndex() const
{
  // Self-ID consistency is a separate invariant; Audit-mode BRepGraph_Validate
  // runs checkDefIds / checkRefIds for that. Keeping the Self-ID check out of
  // this function lets Lightweight/MutationBoundary remain O(active-refs) and
  // preserves the design contract that Lightweight does NOT perform deep id
  // drift detection.
  if (!myReverseIdx.Validate(myVertices.Entities,
                             myEdges.Entities,
                             myCoEdges.Entities,
                             myWires.Entities,
                             myFaces.Entities,
                             myShells.Entities,
                             mySolids.Entities,
                             myCompounds.Entities,
                             myCompSolids.Entities,
                             myShellRefs.Refs,
                             myFaceRefs.Refs,
                             myWireRefs.Refs,
                             myCoEdgeRefs.Refs,
                             mySolidRefs.Refs,
                             myChildRefs.Refs,
                             myVertexRefs.Refs))
  {
    return false;
  }

  // Wire -> CoEdge and Edge -> CoEdge coherence via coedge ref entries.
  const int aNbCoEdgeRefs = myCoEdgeRefs.Nb();
  for (BRepGraph_CoEdgeRefId aCoEdgeRefId(0); aCoEdgeRefId.IsValid(aNbCoEdgeRefs); ++aCoEdgeRefId)
  {
    const BRepGraphInc::CoEdgeRef& aRef = myCoEdgeRefs.Get(aCoEdgeRefId);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Wire || !aRef.CoEdgeDefId.IsValid())
    {
      continue;
    }
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= myWires.Nb() || aRef.CoEdgeDefId.Index < 0
        || aRef.CoEdgeDefId.Index >= myCoEdges.Nb())
    {
      return false;
    }

    const BRepGraphInc::WireDef&   aWire   = myWires.Get(BRepGraph_WireId(aRef.ParentId.Index));
    const BRepGraphInc::CoEdgeDef& aCoEdge = myCoEdges.Get(aRef.CoEdgeDefId);
    if (aWire.IsRemoved || aCoEdge.IsRemoved)
    {
      continue;
    }
    if (!containsNodeIndex(myReverseIdx.WiresOfCoEdge(aRef.CoEdgeDefId), aRef.ParentId.Index))
    {
      return false;
    }
    if (aCoEdge.EdgeDefId.IsValid()
        && !containsNodeIndex(myReverseIdx.CoEdgesOfEdge(aCoEdge.EdgeDefId),
                              aRef.CoEdgeDefId.Index))
    {
      return false;
    }
  }

  // Reverse Edge -> CoEdges entries must point back to active CoEdges.
  const int aNbEdges = myEdges.Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdges = myReverseIdx.CoEdgesOfEdge(anEdgeId);
    if (aCoEdges == nullptr)
    {
      continue;
    }
    for (const BRepGraph_CoEdgeId& aCoEdgeElem : *aCoEdges)
    {
      const int aRefIdx = aCoEdgeElem.Index;
      if (aRefIdx < 0 || aRefIdx >= myCoEdges.Nb())
      {
        return false;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge = myCoEdges.Get(aCoEdgeElem);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid()
          || aCoEdge.EdgeDefId.Index != anEdgeId.Index)
      {
        return false;
      }
    }
  }

  // Compound child reverse maps via child ref entries.
  const int aNbChildRefs = myChildRefs.Nb();
  for (BRepGraph_ChildRefId aChildRefId(0); aChildRefId.IsValid(aNbChildRefs); ++aChildRefId)
  {
    const BRepGraphInc::ChildRef& aRef = myChildRefs.Get(aChildRefId);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Compound || !aRef.ChildDefId.IsValid())
    {
      continue;
    }
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= myCompounds.Nb())
    {
      return false;
    }

    const BRepGraphInc::CompoundDef& aCompound =
      myCompounds.Get(BRepGraph_CompoundId(aRef.ParentId.Index));
    if (aCompound.IsRemoved)
    {
      continue;
    }

    switch (aRef.ChildDefId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Solid:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfSolid(BRepGraph_SolidId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Shell:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfShell(BRepGraph_ShellId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Face:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfFace(BRepGraph_FaceId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Compound:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfCompound(BRepGraph_CompoundId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::CompSolid:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfCompSolid(BRepGraph_CompSolidId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Wire:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfWire(BRepGraph_WireId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Edge:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfEdge(BRepGraph_EdgeId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      case BRepGraph_NodeId::Kind::Vertex:
        if (!containsNodeIndex(
              myReverseIdx.CompoundsOfVertex(BRepGraph_VertexId(aRef.ChildDefId.Index)),
              aRef.ParentId.Index))
        {
          return false;
        }
        break;
      default:
        break;
    }
  }

  // CompSolid -> Solid reverse map via solid ref entries.
  const int aNbSolidRefs = mySolidRefs.Nb();
  for (BRepGraph_SolidRefId aSolidRefId(0); aSolidRefId.IsValid(aNbSolidRefs); ++aSolidRefId)
  {
    const BRepGraphInc::SolidRef& aRef = mySolidRefs.Get(aSolidRefId);
    if (aRef.IsRemoved || !aRef.ParentId.IsValid()
        || aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::CompSolid
        || !aRef.SolidDefId.IsValid())
    {
      continue;
    }
    if (aRef.ParentId.Index < 0 || aRef.ParentId.Index >= myCompSolids.Nb()
        || aRef.SolidDefId.Index < 0 || aRef.SolidDefId.Index >= mySolids.Nb())
    {
      return false;
    }

    const BRepGraphInc::CompSolidDef& aCompSolid =
      myCompSolids.Get(BRepGraph_CompSolidId(aRef.ParentId.Index));
    if (aCompSolid.IsRemoved || mySolids.Get(aRef.SolidDefId).IsRemoved)
    {
      continue;
    }
    if (!containsNodeIndex(myReverseIdx.CompSolidsOfSolid(aRef.SolidDefId), aRef.ParentId.Index))
    {
      return false;
    }
  }

  // Occurrence -> Product reverse map.
  BRepGraph_OccurrenceId anOccurrenceId(0);
  for (const BRepGraphInc::OccurrenceDef& anOcc : myOccurrences.Entities)
  {
    if (!anOcc.IsRemoved)
    {
      if (!anOcc.ChildDefId.IsValid())
      {
        return false;
      }
      if (anOcc.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
      {
        if (anOcc.ChildDefId.Index >= myProducts.Nb())
        {
          return false;
        }
        if (!containsNodeIndex(
              myReverseIdx.OccurrencesOfProduct(BRepGraph_ProductId(anOcc.ChildDefId.Index)),
              anOccurrenceId.Index))
        {
          return false;
        }
      }
    }
    ++anOccurrenceId;
  }

  // Reverse Product -> Occurrences entries must point to active occurrences
  // that reference the same product.
  const int aNbProducts = myProducts.Nb();
  for (BRepGraph_ProductId aProductId(0); aProductId.IsValid(aNbProducts); ++aProductId)
  {
    const NCollection_Vector<BRepGraph_OccurrenceId>* anOccs =
      myReverseIdx.OccurrencesOfProduct(aProductId);
    if (anOccs == nullptr)
    {
      continue;
    }
    for (const BRepGraph_OccurrenceId& anOccElem : *anOccs)
    {
      const int anOccIdx = anOccElem.Index;
      if (anOccIdx < 0 || anOccIdx >= myOccurrences.Nb())
      {
        return false;
      }
      const BRepGraphInc::OccurrenceDef& anOcc = myOccurrences.Get(anOccElem);
      if (anOcc.IsRemoved || !anOcc.ChildDefId.IsValid()
          || anOcc.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Product
          || anOcc.ChildDefId.Index != aProductId.Index)
      {
        return false;
      }
    }
  }

  return true;
}
