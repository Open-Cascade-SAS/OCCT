// File:	BOPTools_CommonBlockAPI.cxx
// Created:	Wed Mar 14 15:03:09 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


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

