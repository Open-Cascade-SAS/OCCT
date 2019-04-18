// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef BOPTools_BoxSelector_HeaderFile
#define BOPTools_BoxSelector_HeaderFile

#include <BVH_Traverse.hxx>
#include <BVH_BoxSet.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_ListOfInteger.hxx>

//! Template Selector for elements selection from BVH tree.
template <int Dimension>
class BOPTools_BoxSelector :
  public BVH_Traverse <Standard_Real, Dimension, BVH_BoxSet <Standard_Real, Dimension, Standard_Integer>, Standard_Boolean>
{
public:

  typedef typename BVH::VectorType<Standard_Real, Dimension>::Type BVH_VecNd;

public: //! @name Constructor

  //! Empty constructor
  BOPTools_BoxSelector() {};

public: //! @name public interfaces

  //! Clears the indices
  void Clear()
  {
    myIndices.Clear();
  }

  //! Sets the box
  void SetBox (const BVH_Box <Standard_Real, Dimension>& theBox)
  {
    myBox = theBox;
  }

  //! Returns the list of accepted indices
  const TColStd_ListOfInteger& Indices() const
  {
    return myIndices;
  }

public: //! @name Rejection/Acceptance rules

  //! Checks if the box should be rejected
  virtual Standard_Boolean RejectNode (const BVH_VecNd& theCMin,
                                       const BVH_VecNd& theCMax,
                                       Standard_Boolean& theIsInside) const Standard_OVERRIDE
  {
    Standard_Boolean hasOverlap;
    theIsInside = myBox.Contains (theCMin, theCMax, hasOverlap);
    return !hasOverlap;
  }

  //! Checks if the element should be rejected
  Standard_Boolean RejectElement (const Standard_Integer theIndex)
  {
    return myBox.IsOut (this->myBVHSet->Box (theIndex));
  }

  //! Checks if the metric of the node may be accepted
  virtual Standard_Boolean AcceptMetric (const Standard_Boolean& theIsInside) const Standard_OVERRIDE
  {
    return theIsInside;
  }

  //! Accepts the element with the index <theIndex> in BVH tree
  virtual Standard_Boolean Accept (const Standard_Integer theIndex,
                                   const Standard_Boolean& theIsInside) Standard_OVERRIDE
  {
    if (theIsInside || !RejectElement (theIndex))
    {
      myIndices.Append (this->myBVHSet->Element (theIndex));
      return Standard_True;
    }
    return Standard_False;
  }

protected: //! @name Fields

  BVH_Box <Standard_Real, Dimension> myBox; //!< Selection box
  TColStd_ListOfInteger myIndices;          //!< Selected indices
};

#endif
