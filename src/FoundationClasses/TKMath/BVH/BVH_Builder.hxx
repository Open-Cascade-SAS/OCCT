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

#ifndef BVH_Builder_HeaderFile
#define BVH_Builder_HeaderFile

#include <BVH_Set.hxx>
#include <BVH_BinaryTree.hxx>

//! A non-template class for using as base for BVH_Builder
//! (just to have a named base class).
class BVH_BuilderTransient : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(BVH_BuilderTransient, Standard_Transient)
public:
  //! Returns the maximum depth of constructed BVH.
  int MaxTreeDepth() const { return myMaxTreeDepth; }

  //! Returns the maximum number of sub-elements in the leaf.
  int LeafNodeSize() const { return myLeafNodeSize; }

  //! Returns parallel flag.
  inline bool IsParallel() const { return myIsParallel; }

  //! Set parallel flag controlling possibility of parallel execution.
  inline void SetParallel(const bool isParallel) { myIsParallel = isParallel; }

protected:
  //! Creates new abstract BVH builder.
  BVH_BuilderTransient(const int theLeafNodeSize,
                       const int theMaxTreeDepth)
      : myMaxTreeDepth(theMaxTreeDepth),
        myLeafNodeSize(theLeafNodeSize),
        myIsParallel(false)
  {
  }

protected:
  int myMaxTreeDepth; //!< Maximum depth of constructed BVH
  int myLeafNodeSize; //!< Maximum number of objects per leaf
  bool myIsParallel;   //!< Parallel execution flag.
};

//! Performs construction of BVH tree using bounding
//! boxes (AABBs) of abstract objects.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
class BVH_Builder : public BVH_BuilderTransient
{
public:
  //! Builds BVH using specific algorithm.
  virtual void Build(BVH_Set<T, N>*       theSet,
                     BVH_Tree<T, N>*      theBVH,
                     const BVH_Box<T, N>& theBox) const = 0;

protected:
  //! Creates new abstract BVH builder.
  BVH_Builder(const int theLeafNodeSize, const int theMaxTreeDepth)
      : BVH_BuilderTransient(theLeafNodeSize, theMaxTreeDepth)
  {
  }

  //! Updates depth of constructed BVH tree.
  void updateDepth(BVH_Tree<T, N>* theBVH, const int theLevel) const
  {
    if (theLevel > theBVH->myDepth)
    {
      theBVH->myDepth = theLevel;
    }
  }
};

#endif // _BVH_Builder_Header
