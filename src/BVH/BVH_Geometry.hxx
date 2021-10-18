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

#ifndef BVH_Geometry_HeaderFile
#define BVH_Geometry_HeaderFile

#include <BVH_ObjectSet.hxx>
#include <BVH_Builder.hxx>
#include <BVH_BinnedBuilder.hxx>

//! BVH geometry as a set of abstract geometric objects
//! organized with bounding volume hierarchy (BVH).
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_Geometry : public BVH_ObjectSet<T, N>
{
public:

  //! Creates uninitialized BVH geometry.
  BVH_Geometry()
  : myIsDirty (Standard_False),
    myBVH (new BVH_Tree<T, N>()),
    // set default builder - binned SAH split
    myBuilder (new BVH_BinnedBuilder<T, N, BVH_Constants_NbBinsOptimal> (BVH_Constants_LeafNodeSizeSingle))
  {
    //
  }

  //! Creates uninitialized BVH geometry.
  BVH_Geometry (const opencascade::handle<BVH_Builder<T, N> >& theBuilder)
  : myIsDirty (Standard_False),
    myBVH (new BVH_Tree<T, N>()),
    myBuilder (theBuilder)
  {
    //
  }

  //! Releases resources of BVH geometry.
  virtual ~BVH_Geometry()
  {
    myBVH.Nullify();
    myBuilder.Nullify();
  }

public:

  //! Returns TRUE if geometry state should be updated.
  virtual Standard_Boolean IsDirty() const { return myIsDirty; }

  //! Marks geometry as outdated.
  virtual void MarkDirty() { myIsDirty = Standard_True; }

  //! Returns AABB of the given object.
  using BVH_ObjectSet<T, N>::Box;

  //! Returns AABB of the whole geometry.
  virtual BVH_Box<T, N> Box() const Standard_OVERRIDE
  {
    if (myIsDirty)
    {
      myBox = BVH_Set<T, N>::Box();
    }
    return myBox;
  }

  //! Returns BVH tree (and builds it if necessary).
  virtual const opencascade::handle<BVH_Tree<T, N> >& BVH()
  {
    if (myIsDirty)
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

  //! Updates internal geometry state.
  virtual void Update()
  {
    if (myIsDirty)
    {
      myBuilder->Build (this, myBVH.operator->(), Box());
      myIsDirty = Standard_False;
    }
  }

protected:

  Standard_Boolean                        myIsDirty; //!< Is geometry state outdated?
  opencascade::handle<BVH_Tree<T, N> >    myBVH;     //!< Constructed hight-level BVH
  opencascade::handle<BVH_Builder<T, N> > myBuilder; //!< Builder for hight-level BVH

  mutable BVH_Box<T, N> myBox; //!< Cached bounding box of geometric objects

};

#endif // _BVH_Geometry_Header
