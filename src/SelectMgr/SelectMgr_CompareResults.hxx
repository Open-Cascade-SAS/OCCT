// Created on: 2003-10-23
// Created by: Alexander GRIGORIEV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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


#include <SelectMgr_IndexedDataMapOfOwnerCriterion.hxx>
#include <SelectMgr_SortCriterion.hxx>
#include <TCollection_CompareOfInteger.hxx>

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
