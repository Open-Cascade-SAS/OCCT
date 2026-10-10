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

#ifndef _BRepGraph_ParentIterator_HeaderFile
#define _BRepGraph_ParentIterator_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_ChildIterator.hxx>
#include <NCollection_ForwardRange.hxx>

//! @brief Allocation-free iterator over the direct active parents of any graph node.
//!
//! Reverse-incidence storage identifies candidate parents. Reference transitions
//! are resolved directly from the parent-owned reference and its native relation
//! step; the three structural relations (Wire->CoEdge, CoEdge->Edge, and
//! Occurrence->child) are resolved directly from their definition storage.
//!
//! A position has two graph-local coordinates because parent relations can come
//! from several reverse-incidence indices. Copy CurrentPosition() and pass it to
//! the resume constructor to continue at the next direct parent.
class BRepGraph_ParentIterator
{
public:
  DEFINE_STANDARD_ALLOC

  using LinkKind = BRepGraph_ChildIterator::LinkKind;

  enum class RelationSource : uint32_t
  {
    Primary,
    Compound,
    Occurrence,
    End
  };

  //! Graph-local position used to resume the iterator.
  struct Position
  {
    RelationSource CurrentSource = RelationSource::Primary;
    uint32_t       Index         = 0;
  };

  //! Complete direct transition from one active parent to the requested child.
  struct ValueType
  {
    BRepGraph_NodeId   Parent;                            //!< Direct parent definition.
    BRepGraph_RefId    Ref;                               //!< Parent-owned ref, if any.
    uint32_t           StepIndex        = 0;              //!< Native parent relation position.
    TopAbs_Orientation LocalOrientation = TopAbs_FORWARD; //!< Orientation contributed by this step.
    LinkKind           Link = LinkKind::Structural;       //!< Reference or structural relationship.
  };

  //! Iterate direct parents from the first reverse-incidence index.
  //! @param[in] theGraph graph containing the child and parents
  //! @param[in] theChild child whose active direct parents are enumerated
  Standard_EXPORT BRepGraph_ParentIterator(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theChild);

  //! Resume direct-parent iteration at a previously saved position.
  //! @param[in] theGraph graph containing the child and parents
  //! @param[in] theChild child whose active direct parents are enumerated
  //! @param[in] thePosition next reverse-incidence position returned by CurrentPosition()
  Standard_EXPORT BRepGraph_ParentIterator(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theChild,
                                           const Position&        thePosition);

  //! True when Current() contains an active direct-parent transition.
  [[nodiscard]] bool More() const { return myHasCurrent; }

  //! Advance to the next active direct parent.
  Standard_EXPORT void Next();

  //! Return the current direct-parent transition.
  [[nodiscard]] const ValueType& Current() const { return myCurrent; }

  //! Return the placement contributed by Current(), or identity for a structural/topology step.
  [[nodiscard]] Standard_EXPORT const TopLoc_Location& CurrentLocalLocation() const;

  //! Return the position of the parent following Current().
  [[nodiscard]] const Position& CurrentPosition() const { return myPosition; }

  //! Return an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<BRepGraph_ParentIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_ParentIterator>(this);
  }

  //! Return a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  Standard_EXPORT void seek();
  Standard_EXPORT bool setParent(const BRepGraph_NodeId theParent,
                                 const BRepGraph_RefId  theExpectedRef = BRepGraph_RefId());

  const BRepGraph* myGraph = nullptr;
  BRepGraph_NodeId myChild;
  Position         myPosition;
  ValueType        myCurrent;
  bool             myHasCurrent = false;
};

#endif // _BRepGraph_ParentIterator_HeaderFile
