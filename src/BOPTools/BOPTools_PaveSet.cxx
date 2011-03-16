// File:	BOPTools_PaveSet.cxx
// Created:	Thu Feb  8 12:47:16 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_PaveSet.ixx>

#include <BOPTools_Array1OfPave.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_QuickSortPave.hxx>
#include <BOPTools_ComparePave.hxx>

#ifdef WNT
#pragma warning ( disable : 4101 )
#endif

//=======================================================================
// function: BOPTools_PaveSet::BOPTools_PaveSet
// purpose: 
//=======================================================================
  BOPTools_PaveSet::BOPTools_PaveSet() {}

//=======================================================================
// function: ChangeSet
// purpose: 
//=======================================================================
  BOPTools_ListOfPave& BOPTools_PaveSet::ChangeSet()
{
  return myPaveList;
}
//=======================================================================
// function: Set
// purpose: 
//=======================================================================
  const BOPTools_ListOfPave& BOPTools_PaveSet::Set() const
{
  return myPaveList;
}
//=======================================================================
// function: Append
// purpose: 
//=======================================================================
  void BOPTools_PaveSet::Append(const BOPTools_Pave& aPave)
{
  myPaveList.Append(aPave);
}
//=======================================================================
// function: SortSet
// purpose: 
//=======================================================================
  void BOPTools_PaveSet::SortSet()
{
  // Not implemented yet
  Standard_Integer aNbPaves, i;
  aNbPaves=myPaveList.Extent();
  if (aNbPaves>1) {
    BOPTools_Array1OfPave anArray1OfPave (1, aNbPaves);
    
    BOPTools_ListIteratorOfListOfPave anIt(myPaveList);
    
    for (i=1; anIt.More(); anIt.Next(), i++) {
      const BOPTools_Pave& aPave=anIt.Value();
      anArray1OfPave(i)=aPave;
    }
    
    BOPTools_QuickSortPave aQuickSortPave;
    BOPTools_ComparePave   aComparePave;
    aQuickSortPave.Sort (anArray1OfPave, aComparePave);
    
    myPaveList.Clear();
    for (i=1; i<=aNbPaves; i++){
      const BOPTools_Pave& aPave=anArray1OfPave(i);
      myPaveList.Append (aPave);
    }
  }
}
#ifdef WNT
#pragma warning ( default : 4101 )
#endif
