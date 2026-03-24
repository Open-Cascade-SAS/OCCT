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

#ifndef _BRepGraph_TypedAttribute_HeaderFile
#define _BRepGraph_TypedAttribute_HeaderFile

#include <BRepGraph_UserAttribute.hxx>

//! Concrete typed wrapper for a lazily-computed per-node value.
//!
//! @tparam T  Cached value type (e.g. double for curvature).
//!            Must be default-constructible and copy-assignable.
template <typename T>
class BRepGraph_TypedAttribute : public BRepGraph_UserAttribute
{
public:
  BRepGraph_TypedAttribute() = default;

  //! Construct with an initial value (marked clean).
  explicit BRepGraph_TypedAttribute(const T& theInitial)
      : myValue(theInitial)
  {
    MarkClean();
  }

  //! Get the cached value, computing via theComputer if dirty.
  //! Thread-safe: uses the base class shared_mutex.
  T Get(const std::function<T()>& theComputer) const
  {
    // Fast path
    if (!IsDirty())
    {
      std::shared_lock<std::shared_mutex> aLock(myMutex);
      if (!IsDirty())
        return myValue;
    }
    // Slow path
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    if (IsDirty())
    {
      myValue = theComputer();
      MarkClean();
    }
    return myValue;
  }

  //! Direct write - stores the value and marks clean.
  void Set(const T& theValue)
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    myValue = theValue;
    MarkClean();
  }

  //! Direct read.  Caller must guarantee freshness.
  const T& UncheckedValue() const { return myValue; }

private:
  mutable T myValue{};
};

#endif // _BRepGraph_TypedAttribute_HeaderFile
