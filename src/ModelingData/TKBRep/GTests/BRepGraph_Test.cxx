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

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_Iterator.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Face.hxx>

#include <atomic>
#include <type_traits>

#include <gtest/gtest.h>

namespace
{
static_assert(!std::is_convertible_v<BRepGraph_FaceId, BRepGraph_EdgeId>);
static_assert(!std::is_constructible_v<BRepGraph_FaceId, BRepGraph_EdgeId>);
static_assert(std::is_convertible_v<BRepGraph_FaceId, BRepGraph_NodeId>);
static_assert(std::is_constructible_v<BRepGraph_ParentExplorer,
                                      const BRepGraph&,
                                      BRepGraph_FaceId,
                                      BRepGraph_NodeId::Kind>);
static_assert(std::is_constructible_v<BRepGraph_ChildExplorer,
                                      const BRepGraph&,
                                      BRepGraph_FaceId,
                                      BRepGraph_NodeId::Kind>);
static_assert(std::is_constructible_v<BRepGraph_ChildExplorer,
                                      const BRepGraph&,
                                      BRepGraph_ProductId,
                                      BRepGraph_NodeId::Kind>);
static_assert(!std::is_convertible_v<BRepGraph_SurfaceRepId, BRepGraph_Curve3DRepId>);
static_assert(!std::is_constructible_v<BRepGraph_SurfaceRepId, BRepGraph_Curve3DRepId>);
static_assert(std::is_convertible_v<BRepGraph_SurfaceRepId, BRepGraph_RepId>);

const occ::handle<BRepGraph_CacheKind>& testDoubleAttrKind()
{
  static const occ::handle<BRepGraph_CacheKind> THE_KIND =
    new BRepGraph_CacheKind(Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10021"),
                            "TestDoubleAttr");
  return THE_KIND;
}

const occ::handle<BRepGraph_CacheKind>& testIntAttrKind()
{
  static const occ::handle<BRepGraph_CacheKind> THE_KIND =
    new BRepGraph_CacheKind(Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10022"), "TestIntAttr");
  return THE_KIND;
}

const occ::handle<BRepGraph_CacheKind>& testAuxAttrKind()
{
  static const occ::handle<BRepGraph_CacheKind> THE_KIND =
    new BRepGraph_CacheKind(Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10023"), "TestAuxAttr");
  return THE_KIND;
}

static int componentKey(const BRepGraph_NodeId theNode)
{
  return theNode.Index * BRepGraph_NodeId::THE_KIND_COUNT + static_cast<int>(theNode.NodeKind);
}

static NCollection_DynamicArray<BRepGraph_EdgeId> collectFreeEdges(const BRepGraph& theGraph)
{
  NCollection_DynamicArray<BRepGraph_EdgeId> aResult(16);
  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = anEdgeIt.Current();
    if (anEdge.IsDegenerate)
    {
      continue;
    }

    NCollection_Map<int> aOwningFaces;
    for (BRepGraph_ParentExplorer aFaceExp(theGraph, anEdgeId, BRepGraph_NodeId::Kind::Face);
         aFaceExp.More();
         aFaceExp.Next())
    {
      aOwningFaces.Add(aFaceExp.Current().DefId.Index);
      if (aOwningFaces.Extent() > 1)
      {
        break;
      }
    }

    if (aOwningFaces.Extent() == 1)
    {
      aResult.Append(anEdgeId);
    }
  }
  return aResult;
}

static BRepGraph_NodeId componentRootOfFace(const BRepGraph&       theGraph,
                                            const BRepGraph_FaceId theFaceId)
{
  for (BRepGraph_ParentExplorer aSolidExp(theGraph, theFaceId, BRepGraph_NodeId::Kind::Solid);
       aSolidExp.More();
       aSolidExp.Next())
  {
    return aSolidExp.Current().DefId;
  }

  for (BRepGraph_ParentExplorer aShellExp(theGraph, theFaceId, BRepGraph_NodeId::Kind::Shell);
       aShellExp.More();
       aShellExp.Next())
  {
    return aShellExp.Current().DefId;
  }

  return theFaceId;
}

static int countFaceComponents(const BRepGraph& theGraph)
{
  NCollection_Map<int> aRoots;
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    aRoots.Add(componentKey(componentRootOfFace(theGraph, aFaceIt.CurrentId())));
  }
  return aRoots.Extent();
}

struct ReverseIndexInputData
{
  NCollection_DynamicArray<BRepGraphInc::VertexDef>    Vertices;
  NCollection_DynamicArray<BRepGraphInc::EdgeDef>      Edges;
  NCollection_DynamicArray<BRepGraphInc::CoEdgeDef>    CoEdges;
  NCollection_DynamicArray<BRepGraphInc::WireDef>      Wires;
  NCollection_DynamicArray<BRepGraphInc::FaceDef>      Faces;
  NCollection_DynamicArray<BRepGraphInc::ShellDef>     Shells;
  NCollection_DynamicArray<BRepGraphInc::SolidDef>     Solids;
  NCollection_DynamicArray<BRepGraphInc::CompoundDef>  Compounds;
  NCollection_DynamicArray<BRepGraphInc::CompSolidDef> CompSolids;
  NCollection_DynamicArray<BRepGraphInc::ShellRef>     ShellRefs;
  NCollection_DynamicArray<BRepGraphInc::FaceRef>      FaceRefs;
  NCollection_DynamicArray<BRepGraphInc::WireRef>      WireRefs;
  NCollection_DynamicArray<BRepGraphInc::CoEdgeRef>    CoEdgeRefs;
  NCollection_DynamicArray<BRepGraphInc::SolidRef>     SolidRefs;
  NCollection_DynamicArray<BRepGraphInc::ChildRef>     ChildRefs;
  NCollection_DynamicArray<BRepGraphInc::VertexRef>    VertexRefs;
};

static ReverseIndexInputData buildReverseIndexBaseInput()
{
  ReverseIndexInputData aData;

  BRepGraphInc::EdgeDef& anEdge = aData.Edges.Appended();
  anEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());

  // Create vertex ref entries for start and end vertices.
  BRepGraphInc::VertexRef& aStartVRef0 = aData.VertexRefs.Appended();
  aStartVRef0.ParentId                 = BRepGraph_EdgeId::Start();
  aStartVRef0.VertexDefId              = BRepGraph_VertexId::Start();
  aStartVRef0.Orientation              = TopAbs_FORWARD;
  anEdge.StartVertexRefId              = BRepGraph_VertexRefId(aData.VertexRefs.Length() - 1);

  BRepGraphInc::VertexRef& anEndVRef0 = aData.VertexRefs.Appended();
  anEndVRef0.ParentId                 = BRepGraph_EdgeId::Start();
  anEndVRef0.VertexDefId              = BRepGraph_VertexId(1);
  anEndVRef0.Orientation              = TopAbs_REVERSED;
  anEdge.EndVertexRefId               = BRepGraph_VertexRefId(aData.VertexRefs.Length() - 1);

  BRepGraphInc::CoEdgeDef& aCoEdge = aData.CoEdges.Appended();
  aCoEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aCoEdge.EdgeDefId = BRepGraph_EdgeId::Start();
  aCoEdge.FaceDefId = BRepGraph_FaceId::Start();

  BRepGraphInc::WireDef& aWire = aData.Wires.Appended();
  aWire.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::CoEdgeRef& aCoEdgeRef = aData.CoEdgeRefs.Appended();
  aCoEdgeRef.ParentId                 = BRepGraph_WireId::Start();
  aCoEdgeRef.CoEdgeDefId              = BRepGraph_CoEdgeId::Start();
  aWire.CoEdgeRefIds.Append(BRepGraph_CoEdgeRefId::Start());

  BRepGraphInc::FaceDef& aFace = aData.Faces.Appended();
  aFace.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::WireRef& aWireRef = aData.WireRefs.Appended();
  aWireRef.ParentId               = BRepGraph_FaceId::Start();
  aWireRef.WireDefId              = BRepGraph_WireId::Start();
  aWireRef.IsOuter                = true;
  aFace.WireRefIds.Append(BRepGraph_WireRefId::Start());

  BRepGraphInc::ShellDef& aShell = aData.Shells.Appended();
  aShell.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::FaceRef& aFaceRef = aData.FaceRefs.Appended();
  aFaceRef.ParentId               = BRepGraph_ShellId::Start();
  aFaceRef.FaceDefId              = BRepGraph_FaceId::Start();
  aShell.FaceRefIds.Append(BRepGraph_FaceRefId::Start());

  BRepGraphInc::SolidDef& aSolid = aData.Solids.Appended();
  aSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::ShellRef& aShellRef = aData.ShellRefs.Appended();
  aShellRef.ParentId                = BRepGraph_SolidId::Start();
  aShellRef.ShellDefId              = BRepGraph_ShellId::Start();
  aSolid.ShellRefIds.Append(BRepGraph_ShellRefId::Start());

  return aData;
}

static void appendReverseIndexDeltaInput(ReverseIndexInputData& theData)
{
  // Active edge/wire/face/shell/solid chain.
  BRepGraphInc::EdgeDef& anEdge = theData.Edges.Appended();
  anEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::VertexRef& aStartVRef1 = theData.VertexRefs.Appended();
  aStartVRef1.ParentId                 = BRepGraph_EdgeId(1);
  aStartVRef1.VertexDefId              = BRepGraph_VertexId(2);
  aStartVRef1.Orientation              = TopAbs_FORWARD;
  anEdge.StartVertexRefId              = BRepGraph_VertexRefId(theData.VertexRefs.Length() - 1);

  BRepGraphInc::VertexRef& anEndVRef1 = theData.VertexRefs.Appended();
  anEndVRef1.ParentId                 = BRepGraph_EdgeId(1);
  anEndVRef1.VertexDefId              = BRepGraph_VertexId(3);
  anEndVRef1.Orientation              = TopAbs_REVERSED;
  anEdge.EndVertexRefId               = BRepGraph_VertexRefId(theData.VertexRefs.Length() - 1);

  BRepGraphInc::CoEdgeDef& aCoEdge = theData.CoEdges.Appended();
  aCoEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aCoEdge.EdgeDefId = BRepGraph_EdgeId(1);
  aCoEdge.FaceDefId = BRepGraph_FaceId(1);

  BRepGraphInc::WireDef& aWire = theData.Wires.Appended();
  aWire.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::CoEdgeRef& aCoEdgeRef = theData.CoEdgeRefs.Appended();
  aCoEdgeRef.ParentId                 = BRepGraph_WireId(1);
  aCoEdgeRef.CoEdgeDefId              = BRepGraph_CoEdgeId(1);
  aWire.CoEdgeRefIds.Append(BRepGraph_CoEdgeRefId(theData.CoEdgeRefs.Length() - 1));

  BRepGraphInc::FaceDef& aFace = theData.Faces.Appended();
  aFace.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::WireRef& aWireRef = theData.WireRefs.Appended();
  aWireRef.ParentId               = BRepGraph_FaceId(1);
  aWireRef.WireDefId              = BRepGraph_WireId(1);
  aWireRef.IsOuter                = true;
  aFace.WireRefIds.Append(BRepGraph_WireRefId(theData.WireRefs.Length() - 1));

  BRepGraphInc::ShellDef& aShell = theData.Shells.Appended();
  aShell.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::FaceRef& aFaceRef = theData.FaceRefs.Appended();
  aFaceRef.ParentId               = BRepGraph_ShellId(1);
  aFaceRef.FaceDefId              = BRepGraph_FaceId(1);
  aShell.FaceRefIds.Append(BRepGraph_FaceRefId(theData.FaceRefs.Length() - 1));

  BRepGraphInc::SolidDef& aSolid = theData.Solids.Appended();
  aSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::ShellRef& aShellRef = theData.ShellRefs.Appended();
  aShellRef.ParentId                = BRepGraph_SolidId(1);
  aShellRef.ShellDefId              = BRepGraph_ShellId(1);
  aSolid.ShellRefIds.Append(BRepGraph_ShellRefId(theData.ShellRefs.Length() - 1));

  BRepGraphInc::CompoundDef& aCompoundOfSolid = theData.Compounds.Appended();
  aCompoundOfSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::ChildRef& aCompoundSolidRef = theData.ChildRefs.Appended();
  aCompoundSolidRef.ParentId                = BRepGraph_CompoundId::Start();
  aCompoundSolidRef.ChildDefId              = BRepGraph_SolidId(1);
  aCompoundOfSolid.ChildRefIds.Append(BRepGraph_ChildRefId(theData.ChildRefs.Length() - 1));

  BRepGraphInc::CompoundDef& aCompoundOfShell = theData.Compounds.Appended();
  aCompoundOfShell.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::ChildRef& aCompoundShellRef = theData.ChildRefs.Appended();
  aCompoundShellRef.ParentId                = BRepGraph_CompoundId(1);
  aCompoundShellRef.ChildDefId              = BRepGraph_ShellId(1);
  aCompoundOfShell.ChildRefIds.Append(BRepGraph_ChildRefId(theData.ChildRefs.Length() - 1));

  BRepGraphInc::CompoundDef& aCompoundOfFace = theData.Compounds.Appended();
  aCompoundOfFace.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::ChildRef& aCompoundFaceRef = theData.ChildRefs.Appended();
  aCompoundFaceRef.ParentId                = BRepGraph_CompoundId(2);
  aCompoundFaceRef.ChildDefId              = BRepGraph_FaceId(1);
  aCompoundOfFace.ChildRefIds.Append(BRepGraph_ChildRefId(theData.ChildRefs.Length() - 1));

  BRepGraphInc::CompoundDef& aNestedCompound = theData.Compounds.Appended();
  aNestedCompound.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::ChildRef& aNestedCompoundRef = theData.ChildRefs.Appended();
  aNestedCompoundRef.ParentId                = BRepGraph_CompoundId(3);
  aNestedCompoundRef.ChildDefId              = BRepGraph_CompoundId::Start();
  aNestedCompound.ChildRefIds.Append(BRepGraph_ChildRefId(theData.ChildRefs.Length() - 1));

  BRepGraphInc::CompSolidDef& aCompSolid = theData.CompSolids.Appended();
  aCompSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::SolidRef& aCompSolidRef = theData.SolidRefs.Appended();
  aCompSolidRef.ParentId                = BRepGraph_CompSolidId::Start();
  aCompSolidRef.SolidDefId              = BRepGraph_SolidId(1);
  aCompSolid.SolidRefIds.Append(BRepGraph_SolidRefId(theData.SolidRefs.Length() - 1));

  BRepGraphInc::CompoundDef& aCompoundOfCompSolid = theData.Compounds.Appended();
  aCompoundOfCompSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::ChildRef& aCompoundCompSolidRef = theData.ChildRefs.Appended();
  aCompoundCompSolidRef.ParentId                = BRepGraph_CompoundId(4);
  aCompoundCompSolidRef.ChildDefId              = BRepGraph_CompSolidId::Start();
  aCompoundOfCompSolid.ChildRefIds.Append(BRepGraph_ChildRefId(theData.ChildRefs.Length() - 1));

  // Removed entities to ensure BuildDelta skips them.
  BRepGraphInc::EdgeDef& aRemovedEdge = theData.Edges.Appended();
  aRemovedEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedEdge.IsRemoved = true;

  BRepGraphInc::VertexRef& aRemovedStartVRef = theData.VertexRefs.Appended();
  aRemovedStartVRef.ParentId                 = BRepGraph_EdgeId(2);
  aRemovedStartVRef.VertexDefId              = BRepGraph_VertexId(10);
  aRemovedStartVRef.Orientation              = TopAbs_FORWARD;
  aRemovedEdge.StartVertexRefId = BRepGraph_VertexRefId(theData.VertexRefs.Length() - 1);

  BRepGraphInc::VertexRef& aRemovedEndVRef = theData.VertexRefs.Appended();
  aRemovedEndVRef.ParentId                 = BRepGraph_EdgeId(2);
  aRemovedEndVRef.VertexDefId              = BRepGraph_VertexId(11);
  aRemovedEndVRef.Orientation              = TopAbs_REVERSED;
  aRemovedEdge.EndVertexRefId              = BRepGraph_VertexRefId(theData.VertexRefs.Length() - 1);

  BRepGraphInc::CoEdgeDef& aRemovedCoEdge = theData.CoEdges.Appended();
  aRemovedCoEdge.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedCoEdge.IsRemoved = true;
  aRemovedCoEdge.EdgeDefId = BRepGraph_EdgeId(2);
  aRemovedCoEdge.FaceDefId = BRepGraph_FaceId(2);

  BRepGraphInc::WireDef& aRemovedWire = theData.Wires.Appended();
  aRemovedWire.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedWire.IsRemoved = true;

  BRepGraphInc::CoEdgeRef& aRemovedCoEdgeRef = theData.CoEdgeRefs.Appended();
  aRemovedCoEdgeRef.ParentId                 = BRepGraph_WireId(2);
  aRemovedCoEdgeRef.CoEdgeDefId              = BRepGraph_CoEdgeId(2);
  aRemovedCoEdgeRef.IsRemoved                = true;

  BRepGraphInc::FaceDef& aRemovedFace = theData.Faces.Appended();
  aRemovedFace.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedFace.IsRemoved = true;

  BRepGraphInc::WireRef& aRemovedWireRef = theData.WireRefs.Appended();
  aRemovedWireRef.ParentId               = BRepGraph_FaceId(2);
  aRemovedWireRef.WireDefId              = BRepGraph_WireId(2);
  aRemovedWireRef.IsRemoved              = true;

  BRepGraphInc::ShellDef& aRemovedShell = theData.Shells.Appended();
  aRemovedShell.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedShell.IsRemoved = true;

  BRepGraphInc::FaceRef& aRemovedFaceRef = theData.FaceRefs.Appended();
  aRemovedFaceRef.ParentId               = BRepGraph_ShellId(2);
  aRemovedFaceRef.FaceDefId              = BRepGraph_FaceId(2);
  aRemovedFaceRef.IsRemoved              = true;

  BRepGraphInc::SolidDef& aRemovedSolid = theData.Solids.Appended();
  aRemovedSolid.InitVectors(occ::handle<NCollection_BaseAllocator>());
  aRemovedSolid.IsRemoved = true;

  BRepGraphInc::ShellRef& aRemovedShellRef = theData.ShellRefs.Appended();
  aRemovedShellRef.ParentId                = BRepGraph_SolidId(2);
  aRemovedShellRef.ShellDefId              = BRepGraph_ShellId(2);
  aRemovedShellRef.IsRemoved               = true;
}

static void verifyBuildDeltaScenario(const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  ReverseIndexInputData aData = buildReverseIndexBaseInput();

  BRepGraphInc_ReverseIndex aRevIdx;
  aRevIdx.SetAllocator(theAllocator);
  aRevIdx.Build(aData.Vertices,
                aData.Edges,
                aData.CoEdges,
                aData.Wires,
                aData.Faces,
                aData.Shells,
                aData.Solids,
                aData.Compounds,
                aData.CompSolids,
                aData.ShellRefs,
                aData.FaceRefs,
                aData.WireRefs,
                aData.CoEdgeRefs,
                aData.SolidRefs,
                aData.ChildRefs,
                aData.VertexRefs);

  EXPECT_TRUE(aRevIdx.Validate(aData.Vertices,
                               aData.Edges,
                               aData.CoEdges,
                               aData.Wires,
                               aData.Faces,
                               aData.Shells,
                               aData.Solids,
                               aData.Compounds,
                               aData.CompSolids,
                               aData.ShellRefs,
                               aData.FaceRefs,
                               aData.WireRefs,
                               aData.CoEdgeRefs,
                               aData.SolidRefs,
                               aData.ChildRefs,
                               aData.VertexRefs));

  const NCollection_DynamicArray<BRepGraph_WireId>* aBaseWires =
    aRevIdx.WiresOfEdge(BRepGraph_EdgeId::Start());
  ASSERT_NE(aBaseWires, nullptr);
  ASSERT_EQ(aBaseWires->Length(), 1);
  EXPECT_EQ(aBaseWires->Value(0), BRepGraph_WireId::Start());

  const int anOldNbEdges      = aData.Edges.Length();
  const int anOldNbWires      = aData.Wires.Length();
  const int anOldNbFaces      = aData.Faces.Length();
  const int anOldNbShells     = aData.Shells.Length();
  const int anOldNbSolids     = aData.Solids.Length();
  const int anOldNbCompounds  = aData.Compounds.Length();
  const int anOldNbCompSolids = aData.CompSolids.Length();
  const int anOldNbChildRefs  = aData.ChildRefs.Length();
  const int anOldNbSolidRefs  = aData.SolidRefs.Length();

  appendReverseIndexDeltaInput(aData);

  aRevIdx.BuildDelta(aData.Vertices,
                     aData.Edges,
                     aData.CoEdges,
                     aData.Wires,
                     aData.Faces,
                     aData.Shells,
                     aData.Solids,
                     aData.Compounds,
                     aData.CompSolids,
                     aData.ShellRefs,
                     aData.FaceRefs,
                     aData.WireRefs,
                     aData.CoEdgeRefs,
                     aData.SolidRefs,
                     aData.ChildRefs,
                     aData.VertexRefs,
                     anOldNbEdges,
                     anOldNbWires,
                     anOldNbFaces,
                     anOldNbShells,
                     anOldNbSolids,
                     anOldNbCompounds,
                     anOldNbCompSolids,
                     anOldNbChildRefs,
                     anOldNbSolidRefs);

  EXPECT_TRUE(aRevIdx.Validate(aData.Vertices,
                               aData.Edges,
                               aData.CoEdges,
                               aData.Wires,
                               aData.Faces,
                               aData.Shells,
                               aData.Solids,
                               aData.Compounds,
                               aData.CompSolids,
                               aData.ShellRefs,
                               aData.FaceRefs,
                               aData.WireRefs,
                               aData.CoEdgeRefs,
                               aData.SolidRefs,
                               aData.ChildRefs,
                               aData.VertexRefs));

  const NCollection_DynamicArray<BRepGraph_WireId>* aActiveWires =
    aRevIdx.WiresOfEdge(BRepGraph_EdgeId(1));
  ASSERT_NE(aActiveWires, nullptr);
  ASSERT_EQ(aActiveWires->Length(), 1);
  EXPECT_EQ(aActiveWires->Value(0), BRepGraph_WireId(1));

  const NCollection_DynamicArray<BRepGraph_FaceId>* aActiveFaces =
    aRevIdx.FacesOfWire(BRepGraph_WireId(1));
  ASSERT_NE(aActiveFaces, nullptr);
  ASSERT_EQ(aActiveFaces->Length(), 1);
  EXPECT_EQ(aActiveFaces->Value(0), BRepGraph_FaceId(1));

  const NCollection_DynamicArray<BRepGraph_ShellId>* anActiveShells =
    aRevIdx.ShellsOfFace(BRepGraph_FaceId(1));
  ASSERT_NE(anActiveShells, nullptr);
  ASSERT_EQ(anActiveShells->Length(), 1);
  EXPECT_EQ(anActiveShells->Value(0), BRepGraph_ShellId(1));

  const NCollection_DynamicArray<BRepGraph_SolidId>* anActiveSolids =
    aRevIdx.SolidsOfShell(BRepGraph_ShellId(1));
  ASSERT_NE(anActiveSolids, nullptr);
  ASSERT_EQ(anActiveSolids->Length(), 1);
  EXPECT_EQ(anActiveSolids->Value(0), BRepGraph_SolidId(1));

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfSolid =
    aRevIdx.CompoundsOfSolid(BRepGraph_SolidId(1));
  ASSERT_NE(aCompoundsOfSolid, nullptr);
  ASSERT_EQ(aCompoundsOfSolid->Length(), 1);
  EXPECT_EQ(aCompoundsOfSolid->Value(0), BRepGraph_CompoundId::Start());

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfShell =
    aRevIdx.CompoundsOfShell(BRepGraph_ShellId(1));
  ASSERT_NE(aCompoundsOfShell, nullptr);
  ASSERT_EQ(aCompoundsOfShell->Length(), 1);
  EXPECT_EQ(aCompoundsOfShell->Value(0), BRepGraph_CompoundId(1));

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfFace =
    aRevIdx.CompoundsOfFace(BRepGraph_FaceId(1));
  ASSERT_NE(aCompoundsOfFace, nullptr);
  ASSERT_EQ(aCompoundsOfFace->Length(), 1);
  EXPECT_EQ(aCompoundsOfFace->Value(0), BRepGraph_CompoundId(2));

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfCompound =
    aRevIdx.CompoundsOfCompound(BRepGraph_CompoundId::Start());
  ASSERT_NE(aCompoundsOfCompound, nullptr);
  ASSERT_EQ(aCompoundsOfCompound->Length(), 1);
  EXPECT_EQ(aCompoundsOfCompound->Value(0), BRepGraph_CompoundId(3));

  const NCollection_DynamicArray<BRepGraph_CompSolidId>* aCompSolidsOfSolid =
    aRevIdx.CompSolidsOfSolid(BRepGraph_SolidId(1));
  ASSERT_NE(aCompSolidsOfSolid, nullptr);
  ASSERT_EQ(aCompSolidsOfSolid->Length(), 1);
  EXPECT_EQ(aCompSolidsOfSolid->Value(0), BRepGraph_CompSolidId::Start());

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfCompSolid =
    aRevIdx.CompoundsOfCompSolid(BRepGraph_CompSolidId::Start());
  ASSERT_NE(aCompoundsOfCompSolid, nullptr);
  ASSERT_EQ(aCompoundsOfCompSolid->Length(), 1);
  EXPECT_EQ(aCompoundsOfCompSolid->Value(0), BRepGraph_CompoundId(4));

  EXPECT_EQ(aRevIdx.NbFacesOfEdge(BRepGraph_EdgeId(1)), 1);

  EXPECT_EQ(aRevIdx.WiresOfEdge(BRepGraph_EdgeId(2)), nullptr);
  EXPECT_EQ(aRevIdx.EdgesOfVertex(BRepGraph_VertexId(10)), nullptr);
  EXPECT_EQ(aRevIdx.NbFacesOfEdge(BRepGraph_EdgeId(2)), 0);
}
} // namespace

class BRepGraphTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
      BRepGraph_Builder::Add(myGraph, aBox);
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraphTest, FaceCountMatchesFacesVector_AfterBindUnbindSequence)
{
  ReverseIndexInputData aData = buildReverseIndexBaseInput();

  // Add a second active face for bind/unbind sequence checks.
  BRepGraphInc::FaceDef& aFace1 = aData.Faces.Appended();
  aFace1.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc_ReverseIndex aRevIdx;
  aRevIdx.Build(aData.Vertices,
                aData.Edges,
                aData.CoEdges,
                aData.Wires,
                aData.Faces,
                aData.Shells,
                aData.Solids,
                aData.Compounds,
                aData.CompSolids,
                aData.ShellRefs,
                aData.FaceRefs,
                aData.WireRefs,
                aData.CoEdgeRefs,
                aData.SolidRefs,
                aData.ChildRefs,
                aData.VertexRefs);

  auto expectCountsMatch = [&]() {
    for (int anEdgeIdx = 0; anEdgeIdx < aData.Edges.Length(); ++anEdgeIdx)
    {
      const NCollection_DynamicArray<BRepGraph_FaceId>* aFaces =
        aRevIdx.FacesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
      const int aExpectedCount = (aFaces == nullptr) ? 0 : aFaces->Length();
      EXPECT_EQ(aRevIdx.NbFacesOfEdge(BRepGraph_EdgeId(anEdgeIdx)), aExpectedCount)
        << "Edge " << anEdgeIdx << " face-count cache mismatch";
    }
  };

  expectCountsMatch();

  // Duplicate bind should be idempotent.
  aRevIdx.BindEdgeToFace(BRepGraph_EdgeId::Start(), BRepGraph_FaceId::Start());
  expectCountsMatch();

  // Bind/unbind/rebind sequence must keep cached count consistent.
  aRevIdx.BindEdgeToFace(BRepGraph_EdgeId::Start(), BRepGraph_FaceId(1));
  expectCountsMatch();
  aRevIdx.UnbindEdgeFromFace(BRepGraph_EdgeId::Start(), BRepGraph_FaceId::Start());
  expectCountsMatch();
  aRevIdx.BindEdgeToFace(BRepGraph_EdgeId::Start(), BRepGraph_FaceId::Start());
  expectCountsMatch();
  aRevIdx.UnbindEdgeFromFace(BRepGraph_EdgeId::Start(), BRepGraph_FaceId(1));
  expectCountsMatch();
}

TEST_F(BRepGraphTest, ReverseIndexValidate_DetectsStaleEdgeWireMapping)
{
  ReverseIndexInputData aData = buildReverseIndexBaseInput();

  BRepGraphInc_ReverseIndex aRevIdx;
  aRevIdx.Build(aData.Vertices,
                aData.Edges,
                aData.CoEdges,
                aData.Wires,
                aData.Faces,
                aData.Shells,
                aData.Solids,
                aData.Compounds,
                aData.CompSolids,
                aData.ShellRefs,
                aData.FaceRefs,
                aData.WireRefs,
                aData.CoEdgeRefs,
                aData.SolidRefs,
                aData.ChildRefs,
                aData.VertexRefs);

  ASSERT_TRUE(aRevIdx.Validate(aData.Vertices,
                               aData.Edges,
                               aData.CoEdges,
                               aData.Wires,
                               aData.Faces,
                               aData.Shells,
                               aData.Solids,
                               aData.Compounds,
                               aData.CompSolids,
                               aData.ShellRefs,
                               aData.FaceRefs,
                               aData.WireRefs,
                               aData.CoEdgeRefs,
                               aData.SolidRefs,
                               aData.ChildRefs,
                               aData.VertexRefs));

  // Inject stale reverse entry (edge 0 -> wire 1) with no matching forward coedge ref.
  BRepGraphInc::WireDef& aWire1 = aData.Wires.Appended();
  aWire1.InitVectors(occ::handle<NCollection_BaseAllocator>());

  aRevIdx.BindEdgeToWire(BRepGraph_EdgeId::Start(), BRepGraph_WireId(1));

  EXPECT_FALSE(aRevIdx.Validate(aData.Vertices,
                                aData.Edges,
                                aData.CoEdges,
                                aData.Wires,
                                aData.Faces,
                                aData.Shells,
                                aData.Solids,
                                aData.Compounds,
                                aData.CompSolids,
                                aData.ShellRefs,
                                aData.FaceRefs,
                                aData.WireRefs,
                                aData.CoEdgeRefs,
                                aData.SolidRefs,
                                aData.ChildRefs,
                                aData.VertexRefs));
}

TEST_F(BRepGraphTest, ReverseIndexValidate_DetectsStaleCompoundAndCompSolidMappings)
{
  ReverseIndexInputData aData = buildReverseIndexBaseInput();
  appendReverseIndexDeltaInput(aData);

  BRepGraphInc_ReverseIndex aRevIdx;
  aRevIdx.Build(aData.Vertices,
                aData.Edges,
                aData.CoEdges,
                aData.Wires,
                aData.Faces,
                aData.Shells,
                aData.Solids,
                aData.Compounds,
                aData.CompSolids,
                aData.ShellRefs,
                aData.FaceRefs,
                aData.WireRefs,
                aData.CoEdgeRefs,
                aData.SolidRefs,
                aData.ChildRefs,
                aData.VertexRefs);

  ASSERT_TRUE(aRevIdx.Validate(aData.Vertices,
                               aData.Edges,
                               aData.CoEdges,
                               aData.Wires,
                               aData.Faces,
                               aData.Shells,
                               aData.Solids,
                               aData.Compounds,
                               aData.CompSolids,
                               aData.ShellRefs,
                               aData.FaceRefs,
                               aData.WireRefs,
                               aData.CoEdgeRefs,
                               aData.SolidRefs,
                               aData.ChildRefs,
                               aData.VertexRefs));

  // Corrupt forward refs after Build() so reverse tables become stale.
  aData.SolidRefs.ChangeValue(0).IsRemoved = true;
  aData.ChildRefs.ChangeValue(0).IsRemoved = true;

  EXPECT_FALSE(aRevIdx.Validate(aData.Vertices,
                                aData.Edges,
                                aData.CoEdges,
                                aData.Wires,
                                aData.Faces,
                                aData.Shells,
                                aData.Solids,
                                aData.Compounds,
                                aData.CompSolids,
                                aData.ShellRefs,
                                aData.FaceRefs,
                                aData.WireRefs,
                                aData.CoEdgeRefs,
                                aData.SolidRefs,
                                aData.ChildRefs,
                                aData.VertexRefs));
}

TEST_F(BRepGraphTest, BuildDelta_IndexesNewRefsOnExistingCompoundAndCompSolid)
{
  ReverseIndexInputData aData = buildReverseIndexBaseInput();

  // Pre-existing compound and compsolid parents (no refs yet).
  BRepGraphInc::CompoundDef& aCompound0 = aData.Compounds.Appended();
  aCompound0.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc::CompSolidDef& aCompSolid0 = aData.CompSolids.Appended();
  aCompSolid0.InitVectors(occ::handle<NCollection_BaseAllocator>());

  BRepGraphInc_ReverseIndex aRevIdx;
  aRevIdx.Build(aData.Vertices,
                aData.Edges,
                aData.CoEdges,
                aData.Wires,
                aData.Faces,
                aData.Shells,
                aData.Solids,
                aData.Compounds,
                aData.CompSolids,
                aData.ShellRefs,
                aData.FaceRefs,
                aData.WireRefs,
                aData.CoEdgeRefs,
                aData.SolidRefs,
                aData.ChildRefs,
                aData.VertexRefs);

  const int anOldNbEdges      = aData.Edges.Length();
  const int anOldNbWires      = aData.Wires.Length();
  const int anOldNbFaces      = aData.Faces.Length();
  const int anOldNbShells     = aData.Shells.Length();
  const int anOldNbSolids     = aData.Solids.Length();
  const int anOldNbCompounds  = aData.Compounds.Length();
  const int anOldNbCompSolids = aData.CompSolids.Length();
  const int anOldNbChildRefs  = aData.ChildRefs.Length();
  const int anOldNbSolidRefs  = aData.SolidRefs.Length();

  // Append refs under existing parents to validate parent-agnostic delta indexing.
  BRepGraphInc::ChildRef& aChildRef = aData.ChildRefs.Appended();
  aChildRef.ParentId                = BRepGraph_CompoundId::Start();
  aChildRef.ChildDefId              = BRepGraph_SolidId::Start();
  aCompound0.ChildRefIds.Append(BRepGraph_ChildRefId(aData.ChildRefs.Length() - 1));

  BRepGraphInc::SolidRef& aSolidRef = aData.SolidRefs.Appended();
  aSolidRef.ParentId                = BRepGraph_CompSolidId::Start();
  aSolidRef.SolidDefId              = BRepGraph_SolidId::Start();
  aCompSolid0.SolidRefIds.Append(BRepGraph_SolidRefId(aData.SolidRefs.Length() - 1));

  aRevIdx.BuildDelta(aData.Vertices,
                     aData.Edges,
                     aData.CoEdges,
                     aData.Wires,
                     aData.Faces,
                     aData.Shells,
                     aData.Solids,
                     aData.Compounds,
                     aData.CompSolids,
                     aData.ShellRefs,
                     aData.FaceRefs,
                     aData.WireRefs,
                     aData.CoEdgeRefs,
                     aData.SolidRefs,
                     aData.ChildRefs,
                     aData.VertexRefs,
                     anOldNbEdges,
                     anOldNbWires,
                     anOldNbFaces,
                     anOldNbShells,
                     anOldNbSolids,
                     anOldNbCompounds,
                     anOldNbCompSolids,
                     anOldNbChildRefs,
                     anOldNbSolidRefs);

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfSolid =
    aRevIdx.CompoundsOfSolid(BRepGraph_SolidId::Start());
  ASSERT_NE(aCompoundsOfSolid, nullptr);
  EXPECT_EQ(aCompoundsOfSolid->Length(), 1);
  EXPECT_EQ(aCompoundsOfSolid->Value(0), BRepGraph_CompoundId::Start());

  const NCollection_DynamicArray<BRepGraph_CompSolidId>* aCompSolidsOfSolid =
    aRevIdx.CompSolidsOfSolid(BRepGraph_SolidId::Start());
  ASSERT_NE(aCompSolidsOfSolid, nullptr);
  EXPECT_EQ(aCompSolidsOfSolid->Length(), 1);
  EXPECT_EQ(aCompSolidsOfSolid->Value(0), BRepGraph_CompSolidId::Start());

  EXPECT_TRUE(aRevIdx.Validate(aData.Vertices,
                               aData.Edges,
                               aData.CoEdges,
                               aData.Wires,
                               aData.Faces,
                               aData.Shells,
                               aData.Solids,
                               aData.Compounds,
                               aData.CompSolids,
                               aData.ShellRefs,
                               aData.FaceRefs,
                               aData.WireRefs,
                               aData.CoEdgeRefs,
                               aData.SolidRefs,
                               aData.ChildRefs,
                               aData.VertexRefs));
}

TEST_F(BRepGraphTest, ReverseIndex_CompoundOfAtomicKinds_WireEdgeVertex)
{
  // A TopoDS_Compound can legally hold atomic topology (wire / edge / vertex).
  // Before the Wave-2 fix, these ChildRefs were silently dropped from the
  // reverse index and CompoundsOfWire/Edge/Vertex returned empty.
  ReverseIndexInputData aData = buildReverseIndexBaseInput();

  // Ensure vertex slots exist for Vertex(0)/Vertex(1)/Vertex(2) that the atomic-compound refs
  // will target below.
  for (int i = 0; i < 3; ++i)
  {
    aData.Vertices.Appended();
  }

  // Parent compound that contains a Wire, an Edge, and a Vertex directly.
  BRepGraphInc::CompoundDef& aCompound = aData.Compounds.Appended();
  aCompound.InitVectors(occ::handle<NCollection_BaseAllocator>());

  auto appendAtomicChild = [&](const BRepGraph_NodeId::Kind theKind, const int theChildIdx) {
    BRepGraphInc::ChildRef& aRef = aData.ChildRefs.Appended();
    aRef.ParentId                = BRepGraph_CompoundId::Start();
    aRef.ChildDefId              = BRepGraph_NodeId(theKind, theChildIdx);
    aCompound.ChildRefIds.Append(BRepGraph_ChildRefId(aData.ChildRefs.Length() - 1));
  };

  appendAtomicChild(BRepGraph_NodeId::Kind::Wire, 0);   // Compound -> Wire(0)
  appendAtomicChild(BRepGraph_NodeId::Kind::Edge, 0);   // Compound -> Edge(0)
  appendAtomicChild(BRepGraph_NodeId::Kind::Vertex, 2); // Compound -> Vertex(2)

  BRepGraphInc_ReverseIndex aRevIdx;
  aRevIdx.Build(aData.Vertices,
                aData.Edges,
                aData.CoEdges,
                aData.Wires,
                aData.Faces,
                aData.Shells,
                aData.Solids,
                aData.Compounds,
                aData.CompSolids,
                aData.ShellRefs,
                aData.FaceRefs,
                aData.WireRefs,
                aData.CoEdgeRefs,
                aData.SolidRefs,
                aData.ChildRefs,
                aData.VertexRefs);

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfWire =
    aRevIdx.CompoundsOfWire(BRepGraph_WireId::Start());
  ASSERT_NE(aCompoundsOfWire, nullptr);
  EXPECT_EQ(aCompoundsOfWire->Length(), 1);
  EXPECT_EQ(aCompoundsOfWire->Value(0), BRepGraph_CompoundId::Start());

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfEdge =
    aRevIdx.CompoundsOfEdge(BRepGraph_EdgeId::Start());
  ASSERT_NE(aCompoundsOfEdge, nullptr);
  EXPECT_EQ(aCompoundsOfEdge->Length(), 1);
  EXPECT_EQ(aCompoundsOfEdge->Value(0), BRepGraph_CompoundId::Start());

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCompoundsOfVertex =
    aRevIdx.CompoundsOfVertex(BRepGraph_VertexId(2));
  ASSERT_NE(aCompoundsOfVertex, nullptr);
  EXPECT_EQ(aCompoundsOfVertex->Length(), 1);
  EXPECT_EQ(aCompoundsOfVertex->Value(0), BRepGraph_CompoundId::Start());

  EXPECT_TRUE(aRevIdx.Validate(aData.Vertices,
                               aData.Edges,
                               aData.CoEdges,
                               aData.Wires,
                               aData.Faces,
                               aData.Shells,
                               aData.Solids,
                               aData.Compounds,
                               aData.CompSolids,
                               aData.ShellRefs,
                               aData.FaceRefs,
                               aData.WireRefs,
                               aData.CoEdgeRefs,
                               aData.SolidRefs,
                               aData.ChildRefs,
                               aData.VertexRefs));
}

TEST_F(BRepGraphTest, Build_SimpleBox_IsDone)
{
  EXPECT_TRUE(myGraph.IsDone());
}

TEST_F(BRepGraphTest, BuildDelta_ValidateAndSkipRemoved_NullAllocator)
{
  verifyBuildDeltaScenario(occ::handle<NCollection_BaseAllocator>());
}

TEST_F(BRepGraphTest, BuildDelta_ValidateAndSkipRemoved_WithAllocator)
{
  const occ::handle<NCollection_IncAllocator> anAllocator = new NCollection_IncAllocator();
  verifyBuildDeltaScenario(anAllocator);
}

TEST_F(BRepGraphTest, Build_SimpleBox_CorrectCounts)
{
  EXPECT_EQ(myGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(myGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(myGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(myGraph.Topo().Wires().Nb(), 6);
  EXPECT_EQ(myGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), 8);
}

TEST_F(BRepGraphTest, Build_SimpleBox_SurfaceCount)
{
  EXPECT_EQ(myGraph.Topo().Faces().Nb(), 6);
}

TEST_F(BRepGraphTest, Face_Surface_IsValid)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceIt.CurrentId()))
      << "Face " << aFaceIt.CurrentId().Index << " has no surface rep";
  }
}

TEST_F(BRepGraphTest, Edge_CurveAndVertices_AreValid)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (!BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeId))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId))
        << "Edge " << anEdgeId.Index << " has no Curve3D rep";
    }
    EXPECT_TRUE(BRepGraph_Tool::Edge::StartVertexRef(myGraph, anEdgeId).VertexDefId.IsValid())
      << "Edge " << anEdgeId.Index << " has invalid StartVertexId";
    EXPECT_TRUE(BRepGraph_Tool::Edge::EndVertexRef(myGraph, anEdgeId).VertexDefId.IsValid())
      << "Edge " << anEdgeId.Index << " has invalid EndVertexId";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWire_Exists)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    EXPECT_TRUE(anOuterWire.IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has no outer wire";
  }
}

TEST_F(BRepGraphTest, FaceDef_HasValidSurface)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceIt.CurrentId()))
      << "Face " << aFaceIt.CurrentId().Index << " has no surface rep";
  }
}

TEST_F(BRepGraphTest, FindPCurve_ValidPair)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    if (!anOuterWire.IsValid())
      continue;
    const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
      BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, anOuterWire);
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aCoEdgeRefs)
    {
      const BRepGraphInc::CoEdgeRef& aCR     = myGraph.Refs().CoEdges().Entry(aCoEdgeRefId);
      const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, BRepGraph_EdgeId(aCoEdge.EdgeDefId)))
        continue;
      const BRepGraphInc::CoEdgeDef* aPCurveEntry =
        BRepGraph_Tool::Edge::FindPCurve(myGraph,
                                         BRepGraph_EdgeId(aCoEdge.EdgeDefId),
                                         aFaceIt.CurrentId());
      EXPECT_NE(aPCurveEntry, nullptr) << "Missing PCurve for edge " << aCoEdge.EdgeDefId.Index
                                       << " on face " << aFaceIt.CurrentId().Index;
    }
  }
}

TEST_F(BRepGraphTest, UID_Unique)
{
  NCollection_Map<BRepGraph_UID> aUIDSet;
  for (BRepGraph_SolidIterator aSolidIt(myGraph); aSolidIt.More(); aSolidIt.Next())
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aSolidIt.CurrentId()))));
  for (BRepGraph_ShellIterator aShellIt(myGraph); aShellIt.More(); aShellIt.Next())
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aShellIt.CurrentId()))));
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aFaceIt.CurrentId()))));
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aWireIt.CurrentId()))));
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(anEdgeIt.CurrentId()))));
  for (BRepGraph_VertexIterator aVertexIt(myGraph); aVertexIt.More(); aVertexIt.Next())
    EXPECT_TRUE(aUIDSet.Add(myGraph.UIDs().Of(BRepGraph_NodeId(aVertexIt.CurrentId()))));
  // Surface/Curve geometry UIDs are no longer separate nodes; geometry is stored inline on defs.
}

TEST_F(BRepGraphTest, UID_NodeIdRoundTrip)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_NodeId     aFaceId   = BRepGraph_NodeId(aFaceIt.CurrentId());
    const BRepGraph_UID& aUID      = myGraph.UIDs().Of(aFaceId);
    BRepGraph_NodeId     aResolved = myGraph.UIDs().NodeIdFrom(aUID);
    EXPECT_EQ(aResolved, aFaceId) << "Round trip failed for face " << aFaceIt.CurrentId().Index;
  }
}

TEST_F(BRepGraphTest, SameDomainFaces_Box_Empty)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_FaceId                           aFaceId = aFaceIt.CurrentId();
    NCollection_DynamicArray<BRepGraph_FaceId> aSameDomain =
      myGraph.Topo().Faces().SameDomain(aFaceId, myGraph.Allocator());
    EXPECT_EQ(aSameDomain.Length(), 0)
      << "Box face " << aFaceId.Index << " should have no same-domain faces";
  }
}

TEST_F(BRepGraphTest, Decompose_TwoSeparateFaces)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);

  TopExp_Explorer    anExp1(aBox1.Shape(), TopAbs_FACE);
  TopExp_Explorer    anExp2(aBox2.Shape(), TopAbs_FACE);
  const TopoDS_Face& aFace1 = TopoDS::Face(anExp1.Current());
  const TopoDS_Face& aFace2 = TopoDS::Face(anExp2.Current());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aFace1);
  aBuilder.Add(aCompound, aFace2);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 2);

  EXPECT_EQ(countFaceComponents(aGraph), 2);
}

TEST_F(BRepGraphTest, UserAttribute_SetGet)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  Handle(BRepGraph_TypedCacheValue<double>) anAttr = new BRepGraph_TypedCacheValue<double>(3.14);
  myGraph.Cache().Set(aFaceId, testDoubleAttrKind(), anAttr);

  occ::handle<BRepGraph_CacheValue> aRetrieved = myGraph.Cache().Get(aFaceId, testDoubleAttrKind());
  ASSERT_FALSE(aRetrieved.IsNull());

  Handle(BRepGraph_TypedCacheValue<double>) aTyped =
    Handle(BRepGraph_TypedCacheValue<double>)::DownCast(aRetrieved);
  ASSERT_FALSE(aTyped.IsNull());
  EXPECT_NEAR(aTyped->UncheckedValue(), 3.14, 1.0e-10);
}

TEST_F(BRepGraphTest, ReBuild_UIDMonotonic)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aBox);
  const uint32_t aGen1 = aGraph.UIDs().Generation();

  // Access a UID from the first build to verify it works.
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  const BRepGraph_NodeId aFirstFace(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_UID    aFirstUID = aGraph.UIDs().Of(aFirstFace);
  EXPECT_TRUE(aFirstUID.IsValid());
  EXPECT_EQ(aFirstUID.Generation(), aGen1);

  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, aBox);
  const uint32_t aGen2 = aGraph.UIDs().Generation();

  EXPECT_GT(aGen2, aGen1);

  // Verify all face UIDs in second build are valid and have new generation.
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_NodeId aNodeId(aFaceIt.CurrentId());
    BRepGraph_UID    aUID = aGraph.UIDs().Of(aNodeId);
    EXPECT_TRUE(aUID.IsValid()) << "Face " << aFaceIt.CurrentId().Index
                                << " should have a valid UID";
    EXPECT_EQ(aUID.Generation(), aGen2)
      << "Face " << aFaceIt.CurrentId().Index << " UID should have new generation";
  }

  // First build's UID should no longer be valid in the new generation.
  EXPECT_FALSE(aGraph.UIDs().Has(aFirstUID));
}

TEST_F(BRepGraphTest, DetectMissingPCurves_ValidBox_Empty)
{
  NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> aMissing(16);
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    for (BRepGraph_ChildExplorer anEdgeExp(myGraph, aFaceId, BRepGraph_NodeId::Kind::Edge);
         anEdgeExp.More();
         anEdgeExp.Next())
    {
      const BRepGraph_EdgeId anEdgeId     = BRepGraph_EdgeId::FromNodeId(anEdgeExp.Current().DefId);
      const BRepGraphInc::EdgeDef& anEdge = myGraph.Topo().Edges().Definition(anEdgeId);
      if (anEdge.IsDegenerate)
      {
        continue;
      }

      if (BRepGraph_Tool::Edge::FindPCurve(myGraph, anEdgeId, aFaceId) == nullptr)
      {
        aMissing.Append(std::make_pair(anEdgeId, aFaceId));
      }
    }
  }
  EXPECT_EQ(aMissing.Length(), 0);
}

TEST_F(BRepGraphTest, DetectDegenerateWires_ValidBox_Empty)
{
  NCollection_DynamicArray<BRepGraph_WireId> aDegenerate(16);
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId  = aWireIt.CurrentId();
    int                    aNbEdges = 0;
    for (BRepGraph_ChildExplorer anEdgeExp(myGraph, aWireId, BRepGraph_NodeId::Kind::Edge);
         anEdgeExp.More();
         anEdgeExp.Next())
    {
      ++aNbEdges;
    }

    if (aNbEdges < 2)
    {
      aDegenerate.Append(aWireId);
      continue;
    }

    bool isOuterWire = false;
    for (BRepGraph_ParentExplorer aFaceExp(myGraph, aWireId, BRepGraph_NodeId::Kind::Face);
         aFaceExp.More();
         aFaceExp.Next())
    {
      const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::FromNodeId(aFaceExp.Current().DefId);
      if (myGraph.Topo().Faces().OuterWire(aFaceId) == aWireId)
      {
        isOuterWire = true;
        break;
      }
    }

    if (isOuterWire && !aWireIt.Current().IsClosed)
    {
      aDegenerate.Append(aWireId);
    }
  }
  EXPECT_EQ(aDegenerate.Length(), 0);
}

TEST_F(BRepGraphTest, MutableEdge_ModifyTolerance)
{
  double anOrigTol = BRepGraph_Tool::Edge::Tolerance(myGraph, BRepGraph_EdgeId::Start());
  BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
    myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start());
  myGraph.Editor().Edges().SetTolerance(anEdge, anOrigTol * 2.0);
  EXPECT_NEAR(BRepGraph_Tool::Edge::Tolerance(myGraph, BRepGraph_EdgeId::Start()),
              anOrigTol * 2.0,
              1.0e-15);
}

TEST_F(BRepGraphTest, NbFacesOfEdge_SharedEdge)
{
  // In a box, each non-degenerate edge is shared by exactly 2 faces.
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (!BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeIt.CurrentId()))
    {
      const uint32_t aCount = myGraph.Topo().Edges().NbFaces(anEdgeIt.CurrentId());
      EXPECT_EQ(aCount, 2u) << "Edge " << anEdgeIt.CurrentId().Index
                            << " should be shared by 2 faces";
    }
  }
}

TEST_F(BRepGraphTest, FreeEdges_ClosedBox_Empty)
{
  NCollection_DynamicArray<BRepGraph_EdgeId> aFree = collectFreeEdges(myGraph);
  EXPECT_EQ(aFree.Length(), 0);
}

TEST_F(BRepGraphTest, RecordHistory_BasicEntry)
{
  size_t                                     aBefore = myGraph.History().NbRecords();
  BRepGraph_NodeId                           anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                           anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_DynamicArray<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("TestOp", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore + 1);
  EXPECT_TRUE(myGraph.History().Record(aBefore).OperationName.IsEqual("TestOp"));
}

TEST_F(BRepGraphTest, ReplaceEdge_Substitution)
{
  // Get the first wire and its first edge via incidence refs.
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefs.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aOldCR = myGraph.Refs().CoEdges().Entry(aCoEdgeRefs.Value(0));
  const BRepGraphInc::CoEdgeDef& anOldCoEdge =
    myGraph.Topo().CoEdges().Definition(aOldCR.CoEdgeDefId);
  const BRepGraph_EdgeId anOldEdgeId = anOldCoEdge.EdgeDefId;

  // Pick a different edge to substitute.
  const BRepGraph_EdgeId aNewEdgeId((anOldEdgeId.Index + 1) % myGraph.Topo().Edges().Nb());

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, false);

  // Verify the substitution via the updated incidence refs.
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefsAfter =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefsAfter.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aNewCR = myGraph.Refs().CoEdges().Entry(aCoEdgeRefsAfter.Value(0));
  const BRepGraphInc::CoEdgeDef& aNewCoEdge =
    myGraph.Topo().CoEdges().Definition(aNewCR.CoEdgeDefId);
  EXPECT_EQ(aNewCoEdge.EdgeDefId.Index, aNewEdgeId.Index);
}

TEST_F(BRepGraphTest, ParallelBuild_SameAsSequential)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aSeqGraph;
  aSeqGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aSeqGraph, aBox, BRepGraph_Builder::Options{{}, true, false, false});
  ASSERT_TRUE(aSeqGraph.IsDone());

  BRepGraph aParGraph;
  aParGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes6 =
    BRepGraph_Builder::Add(aParGraph, aBox, BRepGraph_Builder::Options{{}, true, false, true});
  ASSERT_TRUE(aParGraph.IsDone());

  EXPECT_EQ(aParGraph.Topo().Solids().Nb(), aSeqGraph.Topo().Solids().Nb());
  EXPECT_EQ(aParGraph.Topo().Shells().Nb(), aSeqGraph.Topo().Shells().Nb());
  EXPECT_EQ(aParGraph.Topo().Faces().Nb(), aSeqGraph.Topo().Faces().Nb());
  EXPECT_EQ(aParGraph.Topo().Wires().Nb(), aSeqGraph.Topo().Wires().Nb());
  EXPECT_EQ(aParGraph.Topo().Edges().Nb(), aSeqGraph.Topo().Edges().Nb());
  EXPECT_EQ(aParGraph.Topo().Vertices().Nb(), aSeqGraph.Topo().Vertices().Nb());
  EXPECT_EQ(aParGraph.Topo().Faces().Nb(), aSeqGraph.Topo().Faces().Nb());
  EXPECT_EQ(aParGraph.Topo().Edges().Nb(), aSeqGraph.Topo().Edges().Nb());
}

TEST_F(BRepGraphTest, ParallelBuild_CompoundOfFaces)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (TopExp_Explorer anExp(aBox1.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
    aBuilder.Add(aCompound, anExp.Current());
  for (TopExp_Explorer anExp(aBox2.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
    aBuilder.Add(aCompound, anExp.Current());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes7 =
    BRepGraph_Builder::Add(aGraph, aCompound, BRepGraph_Builder::Options{{}, true, false, true});
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 12);
}

// ===================================================================
// Group 1: Shape Round-Trip
// ===================================================================

TEST_F(BRepGraphTest, ReconstructFace_EachBoxFace_SameSubShapeCounts)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_NodeId    aFaceId        = BRepGraph_NodeId(aFaceIt.CurrentId());
    const TopoDS_Shape& anOrigFace     = myGraph.Shapes().OriginalOf(aFaceId);
    const TopoDS_Shape  aReconstructed = myGraph.Shapes().Reconstruct(aFaceIt.CurrentId());

    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anOrigVerts, anOrigEdges,
      anOrigWires;
    TopExp::MapShapes(anOrigFace, TopAbs_VERTEX, anOrigVerts);
    TopExp::MapShapes(anOrigFace, TopAbs_EDGE, anOrigEdges);
    TopExp::MapShapes(anOrigFace, TopAbs_WIRE, anOrigWires);

    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aReconVerts, aReconEdges,
      aReconWires;
    TopExp::MapShapes(aReconstructed, TopAbs_VERTEX, aReconVerts);
    TopExp::MapShapes(aReconstructed, TopAbs_EDGE, aReconEdges);
    TopExp::MapShapes(aReconstructed, TopAbs_WIRE, aReconWires);

    EXPECT_EQ(aReconVerts.Extent(), anOrigVerts.Extent())
      << "Vertex count mismatch for face " << aFaceIt.CurrentId().Index;
    EXPECT_EQ(aReconEdges.Extent(), anOrigEdges.Extent())
      << "Edge count mismatch for face " << aFaceIt.CurrentId().Index;
    EXPECT_EQ(aReconWires.Extent(), anOrigWires.Extent())
      << "Wire count mismatch for face " << aFaceIt.CurrentId().Index;

    // Verify same surface handle.
    const TopoDS_Face&               anOrigF = TopoDS::Face(anOrigFace);
    const TopoDS_Face&               aReconF = TopoDS::Face(aReconstructed);
    TopLoc_Location                  aLoc1, aLoc2;
    const occ::handle<Geom_Surface>& aSurf1 = BRep_Tool::Surface(anOrigF, aLoc1);
    const occ::handle<Geom_Surface>& aSurf2 = BRep_Tool::Surface(aReconF, aLoc2);
    EXPECT_EQ(aSurf1.get(), aSurf2.get())
      << "Surface handle differs for face " << aFaceIt.CurrentId().Index;
  }
}

TEST_F(BRepGraphTest, ReconstructFace_AfterEdgeReplace_ContainsNewEdge)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefs.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aCR0 = myGraph.Refs().CoEdges().Entry(aCoEdgeRefs.Value(0));
  const BRepGraph_EdgeId         anOldEdgeId =
    myGraph.Topo().CoEdges().Definition(aCR0.CoEdgeDefId).EdgeDefId;

  // Pick a different edge.
  const int              aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Topo().Edges().Nb();
  const BRepGraph_EdgeId aNewEdgeId(aNewIdx);

  // Get 3D curve handles from graph for old/new edges.
  occ::handle<Geom_Curve> aNewCurve =
    BRepGraph_Tool::Edge::Curve(myGraph, BRepGraph_EdgeId(aNewIdx));
  occ::handle<Geom_Curve> anOldCurve = BRepGraph_Tool::Edge::Curve(myGraph, anOldEdgeId);

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, false);

  // Reconstruct face 0 (the face owning wire 0).
  const int aFaceIdx =
    BRepGraph_TestTools::FaceUsesWire(myGraph, BRepGraph_FaceId::Start(), BRepGraph_WireId::Start())
      ? 0
      : -1;
  ASSERT_GE(aFaceIdx, 0);
  const TopoDS_Shape aReconstructed = myGraph.Shapes().Reconstruct(BRepGraph_FaceId(aFaceIdx));

  // Check via 3D curve handle identity (reconstructed edges have new TShapes).
  bool isNewFound = false;
  bool isOldFound = false;
  for (TopExp_Explorer anExp(aReconstructed, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopLoc_Location         aLoc;
    double                  aFirst, aLast;
    occ::handle<Geom_Curve> aCurve =
      BRep_Tool::Curve(TopoDS::Edge(anExp.Current()), aLoc, aFirst, aLast);
    if (!aCurve.IsNull() && !aNewCurve.IsNull() && aCurve.get() == aNewCurve.get())
      isNewFound = true;
    if (!aCurve.IsNull() && !anOldCurve.IsNull() && aCurve.get() == anOldCurve.get())
      isOldFound = true;
  }
  EXPECT_TRUE(isNewFound) << "New edge curve not found in reconstructed face";
  EXPECT_FALSE(isOldFound) << "Old edge curve still present in reconstructed face";
}

TEST_F(BRepGraphTest, ReconstructShape_SolidRoot_SameFaceCount)
{
  BRepGraph_NodeId   aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const TopoDS_Shape aReconstructed = myGraph.Shapes().Reconstruct(aSolidId);

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aReconstructed, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);
}

TEST_F(BRepGraphTest, ReconstructShape_FaceRoot_ReturnsSameShape)
{
  BRepGraph_NodeId    aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const TopoDS_Shape  aReconstructed = myGraph.Shapes().Reconstruct(aFaceId);
  const TopoDS_Shape& anOriginal     = myGraph.Shapes().OriginalOf(aFaceId);

  // Reconstructed face should have the same surface handle.
  const TopoDS_Face&               anOrigF = TopoDS::Face(anOriginal);
  const TopoDS_Face&               aReconF = TopoDS::Face(aReconstructed);
  TopLoc_Location                  aLoc1, aLoc2;
  const occ::handle<Geom_Surface>& aSurf1 = BRep_Tool::Surface(anOrigF, aLoc1);
  const occ::handle<Geom_Surface>& aSurf2 = BRep_Tool::Surface(aReconF, aLoc2);
  EXPECT_EQ(aSurf1.get(), aSurf2.get());
}

TEST_F(BRepGraphTest, Shape_Unmodified_ReturnsSameShape)
{
  BRepGraph_NodeId    aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  TopoDS_Shape        aShape = myGraph.Shapes().Shape(aFaceId);
  const TopoDS_Shape& anOrig = myGraph.Shapes().OriginalOf(aFaceId);
  EXPECT_TRUE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_AfterReplaceEdge_DiffersFromOriginal)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefs.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aCR0 = myGraph.Refs().CoEdges().Entry(aCoEdgeRefs.Value(0));
  const BRepGraph_EdgeId         anOldEdgeId =
    myGraph.Topo().CoEdges().Definition(aCR0.CoEdgeDefId).EdgeDefId;
  const int              aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Topo().Edges().Nb();
  const BRepGraph_EdgeId aNewEdgeId(aNewIdx);

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, false);

  // Find the face that owns wire 0.
  int aFaceDefIdx = -1;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    if (BRepGraph_TestTools::FaceUsesWire(myGraph, aFaceIt.CurrentId(), BRepGraph_WireId::Start()))
    {
      aFaceDefIdx = aFaceIt.CurrentId().Index;
      break;
    }
  }
  ASSERT_GE(aFaceDefIdx, 0);
  BRepGraph_NodeId    aFaceId(BRepGraph_NodeId::Kind::Face, aFaceDefIdx);
  TopoDS_Shape        aShape = myGraph.Shapes().Shape(aFaceId);
  const TopoDS_Shape& anOrig = myGraph.Shapes().OriginalOf(aFaceId);
  EXPECT_FALSE(aShape.IsSame(anOrig));
}

TEST_F(BRepGraphTest, Shape_WireKind_Valid)
{
  BRepGraph_NodeId aWireId(BRepGraph_NodeId::Kind::Wire, 0);
  TopoDS_Shape     aShape = myGraph.Shapes().Shape(aWireId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_WIRE);
}

TEST_F(BRepGraphTest, Shape_EdgeKind_Valid)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  TopoDS_Shape     aShape = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_EDGE);
}

TEST_F(BRepGraphTest, Shape_VertexKind_Valid)
{
  BRepGraph_NodeId aVtxId(BRepGraph_NodeId::Kind::Vertex, 0);
  TopoDS_Shape     aShape = myGraph.Shapes().Shape(aVtxId);
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(aShape.ShapeType(), TopAbs_VERTEX);
}

TEST_F(BRepGraphTest, OwnGen_MutableEdge_PropagatesSubtreeGenUp)
{
  EXPECT_EQ(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);

  myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start()).MarkDirty();

  // Edge was directly mutated: OwnGen incremented.
  EXPECT_GT(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);

  // Check propagation up to parent wire and face (SubtreeGen).
  if (BRepGraph_EdgeId::Start().IsValid(myGraph.Topo().Edges().Nb()))
  {
    // Find a wire containing this edge.
    const NCollection_DynamicArray<BRepGraph_WireId>& aWires =
      myGraph.Topo().Edges().Wires(BRepGraph_EdgeId::Start());
    if (aWires.Length() > 0)
    {
      EXPECT_GT(myGraph.Topo().Wires().Definition(aWires.Value(0)).SubtreeGen, 0u);
      // Check propagation to owning face.
      for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
      {
        if (BRepGraph_TestTools::FaceUsesWire(myGraph, aFaceIt.CurrentId(), aWires.Value(0)))
        {
          EXPECT_GT(myGraph.Topo().Faces().Definition(aFaceIt.CurrentId()).SubtreeGen, 0u);
          break;
        }
      }
    }
  }
}

TEST_F(BRepGraphTest, ReconstructShape_WireKind_NoThrow)
{
  BRepGraph_NodeId aWireId(BRepGraph_NodeId::Kind::Wire, 0);
  TopoDS_Shape     aShape;
  EXPECT_NO_THROW(aShape = myGraph.Shapes().Reconstruct(aWireId));
  EXPECT_FALSE(aShape.IsNull());
}

TEST_F(BRepGraphTest, HasOriginalShape_AfterBuild_True)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    BRepGraph_NodeId aFaceId(aFaceIt.CurrentId());
    EXPECT_TRUE(myGraph.Shapes().HasOriginal(aFaceId))
      << "Face " << aFaceIt.CurrentId().Index
      << " should have original shape after BRepGraph_Builder::Add()";
  }
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    BRepGraph_NodeId anEdgeId(anEdgeIt.CurrentId());
    EXPECT_TRUE(myGraph.Shapes().HasOriginal(anEdgeId))
      << "Edge " << anEdgeIt.CurrentId().Index
      << " should have original shape after BRepGraph_Builder::Add()";
  }
}

TEST_F(BRepGraphTest, Shape_CachedOnSecondCall)
{
  BRepGraph_NodeId aVtxId(BRepGraph_NodeId::Kind::Vertex, 0);
  TopoDS_Shape     aFirst  = myGraph.Shapes().Shape(aVtxId);
  TopoDS_Shape     aSecond = myGraph.Shapes().Shape(aVtxId);
  EXPECT_TRUE(aFirst.IsSame(aSecond));
}

TEST_F(BRepGraphTest, Shape_InvalidatedAfterMutation)
{
  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  TopoDS_Shape     aBefore = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(aBefore.IsNull());

  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.123);
  TopoDS_Shape anAfter = myGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(anAfter.IsNull());

  // After mutation, Shape() reconstructs a new edge - different TShape.
  EXPECT_FALSE(aBefore.IsSame(anAfter));
}

TEST_F(BRepGraphTest, DefaultBuild_AssignsValidUIDs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes8 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());

  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_UID    aUID = aGraph.UIDs().Of(aFaceId);

  EXPECT_TRUE(aUID.IsValid());
  EXPECT_TRUE(aGraph.UIDs().Has(aUID));
  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aUID), aFaceId);
}

TEST_F(BRepGraphTest, UIDsGeneration_IncrementsAcrossBuilds)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(11.0, 21.0, 31.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes9 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker1.Shape());
  const uint32_t aGeneration1 = aGraph.UIDs().Generation();

  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes10 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker2.Shape());
  const uint32_t aGeneration2 = aGraph.UIDs().Generation();

  EXPECT_GT(aGeneration1, 0u);
  EXPECT_EQ(aGeneration2, aGeneration1 + 1);
}

TEST_F(BRepGraphTest, StaleUID_HasReturnsFalseAfterRebuild)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(11.0, 21.0, 31.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes11 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker1.Shape());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  const BRepGraph_UID anOldUID =
    aGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  ASSERT_TRUE(anOldUID.IsValid());
  ASSERT_TRUE(aGraph.UIDs().Has(anOldUID));

  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes12 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker2.Shape());

  EXPECT_FALSE(aGraph.UIDs().Has(anOldUID));
  EXPECT_FALSE(aGraph.UIDs().NodeIdFrom(anOldUID).IsValid());
}

TEST(BRepGraph_UIDsViewTest, ReverseLookupStaysCurrentAfterProgrammaticAdd)
{
  BRepGraph aGraph;

  const BRepGraph_VertexId aFirstVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 0.001);
  const BRepGraph_UID aFirstUID = aGraph.UIDs().Of(aFirstVertex);
  ASSERT_TRUE(aFirstUID.IsValid());
  ASSERT_EQ(aGraph.UIDs().NodeIdFrom(aFirstUID), aFirstVertex);

  const BRepGraph_VertexId aSecondVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 0.001);
  const BRepGraph_UID aSecondUID = aGraph.UIDs().Of(aSecondVertex);
  ASSERT_TRUE(aSecondUID.IsValid());

  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aFirstUID), aFirstVertex);
  EXPECT_EQ(aGraph.UIDs().NodeIdFrom(aSecondUID), aSecondVertex);
  EXPECT_TRUE(aGraph.UIDs().Has(aFirstUID));
  EXPECT_TRUE(aGraph.UIDs().Has(aSecondUID));
}

// ===================================================================
// Group 2: History Tracking
// ===================================================================

TEST_F(BRepGraphTest, RecordHistory_MultipleRecords_SequenceNumbers)
{
  const size_t aBefore = myGraph.History().NbRecords();

  BRepGraph_NodeId                           anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                           anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_DynamicArray<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);

  myGraph.History().Record("OpA", anEdge0, aRepl);
  myGraph.History().Record("OpB", anEdge0, aRepl);
  myGraph.History().Record("OpC", anEdge0, aRepl);

  EXPECT_EQ(myGraph.History().NbRecords(), aBefore + 3);

  // Check monotonically increasing sequence numbers.
  for (size_t anIdx = aBefore + 1; anIdx < aBefore + 3; ++anIdx)
  {
    EXPECT_GT(myGraph.History().Record(anIdx).SequenceNumber,
              myGraph.History().Record(anIdx - 1).SequenceNumber)
      << "SequenceNumber not monotonically increasing at index " << anIdx;
  }

  EXPECT_TRUE(myGraph.History().Record(aBefore).OperationName.IsEqual("OpA"));
  EXPECT_TRUE(myGraph.History().Record(aBefore + 1).OperationName.IsEqual("OpB"));
  EXPECT_TRUE(myGraph.History().Record(aBefore + 2).OperationName.IsEqual("OpC"));
}

TEST_F(BRepGraphTest, FindOriginal_SingleHop_ReturnsSource)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_DynamicArray<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier, "TestHop");

  BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(anEdge1);
  EXPECT_EQ(anOriginal, anEdge0);
}

TEST_F(BRepGraphTest, FindDerived_SingleHop_ContainsTarget)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_DynamicArray<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier, "TestHop");

  NCollection_DynamicArray<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdge0);
  bool                                       isFound  = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived)
  {
    if (aDerivedId == anEdge1)
    {
      isFound = true;
      break;
    }
  }
  EXPECT_TRUE(isFound) << "Edge1 not found in FindDerived(Edge0)";
}

TEST_F(BRepGraphTest, ApplyModification_MultiStepChain_FindOriginalTracesBack)
{
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  BRepGraph_NodeId anEdge2(BRepGraph_NodeId::Kind::Edge, 2);

  // Step 1: edge0 -> edge1
  auto aModifier1 = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_DynamicArray<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };
  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier1, "Step1");

  // Step 2: edge1 -> edge2
  auto aModifier2 = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_DynamicArray<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge2);
    return aResult;
  };
  myGraph.Editor().Gen().ApplyModification(anEdge1, aModifier2, "Step2");

  // FindOriginal from edge2 should trace back to edge0.
  BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(anEdge2);
  EXPECT_EQ(anOriginal, anEdge0);

  // FindDerived from edge0 returns leaf-only transitive descendants.
  // edge1 is an intermediate (it has further derived edge2), so only edge2 is returned.
  NCollection_DynamicArray<BRepGraph_NodeId> aDerived     = myGraph.History().FindDerived(anEdge0);
  bool                                       isEdge2Found = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived)
  {
    if (aDerivedId == anEdge2)
      isEdge2Found = true;
  }
  EXPECT_TRUE(isEdge2Found) << "Edge2 not found in FindDerived(Edge0)";

  // edge1 can be found by querying FindDerived on the intermediate step.
  NCollection_DynamicArray<BRepGraph_NodeId> aDerived1 = myGraph.History().FindDerived(anEdge1);
  bool                                       isEdge2FromEdge1 = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived1)
  {
    if (aDerivedId == anEdge2)
      isEdge2FromEdge1 = true;
  }
  EXPECT_TRUE(isEdge2FromEdge1) << "Edge2 not found in FindDerived(Edge1)";
}

// ===================================================================
// Group 3: Mutation APIs
// ===================================================================

TEST_F(BRepGraphTest, AddPCurve_NewPCurve_RetrievableViaFindPCurve)
{
  BRepGraph_EdgeId anEdgeId(0);
  BRepGraph_FaceId aFaceId(0);

  occ::handle<Geom2d_Line> aCurve2d = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  myGraph.Editor().CoEdges().AddPCurve(anEdgeId, aFaceId, aCurve2d, 0.0, 1.0);

  const BRepGraphInc::CoEdgeDef* aRetrieved =
    BRepGraph_Tool::Edge::FindPCurve(myGraph, anEdgeId, aFaceId);
  EXPECT_NE(aRetrieved, nullptr);
  if (aRetrieved != nullptr)
  {
    EXPECT_TRUE(aRetrieved->Curve2DRepId.IsValid());
  }
}

TEST_F(BRepGraphTest, ReplaceEdge_Reversed_OrientationFlipped)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefs.Length(), 1);

  const BRepGraphInc::CoEdgeRef& anOrigCR = myGraph.Refs().CoEdges().Entry(aCoEdgeRefs.Value(0));
  const BRepGraphInc::CoEdgeDef& anOrigCoEdge =
    myGraph.Topo().CoEdges().Definition(anOrigCR.CoEdgeDefId);
  const BRepGraph_EdgeId anOldEdgeId       = anOrigCoEdge.EdgeDefId;
  TopAbs_Orientation     anOrigOrientation = anOrigCoEdge.Orientation;

  // Pick a different edge.
  const int              aNewIdx = (anOldEdgeId.Index + 1) % myGraph.Topo().Edges().Nb();
  const BRepGraph_EdgeId aNewEdgeId(aNewIdx);

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, true);

  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefsAfter =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefsAfter.Length(), 1);
  const BRepGraphInc::CoEdgeRef& aNewCR = myGraph.Refs().CoEdges().Entry(aCoEdgeRefsAfter.Value(0));
  const BRepGraphInc::CoEdgeDef& aNewCoEdge =
    myGraph.Topo().CoEdges().Definition(aNewCR.CoEdgeDefId);
  EXPECT_EQ(aNewCoEdge.EdgeDefId.Index, aNewEdgeId.Index);

  // Orientation should be flipped relative to original.
  TopAbs_Orientation anExpected =
    (anOrigOrientation == TopAbs_FORWARD) ? TopAbs_REVERSED : TopAbs_FORWARD;
  EXPECT_EQ(aNewCoEdge.Orientation, anExpected);
}

TEST_F(BRepGraphTest, ReplaceEdge_UpdatesEdgeToCoEdgeReverseIndex)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefs.Length(), 1);

  const BRepGraphInc::CoEdgeRef& aRef      = myGraph.Refs().CoEdges().Entry(aCoEdgeRefs.Value(0));
  const BRepGraph_CoEdgeId       aCoEdgeId = aRef.CoEdgeDefId;
  const BRepGraph_EdgeId anOldEdgeId = myGraph.Topo().CoEdges().Definition(aCoEdgeId).EdgeDefId;
  const BRepGraph_EdgeId aNewEdgeId((anOldEdgeId.Index + 1) % myGraph.Topo().Edges().Nb());

  auto hasCoEdge = [&](const BRepGraph_EdgeId theEdgeId) {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
      myGraph.Topo().Edges().CoEdges(theEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdge : aCoEdges)
    {
      if (aCoEdge == aCoEdgeId)
      {
        return true;
      }
    }
    return false;
  };

  ASSERT_TRUE(hasCoEdge(anOldEdgeId));

  myGraph.Editor().Wires().ReplaceEdge(BRepGraph_WireId::Start(), anOldEdgeId, aNewEdgeId, false);

  EXPECT_FALSE(hasCoEdge(anOldEdgeId));
  EXPECT_TRUE(hasCoEdge(aNewEdgeId));
}

TEST_F(BRepGraphTest, RemoveCoEdge_PrunesOrphanAndKeepsReverseIndexConsistent)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefsBefore =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefsBefore.Length(), 1);

  const BRepGraphInc::CoEdgeRef& aRef = myGraph.Refs().CoEdges().Entry(aCoEdgeRefsBefore.Value(0));
  const BRepGraph_CoEdgeRefId    aCoEdgeRefId = aCoEdgeRefsBefore.Value(0);
  const BRepGraph_CoEdgeId       aCoEdgeId    = aRef.CoEdgeDefId;
  const BRepGraph_EdgeId anEdgeId = myGraph.Topo().CoEdges().Definition(aCoEdgeId).EdgeDefId;

  ASSERT_TRUE(myGraph.Editor().Wires().RemoveCoEdge(BRepGraph_WireId::Start(), aCoEdgeRefId));

  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefsAfter =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  EXPECT_EQ(aCoEdgeRefsAfter.Length(), aCoEdgeRefsBefore.Length() - 1);
  EXPECT_TRUE(myGraph.Topo().CoEdges().Definition(aCoEdgeId).IsRemoved);
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());

  for (const BRepGraph_WireId& aWireId : myGraph.Topo().Edges().Wires(anEdgeId))
  {
    EXPECT_NE(aWireId, BRepGraph_WireId::Start());
  }

  for (const BRepGraph_CoEdgeId& aEdgeCoEdgeId : myGraph.Topo().Edges().CoEdges(anEdgeId))
  {
    EXPECT_NE(aEdgeCoEdgeId, aCoEdgeId);
  }
}

TEST_F(BRepGraphTest, RemoveChild_PreservesSharedChildAndRebuildsReverseIndex)
{
  NCollection_DynamicArray<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  const BRepGraph_CompoundId aCompoundId = myGraph.Editor().Compounds().Add(aChildren);
  ASSERT_TRUE(aCompoundId.IsValid());

  const NCollection_DynamicArray<BRepGraph_ChildRefId> aChildRefsBefore =
    BRepGraph_TestTools::ChildRefsOfParent(myGraph, BRepGraph_NodeId(aCompoundId));
  ASSERT_EQ(aChildRefsBefore.Length(), 1);

  const BRepGraph_ChildRefId    aChildRefId = aChildRefsBefore.Value(0);
  const BRepGraphInc::ChildRef& aRef        = myGraph.Refs().Children().Entry(aChildRefId);
  const BRepGraph_NodeId        aChildId    = aRef.ChildDefId;

  ASSERT_TRUE(myGraph.Editor().Compounds().RemoveChild(aCompoundId, aChildRefId));

  EXPECT_EQ(BRepGraph_TestTools::CountChildRefsOfParent(myGraph, BRepGraph_NodeId(aCompoundId)), 0);
  EXPECT_FALSE(myGraph.Topo().Gen().IsRemoved(aChildId));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
  EXPECT_EQ(myGraph.Topo().Compounds().ParentCompounds(aCompoundId).Length(), 0);
}

TEST_F(BRepGraphTest, RemoveChild_RemovesAuxChildUsage)
{
  const BRepGraph_ChildRefId aChildRefId =
    myGraph.Editor().Shells().AddChild(BRepGraph_ShellId::Start(),
                                       BRepGraph_NodeId(BRepGraph_WireId::Start()));
  ASSERT_TRUE(aChildRefId.IsValid());
  ASSERT_EQ(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).AuxChildRefIds.Length(),
            1);

  ASSERT_TRUE(myGraph.Editor().Shells().RemoveChild(BRepGraph_ShellId::Start(), aChildRefId));

  EXPECT_EQ(myGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).AuxChildRefIds.Length(),
            0);
  EXPECT_FALSE(myGraph.Topo().Gen().IsRemoved(BRepGraph_WireId::Start()));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveChild_RemovesAuxChildUsageFromSolid)
{
  const BRepGraph_ChildRefId aChildRefId =
    myGraph.Editor().Solids().AddChild(BRepGraph_SolidId::Start(),
                                       BRepGraph_NodeId(BRepGraph_EdgeId::Start()));
  ASSERT_TRUE(aChildRefId.IsValid());
  ASSERT_EQ(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).AuxChildRefIds.Length(),
            1);

  ASSERT_TRUE(myGraph.Editor().Solids().RemoveChild(BRepGraph_SolidId::Start(), aChildRefId));

  EXPECT_EQ(myGraph.Topo().Solids().Definition(BRepGraph_SolidId::Start()).AuxChildRefIds.Length(),
            0);
  EXPECT_FALSE(myGraph.Topo().Gen().IsRemoved(BRepGraph_EdgeId::Start()));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveFace_PrunesOrphanAndRebuildsReverseIndex)
{
  const NCollection_DynamicArray<BRepGraph_FaceRefId> aFaceRefsBefore =
    BRepGraph_TestTools::FaceRefsOfShell(myGraph, BRepGraph_ShellId::Start());
  ASSERT_GE(aFaceRefsBefore.Length(), 1);

  const BRepGraph_FaceRefId    aFaceRefId = aFaceRefsBefore.Value(0);
  const BRepGraphInc::FaceRef& aRef       = myGraph.Refs().Faces().Entry(aFaceRefId);
  const BRepGraph_FaceId       aFaceId    = aRef.FaceDefId;

  ASSERT_TRUE(myGraph.Editor().Shells().RemoveFace(BRepGraph_ShellId::Start(), aFaceRefId));

  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(myGraph, BRepGraph_ShellId::Start()),
            aFaceRefsBefore.Length() - 1);
  EXPECT_TRUE(myGraph.Topo().Faces().Definition(aFaceId).IsRemoved);
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());

  for (const BRepGraph_SolidId& aSolidId :
       myGraph.Topo().Shells().Solids(BRepGraph_ShellId::Start()))
  {
    EXPECT_NE(aSolidId, BRepGraph_SolidId());
  }

  for (const BRepGraph_ShellId& aShellId : myGraph.Topo().Faces().Shells(aFaceId))
  {
    EXPECT_NE(aShellId, BRepGraph_ShellId::Start());
  }
}

TEST_F(BRepGraphTest, RemoveShell_PrunesOrphanAndRebuildsReverseIndex)
{
  const NCollection_DynamicArray<BRepGraph_ShellRefId> aShellRefsBefore =
    BRepGraph_TestTools::ShellRefsOfSolid(myGraph, BRepGraph_SolidId::Start());
  ASSERT_GE(aShellRefsBefore.Length(), 1);

  const BRepGraph_ShellRefId    aShellRefId = aShellRefsBefore.Value(0);
  const BRepGraphInc::ShellRef& aRef        = myGraph.Refs().Shells().Entry(aShellRefId);
  const BRepGraph_ShellId       aShellId    = aRef.ShellDefId;

  ASSERT_TRUE(myGraph.Editor().Solids().RemoveShell(BRepGraph_SolidId::Start(), aShellRefId));

  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(myGraph, BRepGraph_SolidId::Start()),
            aShellRefsBefore.Length() - 1);
  EXPECT_TRUE(myGraph.Topo().Shells().Definition(aShellId).IsRemoved);
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());

  for (const BRepGraph_CompSolidId& aCompSolidId :
       myGraph.Topo().Solids().CompSolids(BRepGraph_SolidId::Start()))
  {
    EXPECT_NE(aCompSolidId, BRepGraph_CompSolidId());
  }

  for (const BRepGraph_SolidId& aSolidId : myGraph.Topo().Shells().Solids(aShellId))
  {
    EXPECT_NE(aSolidId, BRepGraph_SolidId::Start());
  }
}

TEST_F(BRepGraphTest, RemoveWire_PrunesOrphanAndRebuildsReverseIndex)
{
  BRepGraph_FaceId    aFaceId;
  BRepGraph_WireRefId aWireRefId;
  bool                isFound = false;
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More() && !isFound; aFaceIt.Next())
  {
    const BRepGraph_FaceId                              aCandidateFaceId = aFaceIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_WireRefId> aWireRefs =
      BRepGraph_TestTools::WireRefsOfFace(myGraph, aCandidateFaceId);
    for (const BRepGraph_WireRefId& aCandidateWireRefId : aWireRefs)
    {
      if (myGraph.Refs().Wires().Entry(aCandidateWireRefId).WireDefId == BRepGraph_WireId::Start())
      {
        aFaceId    = aCandidateFaceId;
        aWireRefId = aCandidateWireRefId;
        isFound    = true;
        break;
      }
    }
  }

  ASSERT_TRUE(isFound);
  const int aNbWireRefsBefore = BRepGraph_TestTools::CountWireRefsOfFace(myGraph, aFaceId);

  ASSERT_TRUE(myGraph.Editor().Faces().RemoveWire(aFaceId, aWireRefId));

  EXPECT_EQ(BRepGraph_TestTools::CountWireRefsOfFace(myGraph, aFaceId), aNbWireRefsBefore - 1);
  EXPECT_FALSE(BRepGraph_TestTools::FaceUsesWire(myGraph, aFaceId, BRepGraph_WireId::Start()));
  EXPECT_TRUE(myGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).IsRemoved);
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());

  for (const BRepGraph_FaceId& aWireFaceId :
       myGraph.Topo().Wires().Faces(BRepGraph_WireId::Start()))
  {
    EXPECT_NE(aWireFaceId, aFaceId);
  }
}

TEST_F(BRepGraphTest, RemoveVertex_PrunesDirectVertexUsage)
{
  const BRepGraph_VertexId aVertexId =
    myGraph.Editor().Vertices().Add(gp_Pnt(1.0, 1.0, 1.0), Precision::Confusion());
  ASSERT_TRUE(aVertexId.IsValid());
  const BRepGraph_VertexRefId aVertexRefId =
    myGraph.Editor().Faces().AddVertex(BRepGraph_FaceId::Start(), aVertexId, TopAbs_INTERNAL);
  ASSERT_TRUE(aVertexRefId.IsValid());
  ASSERT_EQ(myGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).VertexRefIds.Length(), 1);

  ASSERT_TRUE(myGraph.Editor().Faces().RemoveVertex(BRepGraph_FaceId::Start(), aVertexRefId));

  EXPECT_EQ(myGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).VertexRefIds.Length(), 0);
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aVertexId));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveOccurrence_PrunesOccurrenceSubtreeAndRebuildsReverseIndex)
{
  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = myGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aAssemblyId.IsValid());
  const BRepGraph_OccurrenceId anOccId =
    myGraph.Editor().Products().LinkProducts(aAssemblyId, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  const NCollection_DynamicArray<BRepGraph_OccurrenceRefId>& aOccurrenceRefsBefore =
    myGraph.Refs().Occurrences().IdsOf(aAssemblyId);
  ASSERT_EQ(aOccurrenceRefsBefore.Length(), 1);
  const BRepGraph_OccurrenceRefId anOccurrenceRefId = aOccurrenceRefsBefore.Value(0);

  ASSERT_TRUE(myGraph.Editor().Products().RemoveOccurrence(aAssemblyId, anOccurrenceRefId));

  EXPECT_EQ(myGraph.Refs().Occurrences().IdsOf(aAssemblyId).Length(), 0);
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(anOccId));
  EXPECT_EQ(myGraph.Topo().Products().NbComponents(aAssemblyId), 0);
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveShapeRoot_PrunesUniqueTopologyRoot)
{
  const BRepGraph_ProductId aPartId          = BRepGraph_ProductId::Start();
  const BRepGraph_NodeId    aShapeRootBefore = myGraph.Topo().Products().ShapeRoot(aPartId);
  ASSERT_TRUE(aShapeRootBefore.IsValid());

  ASSERT_TRUE(myGraph.Editor().Products().RemoveShapeRoot(aPartId));

  EXPECT_FALSE(myGraph.Topo().Products().ShapeRoot(aPartId).IsValid());
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aShapeRootBefore));
  EXPECT_FALSE(myGraph.Topo().Products().IsPart(aPartId));
  EXPECT_FALSE(myGraph.Topo().Products().IsAssembly(aPartId));
  EXPECT_EQ(myGraph.Topo().Products().NbComponents(aPartId), 0);
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveVertex_PrunesInternalDirectVertexUsage)
{
  const BRepGraph_VertexId aVertexId =
    myGraph.Editor().Vertices().Add(gp_Pnt(2.0, 2.0, 2.0), Precision::Confusion());
  ASSERT_TRUE(aVertexId.IsValid());
  const BRepGraph_VertexRefId aVertexRefId =
    myGraph.Editor().Edges().AddInternalVertex(BRepGraph_EdgeId::Start(),
                                               aVertexId,
                                               TopAbs_INTERNAL);
  ASSERT_TRUE(aVertexRefId.IsValid());
  ASSERT_EQ(
    myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).InternalVertexRefIds.Length(),
    1);

  ASSERT_TRUE(myGraph.Editor().Edges().RemoveVertex(BRepGraph_EdgeId::Start(), aVertexRefId));

  EXPECT_EQ(
    myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).InternalVertexRefIds.Length(),
    0);
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aVertexId));
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveVertex_ClearsBoundarySlotAndPrunesUniqueVertex)
{
  const BRepGraph_VertexId aStartVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion());
  const BRepGraph_VertexId anEndVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(10.0, 0.0, 0.0), Precision::Confusion());
  ASSERT_TRUE(aStartVertex.IsValid());
  ASSERT_TRUE(anEndVertex.IsValid());

  const BRepGraph_EdgeId anEdge = myGraph.Editor().Edges().Add(aStartVertex,
                                                               anEndVertex,
                                                               occ::handle<Geom_Curve>(),
                                                               0.0,
                                                               1.0,
                                                               Precision::Confusion());
  ASSERT_TRUE(anEdge.IsValid());

  const BRepGraph_VertexRefId aStartRefId =
    myGraph.Topo().Edges().Definition(anEdge).StartVertexRefId;
  ASSERT_TRUE(aStartRefId.IsValid());

  ASSERT_TRUE(myGraph.Editor().Edges().RemoveVertex(anEdge, aStartRefId));

  EXPECT_FALSE(myGraph.Topo().Edges().Definition(anEdge).StartVertexRefId.IsValid());
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aStartVertex));
  EXPECT_FALSE(BRepGraph_Tool::Edge::StartVertexRef(myGraph, anEdge).VertexDefId.IsValid());
  EXPECT_TRUE(BRepGraph_Tool::Edge::EndVertexRef(myGraph, anEdge).VertexDefId.IsValid());
  EXPECT_TRUE(myGraph.Editor().ValidateMutationBoundary());
}

TEST_F(BRepGraphTest, RemoveNode_EdgeWithReplacement_ReparentsAllCoEdges)
{
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId::Start());
  ASSERT_GE(aCoEdgeRefs.Length(), 1);

  const BRepGraphInc::CoEdgeRef& aRef = myGraph.Refs().CoEdges().Entry(aCoEdgeRefs.Value(0));
  const BRepGraph_EdgeId         anOldEdgeId =
    myGraph.Topo().CoEdges().Definition(aRef.CoEdgeDefId).EdgeDefId;
  const BRepGraph_EdgeId aNewEdgeId((anOldEdgeId.Index + 1) % myGraph.Topo().Edges().Nb());

  myGraph.Editor().Gen().RemoveNode(anOldEdgeId, aNewEdgeId);

  const NCollection_DynamicArray<BRepGraph_CoEdgeId>& anOldCoEdges =
    myGraph.Topo().Edges().CoEdges(anOldEdgeId);
  EXPECT_EQ(anOldCoEdges.Length(), 0);

  const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aNewCoEdges =
    myGraph.Topo().Edges().CoEdges(aNewEdgeId);
  EXPECT_GT(aNewCoEdges.Length(), 0);
  for (const BRepGraph_CoEdgeId& aNewCoEdgeId : aNewCoEdges)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aNewCoEdgeId);
    EXPECT_EQ(aCoEdge.EdgeDefId, aNewEdgeId);
  }
}

TEST_F(BRepGraphTest, MutableVertex_ChangePoint_Verified)
{
  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aMutVert =
    myGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
  myGraph.Editor().Vertices().SetPoint(aMutVert, gp_Pnt(99.0, 99.0, 99.0));

  const BRepGraphInc::VertexDef& aVert =
    myGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start());
  EXPECT_NEAR(aVert.Point.X(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Y(), 99.0, Precision::Confusion());
  EXPECT_NEAR(aVert.Point.Z(), 99.0, Precision::Confusion());
}

// ===================================================================
// Group 4: Geometric Queries
// ===================================================================

TEST_F(BRepGraphTest, EdgeDef_HasValidCurve3d)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeIt.CurrentId()))
      continue;

    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeIt.CurrentId()))
      << "Edge " << anEdgeIt.CurrentId().Index << " has no Curve3D rep";
  }
}

// ===================================================================
// Group 7: Detection Methods
// ===================================================================

TEST_F(BRepGraphTest, FreeEdges_SingleFace_AllEdgesFree)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  const TopoDS_Face&  aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes13 =
    BRepGraph_Builder::Add(aGraph, aFace);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_DynamicArray<BRepGraph_EdgeId> aFreeEdges = collectFreeEdges(aGraph);
  EXPECT_EQ(aFreeEdges.Length(), 4);
}

TEST_F(BRepGraphTest, Decompose_ThreeDisconnectedFaces_ThreeComponents)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);
  BRepPrimAPI_MakeBox aBox3(30.0, 30.0, 30.0);

  TopExp_Explorer anExp1(aBox1.Shape(), TopAbs_FACE);
  TopExp_Explorer anExp2(aBox2.Shape(), TopAbs_FACE);
  TopExp_Explorer anExp3(aBox3.Shape(), TopAbs_FACE);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, anExp1.Current());
  aBuilder.Add(aCompound, anExp2.Current());
  aBuilder.Add(aCompound, anExp3.Current());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes14 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 3);

  EXPECT_EQ(countFaceComponents(aGraph), 3);
}

TEST_F(BRepGraphTest, DetectToleranceConflicts_ManualConflict_Detected)
{
  // Find two edges that share the same Curve3d handle.
  bool           isConflictSetUp = false;
  const uint32_t aNbEdges        = myGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges) && !isConflictSetUp; ++anEdgeId)
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeId)
        || !BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId))
      continue;

    for (BRepGraph_EdgeId anOtherId(anEdgeId.Index + 1); anOtherId.IsValid(aNbEdges); ++anOtherId)
    {
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, anOtherId)
          || !BRepGraph_Tool::Edge::HasCurve(myGraph, anOtherId))
        continue;
      if (BRepGraph_Tool::Edge::Curve(myGraph, anEdgeId).get()
          != BRepGraph_Tool::Edge::Curve(myGraph, anOtherId).get())
        continue;

      // Set very different tolerances on two edges sharing the same curve.
      myGraph.Editor().Edges().SetTolerance(anEdgeId, 0.001);
      myGraph.Editor().Edges().SetTolerance(anOtherId, 1.0);

      isConflictSetUp = true;
      break;
    }
  }

  if (isConflictSetUp)
  {
    NCollection_DynamicArray<BRepGraph_EdgeId> aConflicts(16);
    NCollection_Map<int>                       aConflictKeys;
    for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
    {
      const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
      if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeId)
          || !BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId))
      {
        continue;
      }

      const occ::handle<Geom_Curve>& aCurve = BRepGraph_Tool::Edge::Curve(myGraph, anEdgeId);
      double aMinTol                        = myGraph.Topo().Edges().Definition(anEdgeId).Tolerance;
      double aMaxTol                        = aMinTol;
      NCollection_DynamicArray<BRepGraph_EdgeId> aCurveEdges(4);
      aCurveEdges.Append(anEdgeId);
      for (BRepGraph_EdgeIterator anOtherIt(myGraph); anOtherIt.More(); anOtherIt.Next())
      {
        const BRepGraph_EdgeId anOtherId = anOtherIt.CurrentId();
        if (anOtherId == anEdgeId || BRepGraph_Tool::Edge::Degenerated(myGraph, anOtherId)
            || !BRepGraph_Tool::Edge::HasCurve(myGraph, anOtherId)
            || BRepGraph_Tool::Edge::Curve(myGraph, anOtherId).get() != aCurve.get())
        {
          continue;
        }

        const double aTol = myGraph.Topo().Edges().Definition(anOtherId).Tolerance;
        aMinTol           = std::min(aMinTol, aTol);
        aMaxTol           = std::max(aMaxTol, aTol);
        aCurveEdges.Append(anOtherId);
      }

      if (aCurveEdges.Length() > 1 && aMaxTol - aMinTol > 0.5)
      {
        for (const BRepGraph_EdgeId& aConflictId : aCurveEdges)
        {
          if (aConflictKeys.Add(aConflictId.Index))
          {
            aConflicts.Append(aConflictId);
          }
        }
      }
    }
    EXPECT_GE(aConflicts.Length(), 1);
  }
}

// ===================================================================
// Group 8: User Attributes & Error Cases
// ===================================================================

TEST_F(BRepGraphTest, RemoveUserAttribute_AfterSet_ReturnsNull)
{
  BRepGraph_NodeId                  aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  occ::handle<BRepGraph_CacheValue> anAttr = new BRepGraph_TypedCacheValue<int>(42);

  myGraph.Cache().Set(aFaceId, testIntAttrKind(), anAttr);
  ASSERT_FALSE(myGraph.Cache().Get(aFaceId, testIntAttrKind()).IsNull());

  myGraph.Cache().Remove(aFaceId, testIntAttrKind());
  EXPECT_TRUE(myGraph.Cache().Get(aFaceId, testIntAttrKind()).IsNull());
}

TEST_F(BRepGraphTest, Build_EmptyCompound_IsDoneZeroCounts)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes15 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 0);
}

TEST_F(BRepGraphTest, TopoNode_GenericLookup_MatchesTypedAccess)
{
  BRepGraph_NodeId             aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraphInc::BaseDef* aBase = myGraph.Topo().Gen().TopoEntity(aFaceId);
  EXPECT_NE(aBase, nullptr);

  // Invalid node id should return nullptr.
  BRepGraph_NodeId             anInvalidId;
  const BRepGraphInc::BaseDef* anInvalidBase = myGraph.Topo().Gen().TopoEntity(anInvalidId);
  EXPECT_EQ(anInvalidBase, nullptr);
}

// ===================================================================
// Group 9: Node Counts and Identity
// ===================================================================

TEST_F(BRepGraphTest, NbNodes_Box_TotalCount)
{
  // NbNodes should equal sum of all per-kind counts (topology + assembly).
  size_t anExpected = static_cast<size_t>(myGraph.Topo().Solids().Nb())
                      + myGraph.Topo().Shells().Nb() + myGraph.Topo().Faces().Nb()
                      + myGraph.Topo().Wires().Nb() + myGraph.Topo().CoEdges().Nb()
                      + myGraph.Topo().Edges().Nb() + myGraph.Topo().Vertices().Nb()
                      + myGraph.Topo().Compounds().Nb() + myGraph.Topo().CompSolids().Nb()
                      + myGraph.Topo().Products().Nb() + myGraph.Topo().Occurrences().Nb();
  EXPECT_EQ(myGraph.Topo().Gen().NbNodes(), anExpected);
}

TEST_F(BRepGraphTest, HasUID_ValidFace_ReturnsTrue)
{
  const BRepGraph_UID& aUID = myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0));
  EXPECT_TRUE(myGraph.UIDs().Has(aUID));
}

TEST_F(BRepGraphTest, HasUID_InvalidUID_ReturnsFalse)
{
  BRepGraph_UID anInvalidUID = BRepGraph_UID::Invalid();
  EXPECT_FALSE(myGraph.UIDs().Has(anInvalidUID));
}

TEST_F(BRepGraphTest, NodeIdFromUID_InvalidUID_ReturnsInvalid)
{
  BRepGraph_UID    anInvalidUID = BRepGraph_UID::Invalid();
  BRepGraph_NodeId aResolved    = myGraph.UIDs().NodeIdFrom(anInvalidUID);
  EXPECT_FALSE(aResolved.IsValid());
}

TEST_F(BRepGraphTest, Allocator_DefaultConstructor_NotNull)
{
  EXPECT_FALSE(myGraph.Allocator().IsNull());
}

TEST_F(BRepGraphTest, Build_WithCustomAllocator_IsDone)
{
  occ::handle<NCollection_BaseAllocator> anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  BRepGraph                              aGraph(anAlloc);

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes16 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_FALSE(aGraph.Allocator().IsNull());
}

// ===================================================================
// Group 10: Topology Structure
// ===================================================================

TEST_F(BRepGraphTest, Wire_IsClosed_BoxOuterWires)
{
  // All outer wires of a box face should be closed.
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    ASSERT_TRUE(anOuterWire.IsValid());
    const BRepGraphInc::WireDef& aWireDef = myGraph.Topo().Wires().Definition(anOuterWire);
    EXPECT_TRUE(aWireDef.IsClosed)
      << "Outer wire of face " << aFaceIt.CurrentId().Index << " should be closed";
  }
}

TEST_F(BRepGraphTest, Face_InnerWireRefs_BoxHasNone)
{
  // Box faces have no holes, so non-outer WireRefs should be empty.
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const NCollection_DynamicArray<BRepGraph_WireRefId> aWireRefs =
      BRepGraph_TestTools::WireRefsOfFace(myGraph, aFaceIt.CurrentId());
    int aNonOuterCount = 0;
    for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
    {
      if (!myGraph.Refs().Wires().Entry(aWireRefId).IsOuter)
        ++aNonOuterCount;
    }
    EXPECT_EQ(aNonOuterCount, 0) << "Box face " << aFaceIt.CurrentId().Index
                                 << " should have no inner wires";
  }
}

TEST_F(BRepGraphTest, Face_Orientation_ValidValue)
{
  // Verify face orientations in the shell's incidence refs.
  ASSERT_EQ(myGraph.Topo().Shells().Nb(), 1);
  const NCollection_DynamicArray<BRepGraph_FaceRefId> aFaceRefs =
    BRepGraph_TestTools::FaceRefsOfShell(myGraph, BRepGraph_ShellId::Start());
  for (int aRefIdx = 0; aRefIdx < aFaceRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::FaceRef& aFaceRef = myGraph.Refs().Faces().Entry(aFaceRefs.Value(aRefIdx));
    TopAbs_Orientation           anOri    = aFaceRef.Orientation;
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED)
      << "Face ref " << aRefIdx << " has unexpected orientation " << anOri;
  }
}

TEST_F(BRepGraphTest, Shell_ContainsSixFaces)
{
  ASSERT_EQ(myGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(myGraph, BRepGraph_ShellId::Start()), 6);
}

TEST_F(BRepGraphTest, Solid_ContainsOneShell)
{
  ASSERT_EQ(myGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(myGraph, BRepGraph_SolidId::Start()), 1);
}

TEST_F(BRepGraphTest, Edge_ParamRange_ValidBounds)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(myGraph, anEdgeIt.CurrentId()))
      continue;
    const auto [aFirst, aLast] = BRepGraph_Tool::Edge::Range(myGraph, anEdgeIt.CurrentId());
    EXPECT_LT(aFirst, aLast) << "Edge " << anEdgeIt.CurrentId().Index
                             << " has invalid parameter range [" << aFirst << ", " << aLast << "]";
  }
}

TEST_F(BRepGraphTest, Vertex_TolerancePositive)
{
  for (BRepGraph_VertexIterator aVertexIt(myGraph); aVertexIt.More(); aVertexIt.Next())
  {
    EXPECT_GT(BRepGraph_Tool::Vertex::Tolerance(myGraph, aVertexIt.CurrentId()), 0.0)
      << "Vertex " << aVertexIt.CurrentId().Index << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Edge_TolerancePositive)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_GT(BRepGraph_Tool::Edge::Tolerance(myGraph, anEdgeIt.CurrentId()), 0.0)
      << "Edge " << anEdgeIt.CurrentId().Index << " has non-positive tolerance";
  }
}

TEST_F(BRepGraphTest, Face_ToleranceNonNegative)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_GE(BRepGraph_Tool::Face::Tolerance(myGraph, aFaceIt.CurrentId()), 0.0)
      << "Face " << aFaceIt.CurrentId().Index << " has negative tolerance";
  }
}

// ===================================================================
// Group 13: Mutation (Extended)
// ===================================================================

TEST_F(BRepGraphTest, MutableWireDef_ModifyClosure_Verified)
{
  BRepGraph_MutGuard<BRepGraphInc::WireDef> aMutWD =
    myGraph.Editor().Wires().Mut(BRepGraph_WireId::Start());
  bool anOrigClosed = aMutWD->IsClosed;
  myGraph.Editor().Wires().SetIsClosed(aMutWD, !anOrigClosed);

  EXPECT_EQ(myGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).IsClosed, !anOrigClosed);

  // Restore original state.
  myGraph.Editor().Wires().SetIsClosed(BRepGraph_WireId::Start(), anOrigClosed);
  EXPECT_EQ(myGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).IsClosed, anOrigClosed);
}

TEST_F(BRepGraphTest, MultipleUserAttributes_SameNode_Independent)
{
  BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  Handle(BRepGraph_TypedCacheValue<int>)    anAttr1 = new BRepGraph_TypedCacheValue<int>(100);
  Handle(BRepGraph_TypedCacheValue<double>) anAttr2 = new BRepGraph_TypedCacheValue<double>(2.718);

  myGraph.Cache().Set(aFaceId, testIntAttrKind(), anAttr1);
  myGraph.Cache().Set(aFaceId, testAuxAttrKind(), anAttr2);

  Handle(BRepGraph_TypedCacheValue<int>) aRetrieved1 =
    Handle(BRepGraph_TypedCacheValue<int>)::DownCast(
      myGraph.Cache().Get(aFaceId, testIntAttrKind()));
  Handle(BRepGraph_TypedCacheValue<double>) aRetrieved2 =
    Handle(BRepGraph_TypedCacheValue<double>)::DownCast(
      myGraph.Cache().Get(aFaceId, testAuxAttrKind()));

  ASSERT_FALSE(aRetrieved1.IsNull());
  ASSERT_FALSE(aRetrieved2.IsNull());
  EXPECT_EQ(aRetrieved1->UncheckedValue(), 100);
  EXPECT_NEAR(aRetrieved2->UncheckedValue(), 2.718, 1.0e-10);

  // Remove one; the other should remain.
  myGraph.Cache().Remove(aFaceId, testIntAttrKind());
  EXPECT_TRUE(myGraph.Cache().Get(aFaceId, testIntAttrKind()).IsNull());
  EXPECT_FALSE(myGraph.Cache().Get(aFaceId, testAuxAttrKind()).IsNull());
}

TEST_F(BRepGraphTest, InvalidateUserAttribute_SpecificKey)
{
  BRepGraph_NodeId                       aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  Handle(BRepGraph_TypedCacheValue<int>) anAttr = new BRepGraph_TypedCacheValue<int>(42);
  myGraph.Cache().Set(aFaceId, testIntAttrKind(), anAttr);

  // Invalidate should not remove, just mark dirty.
  myGraph.Cache().Invalidate(aFaceId, testIntAttrKind());

  occ::handle<BRepGraph_CacheValue> aRetrieved = myGraph.Cache().Get(aFaceId, testIntAttrKind());
  EXPECT_FALSE(aRetrieved.IsNull()); // still present
}

TEST_F(BRepGraphTest, UserAttribute_OnEdgeNode)
{
  BRepGraph_NodeId                          anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  Handle(BRepGraph_TypedCacheValue<double>) anAttr = new BRepGraph_TypedCacheValue<double>(1.5);

  myGraph.Cache().Set(anEdgeId, testDoubleAttrKind(), anAttr);

  Handle(BRepGraph_TypedCacheValue<double>) aRetrieved =
    Handle(BRepGraph_TypedCacheValue<double>)::DownCast(
      myGraph.Cache().Get(anEdgeId, testDoubleAttrKind()));
  ASSERT_FALSE(aRetrieved.IsNull());
  EXPECT_NEAR(aRetrieved->UncheckedValue(), 1.5, 1.0e-10);
}

// ===================================================================
// Group 14: Build From Different Root Shapes
// ===================================================================

TEST_F(BRepGraphTest, Build_SingleFace_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_FACE);
  const TopoDS_Face&  aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes17 =
    BRepGraph_Builder::Add(aGraph, aFace);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 4);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 4);
}

TEST_F(BRepGraphTest, Build_Shell_CorrectCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes18 =
    BRepGraph_Builder::Add(aGraph, anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);
}

TEST_F(BRepGraphTest, Build_CompoundOfTwoSolids)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(20.0, 20.0, 20.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1.Shape());
  aBuilder.Add(aCompound, aBox2.Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes19 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 12);
}

TEST_F(BRepGraphTest, ReconstructShape_ShellRoot_SameFaceCount)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  TopExp_Explorer     anExp(aBoxMaker.Shape(), TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes20 =
    BRepGraph_Builder::Add(aGraph, anExp.Current());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().Shells().Nb(), 1);

  BRepGraph_NodeId   aShellId(BRepGraph_NodeId::Kind::Shell, 0);
  const TopoDS_Shape aReconstructed = aGraph.Shapes().Reconstruct(aShellId);

  int aFaceCount = 0;
  for (TopExp_Explorer aFaceExp(aReconstructed, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);
}

// ===================================================================
// Group 15: UID Properties
// ===================================================================

TEST_F(BRepGraphTest, UID_FaceIsTopology)
{
  EXPECT_TRUE(myGraph.UIDs().Of(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0)).IsTopology());
}

TEST_F(BRepGraphTest, UID_AllTopoNodesHaveValidUIDs)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(myGraph.UIDs().Of(BRepGraph_NodeId(aFaceIt.CurrentId())).IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has invalid UID";
  }
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_TRUE(myGraph.UIDs().Of(BRepGraph_NodeId(anEdgeIt.CurrentId())).IsValid())
      << "Edge " << anEdgeIt.CurrentId().Index << " has invalid UID";
  }
}

TEST_F(BRepGraphTest, Wire_OuterWireIdx_MatchesFaceDef)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    EXPECT_TRUE(anOuterWire.IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has no outer wire";
    if (!anOuterWire.IsValid())
      continue;
    EXPECT_LT(anOuterWire.Index, myGraph.Topo().Wires().Nb())
      << "Face " << aFaceIt.CurrentId().Index << " outer wire index out of range";
  }
}

TEST_F(BRepGraphTest, Wire_CoEdgeRefs_FourEdgesPerBoxFace)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_WireId anOuterWire =
      BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceIt.CurrentId());
    ASSERT_TRUE(anOuterWire.IsValid());
    EXPECT_EQ(BRepGraph_TestTools::CountCoEdgeRefsOfWire(myGraph, anOuterWire), 4)
      << "Box face " << aFaceIt.CurrentId().Index << " should have 4 coedge refs in its outer wire";
  }
}

// ===================================================================
// Group: History Enabled Flag
// ===================================================================

TEST_F(BRepGraphTest, SetHistoryEnabled_DefaultTrue)
{
  EXPECT_TRUE(myGraph.History().IsEnabled());
}

TEST_F(BRepGraphTest, SetHistoryEnabled_DisableAndQuery)
{
  myGraph.History().SetEnabled(false);
  EXPECT_FALSE(myGraph.History().IsEnabled());
  myGraph.History().SetEnabled(true);
  EXPECT_TRUE(myGraph.History().IsEnabled());
}

TEST_F(BRepGraphTest, RecordHistory_Disabled_NoRecordAdded)
{
  const size_t aBefore = myGraph.History().NbRecords();

  myGraph.History().SetEnabled(false);

  BRepGraph_NodeId                           anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                           anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_DynamicArray<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("ShouldNotRecord", anEdge0, aRepl);

  EXPECT_EQ(myGraph.History().NbRecords(), aBefore);
}

TEST_F(BRepGraphTest, RecordHistory_ReEnabled_RecordsAgain)
{
  myGraph.History().SetEnabled(false);

  const size_t aBefore = myGraph.History().NbRecords();

  BRepGraph_NodeId                           anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId                           anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  NCollection_DynamicArray<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("Skipped", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore);

  myGraph.History().SetEnabled(true);
  myGraph.History().Record("Recorded", anEdge0, aRepl);
  EXPECT_EQ(myGraph.History().NbRecords(), aBefore + 1);
  EXPECT_TRUE(myGraph.History().Record(aBefore).OperationName.IsEqual("Recorded"));
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_NoHistoryNoDerivedEdges)
{
  myGraph.History().SetEnabled(false);

  const size_t aNbHistBefore = myGraph.History().NbRecords();

  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    NCollection_DynamicArray<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge1);
    return aResult;
  };

  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier, "NoHistory");

  // No history records should be added.
  EXPECT_EQ(myGraph.History().NbRecords(), aNbHistBefore);
}

TEST_F(BRepGraphTest, ApplyModification_HistoryDisabled_ModifierStillRuns)
{
  myGraph.History().SetEnabled(false);

  bool             isModifierCalled = false;
  BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);

  auto aModifier = [&](BRepGraph& /*theGraph*/, BRepGraph_NodeId /*theTarget*/) {
    isModifierCalled = true;
    NCollection_DynamicArray<BRepGraph_NodeId> aResult;
    aResult.Append(anEdge0);
    return aResult;
  };

  myGraph.Editor().Gen().ApplyModification(anEdge0, aModifier, "CheckModifier");
  EXPECT_TRUE(isModifierCalled);
}
