// Created on: 2011-11-29
// Created by: ANNA MASALSKAYA
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef BRepBuilderAPI_BndBoxTreeSelector_HeaderFile
#define BRepBuilderAPI_BndBoxTreeSelector_HeaderFile

#include <TColStd_ListOfInteger.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_UBTree.hxx>

typedef NCollection_UBTree<Standard_Integer, Bnd_Box> BRepBuilderAPI_BndBoxTree;

//=======================================================================
//! Class BRepBuilderAPI_BndBoxTreeSelector
//!   derived from UBTree::Selector
//!   This class is used to select overlapping boxes, stored in
//!   NCollection::UBTree; contains methods to maintain the selection
//!   condition and to retrieve selected objects after search.
//=======================================================================

class BRepBuilderAPI_BndBoxTreeSelector : public BRepBuilderAPI_BndBoxTree::Selector
{
public:
  //! Constructor; calls the base class constructor
  BRepBuilderAPI_BndBoxTreeSelector()
      : BRepBuilderAPI_BndBoxTree::Selector()
  {
  }

  //! Implementation of rejection method
  //! @return
  //!   True if the bounding box does not intersect with the current
  Standard_Boolean Reject(const Bnd_Box& theBox) const { return (myBox.IsOut(theBox)); }

  //! Implementation of acceptance method
  //!   This method is called when the bounding box intersect with the current.
  //!   It stores the object - the index of box in the list of accepted objects.
  //! @return
  //!   True, because the object is accepted
  Standard_Boolean Accept(const Standard_Integer& theObj)
  {
    myResInd.Append(theObj);
    return Standard_True;
  }

  //! Clear the list of intersecting boxes
  void ClearResList() { myResInd.Clear(); }

  //! Set current box to search for overlapping with him
  void SetCurrent(const Bnd_Box& theBox) { myBox = theBox; }

  //! Get list of indexes of boxes intersecting with the current box
  const TColStd_ListOfInteger& ResInd() { return myResInd; }

private:
  TColStd_ListOfInteger myResInd;
  Bnd_Box               myBox;
};

#endif
