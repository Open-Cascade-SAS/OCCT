// Created on: 2003-10-23
// Created by: Alexander GRIGORIEV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <SelectMgr_IndexedDataMapOfOwnerCriterion.hxx>
#include <SelectMgr_SortCriterion.hxx>
#include <TCollection_CompareOfInteger.hxx>

// Purpose: Redefine CompareOfInteger from TCollection, to be used
// in method SortResult from class ViewerSelector

class SelectMgr_CompareResults: public TCollection_CompareOfInteger
{
 public:
  SelectMgr_CompareResults
        (const SelectMgr_IndexedDataMapOfOwnerCriterion& aMapOfCriterion)
          : myMapOfCriterion (aMapOfCriterion) {}
  Standard_Boolean      IsLower
                (const Standard_Integer&, const Standard_Integer&) const;
  Standard_Boolean      IsGreater
                (const Standard_Integer&, const Standard_Integer&) const;
 private:
  const SelectMgr_IndexedDataMapOfOwnerCriterion&  myMapOfCriterion;
  void operator= (const SelectMgr_CompareResults& );
};

//=======================================================================
//function : IsLower
//purpose  : The sort order should be inverse (the greatest to come first)
//=======================================================================

inline Standard_Boolean SelectMgr_CompareResults::IsLower
        (const Standard_Integer &Left,const Standard_Integer &Right) const
{
  return (myMapOfCriterion.FindFromIndex(Left) >
          myMapOfCriterion.FindFromIndex(Right));
}

//=======================================================================
//function : IsGreater
//purpose  : The sort order should be inverse (the greatest to come first)
//=======================================================================

inline Standard_Boolean SelectMgr_CompareResults::IsGreater
       (const Standard_Integer &Left,const Standard_Integer &Right) const
{
  return (myMapOfCriterion.FindFromIndex(Left) <
          myMapOfCriterion.FindFromIndex(Right));
}
