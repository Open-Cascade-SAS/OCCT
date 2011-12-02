// File:        BRepBuilderAPI_BndBoxTreeSelector.hxx
// Created:     Nov 29 17:47:12 2011
// Author:      ANNA MASALSKAYA
// Copyright:   Open CASCADE SAS 2011

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
