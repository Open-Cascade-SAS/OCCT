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

#ifndef _BRepGraph_SupplementIterator_HeaderFile
#define _BRepGraph_SupplementIterator_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <NCollection_ForwardRange.hxx>

//! @brief Iterator over supplemental TopoDS attachments owned by one core node.
//!
//! The iterator resolves `BRepGraph_LayerTopoSupplement` through the graph layer
//! registry and yields only explicit supplement attachments. Core traversal
//! remains core-only and does not surface these entries.
class BRepGraph_SupplementIterator
{
public:
  //! @brief Construct an iterator over supplement attachments of one owner.
  //! @param[in] theGraph graph providing the supplement layer
  //! @param[in] theOwner core owner node whose attachments should be iterated
  explicit BRepGraph_SupplementIterator(const BRepGraph& theGraph, const BRepGraph_NodeId theOwner)
      : myLayer(theGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>()),
        myUids(myLayer.IsNull() ? nullptr : &myLayer->AttachedTo(theOwner))
  {
    skipInvalid();
  }

  //! @brief Return true when the iterator currently points to an attachment.
  [[nodiscard]] bool More() const
  {
    return myUids != nullptr && myEntry != nullptr && myIndex < myUids->Size();
  }

  //! @brief Advance to the next attachment.
  void Next()
  {
    ++myIndex;
    skipInvalid();
  }

  //! @brief Return the current layer-local attachment uid.
  [[nodiscard]] uint64_t Uid() const { return More() ? myUids->Value(myIndex) : uint64_t(0); }

  //! @brief Return the current attachment entry.
  [[nodiscard]] const BRepGraph_LayerTopoSupplement::Entry& Value() const { return *myEntry; }

  //! @brief STL range-for support.
  NCollection_ForwardRangeIterator<BRepGraph_SupplementIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_SupplementIterator>(this);
  }

  //! @brief Sentinel marking end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  //! @brief Skip missing entries in owner-local uid storage.
  Standard_EXPORT void skipInvalid();

private:
  occ::handle<BRepGraph_LayerTopoSupplement>  myLayer;
  const NCollection_LinearVector<uint64_t>*   myUids  = nullptr;
  const BRepGraph_LayerTopoSupplement::Entry* myEntry = nullptr;
  size_t                                      myIndex = 0;
};

#endif // _BRepGraph_SupplementIterator_HeaderFile
