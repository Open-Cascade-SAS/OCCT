// Created on: 2001-03-14
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



#include <BOPTools_CommonBlockAPI.ixx>

#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>


//=======================================================================
// function:  BOPTools_CommonBlockAPI::BOPTools_CommonBlockAPI
// purpose: 
//=======================================================================
BOPTools_CommonBlockAPI::BOPTools_CommonBlockAPI
     (const BOPTools_ListOfCommonBlock& aLCB)
{
  myListOfCommonBlock=(void *)&aLCB;
}
//=======================================================================
// function:  List
// purpose: 
//=======================================================================
  const BOPTools_ListOfCommonBlock& BOPTools_CommonBlockAPI::List () const
{
  BOPTools_ListOfCommonBlock* pListOfCommonBlock=
    (BOPTools_ListOfCommonBlock*)myListOfCommonBlock;
  return *pListOfCommonBlock;
}
//=======================================================================
// function:  CommonPaveBlocks
// purpose:   get all CommonPaveBlocks
//=======================================================================
  const BOPTools_ListOfPaveBlock&
    BOPTools_CommonBlockAPI::CommonPaveBlocks
      (const Standard_Integer anE) const
{
  Standard_Integer anECurrent, i;

  BOPTools_ListOfPaveBlock* pmyListOfPaveBlock=
    (BOPTools_ListOfPaveBlock*) &myListOfPaveBlock;
  pmyListOfPaveBlock->Clear();

  BOPTools_ListOfCommonBlock* pListOfCommonBlock=
    (BOPTools_ListOfCommonBlock*)myListOfCommonBlock;

  BOPTools_ListIteratorOfListOfCommonBlock anIt(*pListOfCommonBlock);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_CommonBlock& aCB=anIt.Value();
    
    for (i=0; i<2; i++) {
      const BOPTools_PaveBlock& aPB=(!i) ? aCB.PaveBlock1() : aCB.PaveBlock2();
      anECurrent=aPB.OriginalEdge();
      if (anECurrent==anE) {
	pmyListOfPaveBlock->Append(aPB);
	break;
      }
    }
  }

  return myListOfPaveBlock;
}
//=======================================================================
// function:  IsCommonBlock
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_CommonBlockAPI::IsCommonBlock 
    (const BOPTools_PaveBlock& aPB) const
{
  Standard_Integer anE;

  anE=aPB.OriginalEdge();

  CommonPaveBlocks(anE);
  
  BOPTools_ListIteratorOfListOfPaveBlock anIt(myListOfPaveBlock);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPB1=anIt.Value();
    if (aPB1.IsEqual(aPB)) {
      return Standard_True;
    }
  }
  return Standard_False;
}

