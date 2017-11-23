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

#include <TColStd_ListOfInteger.hxx>
#include <NCollection_UBTree.hxx>
#include <Standard_Integer.hxx>

//! Template Selector for the unbalanced binary tree
//! of overlapped bounding boxes.
template <class BoxType> class BOPTools_BoxSelector :
  public NCollection_UBTree<Standard_Integer, BoxType>::Selector
{
public:

  //! Empty constructor
  BOPTools_BoxSelector() {};

  //! Checks if the box should be rejected
  virtual Standard_Boolean Reject(const BoxType& theOther) const
  {
    return myBox.IsOut(theOther);
  }

  //! Accepts the index
  virtual Standard_Boolean Accept(const Standard_Integer& theIndex)
  {
    myIndices.Append(theIndex);
    return Standard_True;
  }

  //! Clears the indices
  void Clear()
  {
    myIndices.Clear();
  }

  //! Sets the box
  void SetBox(const BoxType& theBox)
  {
    myBox = theBox;
  }

  //! Returns the list of accepted indices
  const TColStd_ListOfInteger& Indices() const
  {
    return myIndices;
  }

private:

  BoxType myBox;
  TColStd_ListOfInteger myIndices;

};

#endif
