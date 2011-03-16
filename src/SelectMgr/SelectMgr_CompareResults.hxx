// File:      SelectMgr_CompareResults.hxx
// Created:   23.10.03 20:31:03
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2003

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
