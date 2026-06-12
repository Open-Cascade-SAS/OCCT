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

#ifndef _BRepGraph_UsagePath_HeaderFile
#define _BRepGraph_UsagePath_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <NCollection_LinearVector.hxx>

#include <cstddef>
#include <functional>

//! Explicit identity of a concrete usage from traversal root to selected node.
//!
//! A usage path is an ordered sequence of steps that records the exact
//! traversal from a root node down to a specific graph entity. Each step
//! captures the node reached, the reference through which it was reached,
//! and the sibling order (step index) at that level.
//!
//! Paths are used to disambiguate multiple occurrences of the same
//! definition reachable through different references or sibling positions.
class BRepGraph_UsagePath
{
public:
  //! One concrete traversal step in a usage path.
  //!
  //! Ref is valid for reference-owned links and invalid for structural links
  //! such as CoEdge -> Edge or Occurrence -> Product/topology-root. Step keeps
  //! sibling order explicit, so coincident or structurally-linked usages remain
  //! distinguishable without relying on location or hashes.
  struct Step
  {
    BRepGraph_NodeId Node;
    BRepGraph_RefId  Ref;
    int              StepIndex = -1;

    bool operator==(const Step& theOther) const
    {
      return Node == theOther.Node && Ref == theOther.Ref && StepIndex == theOther.StepIndex;
    }
  };

public:
  //! Creates an empty usage path.
  BRepGraph_UsagePath() = default;

  //! Creates a usage path with pre-allocated capacity.
  //! @param[in] theCapacity number of steps to pre-allocate
  explicit BRepGraph_UsagePath(const size_t theCapacity)
      : mySteps(theCapacity)
  {
  }

  //! Returns the number of steps in the path.
  size_t Size() const { return mySteps.Size(); }

  //! Returns true if the path has no steps.
  bool IsEmpty() const { return mySteps.IsEmpty(); }

  //! Returns the step at the given index.
  //! @param[in] theIdx zero-based index
  const Step& Value(const size_t theIdx) const { return mySteps.Value(theIdx); }

  //! Returns the first step in the path.
  const Step& First() const { return mySteps.First(); }

  //! Returns the last step in the path.
  const Step& Last() const { return mySteps.Last(); }

  //! Appends a step to the end of the path.
  //! @param[in] theStep step to append
  void Append(Step theStep) { mySteps.Append(std::move(theStep)); }

  //! Inserts a step before the given index.
  //! @param[in] theIdx zero-based index to insert before
  //! @param[in] theStep step to insert
  void InsertBefore(const size_t theIdx, Step theStep)
  {
    mySteps.InsertBefore(theIdx, std::move(theStep));
  }

  //! Removes all steps from the path.
  void Clear() { mySteps.Clear(); }

  //! Returns true if this path is equal to the other path.
  //! @param[in] theOther path to compare with
  bool IsEqual(const BRepGraph_UsagePath& theOther) const;

  //! Returns true if this path is equal to the other path.
  //! @param[in] theOther path to compare with
  bool operator==(const BRepGraph_UsagePath& theOther) const { return IsEqual(theOther); }

  //! Returns a hash code for this path.
  //! Uses first step, last step, and size for O(1) computation.
  size_t HashCode() const;

private:
  NCollection_LinearVector<Step> mySteps;
};

//! std::hash specialization for BRepGraph_UsagePath.
template <>
struct std::hash<BRepGraph_UsagePath>
{
  size_t operator()(const BRepGraph_UsagePath& thePath) const noexcept
  {
    return thePath.HashCode();
  }
};

#endif // _BRepGraph_UsagePath_HeaderFile
