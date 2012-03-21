// Created on: 2001-03-07
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

