// Created on: 2001-02-08
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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
