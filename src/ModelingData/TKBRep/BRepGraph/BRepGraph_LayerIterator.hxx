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

#ifndef _BRepGraph_LayerIterator_HeaderFile
#define _BRepGraph_LayerIterator_HeaderFile

#include <BRepGraph_LayerRegistry.hxx>
#include <NCollection_ForwardRange.hxx>

//! @brief Iterator over registered layers in a BRepGraph_LayerRegistry.
//!
//! Provides zero-allocation iteration with OCCT More()/Next()/Value() pattern
//! and STL range-for via begin()/end().
//!
//! @code
//!   // Range-for:
//!   for (const occ::handle<BRepGraph_Layer>& aLayer :
//!   BRepGraph_LayerIterator(aGraph.LayerRegistry()))
//!     doSomething(aLayer);
//!
//!   // Traditional:
//!   for (BRepGraph_LayerIterator anIt(aGraph.LayerRegistry()); anIt.More(); anIt.Next())
//!     doSomething(anIt.Value());
//! @endcode
class BRepGraph_LayerIterator
{
public:
  //! Construct an iterator over all layers in the registry.
  explicit BRepGraph_LayerIterator(const BRepGraph_LayerRegistry& theRegistry)
      : myRegistry(&theRegistry),
        myCount(theRegistry.NbLayers()),
        myCurrent(0)
  {
  }

  //! True if the iterator has a current element.
  [[nodiscard]] bool More() const { return myCurrent < myCount; }

  //! Advance to the next layer.
  void Next() { ++myCurrent; }

  //! Return the current layer handle.
  [[nodiscard]] const occ::handle<BRepGraph_Layer>& Value() const
  {
    return myRegistry->Layer(myCurrent);
  }

  //! Return the current slot index in the registry.
  [[nodiscard]] int Slot() const { return myCurrent; }

  //! Number of layers in the registry.
  [[nodiscard]] int NbLayers() const { return myCount; }

  //! STL range-for support.
  NCollection_ForwardRangeIterator<BRepGraph_LayerIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_LayerIterator>(this);
  }

  //! Sentinel marking end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  const BRepGraph_LayerRegistry* myRegistry;
  int                            myCount;
  int                            myCurrent;
};

#endif // _BRepGraph_LayerIterator_HeaderFile
