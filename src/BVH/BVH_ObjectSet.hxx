// Created on: 2013-12-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef BVH_ObjectSet_HeaderFile
#define BVH_ObjectSet_HeaderFile

#include <BVH_Set.hxx>
#include <BVH_Object.hxx>

//! Array of abstract entities (bounded by BVH boxes) to built BVH.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
class BVH_ObjectSet : public BVH_Set<T, N>
{
public:
  //! Type of array of geometric objects.
  typedef NCollection_Vector<opencascade::handle<BVH_Object<T, N>>> BVH_ObjectList;

public:
  //! Creates new set of geometric objects.
  BVH_ObjectSet() {}

  //! Releases resources of set of geometric objects.
  virtual ~BVH_ObjectSet() {}

public:
  //! Removes all geometric objects.
  virtual void Clear()
  {
    for (typename BVH_ObjectList::Iterator anObjectIter(myObjects); anObjectIter.More();
         anObjectIter.Next())
    {
      anObjectIter.ChangeValue().Nullify();
    }
    myObjects.Clear();
  }

  //! Returns reference to the array of geometric objects.
  BVH_ObjectList& Objects() { return myObjects; }

  //! Returns reference to the  array of geometric objects.
  const BVH_ObjectList& Objects() const { return myObjects; }

public:
  //! Return total number of objects.
  virtual Standard_Integer Size() const Standard_OVERRIDE { return myObjects.Size(); }

  //! Returns AABB of entire set of objects.
  using BVH_Set<T, N>::Box;

  //! Returns AABB of the given object.
  virtual BVH_Box<T, N> Box(const Standard_Integer theIndex) const Standard_OVERRIDE
  {
    return myObjects.Value(theIndex)->Box();
  }

  //! Returns centroid position along the given axis.
  virtual T Center(const Standard_Integer theIndex,
                   const Standard_Integer theAxis) const Standard_OVERRIDE
  {
    // Note: general implementation, not optimal
    return BVH::CenterAxis<T, N>::Center(myObjects.Value(theIndex)->Box(), theAxis);
  }

  //! Performs transposing the two given objects in the set.
  virtual void Swap(const Standard_Integer theIndex1,
                    const Standard_Integer theIndex2) Standard_OVERRIDE
  {
    std::swap(myObjects.ChangeValue(theIndex1), myObjects.ChangeValue(theIndex2));
  }

protected:
  BVH_ObjectList myObjects; //!< Array of geometric objects
};

#endif // _BVH_ObjectSet_Header
