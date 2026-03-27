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

#ifndef _BRepGraph_MutRefEntry_HeaderFile
#define _BRepGraph_MutRefEntry_HeaderFile

//! @brief RAII guard wrapping a mutable reference-entry record.
//!
//! Obtained via BRepGraph::Builder().Mut*Ref() methods.
//! Calls markRefModified() exactly once on scope exit.
template <typename RefT>
class BRepGraph_MutRefEntry
{
public:
  BRepGraph_MutRefEntry(BRepGraph* theGraph, RefT* theRef, const BRepGraph_RefId theRefId)
      : myGraph(theGraph),
        myRef(theRef),
        myRefId(theRefId)
  {
  }

  ~BRepGraph_MutRefEntry()
  {
    if (myGraph != nullptr)
      myGraph->markRefModified(myRefId, *myRef);
  }

  BRepGraph_MutRefEntry(BRepGraph_MutRefEntry&& theOther) noexcept
      : myGraph(theOther.myGraph),
        myRef(theOther.myRef),
        myRefId(theOther.myRefId)
  {
    theOther.myGraph = nullptr;
  }

  BRepGraph_MutRefEntry& operator=(BRepGraph_MutRefEntry&& theOther) noexcept
  {
    if (this != &theOther)
    {
      if (myGraph != nullptr)
        myGraph->markRefModified(myRefId, *myRef);
      myGraph          = theOther.myGraph;
      myRef            = theOther.myRef;
      myRefId          = theOther.myRefId;
      theOther.myGraph = nullptr;
    }
    return *this;
  }

  BRepGraph_MutRefEntry(const BRepGraph_MutRefEntry&)            = delete;
  BRepGraph_MutRefEntry& operator=(const BRepGraph_MutRefEntry&) = delete;

  [[nodiscard]] RefT* operator->() { return myRef; }

  [[nodiscard]] RefT& operator*() { return *myRef; }

private:
  BRepGraph*      myGraph;
  RefT*           myRef;
  BRepGraph_RefId myRefId;
};

#endif // _BRepGraph_MutRefEntry_HeaderFile
