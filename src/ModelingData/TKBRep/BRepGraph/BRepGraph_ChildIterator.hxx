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

#ifndef _BRepGraph_ChildIterator_HeaderFile
#define _BRepGraph_ChildIterator_HeaderFile

#include <BRepGraph.hxx>
#include <NCollection_ForwardRange.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! @brief Allocation-free iterator over the direct active children of any graph node.
//!
//! Unlike the typed BRepGraph_DefsIterator and BRepGraph_RefsIterator types,
//! this iterator accepts any node kind and returns the complete transition to
//! each direct child. It is the common primitive for generic hierarchy walkers:
//! recursive policy and accumulated placement remain the walker's responsibility.
//!
//! The continuation value is a raw child position. Copying CurrentPosition()
//! and passing it to the resume constructor continues at the same next child
//! without retaining an iterator object. The position is graph-local.
class BRepGraph_ChildIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Relationship representation used by the current direct step.
  enum class LinkKind
  {
    Reference,  //!< The parent owns a RefId for the child.
    Structural, //!< The relationship has no separate reference record.
  };

  //! Complete direct transition from the parent to one active child.
  struct ValueType
  {
    BRepGraph_NodeId   Child;                             //!< Direct child definition.
    BRepGraph_RefId    Ref;                               //!< Parent-owned ref, if any.
    uint32_t           StepIndex        = 0;              //!< Native parent relation position.
    TopAbs_Orientation LocalOrientation = TopAbs_FORWARD; //!< Orientation contributed by this step.
    LinkKind           Link = LinkKind::Structural;       //!< Reference or structural relationship.
  };

  //! Iterate direct children from the first native relation position.
  //! @param[in] theGraph graph containing the parent and children
  //! @param[in] theParent parent whose active direct children are enumerated
  Standard_EXPORT BRepGraph_ChildIterator(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theParent);

  //! Resume direct-child iteration at a previously exported continuation position.
  //! @param[in] theGraph graph containing the parent and children
  //! @param[in] theParent parent whose active direct children are enumerated
  //! @param[in] thePosition next native relation position returned by CurrentPosition()
  Standard_EXPORT BRepGraph_ChildIterator(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theParent,
                                          const uint32_t         thePosition);

  //! True when Current() contains an active direct-child transition.
  [[nodiscard]] bool More() const { return myHasCurrent; }

  //! Advance to the next active direct child.
  Standard_EXPORT void Next();

  //! Return the current direct-child transition.
  [[nodiscard]] const ValueType& Current() const { return myCurrent; }

  //! Return the placement contributed by Current(), or identity for a structural/topology step.
  [[nodiscard]] Standard_EXPORT const TopLoc_Location& CurrentLocalLocation() const;

  //! Return the continuation position for the child following Current().
  [[nodiscard]] uint32_t CurrentPosition() const { return myNextPosition; }

  //! Return an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<BRepGraph_ChildIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_ChildIterator>(this);
  }

  //! Return a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  Standard_EXPORT void seek();

  const BRepGraph* myGraph = nullptr;
  BRepGraph_NodeId myParent;
  uint32_t         myNextPosition = 0;
  ValueType        myCurrent;
  bool             myHasCurrent = false;
};

#endif // _BRepGraph_ChildIterator_HeaderFile
