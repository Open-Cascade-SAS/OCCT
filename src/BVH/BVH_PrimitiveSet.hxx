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

#ifndef _BVH_PrimitiveSet_Header
#define _BVH_PrimitiveSet_Header

#include <BVH_Object.hxx>
#include <BVH_Builder.hxx>
#include <BVH_BinnedBuilder.hxx>

//! Set of abstract geometric primitives organized with bounding
//! volume hierarchy (BVH). Unlike an object set, this collection
//! is designed for storing structural elements of a single object
//! (such as triangles in the object triangulation). Because there
//! may be a large number of such elements, the implementations of
//! this interface should be sufficiently optimized.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_PrimitiveSet : public BVH_Object<T, N>, public BVH_Set<T, N>
{
protected:

  using BVH_Set<T, N>::Box;

public:
  static const Standard_Integer MaxTreeDepth = BVH_Constants_MaxTreeDepth;

  //! Creates set of abstract primitives.
  BVH_PrimitiveSet()
  : myBVH (new BVH_Tree<T, N>()),
    // set default builder - binned SAH split
    myBuilder (new BVH_BinnedBuilder<T, N, BVH_Constants_NbBinsBest> (BVH_Constants_LeafNodeSizeDefault, BVH_Constants_MaxTreeDepth))
  {
    //
  }

  //! Creates set of abstract primitives.
  BVH_PrimitiveSet (const opencascade::handle<BVH_Builder<T, N> >& theBuilder)
  : myBVH (new BVH_Tree<T, N>()),
    myBuilder (theBuilder)
  {
    //
  }

  //! Releases resources of set of abstract primitives.
  virtual ~BVH_PrimitiveSet()
  {
    myBVH.Nullify();
    myBuilder.Nullify();
  }

public:

  //! Returns AABB of primitive set.
  virtual BVH_Box<T, N> Box() const Standard_OVERRIDE
  {
    if (BVH_Object<T, N>::myIsDirty)
    {
      myBox = BVH_Set<T, N>::Box();
    }
    return myBox;
  }

  //! Returns BVH tree (and builds it if necessary).
  virtual const opencascade::handle<BVH_Tree<T, N> >& BVH()
  {
    if (BVH_Object<T, N>::myIsDirty)
    {
      Update();
    }
    return myBVH;
  }

  //! Returns the method (builder) used to construct BVH.
  virtual const opencascade::handle<BVH_Builder<T, N> >& Builder() const { return myBuilder; }

  //! Sets the method (builder) used to construct BVH.
  virtual void SetBuilder (const opencascade::handle<BVH_Builder<T, N> >& theBuilder) { myBuilder = theBuilder; }

protected:

  //! Updates BVH of primitive set.
  virtual void Update()
  {
    if (BVH_Object<T, N>::myIsDirty)
    {
      myBuilder->Build (this, myBVH.operator->(), Box());
      BVH_Object<T, N>::myIsDirty = Standard_False;
    }
  }

protected:

  opencascade::handle<BVH_Tree<T, N> >    myBVH;     //!< Constructed bottom-level BVH
  opencascade::handle<BVH_Builder<T, N> > myBuilder; //!< Builder for bottom-level BVH

  mutable BVH_Box<T, N> myBox; //!< Cached bounding box of geometric primitives

};

#endif // _BVH_PrimitiveSet_Header
