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

#ifndef _BVH_ObjectSet_Header
#define _BVH_ObjectSet_Header

#include <BVH_Set.hxx>
#include <BVH_Object.hxx>

//! Array of abstract entities (bounded by BVH boxes) to built BVH.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_ObjectSet : public BVH_Set<T, N>
{
public:

  //! Type of array of geometric objects.
  typedef NCollection_Vector<NCollection_Handle<BVH_Object<T, N> > > BVH_ObjectList;

public:

  //! Creates new set of geometric objects.
  BVH_ObjectSet();

  //! Releases resources of set of geometric objects.
  virtual ~BVH_ObjectSet();

public:

  //! Removes all geometric objects.
  virtual void Clear();

  //! Returns reference to the array of geometric objects.
  BVH_ObjectList& Objects();

  //! Returns reference to the  array of geometric objects.
  const BVH_ObjectList& Objects() const;

public:

  //! Return total number of objects.
  virtual Standard_Integer Size() const;

  //! Returns AABB of entire set of objects.
  using BVH_Set<T, N>::Box;

  //! Returns AABB of the given object.
  virtual BVH_Box<T, N> Box (const Standard_Integer theIndex) const;

  //! Returns centroid position along the given axis.
  virtual T Center (const Standard_Integer theIndex, const Standard_Integer theAxis) const;

  //! Performs transposing the two given objects in the set.
  virtual void Swap (const Standard_Integer theIndex1, const Standard_Integer theIndex2);

protected:

  BVH_ObjectList myObjects; //!< Array of geometric objects

};

#include <BVH_ObjectSet.lxx>

#endif // _BVH_ObjectSet_Header
