// Created on: 2011-11-29
// Created by: ANNA MASALSKAYA
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef _BRepBuilderAPI_BndBoxTreeSelector_Header
#define _BRepBuilderAPI_BndBoxTreeSelector_Header

#ifndef _TColStd_ListOfInteger_HeaderFile
#include <TColStd_ListOfInteger.hxx>
#endif
#ifndef _Bnd_Box_HeaderFile
#include <Bnd_Box.hxx>
#endif
#ifndef NCollection_UBTree_HeaderFile
#include <NCollection_UBTree.hxx>
#endif

typedef NCollection_UBTree <Standard_Integer, Bnd_Box> BRepBuilderAPI_BndBoxTree;

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
  BRepBuilderAPI_BndBoxTreeSelector() : BRepBuilderAPI_BndBoxTree::Selector() {}

  //! Implementation of rejection method
  //! @return
  //!   True if the bounding box does not intersect with the current 
  Standard_Boolean Reject (const Bnd_Box& theBox) const
  {
    return (myBox.IsOut (theBox));
  }

  //! Implementation of acceptance method
  //!   This method is called when the bounding box intersect with the current.
  //!   It stores the object - the index of box in the list of accepted objects.
  //! @return
  //!   True, because the object is accepted
  Standard_Boolean Accept (const Standard_Integer& theObj)
  {
    myResInd.Append (theObj);
    return Standard_True;
  }

  //! Clear the list of intersecting boxes
  void ClearResList()
  {
    myResInd.Clear();
  }

  //! Set current box to search for overlapping with him
  void SetCurrent (const Bnd_Box& theBox) 
  { 
    myBox = theBox;
  }

  //! Get list of indexes of boxes intersecting with the current box
  const TColStd_ListOfInteger& ResInd()
  {
    return myResInd;
  }

private:
  TColStd_ListOfInteger myResInd;
  Bnd_Box myBox;
};

#endif
