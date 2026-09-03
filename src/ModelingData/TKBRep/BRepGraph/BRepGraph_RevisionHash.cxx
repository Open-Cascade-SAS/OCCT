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

#include <BRepGraph_RevisionHash.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraphSupInc_Storage.hxx>
#include <BRepGraphSupInc_Store.hxx>
#include <GeomHash_CanonicalHashStream.hxx>
#include <GeomHash_GeometryAppender.hxx>
#include <GeomHash_MeshAppender.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_PersistentRadixMap.hxx>
#include <Standard_SHA256.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

#include <algorithm>
#include <array>
#include <charconv>
#include <utility>

namespace
{
class RevisionMerkleHasher
{
public:
  using ChildHash = std::pair<uint8_t, BRepGraph_RevisionHash>;

  [[nodiscard]] static BRepGraph_RevisionHash EmptyHash();
  [[nodiscard]] static BRepGraph_RevisionHash LeafHash(const uint64_t                theKey,
                                                       const BRepGraph_RevisionHash& theValue);
  [[nodiscard]] static BRepGraph_RevisionHash BranchHash(const size_t     theDepth,
                                                         const ChildHash* theChildren,
                                                         const size_t     theNbChildren);
};

using RevisionMerkle = NCollection_PersistentRadixMap<uint64_t,
                                                      BRepGraph_RevisionHash,
                                                      BRepGraph_RevisionHash,
                                                      RevisionMerkleHasher>;

constexpr uint32_t THE_MERKLE_FORMAT_VERSION = 1;
constexpr size_t   THE_MERKLE_MAX_INPUT_SIZE =
  4 + 4 + 1 + 2 + 256 * (1 + sizeof(BRepGraph_RevisionHash));

struct MerkleHashInput
{
  void AppendByte(const uint8_t theValue) { Bytes[Size++] = theValue; }

  void AppendUInt16(const uint16_t theValue)
  {
    AppendByte(static_cast<uint8_t>(theValue));
    AppendByte(static_cast<uint8_t>(theValue >> 8));
  }

  void AppendUInt32(const uint32_t theValue)
  {
    for (int aByte = 0; aByte < 4; ++aByte)
    {
      AppendByte(static_cast<uint8_t>(theValue >> (aByte * 8)));
    }
  }

  void AppendUInt64(const uint64_t theValue)
  {
    for (int aByte = 0; aByte < 8; ++aByte)
    {
      AppendByte(static_cast<uint8_t>(theValue >> (aByte * 8)));
    }
  }

  void AppendHash(const BRepGraph_RevisionHash& theHash)
  {
    for (const uint64_t aWord : theHash.Words)
    {
      AppendUInt64(aWord);
    }
  }

  BRepGraph_RevisionHash Hash() const
  {
    return BRepGraph_RevisionHash::Hasher::Bytes(Bytes.data(), Size);
  }

  std::array<uint8_t, THE_MERKLE_MAX_INPUT_SIZE> Bytes;
  size_t                                         Size = 0;
};

BRepGraph_RevisionHash RevisionMerkleHasher::EmptyHash()
{
  MerkleHashInput anInput;
  anInput.AppendUInt32(0x4d454d50u); // MEMP
  anInput.AppendUInt32(THE_MERKLE_FORMAT_VERSION);
  return anInput.Hash();
}

BRepGraph_RevisionHash RevisionMerkleHasher::LeafHash(const uint64_t                theKey,
                                                      const BRepGraph_RevisionHash& theValue)
{
  MerkleHashInput anInput;
  anInput.AppendUInt32(0x4d4c4546u); // MLEF
  anInput.AppendUInt32(THE_MERKLE_FORMAT_VERSION);
  anInput.AppendUInt64(theKey);
  anInput.AppendHash(theValue);
  return anInput.Hash();
}

BRepGraph_RevisionHash RevisionMerkleHasher::BranchHash(const size_t     theDepth,
                                                        const ChildHash* theChildren,
                                                        const size_t     theNbChildren)
{
  MerkleHashInput anInput;
  anInput.AppendUInt32(0x4d42524eu); // MBRN
  anInput.AppendUInt32(THE_MERKLE_FORMAT_VERSION);
  anInput.AppendByte(static_cast<uint8_t>(theDepth));
  anInput.AppendUInt16(static_cast<uint16_t>(theNbChildren));
  for (size_t anIndex = 0; anIndex < theNbChildren; ++anIndex)
  {
    const auto& [aSlot, aHash] = theChildren[anIndex];
    anInput.AppendByte(aSlot);
    anInput.AppendHash(aHash);
  }
  return anInput.Hash();
}
} // namespace

//! Immutable pair of persistent Merkle indexes retained by graph revisions.
class BRepGraph_RevisionHash::Hasher::Index
{
public:
  Index(const RevisionMerkle& theSemantic, const RevisionMerkle& theStorage)
      : Semantic(theSemantic),
        Storage(theStorage)
  {
  }

  RevisionMerkle Semantic;
  RevisionMerkle Storage;
};

namespace
{

class RevisionHashByteSink
{
public:
  using result_type = std::optional<BRepGraph_RevisionHash>;

  void AppendBytes(const void* theData, size_t theSize)
  {
    if (!myIsValid)
    {
      return;
    }
    myIsValid = mySHA256.Append(theData, theSize);
  }

  [[nodiscard]] bool IsValid() const noexcept { return myIsValid; }

  [[nodiscard]] result_type Finish() const
  {
    Standard_SHA256::Digest aDigest;
    if (!myIsValid || !mySHA256.Finish(aDigest))
    {
      return std::nullopt;
    }

    BRepGraph_RevisionHash         aResult;
    const std::array<uint8_t, 32>& aBytes = aDigest.Bytes();
    for (size_t aWordIndex = 0; aWordIndex < 4; ++aWordIndex)
    {
      uint64_t aWord = 0;
      for (size_t aByteIndex = 0; aByteIndex < 8; ++aByteIndex)
      {
        aWord = (aWord << 8) | aBytes[aWordIndex * 8 + aByteIndex];
      }
      aResult.Words[aWordIndex] = aWord;
    }
    return aResult;
  }

private:
  Standard_SHA256 mySHA256;
  bool            myIsValid = true;
};

class RevisionHashStream : public GeomHash_CanonicalHashStream<RevisionHashByteSink>
{
public:
  explicit RevisionHashStream(const uint64_t theDomain) { AppendUInt64(theDomain); }

  RevisionHashStream() = default;

  [[nodiscard]] bool IsValid() const noexcept { return ByteSink().IsValid(); }

  void AppendUID(const BRepGraph_UID& theUID)
  {
    AppendBool(theUID.IsValid());
    if (theUID.IsValid())
    {
      AppendUInt32(static_cast<uint32_t>(theUID.Kind));
      AppendUInt32(theUID.Counter);
    }
  }

  void AppendRefUID(const BRepGraph_RefUID& theUID)
  {
    AppendBool(theUID.IsValid());
    if (theUID.IsValid())
    {
      AppendUInt32(static_cast<uint32_t>(theUID.Kind));
      AppendUInt32(theUID.Counter);
    }
  }

  void AppendNodeId(const BRepGraph_NodeId theId)
  {
    AppendBool(theId.IsValid());
    if (theId.IsValid())
    {
      AppendUInt32(static_cast<uint32_t>(theId.NodeKind));
      AppendUInt32(theId.Index);
    }
  }

  void AppendRefId(const BRepGraph_RefId theId)
  {
    AppendBool(theId.IsValid());
    if (theId.IsValid())
    {
      AppendUInt32(static_cast<uint32_t>(theId.RefKind));
      AppendUInt32(theId.Index);
    }
  }

  void AppendHash(const BRepGraph_RevisionHash& theHash)
  {
    for (const uint64_t aWord : theHash.Words)
    {
      AppendUInt64(aWord);
    }
  }

  void AppendLocation(const TopLoc_Location& theLocation)
  {
    const gp_Trsf& aTrsf = theLocation.Transformation();
    for (int aRow = 1; aRow <= 3; ++aRow)
    {
      for (int aCol = 1; aCol <= 4; ++aCol)
      {
        AppendDouble(aTrsf.Value(aRow, aCol));
      }
    }
  }

  void AppendCurve(const occ::handle<Geom_Curve>& theCurve)
  {
    GeomHash_GeometryAppender<RevisionHashStream>::AppendCurveRecord(*this, theCurve);
  }

  void AppendCurve2d(const occ::handle<Geom2d_Curve>& theCurve)
  {
    GeomHash_GeometryAppender<RevisionHashStream>::AppendCurveRecord(*this, theCurve);
  }

  void AppendSurface(const occ::handle<Geom_Surface>& theSurface)
  {
    GeomHash_GeometryAppender<RevisionHashStream>::AppendSurfaceRecord(*this, theSurface);
  }

  void AppendPolygon3D(const occ::handle<Poly_Polygon3D>& thePolygon)
  {
    GeomHash_MeshAppender<RevisionHashStream>::AppendPolygonRecord(*this, thePolygon);
  }

  void AppendPolygon2D(const occ::handle<Poly_Polygon2D>& thePolygon)
  {
    GeomHash_MeshAppender<RevisionHashStream>::AppendPolygonRecord(*this, thePolygon);
  }

  void AppendPolygonOnTriangulation(const occ::handle<Poly_PolygonOnTriangulation>& thePolygon)
  {
    GeomHash_MeshAppender<RevisionHashStream>::AppendPolygonRecord(*this, thePolygon);
  }

  void AppendTriangulation(const occ::handle<Poly_Triangulation>& theTriangulation)
  {
    GeomHash_MeshAppender<RevisionHashStream>::AppendTriangulationRecord(*this, theTriangulation);
  }
};

BRepGraph_RevisionHash finishOrNull(const RevisionHashStream& theStream)
{
  return theStream.Finish().value_or(BRepGraph_RevisionHash());
}

void appendNodeUID(RevisionHashStream&    theBuilder,
                   const BRepGraph&       theGraph,
                   const BRepGraph_NodeId theId)
{
  theBuilder.AppendUID(theId.IsValid() ? theGraph.UIDs().Of(theId) : BRepGraph_UID());
}

void appendRefUID(RevisionHashStream&   theBuilder,
                  const BRepGraph&      theGraph,
                  const BRepGraph_RefId theId)
{
  theBuilder.AppendRefUID(theId.IsValid() ? theGraph.UIDs().Of(theId) : BRepGraph_RefUID());
}

template <typename TheIdType>
void appendNodeIds(RevisionHashStream&                        theBuilder,
                   const BRepGraph&                           theGraph,
                   const NCollection_LinearVector<TheIdType>& theIds)
{
  // A mutation may leave removed entries in an incidence list until Compact().
  // They are storage artifacts and must not change the semantic hash.
  NCollection_LinearVector<BRepGraph_UID> aUIDs(theIds.Size());
  for (const TheIdType anId : theIds)
  {
    const BRepGraph_NodeId aNodeId(anId);
    if (aNodeId.IsValid() && !aNodeId.IsRemoved(theGraph))
    {
      aUIDs.Append(theGraph.UIDs().Of(aNodeId));
    }
  }

  theBuilder.AppendUInt64(static_cast<uint64_t>(aUIDs.Size()));
  for (const BRepGraph_UID& aUID : aUIDs)
  {
    theBuilder.AppendUID(aUID);
  }
}

template <typename TheIdType>
void appendRefIds(RevisionHashStream&                        theBuilder,
                  const BRepGraph&                           theGraph,
                  const NCollection_LinearVector<TheIdType>& theIds)
{
  // A mutation may leave removed entries in an incidence list until Compact().
  // They are storage artifacts and must not change the semantic hash.
  NCollection_LinearVector<BRepGraph_RefUID> aUIDs(theIds.Size());
  for (const TheIdType anId : theIds)
  {
    const BRepGraph_RefId aRefId(anId);
    if (aRefId.IsValid() && !aRefId.IsRemoved(theGraph))
    {
      aUIDs.Append(theGraph.UIDs().Of(aRefId));
    }
  }

  theBuilder.AppendUInt64(static_cast<uint64_t>(aUIDs.Size()));
  for (const BRepGraph_RefUID& aUID : aUIDs)
  {
    theBuilder.AppendRefUID(aUID);
  }
}

void appendBase(RevisionHashStream&    theBuilder,
                const BRepGraph&       theGraph,
                const BRepGraph_NodeId theNode)
{
  theBuilder.AppendUID(theGraph.UIDs().Of(theNode));
  theBuilder.AppendUInt32(static_cast<uint32_t>(theNode.NodeKind));
}

void appendDefinitionSignature(RevisionHashStream&    theBuilder,
                               const BRepGraph&       theGraph,
                               const BRepGraph_NodeId theNode)
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraph_VertexId       anId = BRepGraph_VertexId::FromNodeId(theNode);
      const BRepGraphInc::VertexDef& aDef = theGraph.Topo().Vertices().Definition(anId);
      appendBase(theBuilder, theGraph, theNode);
      theBuilder.AppendPoint(aDef.Point);
      theBuilder.AppendDouble(aDef.Tolerance);
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_EdgeId       anId = BRepGraph_EdgeId::FromNodeId(theNode);
      const BRepGraphInc::EdgeDef& aDef = theGraph.Topo().Edges().Definition(anId);
      appendBase(theBuilder, theGraph, theNode);
      theBuilder.AppendDouble(aDef.Tolerance);
      appendRefUID(theBuilder, theGraph, aDef.StartVertexRefId);
      appendRefUID(theBuilder, theGraph, aDef.EndVertexRefId);
      const occ::handle<Geom_Curve>& aCurve = BRepGraph_Tool::Edge::Curve(theGraph, anId);
      theBuilder.AppendCurve(aCurve);
      const std::pair<double, double> aRange = BRepGraph_Tool::Edge::Range(theGraph, anId);
      theBuilder.AppendDouble(aRange.first);
      theBuilder.AppendDouble(aRange.second);
      theBuilder.AppendPolygon3D(theGraph.Mesh().Persistent().Edges().Polygon3D(anId));
      break;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraph_CoEdgeId       anId = BRepGraph_CoEdgeId::FromNodeId(theNode);
      const BRepGraphInc::CoEdgeDef& aDef = theGraph.Topo().CoEdges().Definition(anId);
      appendBase(theBuilder, theGraph, theNode);
      appendNodeUID(theBuilder, theGraph, aDef.ParentWireId);
      appendNodeUID(theBuilder, theGraph, aDef.ChildEdgeId);
      appendNodeUID(theBuilder, theGraph, aDef.FaceId);
      theBuilder.AppendBool(aDef.Orientation.IsReversed);
      const occ::handle<Geom2d_Curve>& aPCurve = BRepGraph_Tool::CoEdge::PCurve(theGraph, anId);
      theBuilder.AppendCurve2d(aPCurve);
      const std::pair<double, double> aRange = BRepGraph_Tool::CoEdge::Range(theGraph, anId);
      theBuilder.AppendDouble(aRange.first);
      theBuilder.AppendDouble(aRange.second);
      theBuilder.AppendPolygon2D(theGraph.Mesh().Persistent().CoEdges().PolygonOnSurface(anId));
      theBuilder.AppendPolygonOnTriangulation(
        theGraph.Mesh().Persistent().CoEdges().PolygonOnTriangulation(anId));
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_WireId anId = BRepGraph_WireId::FromNodeId(theNode);
      appendBase(theBuilder, theGraph, theNode);
      appendNodeIds(theBuilder, theGraph, theGraph.Topo().Wires().Relations(anId).CoEdgeIds);
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_FaceId       anId = BRepGraph_FaceId::FromNodeId(theNode);
      const BRepGraphInc::FaceDef& aDef = theGraph.Topo().Faces().Definition(anId);
      appendBase(theBuilder, theGraph, theNode);
      theBuilder.AppendDouble(aDef.Tolerance);
      theBuilder.AppendSurface(BRepGraph_Tool::Face::Surface(theGraph, anId));
      theBuilder.AppendTriangulation(theGraph.Mesh().Persistent().Faces().Triangulation(anId));
      appendRefIds(theBuilder, theGraph, theGraph.Topo().Faces().Relations(anId).WireRefIds);
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId anId = BRepGraph_ShellId::FromNodeId(theNode);
      appendBase(theBuilder, theGraph, theNode);
      appendRefIds(theBuilder, theGraph, theGraph.Topo().Shells().Relations(anId).FaceRefIds);
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_SolidId anId = BRepGraph_SolidId::FromNodeId(theNode);
      appendBase(theBuilder, theGraph, theNode);
      appendRefIds(theBuilder, theGraph, theGraph.Topo().Solids().Relations(anId).ShellRefIds);
      break;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_CompoundId anId = BRepGraph_CompoundId::FromNodeId(theNode);
      appendBase(theBuilder, theGraph, theNode);
      appendRefIds(theBuilder, theGraph, theGraph.Topo().Compounds().Relations(anId).ChildRefIds);
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_CompSolidId anId = BRepGraph_CompSolidId::FromNodeId(theNode);
      appendBase(theBuilder, theGraph, theNode);
      appendRefIds(theBuilder, theGraph, theGraph.Topo().CompSolids().Relations(anId).SolidRefIds);
      break;
    }
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraph_ProductId anId = BRepGraph_ProductId::FromNodeId(theNode);
      appendBase(theBuilder, theGraph, theNode);
      appendRefIds(theBuilder,
                   theGraph,
                   theGraph.Topo().Products().Relations(anId).OccurrenceRefIds);
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraph_OccurrenceId       anId = BRepGraph_OccurrenceId::FromNodeId(theNode);
      const BRepGraphInc::OccurrenceDef& aDef = theGraph.Topo().Occurrences().Definition(anId);
      appendBase(theBuilder, theGraph, theNode);
      appendNodeUID(theBuilder, theGraph, aDef.ChildNodeId);
      break;
    }
  }
}

void appendReferenceSignature(RevisionHashStream&   theBuilder,
                              const BRepGraph&      theGraph,
                              const BRepGraph_RefId theRef)
{
  theBuilder.AppendRefUID(theGraph.UIDs().Of(theRef));
  theBuilder.AppendUInt32(static_cast<uint32_t>(theRef.RefKind));

  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell: {
      const BRepGraph_ShellRefId    anId = BRepGraph_ShellRefId::FromRefId(theRef);
      const BRepGraphInc::ShellRef& aRef = theGraph.Refs().Shells().Entry(anId);
      appendNodeUID(theBuilder, theGraph, aRef.ParentSolidId);
      appendNodeUID(theBuilder, theGraph, aRef.ChildShellId);
      theBuilder.AppendBool(aRef.Orientation.IsReversed);
      break;
    }
    case BRepGraph_RefId::Kind::Face: {
      const BRepGraph_FaceRefId    anId = BRepGraph_FaceRefId::FromRefId(theRef);
      const BRepGraphInc::FaceRef& aRef = theGraph.Refs().Faces().Entry(anId);
      appendNodeUID(theBuilder, theGraph, aRef.ParentShellId);
      appendNodeUID(theBuilder, theGraph, aRef.ChildFaceId);
      theBuilder.AppendBool(aRef.Orientation.IsReversed);
      break;
    }
    case BRepGraph_RefId::Kind::Wire: {
      const BRepGraph_WireRefId    anId = BRepGraph_WireRefId::FromRefId(theRef);
      const BRepGraphInc::WireRef& aRef = theGraph.Refs().Wires().Entry(anId);
      appendNodeUID(theBuilder, theGraph, aRef.ParentFaceId);
      appendNodeUID(theBuilder, theGraph, aRef.ChildWireId);
      theBuilder.AppendBool(aRef.Orientation.IsReversed);
      break;
    }
    case BRepGraph_RefId::Kind::Vertex: {
      const BRepGraph_VertexRefId    anId = BRepGraph_VertexRefId::FromRefId(theRef);
      const BRepGraphInc::VertexRef& aRef = theGraph.Refs().Vertices().Entry(anId);
      appendNodeUID(theBuilder, theGraph, aRef.ParentEdgeId);
      appendNodeUID(theBuilder, theGraph, aRef.ChildVertexId);
      theBuilder.AppendBool(aRef.Orientation.IsReversed);
      break;
    }
    case BRepGraph_RefId::Kind::Solid: {
      const BRepGraph_SolidRefId    anId = BRepGraph_SolidRefId::FromRefId(theRef);
      const BRepGraphInc::SolidRef& aRef = theGraph.Refs().Solids().Entry(anId);
      appendNodeUID(theBuilder, theGraph, aRef.ParentCompSolidId);
      appendNodeUID(theBuilder, theGraph, aRef.ChildSolidId);
      theBuilder.AppendBool(aRef.Orientation.IsReversed);
      break;
    }
    case BRepGraph_RefId::Kind::Child: {
      const BRepGraph_ChildRefId    anId = BRepGraph_ChildRefId::FromRefId(theRef);
      const BRepGraphInc::ChildRef& aRef = theGraph.Refs().Children().Entry(anId);
      appendNodeUID(theBuilder, theGraph, aRef.ParentCompoundId);
      appendNodeUID(theBuilder, theGraph, aRef.ChildNodeId);
      theBuilder.AppendBool(aRef.Orientation.IsReversed);
      theBuilder.AppendLocation(aRef.LocalLocation);
      break;
    }
    case BRepGraph_RefId::Kind::Occurrence: {
      const BRepGraph_OccurrenceRefId    anId = BRepGraph_OccurrenceRefId::FromRefId(theRef);
      const BRepGraphInc::OccurrenceRef& aRef = theGraph.Refs().Occurrences().Entry(anId);
      appendNodeUID(theBuilder, theGraph, aRef.ParentProductId);
      appendNodeUID(theBuilder, theGraph, aRef.ChildOccurrenceId);
      theBuilder.AppendLocation(aRef.LocalLocation);
      break;
    }
  }
}

BRepGraph_RevisionHash physicalNodeHash(const BRepGraph&              theGraph,
                                        const BRepGraph_NodeId        theNode,
                                        const BRepGraph_RevisionHash& theSemanticHash)
{
  RevisionHashStream           aBuilder(0x504e4f44ull);
  const BRepGraphInc::BaseDef* aDefinition = theGraph.Topo().Gen().TopoEntity(theNode);
  aBuilder.AppendNodeId(theNode);
  aBuilder.AppendBool(theNode.IsRemoved(theGraph));
  aBuilder.AppendBool(theNode.IsOwned(theGraph));
  if (aDefinition != nullptr)
  {
    aBuilder.AppendUInt32(aDefinition->UID);
  }
  aBuilder.AppendHash(theSemanticHash);
  if (theSemanticHash.IsNull() && aDefinition != nullptr)
  {
    RevisionHashStream aDefinitionBuilder(0x4e4f4445ull);
    appendDefinitionSignature(aDefinitionBuilder, theGraph, theNode);
    const std::optional<BRepGraph_RevisionHash> aDefinitionHash = aDefinitionBuilder.Finish();
    if (!aDefinitionHash.has_value())
    {
      return BRepGraph_RevisionHash();
    }
    aBuilder.AppendHash(*aDefinitionHash);
  }
  return finishOrNull(aBuilder);
}

BRepGraph_RevisionHash physicalReferenceHash(const BRepGraph&              theGraph,
                                             const BRepGraph_RefId         theRef,
                                             const BRepGraph_RevisionHash& theSemanticHash)
{
  RevisionHashStream aBuilder(0x50524546ull);
  aBuilder.AppendRefId(theRef);
  aBuilder.AppendBool(theGraph.Refs().Gen().IsRemoved(theRef));
  aBuilder.AppendBool(theRef.IsOwned(theGraph));
  aBuilder.AppendHash(theSemanticHash);
  if (theSemanticHash.IsNull())
  {
    RevisionHashStream aReferenceBuilder(0x52454653ull);
    appendReferenceSignature(aReferenceBuilder, theGraph, theRef);
    const std::optional<BRepGraph_RevisionHash> aReferenceHash = aReferenceBuilder.Finish();
    if (!aReferenceHash.has_value())
    {
      return BRepGraph_RevisionHash();
    }
    aBuilder.AppendHash(*aReferenceHash);
  }
  return finishOrNull(aBuilder);
}

enum class MerkleNamespace : uint8_t
{
  SemanticNode      = 1,
  SemanticReference = 2,
  SemanticMetadata  = 3,
  StorageNode       = 4,
  StorageReference  = 5,
  StorageMetadata   = 6
};

uint64_t merkleKey(const MerkleNamespace theNamespace,
                   const uint8_t         theKind,
                   const uint32_t        theIdentity)
{
  return (static_cast<uint64_t>(theNamespace) << 36) | (static_cast<uint64_t>(theKind) << 32)
         | static_cast<uint64_t>(theIdentity);
}

template <typename TheDefinition>
void addNodeLeaves(const BRepGraph&                                 theGraph,
                   NCollection_LinearVector<RevisionMerkle::Entry>& theSemantic,
                   NCollection_LinearVector<RevisionMerkle::Entry>& theStorage)
{
  using Traits          = BRepGraph_IteratorDetail::NodeTraits<TheDefinition>;
  const uint32_t aCount = Traits::Count(theGraph);
  for (uint32_t anIndex = 0; anIndex < aCount; ++anIndex)
  {
    const typename Traits::TypedId aTypedId(anIndex);
    const BRepGraph_NodeId         aNode(aTypedId);
    const BRepGraph_RevisionHash   aSemanticHash =
      BRepGraph_RevisionHash::Hasher::Node(theGraph, aNode);
    if (!aNode.IsRemoved(theGraph))
    {
      const BRepGraph_UID aUID = theGraph.UIDs().Of(aNode);
      theSemantic.Append(
        {merkleKey(MerkleNamespace::SemanticNode, static_cast<uint8_t>(aUID.Kind), aUID.Counter),
         aSemanticHash});
    }
    theStorage.Append(
      {merkleKey(MerkleNamespace::StorageNode, static_cast<uint8_t>(aNode.NodeKind), anIndex),
       physicalNodeHash(theGraph, aNode, aSemanticHash)});
  }
}

template <typename TheRefId>
void addReferenceLeaves(const BRepGraph&                                 theGraph,
                        const uint32_t                                   theCount,
                        NCollection_LinearVector<RevisionMerkle::Entry>& theSemantic,
                        NCollection_LinearVector<RevisionMerkle::Entry>& theStorage)
{
  for (uint32_t anIndex = 0; anIndex < theCount; ++anIndex)
  {
    const TheRefId               aTypedId(anIndex);
    const BRepGraph_RefId        aRef(aTypedId);
    const BRepGraph_RevisionHash aSemanticHash =
      BRepGraph_RevisionHash::Hasher::Reference(theGraph, aRef);
    if (theGraph.Refs().Gen().IsActive(aRef))
    {
      const BRepGraph_RefUID aUID = theGraph.UIDs().Of(aRef);
      theSemantic.Append({merkleKey(MerkleNamespace::SemanticReference,
                                    static_cast<uint8_t>(aUID.Kind),
                                    aUID.Counter),
                          aSemanticHash});
    }
    theStorage.Append(
      {merkleKey(MerkleNamespace::StorageReference, static_cast<uint8_t>(aRef.RefKind), anIndex),
       physicalReferenceHash(theGraph, aRef, aSemanticHash)});
  }
}

BRepGraph_RevisionHash semanticMetadataHash(const BRepGraph& theGraph)
{
  RevisionHashStream aBuilder(0x534d4554ull);
  aBuilder.AppendUInt32(1);
  aBuilder.AppendUInt64(static_cast<uint64_t>(theGraph.RootProductIds().Size()));
  for (const BRepGraph_ProductId aRoot : theGraph.RootProductIds())
  {
    appendNodeUID(aBuilder, theGraph, aRoot);
  }
  return finishOrNull(aBuilder);
}

BRepGraph_RevisionHash storageMetadataHash(const BRepGraph&              theGraph,
                                           const BRepGraphInc_Storage&   theStorage,
                                           const BRepGraph_RevisionHash& theSemanticRoot)
{
  RevisionHashStream aBuilder(0x53544554ull);
  aBuilder.AppendUInt32(1);
  aBuilder.AppendHash(theSemanticRoot);
  for (int aKindIdx = static_cast<int>(BRepGraph_NodeId::Kind::Vertex);
       aKindIdx <= static_cast<int>(BRepGraph_NodeId::Kind::Occurrence);
       ++aKindIdx)
  {
    const BRepGraph_NodeId::Kind aKind = static_cast<BRepGraph_NodeId::Kind>(aKindIdx);
    if (BRepGraph_NodeId::IsValidKind(aKind))
    {
      aBuilder.AppendUInt32(theStorage.NextNodeUIDCounter(aKind));
    }
  }
  for (int aKindIdx = static_cast<int>(BRepGraph_RefId::Kind::Shell);
       aKindIdx <= static_cast<int>(BRepGraph_RefId::Kind::Occurrence);
       ++aKindIdx)
  {
    const BRepGraph_RefId::Kind aKind = static_cast<BRepGraph_RefId::Kind>(aKindIdx);
    if (BRepGraph_RefId::IsValidKind(aKind))
    {
      aBuilder.AppendUInt32(theStorage.NextRefUIDCounter(aKind));
    }
  }
  aBuilder.AppendUInt64(static_cast<uint64_t>(theGraph.RootProductIds().Size()));
  for (const BRepGraph_ProductId aRoot : theGraph.RootProductIds())
  {
    aBuilder.AppendNodeId(aRoot);
  }
  return finishOrNull(aBuilder);
}

uint32_t nodeCount(const BRepGraph& theGraph, const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return theGraph.Topo().Vertices().Nb();
    case BRepGraph_NodeId::Kind::Edge:
      return theGraph.Topo().Edges().Nb();
    case BRepGraph_NodeId::Kind::CoEdge:
      return theGraph.Topo().CoEdges().Nb();
    case BRepGraph_NodeId::Kind::Wire:
      return theGraph.Topo().Wires().Nb();
    case BRepGraph_NodeId::Kind::Face:
      return theGraph.Topo().Faces().Nb();
    case BRepGraph_NodeId::Kind::Shell:
      return theGraph.Topo().Shells().Nb();
    case BRepGraph_NodeId::Kind::Solid:
      return theGraph.Topo().Solids().Nb();
    case BRepGraph_NodeId::Kind::Compound:
      return theGraph.Topo().Compounds().Nb();
    case BRepGraph_NodeId::Kind::CompSolid:
      return theGraph.Topo().CompSolids().Nb();
    case BRepGraph_NodeId::Kind::Product:
      return theGraph.Topo().Products().Nb();
    case BRepGraph_NodeId::Kind::Occurrence:
      return theGraph.Topo().Occurrences().Nb();
  }
  return 0;
}

uint32_t refCount(const BRepGraph& theGraph, const BRepGraph_RefId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return theGraph.Refs().Shells().Nb();
    case BRepGraph_RefId::Kind::Face:
      return theGraph.Refs().Faces().Nb();
    case BRepGraph_RefId::Kind::Wire:
      return theGraph.Refs().Wires().Nb();
    case BRepGraph_RefId::Kind::Vertex:
      return theGraph.Refs().Vertices().Nb();
    case BRepGraph_RefId::Kind::Solid:
      return theGraph.Refs().Solids().Nb();
    case BRepGraph_RefId::Kind::Child:
      return theGraph.Refs().Children().Nb();
    case BRepGraph_RefId::Kind::Occurrence:
      return theGraph.Refs().Occurrences().Nb();
  }
  return 0;
}

} // namespace

//=================================================================================================

class BRepGraph_RevisionHash::Hasher::ByteAccumulator::Data
{
public:
  explicit Data(const size_t theSize)
      : Builder(0x434f4e54ull),
        Remaining(theSize)
  {
    Builder.AppendUInt64(static_cast<uint64_t>(theSize));
  }

  RevisionHashStream Builder;
  size_t             Remaining;
};

//=================================================================================================

BRepGraph_RevisionHash::Hasher::ByteAccumulator::ByteAccumulator(const size_t theSize)
    : myData(new Data(theSize))
{
}

//=================================================================================================

BRepGraph_RevisionHash::Hasher::ByteAccumulator::~ByteAccumulator() = default;

//=================================================================================================

bool BRepGraph_RevisionHash::Hasher::ByteAccumulator::Append(const void*  theData,
                                                             const size_t theSize)
{
  if (!myData || theSize > myData->Remaining || (theSize != 0 && theData == nullptr))
  {
    return false;
  }
  myData->Builder.AppendBytes(theData, theSize);
  if (!myData->Builder.IsValid())
  {
    return false;
  }
  myData->Remaining -= theSize;
  return true;
}

//=================================================================================================

std::optional<BRepGraph_RevisionHash> BRepGraph_RevisionHash::Hasher::ByteAccumulator::Finish()
{
  if (!myData || myData->Remaining != 0)
  {
    return std::nullopt;
  }
  std::optional<BRepGraph_RevisionHash> aResult = myData->Builder.Finish();
  myData.reset();
  return aResult;
}

//=================================================================================================

TCollection_AsciiString BRepGraph_RevisionHash::ToString() const
{
  static constexpr char THE_HEX_DIGITS[] = "0123456789abcdef";
  std::array<char, 65>  aText{};
  size_t                aCharIndex = 0;
  for (const uint64_t aWord : Words)
  {
    for (int aNibble = 15; aNibble >= 0; --aNibble)
    {
      aText[aCharIndex++] = THE_HEX_DIGITS[(aWord >> (aNibble * 4)) & 0xfu];
    }
  }
  return TCollection_AsciiString(aText.data());
}

//=================================================================================================

bool BRepGraph_RevisionHash::Parse(const TCollection_AsciiString& theText,
                                   BRepGraph_RevisionHash&        theHash)
{
  if (theText.Length() != 64)
  {
    return false;
  }

  BRepGraph_RevisionHash aParsed;
  for (int anIndex = 0; anIndex < 4; ++anIndex)
  {
    const char*                  aBegin  = theText.ToCString() + anIndex * 16;
    const char*                  anEnd   = aBegin + 16;
    uint64_t                     aWord   = 0;
    const std::from_chars_result aResult = std::from_chars(aBegin, anEnd, aWord, 16);
    if (aResult.ec != std::errc() || aResult.ptr != anEnd)
    {
      return false;
    }
    aParsed.Words[anIndex] = aWord;
  }
  theHash = aParsed;
  return true;
}

//=================================================================================================

BRepGraph_RevisionHash BRepGraph_RevisionHash::Hasher::Node(const BRepGraph&       theGraph,
                                                            const BRepGraph_NodeId theNode)
{
  if (!theNode.IsValid() || theNode.IsRemoved(theGraph))
  {
    return BRepGraph_RevisionHash();
  }

  RevisionHashStream aBuilder(0x4e4f4445ull);
  appendDefinitionSignature(aBuilder, theGraph, theNode);
  return finishOrNull(aBuilder);
}

//=================================================================================================

BRepGraph_RevisionHash BRepGraph_RevisionHash::Hasher::Reference(const BRepGraph&      theGraph,
                                                                 const BRepGraph_RefId theRef)
{
  if (!theRef.IsValid() || theGraph.Refs().Gen().IsRemoved(theRef))
  {
    return BRepGraph_RevisionHash();
  }

  RevisionHashStream aBuilder(0x52454653ull);
  appendReferenceSignature(aBuilder, theGraph, theRef);
  return finishOrNull(aBuilder);
}

//=================================================================================================

BRepGraph_RevisionHash BRepGraph_RevisionHash::Hasher::Curve(
  const occ::handle<Geom_Curve>& theCurve)
{
  RevisionHashStream aBuilder(0x43555256u); // CURV
  aBuilder.AppendCurve(theCurve);
  return finishOrNull(aBuilder);
}

//=================================================================================================

BRepGraph_RevisionHash BRepGraph_RevisionHash::Hasher::Bytes(const void*  theData,
                                                             const size_t theSize)
{
  if (theSize != 0 && theData == nullptr)
  {
    return BRepGraph_RevisionHash();
  }

  RevisionHashStream aBuilder(0x434f4e54ull);
  aBuilder.AppendUInt64(static_cast<uint64_t>(theSize));
  aBuilder.AppendBytes(theData, theSize);
  return finishOrNull(aBuilder);
}

//=================================================================================================

BRepGraph_RevisionHash::Hasher::Result BRepGraph_RevisionHash::Hasher::Compute(
  const BRepGraph& theGraph)
{
  NCollection_LinearVector<RevisionMerkle::Entry> aSemanticEntries;
  NCollection_LinearVector<RevisionMerkle::Entry> aStorageEntries;

  addNodeLeaves<BRepGraphInc::VertexDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::EdgeDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::CoEdgeDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::WireDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::FaceDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::ShellDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::SolidDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::CompoundDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::CompSolidDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::ProductDef>(theGraph, aSemanticEntries, aStorageEntries);
  addNodeLeaves<BRepGraphInc::OccurrenceDef>(theGraph, aSemanticEntries, aStorageEntries);

  addReferenceLeaves<BRepGraph_ShellRefId>(theGraph,
                                           theGraph.Refs().Shells().Nb(),
                                           aSemanticEntries,
                                           aStorageEntries);
  addReferenceLeaves<BRepGraph_FaceRefId>(theGraph,
                                          theGraph.Refs().Faces().Nb(),
                                          aSemanticEntries,
                                          aStorageEntries);
  addReferenceLeaves<BRepGraph_WireRefId>(theGraph,
                                          theGraph.Refs().Wires().Nb(),
                                          aSemanticEntries,
                                          aStorageEntries);
  addReferenceLeaves<BRepGraph_VertexRefId>(theGraph,
                                            theGraph.Refs().Vertices().Nb(),
                                            aSemanticEntries,
                                            aStorageEntries);
  addReferenceLeaves<BRepGraph_SolidRefId>(theGraph,
                                           theGraph.Refs().Solids().Nb(),
                                           aSemanticEntries,
                                           aStorageEntries);
  addReferenceLeaves<BRepGraph_ChildRefId>(theGraph,
                                           theGraph.Refs().Children().Nb(),
                                           aSemanticEntries,
                                           aStorageEntries);
  addReferenceLeaves<BRepGraph_OccurrenceRefId>(theGraph,
                                                theGraph.Refs().Occurrences().Nb(),
                                                aSemanticEntries,
                                                aStorageEntries);

  aSemanticEntries.Append(
    {merkleKey(MerkleNamespace::SemanticMetadata, 0, 0), semanticMetadataHash(theGraph)});
  RevisionMerkle aSemantic;
  if (!RevisionMerkle::Build(std::move(aSemanticEntries), aSemantic))
  {
    return {};
  }
  const BRepGraph_RevisionHash aSemanticRoot = aSemantic.RootHash();
  aStorageEntries.Append({merkleKey(MerkleNamespace::StorageMetadata, 0, 0),
                          storageMetadataHash(theGraph, theGraph.incStorage(), aSemanticRoot)});
  RevisionMerkle aStorage;
  if (!RevisionMerkle::Build(std::move(aStorageEntries), aStorage))
  {
    return {};
  }
  const std::shared_ptr<const Index> anIndex = std::make_shared<const Index>(aSemantic, aStorage);
  return {aSemanticRoot, aStorage.RootHash(), anIndex};
}

//=================================================================================================

BRepGraph_RevisionHash::Hasher::Result BRepGraph_RevisionHash::Hasher::Update(
  const std::shared_ptr<const Index>&               theBaseIndex,
  const BRepGraph&                                  theBaseGraph,
  const BRepGraph&                                  theResultGraph,
  const NCollection_LinearVector<BRepGraph_UID>&    theChangedNodes,
  const NCollection_LinearVector<BRepGraph_RefUID>& theChangedRefs)
{
  if (theBaseIndex == nullptr)
  {
    return Result();
  }

  NCollection_FlatMap<BRepGraph_UID>    aNodes;
  NCollection_FlatMap<BRepGraph_RefUID> aRefs;
  aNodes.Reserve(theChangedNodes.Size());
  aRefs.Reserve(theChangedRefs.Size());
  for (const BRepGraph_UID& aUID : theChangedNodes)
  {
    if (aUID.IsValid())
    {
      aNodes.Add(aUID);
    }
  }
  for (const BRepGraph_RefUID& aUID : theChangedRefs)
  {
    if (aUID.IsValid())
    {
      aRefs.Add(aUID);
    }
  }
  RevisionMerkle aSemantic = theBaseIndex->Semantic;
  RevisionMerkle aStorage  = theBaseIndex->Storage;
  for (NCollection_FlatMap<BRepGraph_UID>::Iterator anIt(aNodes); anIt.More(); anIt.Next())
  {
    const BRepGraph_UID& aUID = anIt.Key();
    const uint64_t       aSemanticKey =
      merkleKey(MerkleNamespace::SemanticNode, static_cast<uint8_t>(aUID.Kind), aUID.Counter);
    const BRepGraph_NodeId aBaseNode   = theBaseGraph.UIDs().NodeIdFrom(aUID);
    const BRepGraph_NodeId aResultNode = theResultGraph.UIDs().NodeIdFrom(aUID);
    if (aResultNode.IsValid())
    {
      const BRepGraph_RevisionHash aHash = Node(theResultGraph, aResultNode);
      aSemantic                          = aSemantic.Insert(aSemanticKey, aHash);
      aStorage                           = aStorage.Insert(merkleKey(MerkleNamespace::StorageNode,
                                           static_cast<uint8_t>(aResultNode.NodeKind),
                                           aResultNode.Index),
                                 physicalNodeHash(theResultGraph, aResultNode, aHash));
    }
    else
    {
      aSemantic = aSemantic.Remove(aSemanticKey);
    }
    if (aBaseNode.IsValid() && aBaseNode != aResultNode)
    {
      const uint64_t aStorageKey = merkleKey(MerkleNamespace::StorageNode,
                                             static_cast<uint8_t>(aBaseNode.NodeKind),
                                             aBaseNode.Index);
      if (aBaseNode.Index < nodeCount(theResultGraph, aBaseNode.NodeKind))
      {
        aStorage = aStorage.Insert(
          aStorageKey,
          physicalNodeHash(theResultGraph, aBaseNode, Node(theResultGraph, aBaseNode)));
      }
      else
      {
        aStorage = aStorage.Remove(aStorageKey);
      }
    }
  }

  for (NCollection_FlatMap<BRepGraph_RefUID>::Iterator anIt(aRefs); anIt.More(); anIt.Next())
  {
    const BRepGraph_RefUID& aUID = anIt.Key();
    const uint64_t          aSemanticKey =
      merkleKey(MerkleNamespace::SemanticReference, static_cast<uint8_t>(aUID.Kind), aUID.Counter);
    const BRepGraph_RefId aBaseRef   = theBaseGraph.UIDs().RefIdFrom(aUID);
    const BRepGraph_RefId aResultRef = theResultGraph.UIDs().RefIdFrom(aUID);
    if (aResultRef.IsValid())
    {
      const BRepGraph_RevisionHash aHash = Reference(theResultGraph, aResultRef);
      aSemantic                          = aSemantic.Insert(aSemanticKey, aHash);
      aStorage = aStorage.Insert(merkleKey(MerkleNamespace::StorageReference,
                                           static_cast<uint8_t>(aResultRef.RefKind),
                                           aResultRef.Index),
                                 physicalReferenceHash(theResultGraph, aResultRef, aHash));
    }
    else
    {
      aSemantic = aSemantic.Remove(aSemanticKey);
    }
    if (aBaseRef.IsValid() && aBaseRef != aResultRef)
    {
      const uint64_t aStorageKey = merkleKey(MerkleNamespace::StorageReference,
                                             static_cast<uint8_t>(aBaseRef.RefKind),
                                             aBaseRef.Index);
      if (aBaseRef.Index < refCount(theResultGraph, aBaseRef.RefKind))
      {
        aStorage = aStorage.Insert(
          aStorageKey,
          physicalReferenceHash(theResultGraph, aBaseRef, Reference(theResultGraph, aBaseRef)));
      }
      else
      {
        aStorage = aStorage.Remove(aStorageKey);
      }
    }
  }

  aSemantic = aSemantic.Insert(merkleKey(MerkleNamespace::SemanticMetadata, 0, 0),
                               semanticMetadataHash(theResultGraph));
  const BRepGraph_RevisionHash aSemanticRoot = aSemantic.RootHash();
  aStorage                                   = aStorage.Insert(
    merkleKey(MerkleNamespace::StorageMetadata, 0, 0),
    storageMetadataHash(theResultGraph, theResultGraph.incStorage(), aSemanticRoot));
  const std::shared_ptr<const Index> anIndex = std::make_shared<const Index>(aSemantic, aStorage);
  return {aSemanticRoot, aStorage.RootHash(), anIndex};
}

//=================================================================================================

BRepGraph_RevisionHash BRepGraph_RevisionHash::Hasher::Semantic(const BRepGraph& theGraph)
{
  return Compute(theGraph).Semantic;
}

//=================================================================================================

BRepGraph_RevisionHash BRepGraph_RevisionHash::Hasher::Storage(const BRepGraph& theGraph)
{
  return Compute(theGraph).Storage;
}
