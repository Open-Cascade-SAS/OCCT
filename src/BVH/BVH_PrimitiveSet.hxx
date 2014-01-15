// Created on: 2013-12-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BVH_PrimitiveSet_Header
#define _BVH_PrimitiveSet_Header

#include <BVH_Object.hxx>
#include <BVH_Builder.hxx>

//! Set of abstract geometric primitives.
template<class T, int N>
class BVH_PrimitiveSet : public BVH_Object<T, N>, public BVH_Set<T, N>
{
  using BVH_Set<T, N>::Box;

public:

  //! Creates set of abstract primitives.
  BVH_PrimitiveSet();

  //! Releases resources of set of abstract primitives.
  virtual ~BVH_PrimitiveSet();

public:

  //! Returns AABB of primitive set.
  virtual BVH_Box<T, N> Box() const;

  //! Returns constructed BVH tree.
  virtual const NCollection_Handle<BVH_Tree<T, N> >& BVH();

  //! Returns the method (builder) to construct BVH.
  virtual const NCollection_Handle<BVH_Builder<T, N> >& Builder() const;

  //! Sets the method (builder) to construct BVH.
  virtual void SetBuilder (NCollection_Handle<BVH_Builder<T, N> >& theBuilder);

protected:

  //! Updates BVH of primitive set.
  virtual void Update();

protected:

  NCollection_Handle<BVH_Tree<T, N> >    myBVH;     //!< Constructed bottom-level BVH
  NCollection_Handle<BVH_Builder<T, N> > myBuilder; //!< Builder for bottom-level BVH

  mutable BVH_Box<T, N> myBox; //!< Cached bounding box of geometric primitives

};

#include <BVH_PrimitiveSet.lxx>

#endif // _BVH_PrimitiveSet_Header
