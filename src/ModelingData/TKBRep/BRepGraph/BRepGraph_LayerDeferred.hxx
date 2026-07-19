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
#include <NCollection_DataMap.hxx>
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

      RepresentationStorage(const RepresentationStorage& theOther) { copyFrom(theOther); }

      RepresentationStorage(RepresentationStorage&& theOther) noexcept { moveFrom(theOther); }

      Standard_EXPORT RepresentationStorage& operator=(const RepresentationStorage& theOther);
      Standard_EXPORT RepresentationStorage& operator=(RepresentationStorage&& theOther) noexcept;

      ~RepresentationStorage() { Clear(); }

      [[nodiscard]] size_t Size() const { return mySize; }

      [[nodiscard]] bool IsEmpty() const { return mySize == 0; }

      [[nodiscard]] Standard_EXPORT bool ContainsKind(const RepresentationKind theKind) const;

      [[nodiscard]] const Representation& Value(const size_t theIndex) const
      {
        Standard_ASSERT_RAISE(theIndex < mySize, "Deferred representation index is out of range");
        return *representationPtr(theIndex);
      }

      [[nodiscard]] Representation& ChangeValue(const size_t theIndex)
      {
        Standard_ASSERT_RAISE(theIndex < mySize, "Deferred representation index is out of range");
        return *representationPtr(theIndex);
      }

      [[nodiscard]] const Representation& First() const
      {
        Standard_ASSERT_RAISE(mySize > 0, "Deferred representation list is empty");
        return *representationPtr(0);
      }

      Standard_EXPORT void Append(const Representation& theRepresentation);

      Standard_EXPORT void Clear();

    private:
      [[nodiscard]] const Representation* representationPtr(const size_t theIndex) const
      {
        return reinterpret_cast<const Representation*>(&myRepresentationStorage[theIndex]);
      }

      [[nodiscard]] Representation* representationPtr(const size_t theIndex)
      {
        return reinterpret_cast<Representation*>(&myRepresentationStorage[theIndex]);
      }

      Standard_EXPORT void copyFrom(const RepresentationStorage& theOther);
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
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! Return this layer type GUID.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  //! Return deferred entry for an item, or null if none exists.
  [[nodiscard]] Standard_EXPORT const Entry* FindDeferred(const BRepGraph_ItemId theItem) const;

  //! Return deferred entry for a node, or null if none exists.
  [[nodiscard]] const Entry* FindDeferred(const BRepGraph_NodeId theNode) const
  {
    return FindDeferred(BRepGraph_ItemId(theNode));
  }

  //! Return deferred entry for a reference, or null if none exists.
  [[nodiscard]] const Entry* FindDeferred(const BRepGraph_RefId theRef) const
  {
    return FindDeferred(BRepGraph_ItemId(theRef));
  }

  //! Return true if an item has deferred representations.
  [[nodiscard]] Standard_EXPORT bool HasDeferred(const BRepGraph_ItemId theItem) const;

  //! Return true if a node has deferred representations.
  [[nodiscard]] bool HasDeferred(const BRepGraph_NodeId theNode) const
  {
    return HasDeferred(BRepGraph_ItemId(theNode));
  }

  //! Return true if a reference has deferred representations.
  [[nodiscard]] bool HasDeferred(const BRepGraph_RefId theRef) const
  {
    return HasDeferred(BRepGraph_ItemId(theRef));
  }

  //! Register one postponed representation and lock the item.
  Standard_EXPORT void RegisterDeferred(const BRepGraph_ItemId         theItem,
                                        const TCollection_AsciiString& theProvider,
                                        const TCollection_AsciiString& theSourceKey,
                                        const RepresentationKind       theRepresentationKind,
                                        const TCollection_AsciiString& theRepresentationName,
                                        const uint32_t                 theSourceIndex);

  //! Register postponed representations for one item and lock the item once.
  Standard_EXPORT void RegisterDeferredRepresentations(const BRepGraph_ItemId         theItem,
                                                       const TCollection_AsciiString& theProvider,
                                                       const TCollection_AsciiString& theSourceKey,
                                                       const Representation* theRepresentations,
                                                       const size_t          theNbRepresentations);

  //! Register postponed representations for a new item and lock it once.
  //!
  //! This is a trusted bulk-load fast path: the caller must ensure the item is valid,
  //! has no existing deferred entry, and `theRepresentations` contains no duplicates.
  Standard_EXPORT void RegisterDeferredRepresentationsDirect(
    const BRepGraph_ItemId         theItem,
    const TCollection_AsciiString& theProvider,
    const TCollection_AsciiString& theSourceKey,
    const Representation*          theRepresentations,
    const size_t                   theNbRepresentations);

  //! Register one postponed node representation and lock the node.
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
  Standard_EXPORT void UnregisterDeferred(const BRepGraph_ItemId theItem);

  //! Remove all deferred representations for a node and unlock it.
  void UnregisterDeferred(const BRepGraph_NodeId theNode)
  {
    UnregisterDeferred(BRepGraph_ItemId(theNode));
  }

  //! Remove all deferred representations for a reference and unlock it.
  void UnregisterDeferred(const BRepGraph_RefId theRef)
  {
    UnregisterDeferred(BRepGraph_ItemId(theRef));
  }

  //! Return true if at least one item has deferred representations.
  [[nodiscard]] bool HasDeferredItems() const { return myEntries.Extent() != 0; }

  //! Return first deferred entry with at least one representation of the requested kind, or null.
  [[nodiscard]] Standard_EXPORT const Entry* FindFirstDeferred(
    const RepresentationKind theKind,
    BRepGraph_ItemId*        theItem = nullptr) const;

  //! Reserve deferred and lock layer buckets for bulk registration.
  Standard_EXPORT void ReserveDeferredItems(const size_t theNbItems);

  //! Begin bulk deferred registration. Revision updates are postponed until EndBulkRegistration().
  Standard_EXPORT void BeginBulkRegistration();

  //! Finish bulk deferred registration and publish one revision update if anything changed.
  Standard_EXPORT void EndBulkRegistration();

  //! Visit deferred entries. Callback receives item id and an entry copy; returning false stops.
  template <typename VisitorT>
  void ForEachDeferred(VisitorT&& theVisitor) const
  {
    for (NCollection_DataMap<BRepGraph_ItemId, Entry>::Iterator anIt(myEntries); anIt.More();)
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
  template <typename VisitorT>
  void ForEachDeferred(const RepresentationKind theKind, VisitorT&& theVisitor) const
  {
    for (NCollection_DataMap<BRepGraph_ItemId, Entry>::Iterator anIt(myEntries); anIt.More();)
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

  Standard_EXPORT const TCollection_AsciiString& Name() const override;
  Standard_EXPORT void OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept override;
  Standard_EXPORT void OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                      const BRepGraph_NodeId theNewNode) noexcept override;
  Standard_EXPORT void CopyTo(const BRepGraph_CopyRemap& theCopy) const override;
  Standard_EXPORT void OnRefRemoved(const BRepGraph_RefId theRef) noexcept override;
  Standard_EXPORT void InvalidateAll() noexcept override;
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerDeferred, BRepGraph_Layer)

private:
  void removeItem(const BRepGraph_ItemId theItem) noexcept;
  void lockItem(const BRepGraph_ItemId theItem);
  void unlockItem(const BRepGraph_ItemId theItem);

private:
  NCollection_DataMap<BRepGraph_ItemId, Entry> myEntries;
  uint32_t                                     myBulkRegistrationDepth = 0;
  bool                                         myHasBulkChanges        = false;
};

#endif // _BRepGraph_LayerDeferred_HeaderFile
