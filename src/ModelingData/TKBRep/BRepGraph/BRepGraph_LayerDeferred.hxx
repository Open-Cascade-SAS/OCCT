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

#ifndef _BRepGraph_LayerDeferred_HeaderFile
#define _BRepGraph_LayerDeferred_HeaderFile

#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_Layer.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <Standard_Assert.hxx>
#include <TCollection_AsciiString.hxx>

#include <limits>
#include <new>
#include <type_traits>
#include <utility>

//! Base layer for postponed graph item loading.
//!
//! The layer stores provider-neutral deferred representation records and owns the lock
//! state through BRepGraph_LayerLock. Format-specific loaders, such as ODE or
//! STEP, should derive from this class or use the same representation contract rather
//! than storing deferred ownership in topology definitions.
class BRepGraph_LayerDeferred : public BRepGraph_Layer
{
public:
  //! Constructor for generic deferred layers.
  Standard_EXPORT BRepGraph_LayerDeferred();

  //! Representation category.
  enum class RepresentationKind
  {
    Unknown,
    Geometry,
    Mesh,
    Topology,
    Assembly,
    Parametric
  };

  //! One postponed representation attached to a graph item.
  struct Representation
  {
    static constexpr uint32_t THE_INVALID_SOURCE_INDEX = std::numeric_limits<uint32_t>::max();

    TCollection_AsciiString Provider;
    TCollection_AsciiString SourceKey;
    RepresentationKind      Kind = RepresentationKind::Unknown;
    uint32_t                Role = 0;
    TCollection_AsciiString Name;
    uint32_t                SourceIndex = THE_INVALID_SOURCE_INDEX;
  };

  //! Deferred ownership entry for one graph item.
  struct Entry
  {
    //! Small fixed representation list. Deferred graph items have a bounded number of
    //! persisted representations, so avoid one heap allocation per item.
    class RepresentationStorage
    {
    public:
      static constexpr size_t THE_MAX_REPRESENTATIONS_PER_ITEM = 8;

      RepresentationStorage() = default;

      //! Copy fixed representation storage.
      //! @param[in] theOther source storage
      RepresentationStorage(const RepresentationStorage& theOther) { copyFrom(theOther); }

      //! Move fixed representation storage.
      //! @param[in,out] theOther source storage
      RepresentationStorage(RepresentationStorage&& theOther) noexcept { moveFrom(theOther); }

      //! Copy fixed representation storage.
      //! @param[in] theOther source storage
      //! @return this storage.
      Standard_EXPORT RepresentationStorage& operator=(const RepresentationStorage& theOther);

      //! Move fixed representation storage.
      //! @param[in,out] theOther source storage
      //! @return this storage.
      Standard_EXPORT RepresentationStorage& operator=(RepresentationStorage&& theOther) noexcept;

      ~RepresentationStorage() { Clear(); }

      //! Return number of stored representations.
      //! @return number of stored representations.
      [[nodiscard]] size_t Size() const { return mySize; }

      //! Check whether no representation is stored.
      //! @return true if no representation is stored.
      [[nodiscard]] bool IsEmpty() const { return mySize == 0; }

      //! Check whether at least one stored representation has the requested kind.
      //! @param[in] theKind representation kind to search for
      //! @return true if at least one stored representation has the requested kind.
      [[nodiscard]] Standard_EXPORT bool ContainsKind(const RepresentationKind theKind) const;

      //! Return representation by zero-based index.
      //! @param[in] theIndex representation index
      //! @return stored representation.
      [[nodiscard]] const Representation& Value(const size_t theIndex) const
      {
        Standard_ASSERT_RAISE(theIndex < mySize, "Deferred representation index is out of range");
        return *representationPtr(theIndex);
      }

      //! Return mutable representation by zero-based index.
      //! @param[in] theIndex representation index
      //! @return mutable stored representation.
      [[nodiscard]] Representation& ChangeValue(const size_t theIndex)
      {
        Standard_ASSERT_RAISE(theIndex < mySize, "Deferred representation index is out of range");
        return *representationPtr(theIndex);
      }

      //! Return first stored representation.
      //! @return first stored representation.
      [[nodiscard]] const Representation& First() const
      {
        Standard_ASSERT_RAISE(mySize > 0, "Deferred representation list is empty");
        return *representationPtr(0);
      }

      //! Append a representation.
      //! @param[in] theRepresentation representation to append
      Standard_EXPORT void Append(const Representation& theRepresentation);

      //! Remove all stored representations.
      Standard_EXPORT void Clear();

    private:
      //! Return pointer to stored representation.
      //! @param[in] theIndex representation index
      //! @return pointer to stored representation.
      [[nodiscard]] const Representation* representationPtr(const size_t theIndex) const
      {
        return reinterpret_cast<const Representation*>(&myRepresentationStorage[theIndex]);
      }

      //! Return pointer to mutable stored representation.
      //! @param[in] theIndex representation index
      //! @return pointer to mutable stored representation.
      [[nodiscard]] Representation* representationPtr(const size_t theIndex)
      {
        return reinterpret_cast<Representation*>(&myRepresentationStorage[theIndex]);
      }

      //! Copy representation storage from another instance.
      //! @param[in] theOther source storage
      Standard_EXPORT void copyFrom(const RepresentationStorage& theOther);

      //! Move representation storage from another instance.
      //! @param[in,out] theOther source storage
      Standard_EXPORT void moveFrom(RepresentationStorage& theOther);

      using RepresentationSlot =
        std::aligned_storage_t<sizeof(Representation), alignof(Representation)>;

      RepresentationSlot myRepresentationStorage[THE_MAX_REPRESENTATIONS_PER_ITEM];
      size_t             mySize = 0;
    };

    TCollection_AsciiString Provider;
    TCollection_AsciiString SourceKey;
    RepresentationStorage   Representations;
  };

  //! Return fixed layer type GUID.
  //! @return fixed layer type GUID.
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! Return this layer type GUID.
  //! @return this layer type GUID.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  //! Return deferred entry for an item, or null if none exists.
  //! @param[in] theItem graph item
  //! @return deferred entry, or null if none exists.
  [[nodiscard]] Standard_EXPORT const Entry* FindDeferred(const BRepGraph_ItemId theItem) const;

  //! Return deferred entry for a node, or null if none exists.
  //! @param[in] theNode graph node
  //! @return deferred entry, or null if none exists.
  [[nodiscard]] const Entry* FindDeferred(const BRepGraph_NodeId theNode) const
  {
    return FindDeferred(BRepGraph_ItemId(theNode));
  }

  //! Return deferred entry for a reference, or null if none exists.
  //! @param[in] theRef graph reference
  //! @return deferred entry, or null if none exists.
  [[nodiscard]] const Entry* FindDeferred(const BRepGraph_RefId theRef) const
  {
    return FindDeferred(BRepGraph_ItemId(theRef));
  }

  //! Check whether an item has deferred representations.
  //! @param[in] theItem graph item
  //! @return true if an item has deferred representations.
  [[nodiscard]] Standard_EXPORT bool HasDeferred(const BRepGraph_ItemId theItem) const;

  //! Check whether a node has deferred representations.
  //! @param[in] theNode graph node
  //! @return true if a node has deferred representations.
  [[nodiscard]] bool HasDeferred(const BRepGraph_NodeId theNode) const
  {
    return HasDeferred(BRepGraph_ItemId(theNode));
  }

  //! Check whether a reference has deferred representations.
  //! @param[in] theRef graph reference
  //! @return true if a reference has deferred representations.
  [[nodiscard]] bool HasDeferred(const BRepGraph_RefId theRef) const
  {
    return HasDeferred(BRepGraph_ItemId(theRef));
  }

  //! Register one postponed representation and lock the item.
  //! @param[in] theItem graph item
  //! @param[in] theProvider deferred representation provider
  //! @param[in] theSourceKey provider-specific source key
  //! @param[in] theRepresentationKind representation kind
  //! @param[in] theRepresentationName representation name
  //! @param[in] theSourceIndex source representation index
  Standard_EXPORT void RegisterDeferred(const BRepGraph_ItemId         theItem,
                                        const TCollection_AsciiString& theProvider,
                                        const TCollection_AsciiString& theSourceKey,
                                        const RepresentationKind       theRepresentationKind,
                                        const TCollection_AsciiString& theRepresentationName,
                                        const uint32_t                 theSourceIndex);

  //! Register postponed representations for one item and lock the item once.
  //! @param[in] theItem graph item
  //! @param[in] theProvider deferred representation provider
  //! @param[in] theSourceKey provider-specific source key
  //! @param[in] theRepresentations representation array
  //! @param[in] theNbRepresentations number of representations
  Standard_EXPORT void RegisterDeferredRepresentations(const BRepGraph_ItemId         theItem,
                                                       const TCollection_AsciiString& theProvider,
                                                       const TCollection_AsciiString& theSourceKey,
                                                       const Representation* theRepresentations,
                                                       const size_t          theNbRepresentations);

  //! Register postponed representations for a new item and lock it once.
  //!
  //! This is a trusted bulk-load fast path: the caller must ensure the item is valid,
  //! has no existing deferred entry, and `theRepresentations` contains no duplicates.
  //! @param[in] theItem graph item
  //! @param[in] theProvider deferred representation provider
  //! @param[in] theSourceKey provider-specific source key
  //! @param[in] theRepresentations representation array
  //! @param[in] theNbRepresentations number of representations
  Standard_EXPORT void RegisterDeferredRepresentationsDirect(
    const BRepGraph_ItemId         theItem,
    const TCollection_AsciiString& theProvider,
    const TCollection_AsciiString& theSourceKey,
    const Representation*          theRepresentations,
    const size_t                   theNbRepresentations);

  //! Register one postponed node representation and lock the node.
  //! @param[in] theNode graph node
  //! @param[in] theProvider deferred representation provider
  //! @param[in] theSourceKey provider-specific source key
  //! @param[in] theRepresentationKind representation kind
  //! @param[in] theRepresentationName representation name
  //! @param[in] theSourceIndex source representation index
  void RegisterDeferred(const BRepGraph_NodeId         theNode,
                        const TCollection_AsciiString& theProvider,
                        const TCollection_AsciiString& theSourceKey,
                        const RepresentationKind       theRepresentationKind,
                        const TCollection_AsciiString& theRepresentationName,
                        const uint32_t                 theSourceIndex)
  {
    RegisterDeferred(BRepGraph_ItemId(theNode),
                     theProvider,
                     theSourceKey,
                     theRepresentationKind,
                     theRepresentationName,
                     theSourceIndex);
  }

  //! Register one postponed reference representation and lock the reference.
  //! @param[in] theRef graph reference
  //! @param[in] theProvider deferred representation provider
  //! @param[in] theSourceKey provider-specific source key
  //! @param[in] theRepresentationKind representation kind
  //! @param[in] theRepresentationName representation name
  //! @param[in] theSourceIndex source representation index
  void RegisterDeferred(const BRepGraph_RefId          theRef,
                        const TCollection_AsciiString& theProvider,
                        const TCollection_AsciiString& theSourceKey,
                        const RepresentationKind       theRepresentationKind,
                        const TCollection_AsciiString& theRepresentationName,
                        const uint32_t                 theSourceIndex)
  {
    RegisterDeferred(BRepGraph_ItemId(theRef),
                     theProvider,
                     theSourceKey,
                     theRepresentationKind,
                     theRepresentationName,
                     theSourceIndex);
  }

  //! Remove all deferred representations for an item and unlock it.
  //! @param[in] theItem graph item
  Standard_EXPORT void UnregisterDeferred(const BRepGraph_ItemId theItem);

  //! Remove all deferred representations for a node and unlock it.
  //! @param[in] theNode graph node
  void UnregisterDeferred(const BRepGraph_NodeId theNode)
  {
    UnregisterDeferred(BRepGraph_ItemId(theNode));
  }

  //! Remove all deferred representations for a reference and unlock it.
  //! @param[in] theRef graph reference
  void UnregisterDeferred(const BRepGraph_RefId theRef)
  {
    UnregisterDeferred(BRepGraph_ItemId(theRef));
  }

  //! Check whether at least one item has deferred representations.
  //! @return true if at least one item has deferred representations.
  [[nodiscard]] bool HasDeferredItems() const { return !myEntries.IsEmpty(); }

  //! Return first deferred entry with at least one representation of the requested kind, or null.
  //! @param[in] theKind requested representation kind
  //! @param[out] theItem receives owning item id when non-null and an entry is found
  //! @return deferred entry, or null if none exists.
  [[nodiscard]] Standard_EXPORT const Entry* FindFirstDeferred(
    const RepresentationKind theKind,
    BRepGraph_ItemId*        theItem = nullptr) const;

  //! Reserve deferred and lock layer buckets for bulk registration.
  //! @param[in] theNbItems number of expected deferred items
  Standard_EXPORT void ReserveDeferredItems(const size_t theNbItems);

  //! Begin bulk deferred registration. Generation updates are postponed until EndBulkRegistration().
  Standard_EXPORT void BeginBulkRegistration();

  //! Finish bulk deferred registration and publish one generation update if anything changed.
  Standard_EXPORT void EndBulkRegistration();

  //! Visit deferred entries. Callback receives item id and an entry copy; returning false stops.
  //! @param[in] theVisitor visitor callable
  template <typename VisitorT>
  void ForEachDeferred(VisitorT&& theVisitor) const
  {
    for (NCollection_FlatDataMap<BRepGraph_ItemId, Entry>::Iterator anIt(myEntries); anIt.More();)
    {
      const BRepGraph_ItemId anItem  = anIt.Key();
      const Entry            anEntry = anIt.Value();
      anIt.Next();
      if (!theVisitor(anItem, anEntry))
      {
        break;
      }
    }
  }

  //! Visit deferred entries with at least one representation of the requested kind.
  //! Callback receives item id and an entry copy; returning false stops.
  //! @param[in] theKind requested representation kind
  //! @param[in] theVisitor visitor callable
  template <typename VisitorT>
  void ForEachDeferred(const RepresentationKind theKind, VisitorT&& theVisitor) const
  {
    for (NCollection_FlatDataMap<BRepGraph_ItemId, Entry>::Iterator anIt(myEntries); anIt.More();)
    {
      if (!anIt.Value().Representations.ContainsKind(theKind))
      {
        anIt.Next();
        continue;
      }

      const BRepGraph_ItemId anItem  = anIt.Key();
      const Entry            anEntry = anIt.Value();
      anIt.Next();
      if (!theVisitor(anItem, anEntry))
      {
        break;
      }
    }
  }

  //! Return layer type name.
  //! @return layer type name.
  Standard_EXPORT const TCollection_AsciiString& Name() const override;

  //! Remove deferred data for a removed node.
  //! @param[in] theNode removed node
  Standard_EXPORT void OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept override;

  //! Migrate deferred data after node replacement.
  //! @param[in] theOldNode replaced node
  //! @param[in] theNewNode replacement node
  Standard_EXPORT void OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                      const BRepGraph_NodeId theNewNode) noexcept override;

  //! Copy deferred data to the target graph.
  //! @param[in] theCopy source, target, and item remap context
  Standard_EXPORT void CopyTo(const BRepGraph_CopyRemap& theCopy) const override;

  //! Remove deferred data for a removed reference.
  //! @param[in] theRef removed reference
  Standard_EXPORT void OnRefRemoved(const BRepGraph_RefId theRef) noexcept override;

  //! Mark all deferred entries dirty.
  Standard_EXPORT void InvalidateAll() noexcept override;

  //! Remove all deferred entries.
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerDeferred, BRepGraph_Layer)

private:
  //! Remove deferred data for an item.
  //! @param[in] theItem graph item
  void removeItem(const BRepGraph_ItemId theItem) noexcept;

  //! Lock a deferred item.
  //! @param[in] theItem graph item
  void lockItem(const BRepGraph_ItemId theItem);

  //! Unlock a deferred item.
  //! @param[in] theItem graph item
  void unlockItem(const BRepGraph_ItemId theItem);

private:
  NCollection_FlatDataMap<BRepGraph_ItemId, Entry> myEntries;
  uint32_t                                         myBulkRegistrationDepth = 0;
  bool                                             myHasBulkChanges        = false;
};

#endif // _BRepGraph_LayerDeferred_HeaderFile
