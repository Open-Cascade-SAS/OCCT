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

#ifndef _BVH_Set_Header
#define _BVH_Set_Header

#include <BVH_Box.hxx>

//! Set of abstract entities (bounded by BVH boxes). This is
//! the minimal geometry interface needed to construct BVH.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_Set
{
public:

  typedef BVH_Box<T, N> BVH_BoxNt;

public:

  //! Creates new abstract set of objects.
  BVH_Set();

  //! Releases resources of set of objects.
  virtual ~BVH_Set() = 0;

  //! Returns AABB of the entire set of objects.
  virtual BVH_Box<T, N> Box() const;

public:

  //! Returns total number of objects.
  virtual Standard_Integer Size() const = 0;

  //! Returns AABB of the given object.
  virtual BVH_Box<T, N> Box (const Standard_Integer theIndex) const = 0;

  //! Returns centroid position along the given axis.
  virtual T Center (const Standard_Integer theIndex,
                    const Standard_Integer theAxis) const = 0;

  //! Performs transposing the two given objects in the set.
  virtual void Swap (const Standard_Integer theIndex1,
                     const Standard_Integer theIndex2) = 0;

};

#include <BVH_Set.lxx>

#endif // _BVH_Set_Header
