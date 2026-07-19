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

#ifndef _BRepGraph_LayerHistory_HeaderFile
#define _BRepGraph_LayerHistory_HeaderFile

#include <BRepGraph_Layer.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ItemUID.hxx>
#include <BRepGraph_UID.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>

#include <cstdint>

class BRepGraph;
class BRepTools_History;

//! History layer for BRepGraph.
//!
//! BRepGraph_LayerHistory maintains an append-only log of modification events
//! and per-kind lookup maps for efficient queries.  Four event kinds are
//! tracked (see #BRepGraph_LayerHistory::Kind):
//!   - **Modified**: input -> { modified images } (default).
//!   - **Generated**: input -> { generated images } (new entities born
//!     from the input but not sharing its identity).
//!   - **Deleted**: input has been consumed and has no image in the
//!     result.
//!   - **Replaced**: input was structurally detached and replaced by another
//!     node; this maps as Modified and also marks the input as deleted.
//!
//! Recording can be toggled on/off at runtime.  Graph-owned history is registered
//! as a layer and accessed through #Ensure / #Find; algorithms wrapping OCCT's
//! `BRepTools_History` can import results through #Absorb.
class BRepGraph_LayerHistory : public BRepGraph_Layer
{
public:
  //! Classification of a history event.
  enum class Kind : std::uint8_t
  {
    Modified  = 0, //!< Default; input persists into the result(s).
    Generated = 1, //!< Output entity is freshly produced from the input.
    Deleted   = 2, //!< Input has no image in the result.
    Replaced  = 3  //!< Input was detached and continued by replacement(s).
  };

  //! One atomic modification event recorded in the graph's history log.
  struct Event
  {
    Event() = default;

    TCollection_AsciiString OperationName;
    size_t                  SequenceNumber = 0;
    Kind                    RecordKind     = Kind::Modified;

    //! Key: original node id before the operation.
    //! Value: sequence of replacement node ids after the operation.
    NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>> Mapping;

    //! UID-keyed mapping for cross-graph history records.
    NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>> UidMapping;

    //! ItemUID-keyed mapping for durable all-domain history records.
    NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>
      ItemUidMapping;

    //! Optional diagnostic representation.
    TCollection_AsciiString ExtraInfo;
  };

  //! Default constructor.
  Standard_EXPORT BRepGraph_LayerHistory();

  //! Stable layer GUID.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Layer type identity.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  //! Layer display name.
  [[nodiscard]] Standard_EXPORT const TCollection_AsciiString& Name() const override;

  //! Record a modification: theOriginal was replaced by theReplacements.
  //!
  //! @note When @p theReplacements is empty the record is auto-downgraded to
  //!       Kind::Deleted and @p theOriginal is added to the deleted set,
  //!       regardless of @p theKind.  Use #RecordDeleted directly for the
  //!       deletion case to avoid relying on this implicit conversion.
  //! @param[in] theOpLabel      human-readable operation name
  //! @param[in] theOriginal     node id before the operation
  //! @param[in] theReplacements node ids after the operation
  //! @param[in] theKind         classification of this record (default Modified)
  Standard_EXPORT void Record(
    const TCollection_AsciiString&              theOpLabel,
    const BRepGraph_NodeId                      theOriginal,
    const NCollection_Array1<BRepGraph_NodeId>& theReplacements,
    const BRepGraph_LayerHistory::Kind          theKind = BRepGraph_LayerHistory::Kind::Modified);

  //! Record a batch of 1-to-1 modifications in a single history event.
  //! Each original is paired with the replacement at the same logical position.
  //! More efficient than calling Record() in a loop: creates one HistoryRecord
  //! and updates the per-kind maps with minimal overhead.
  //! @param[in] theOpLabel      human-readable operation name
  //! @param[in] theOriginals    node ids before the operation
  //! @param[in] theReplacements node ids after the operation (same length)
  //! @param[in] theExtraInfo    optional diagnostic info stored on the record
  //! @param[in] theKind         classification of this record (default Modified)
  Standard_EXPORT void RecordBatch(
    const TCollection_AsciiString&              theOpLabel,
    const NCollection_Array1<BRepGraph_NodeId>& theOriginals,
    const NCollection_Array1<BRepGraph_NodeId>& theReplacements,
    const TCollection_AsciiString&              theExtraInfo = TCollection_AsciiString(),
    const BRepGraph_LayerHistory::Kind          theKind = BRepGraph_LayerHistory::Kind::Modified);

  //! Record that a collection of inputs has been consumed by the operation
  //! and has no image in the result.  Each input is appended to the
  //! deleted set and emits a single audit record with empty replacements.
  //! @param[in] theOpLabel human-readable operation name
  //! @param[in] theDeleted node ids that have been removed
  Standard_EXPORT void RecordDeleted(const TCollection_AsciiString&              theOpLabel,
                                     const NCollection_Array1<BRepGraph_NodeId>& theDeleted);

  //! Record replacements: each original is logically removed/detached and
  //! continued by the corresponding replacement.  Replaced records participate
  //! in modified-image queries and also mark originals as deleted.
  Standard_EXPORT void RecordReplaced(const TCollection_AsciiString& theOpLabel,
                                      const BRepGraph_NodeId         theOriginal,
                                      const BRepGraph_NodeId         theReplacement);

  //! Record a batch of 1-to-1 replacements in a single history event.
  Standard_EXPORT void RecordReplacedBatch(
    const TCollection_AsciiString&              theOpLabel,
    const NCollection_Array1<BRepGraph_NodeId>& theOriginals,
    const NCollection_Array1<BRepGraph_NodeId>& theReplacements,
    const TCollection_AsciiString&              theExtraInfo = TCollection_AsciiString());

  //! Record a UID-keyed modification/generation event.
  //!
  //! This is the durable-history path for operations whose source and result
  //! identities may live in different BRepGraph instances.  Existing NodeId
  //! records remain available for in-graph algorithms; UID records are queried
  //! directly by cross-graph consumers.
  Standard_EXPORT void RecordUid(
    const TCollection_AsciiString&           theOpLabel,
    const BRepGraph_UID&                     theOriginal,
    const NCollection_Array1<BRepGraph_UID>& theReplacements,
    const BRepGraph_LayerHistory::Kind       theKind = BRepGraph_LayerHistory::Kind::Modified);

  //! Record UID-keyed deletions.
  Standard_EXPORT void RecordDeletedUid(const TCollection_AsciiString&           theOpLabel,
                                        const NCollection_Array1<BRepGraph_UID>& theDeleted);

  //! Record an all-domain ItemUID-keyed modification/generation event.
  Standard_EXPORT void RecordItemUid(
    const TCollection_AsciiString&               theOpLabel,
    const BRepGraph_ItemUID&                     theOriginal,
    const NCollection_Array1<BRepGraph_ItemUID>& theReplacements,
    const BRepGraph_LayerHistory::Kind           theKind = BRepGraph_LayerHistory::Kind::Modified);

  //! Record ItemUID-keyed deletions.
  Standard_EXPORT void RecordDeletedItemUid(
    const TCollection_AsciiString&               theOpLabel,
    const NCollection_Array1<BRepGraph_ItemUID>& theDeleted);

  //! Import a BRepTools_History into this graph-native history log.
  //!
  //! Iterates @p theInputs, queries @p theSource for Modified / Generated /
  //! IsRemoved, translates each TopoDS_Shape image to a NodeId via
  //! @p theOutputs, and emits the corresponding records.
  //!
  //! Semantics:
  //!   - For every input shape whose Modified() list is non-empty:
  //!     emit a Modified record.
  //!   - For every input shape whose Generated() list is non-empty:
  //!     emit a Generated record.
  //!   - For every input shape with IsRemoved() == true: accumulate into
  //!     a single Deleted record (IsRemoved takes precedence over
  //!     Modified/Generated to handle a known OCCT bug where a shape can
  //!     appear in both the removed set and the generated map).
  //!
  //! Output TopoDS_Shapes that do not appear in @p theOutputs are silently
  //! dropped (expected for subshapes merged into a parent compound whose
  //! identity is preserved at a higher level).
  //!
  //! @param[in] theInputs  TopoDS_Shape -> NodeId for every input subshape
  //!                        that should be tracked
  //! @param[in] theOutputs TopoDS_Shape -> NodeId for every subshape added
  //!                        to the graph by this operation (typically from
  //!                        BRepGraph::ShapesView::Add with TrackAddedNodes)
  //! @param[in] theSource  BRepTools_History from the OCCT algorithm.
  //!                        Null is accepted (no-op).
  //! @param[in] theOpLabel record label written into every emitted record
  Standard_EXPORT void Absorb(
    const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theInputs,
    const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theOutputs,
    const occ::handle<BRepTools_History>&                                               theSource,
    const TCollection_AsciiString&                                                      theOpLabel);

  //! Import a BRepTools_History using persistent UIDs from source/result graphs.
  //!
  //! This overload is the canonical bridge for cross-graph algorithms: input
  //! shapes are resolved in @p theInputGraph, output shapes are resolved in
  //! @p theOutputGraph, and the resulting history is stored by UID.
  Standard_EXPORT void Absorb(
    const BRepGraph& theInputGraph,
    const BRepGraph& theOutputGraph,
    const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theInputs,
    const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theOutputs,
    const occ::handle<BRepTools_History>&                                               theSource,
    const TCollection_AsciiString&                                                      theOpLabel);

  //! Walk backwards from a modified node to its original.
  //! Follows the reverse map recursively until a root is reached.
  //! @param[in] theModified node id to trace back
  //! @return the root original node id, or theModified itself if not found
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    FindOriginal(const BRepGraph_NodeId theModified) const;

  //! Walk forwards from an original node to all derived nodes, including
  //! both Modified and Generated descendants.  Follows the forward maps
  //! recursively, collecting every transitively-reachable descendant
  //! (intermediate nodes and leaves alike, but not @p theOriginal itself).
  //! @param[in] theOriginal node id to trace forward
  //! @return all transitively derived node ids in breadth-first order
  [[nodiscard]] Standard_EXPORT NCollection_LinearVector<BRepGraph_NodeId> FindDerived(
    const BRepGraph_NodeId theOriginal) const;

  //! Direct lookup of the Modified images of @p theOriginal, non-recursive.
  //! @param[in] theOriginal node id to query
  //! @return pointer to the stored vector, or nullptr if @p theOriginal has
  //!         no Modified record (note: nullptr does not imply IsDeleted).
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_NodeId>* FindModified(
    const BRepGraph_NodeId theOriginal) const;

  //! Direct lookup of the Generated images of @p theOriginal, non-recursive.
  //! @param[in] theOriginal node id to query
  //! @return pointer to the stored vector, or nullptr if @p theOriginal has
  //!         no Generated record.
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_NodeId>* FindGenerated(
    const BRepGraph_NodeId theOriginal) const;

  //! Test whether @p theOriginal was deleted by some recorded operation.
  //! @param[in] theOriginal node id to query
  //! @return true if @p theOriginal is in the deleted set
  [[nodiscard]] Standard_EXPORT bool IsDeleted(const BRepGraph_NodeId theOriginal) const;

  //! Borrowed access to the full deleted set.
  //! @return reference to the deleted-node set
  [[nodiscard]] Standard_EXPORT const NCollection_FlatMap<BRepGraph_NodeId>& DeletedNodes() const;

  //! UID-keyed Modified images stored directly in this history.
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_UID>* FindModified(
    const BRepGraph_UID& theUID) const;

  //! Direct lookup of all immediate node origins of @p theDerived.
  //! A derived entity can have more than one parent in reconstructive algorithms.
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_NodeId>* FindOriginals(
    const BRepGraph_NodeId theDerived) const;

  //! UID-keyed Generated images stored directly in this history.
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_UID>* FindGenerated(
    const BRepGraph_UID& theUID) const;

  //! UID-keyed deletion test stored directly in this history.
  [[nodiscard]] Standard_EXPORT bool IsDeleted(const BRepGraph_UID& theUID) const;

  //! UID-keyed deleted set stored directly in this history.
  [[nodiscard]] Standard_EXPORT const NCollection_FlatMap<BRepGraph_UID>& DeletedUids() const;

  //! Test whether @p theUID was registered as an operation input.
  [[nodiscard]] Standard_EXPORT bool HasKnownInput(const BRepGraph_UID& theUID) const;

  //! ItemUID-keyed Modified images stored directly in this history.
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_ItemUID>* FindModified(
    const BRepGraph_ItemUID& theUID) const;

  //! ItemUID-keyed Generated images stored directly in this history.
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_ItemUID>* FindGenerated(
    const BRepGraph_ItemUID& theUID) const;

  //! ItemUID-keyed deletion test stored directly in this history.
  [[nodiscard]] Standard_EXPORT bool IsDeleted(const BRepGraph_ItemUID& theUID) const;

  //! ItemUID-keyed deleted set stored directly in this history.
  [[nodiscard]] Standard_EXPORT const NCollection_FlatMap<BRepGraph_ItemUID>& DeletedItemUids()
    const;

  //! Test whether @p theUID was registered as an operation input.
  [[nodiscard]] Standard_EXPORT bool HasKnownInput(const BRepGraph_ItemUID& theUID) const;

  //! UID-keyed convenience: Modified images of the input identified by
  //! @p theUID, resolved against @p theGraph.  Returns an empty vector if
  //! the UID cannot be resolved or has no Modified record.
  //! @param[in] theGraph graph used to translate UID <-> NodeId
  //! @param[in] theUID   UID of the input entity
  //! @return UIDs of the modified images (in record-insertion order)
  [[nodiscard]] Standard_EXPORT NCollection_LinearVector<BRepGraph_UID> FindModified(
    const BRepGraph&     theGraph,
    const BRepGraph_UID& theUID) const;

  //! UID-keyed convenience: Generated images.  See #FindModified for the
  //! resolution contract.
  //! @param[in] theGraph graph used to translate UID <-> NodeId
  //! @param[in] theUID   UID of the input entity
  //! @return UIDs of the generated images (in record-insertion order)
  [[nodiscard]] Standard_EXPORT NCollection_LinearVector<BRepGraph_UID> FindGenerated(
    const BRepGraph&     theGraph,
    const BRepGraph_UID& theUID) const;

  //! UID-keyed convenience: deletion test.
  //! @param[in] theGraph graph used to resolve the UID
  //! @param[in] theUID   UID of the input entity
  //! @return true if the resolved NodeId is in the deleted set
  [[nodiscard]] Standard_EXPORT bool IsDeleted(const BRepGraph&     theGraph,
                                               const BRepGraph_UID& theUID) const;

  //! UID-keyed convenience: dump the full deleted set as UIDs.
  //! @param[in] theGraph graph used to translate NodeId -> UID
  //! @return UIDs of all deleted entities (insertion order is not stable)
  [[nodiscard]] Standard_EXPORT NCollection_LinearVector<BRepGraph_UID> DeletedUids(
    const BRepGraph& theGraph) const;

  //! Number of recorded history events.
  //! @return record count
  [[nodiscard]] Standard_EXPORT size_t NbRecords() const;

  //! Access a record by index (0-based).
  //! @param[in] theRecordIdx zero-based index into the records vector
  //! @return the history record at the given index
  [[nodiscard]] Standard_EXPORT const Event& Record(const size_t theRecordIdx) const;

  //! Enable or disable history recording.
  //! @param[in] theVal true to enable, false to disable
  Standard_EXPORT void SetEnabled(const bool theVal);

  //! Query whether history recording is enabled.
  //! @return true if recording is active
  [[nodiscard]] Standard_EXPORT bool IsEnabled() const;

  //! Clear all records and lookup maps.
  Standard_EXPORT void Clear() noexcept override;

  //! Layer removal callback. Records pure graph deletions when enabled.
  Standard_EXPORT void OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept override;

  //! Copy history records whose source items have copied target items.
  Standard_EXPORT void CopyTo(const BRepGraph_CopyRemap& theCopy) const override;

  //! Clear derived caches by dropping collected history.
  Standard_EXPORT void InvalidateAll() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerHistory, BRepGraph_Layer)

private:
  //! Rebuild all lookup caches from myRecords.
  void rebuildCaches();

  NCollection_DynamicArray<Event> myRecords;

  //! Full reverse map: derived node -> all immediate original nodes.
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>
    myDerivedToOriginals;

  //! Forward map: original node -> vector of Modified images.
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>
    myOriginalToModified;

  //! Forward map: original node -> vector of Generated images.
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_NodeId>>
    myOriginalToGenerated;

  //! Flat set of inputs that have been consumed (no image in the result).
  NCollection_FlatMap<BRepGraph_NodeId> myDeleted;

  //! UID-keyed forward map: original UID -> Modified image UIDs.
  NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>
    myUidOriginalToModified;

  //! UID-keyed forward map: original UID -> Generated image UIDs.
  NCollection_DataMap<BRepGraph_UID, NCollection_LinearVector<BRepGraph_UID>>
    myUidOriginalToGenerated;

  //! UID-keyed operation inputs, including inputs with no images.
  NCollection_FlatMap<BRepGraph_UID> myUidKnownInputs;

  //! UID-keyed consumed inputs.
  NCollection_FlatMap<BRepGraph_UID> myUidDeleted;

  //! ItemUID-keyed forward map: original UID -> Modified image UIDs.
  NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>
    myItemUidOriginalToModified;

  //! ItemUID-keyed forward map: original UID -> Generated image UIDs.
  NCollection_DataMap<BRepGraph_ItemUID, NCollection_LinearVector<BRepGraph_ItemUID>>
    myItemUidOriginalToGenerated;

  //! ItemUID-keyed operation inputs, including inputs with no images.
  NCollection_FlatMap<BRepGraph_ItemUID> myItemUidKnownInputs;

  //! ItemUID-keyed consumed inputs.
  NCollection_FlatMap<BRepGraph_ItemUID> myItemUidDeleted;

  bool myEnabled = true;
};

#endif // _BRepGraph_LayerHistory_HeaderFile
