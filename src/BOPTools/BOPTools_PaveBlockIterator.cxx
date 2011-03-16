// File:	BOPTools_PaveBlockIterator.cxx
// Created:	Wed Mar  7 16:05:08 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_PaveBlockIterator.ixx>

#include <BOPTools_ListOfPave.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_Pave.hxx>

//=======================================================================
// function:BOPTools_PaveBlockIterator
// purpose: 
//=======================================================================
  BOPTools_PaveBlockIterator::BOPTools_PaveBlockIterator()
  : 
  myEdge(0),
  myIndex(1) 
{
}
//=======================================================================
// function:BOPTools_PaveBlockIterator
// purpose: 
//=======================================================================
  BOPTools_PaveBlockIterator::BOPTools_PaveBlockIterator (const Standard_Integer anEdge,
							  const BOPTools_PaveSet& aPaveSet)
{
  Initialize (anEdge, aPaveSet);
}
//=======================================================================
// function:Initialize
// purpose: 
//=======================================================================
  void BOPTools_PaveBlockIterator::Initialize(const Standard_Integer anEdge,
					      const BOPTools_PaveSet& aPaveSet)
{
  Standard_Integer i, aNb;

  myPaveSet=aPaveSet;
  myIndex=1;
  myEdge=anEdge;
  myPaveBlock.SetOriginalEdge(myEdge);

  myPaveSet.SortSet();
  BOPTools_ListOfPave& aLP=myPaveSet.ChangeSet();
  aNb=aLP.Extent();
  myPaves.Resize(aNb);
  BOPTools_ListIteratorOfListOfPave anIt(aLP);
  for (i=1; anIt.More(); anIt.Next(), i++) {
    const BOPTools_Pave& aPave=anIt.Value();
    myPaves(i)=aPave;
  }
}

//=======================================================================
// function:More
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_PaveBlockIterator::More() const
{
  Standard_Integer aNb;
  aNb=myPaves.Extent();
  return (myIndex < aNb);
}
//=======================================================================
// function:Next
// purpose: 
//=======================================================================
  void BOPTools_PaveBlockIterator::Next()
{
  myIndex++;
}
//=======================================================================
// function:Value
// purpose: 
//=======================================================================
  BOPTools_PaveBlock& BOPTools_PaveBlockIterator::Value() 
{
  Standard_Integer i1, i2;
  Standard_Real    aT1, aT2;
  BOPTools_PaveBlock* pPB= (BOPTools_PaveBlock*) &myPaveBlock;

  i1=myIndex;
  i2=i1+1;
  const BOPTools_Pave& aPave1=myPaves(i1);
  const BOPTools_Pave& aPave2=myPaves(i2);
  aT1=aPave1.Param();
  aT2=aPave2.Param();
  
  if (aT1 > aT2) {
    pPB->SetPave1(aPave1);
    pPB->SetPave2(aPave2);
  }
  else {
    pPB->SetPave1(aPave2);
    pPB->SetPave2(aPave1);
  }
  return myPaveBlock;
}

