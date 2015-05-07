// Created on: 2014-05-30
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Select3D_BVHPrimitiveContent_Header
#define _Select3D_BVHPrimitiveContent_Header

#include <BVH_PrimitiveSet.hxx>

class Select3D_SensitiveSet;

//! The purpose of this class is to provide a link between BVH_PrimitiveSet
//! and Select3D_SensitiveSet instance to build BVH tree for set of sensitives
class Select3D_BVHPrimitiveContent : public BVH_PrimitiveSet<Standard_Real, 3>
{
public:

  //! Initializes new linear BVH builder for the set of sensitives
  //! theSensitiveSet given
  Select3D_BVHPrimitiveContent (const Handle(Select3D_SensitiveSet)& theSensitiveSet);

  ~Select3D_BVHPrimitiveContent() {};

  //! Returns the length of set of sensitives
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns bounding box of sensitive with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIdx) const Standard_OVERRIDE;

  //! Returns center of sensitive with index theIdx in the set along the
  //! given axis theAxis
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIdx,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2 in the set
  Standard_EXPORT virtual void Swap (const Standard_Integer theIdx1,
                                     const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Returns the tree built for set of sensitives
  Standard_EXPORT const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& GetBVH();

  //! Returns a number of nodes in 1 BVH leaf
  Standard_Integer GetLeafNodeSize() const { return myLeafNodeSize; }

protected:
  Handle(Select3D_SensitiveSet) mySensitiveSet;     //!< Set of sensitive entities

private:
  static const Standard_Integer myLeafNodeSize = 8;     //!< Number of sub-elements in the leaf
};

#endif // _Select3D_BVHPrimitiveContent_Header
