// Created on: 2001-02-15
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



#include <BOPTools_CommonBlock.ixx>

//=======================================================================
// function:  BOPTools_CommonBlock::BOPTools_CommonBlock
// purpose: 
//=======================================================================
BOPTools_CommonBlock::BOPTools_CommonBlock()
: 
  myFace(0)   
{}
//=======================================================================
// function:  BOPTools_CommonBlock::BOPTools_CommonBlock
// purpose: 
//=======================================================================
  BOPTools_CommonBlock::BOPTools_CommonBlock(const BOPTools_PaveBlock& thePB1,
					     const BOPTools_PaveBlock& thePB2)
{
  myPB1=thePB1;
  myPB2=thePB2;
  myFace=0;
}
//=======================================================================
// function:  BOPTools_CommonBlock::BOPTools_CommonBlock
// purpose: 
//=======================================================================
  BOPTools_CommonBlock::BOPTools_CommonBlock(const BOPTools_PaveBlock& thePB1,
					     const Standard_Integer aF)
{
  myPB1=thePB1;
  myFace=aF;
}
//=======================================================================
// function:  SetFace
// purpose: 
//=======================================================================
  void BOPTools_CommonBlock::SetFace(const  Standard_Integer aF)
{
   myFace=aF;
}
//=======================================================================
// function:  SetPaveBlock1
// purpose: 
//=======================================================================
  void BOPTools_CommonBlock::SetPaveBlock1(const BOPTools_PaveBlock& thePB1)
{
  myPB1=thePB1;
}
//=======================================================================
// function:  SetPaveBlock2
// purpose: 
//=======================================================================
  void BOPTools_CommonBlock::SetPaveBlock2(const BOPTools_PaveBlock& thePB2)
{
  myPB2=thePB2;
}
//=======================================================================
// function:  PaveBlock1
// purpose: 
//=======================================================================
  const BOPTools_PaveBlock& BOPTools_CommonBlock::PaveBlock1() const
{
  return myPB1;
}
//=======================================================================
// function:  Face
// purpose: 
//=======================================================================
  Standard_Integer  BOPTools_CommonBlock::Face() const
{
  return myFace;
}

//=======================================================================
// function:  PaveBlock1
// purpose: 
//=======================================================================
  BOPTools_PaveBlock& BOPTools_CommonBlock::PaveBlock1(const Standard_Integer anIndex) 
{
  Standard_Integer anOriginalEdge;
  
  anOriginalEdge=myPB1.OriginalEdge();
  if (anIndex==anOriginalEdge) {
    return myPB1;
  }
  return myPB2;
}
//=======================================================================
// function: PaveBlock2
// purpose: 
//=======================================================================
  const BOPTools_PaveBlock& BOPTools_CommonBlock::PaveBlock2() const
{
  return myPB2;
}

//=======================================================================
// function:  PaveBlock2
// purpose: 
//=======================================================================
  BOPTools_PaveBlock& BOPTools_CommonBlock::PaveBlock2(const Standard_Integer anIndex) 
{
  Standard_Integer anOriginalEdge;
  
  anOriginalEdge=myPB1.OriginalEdge();
  if (anIndex==anOriginalEdge) {
    return myPB2;
  }
  return myPB1;
}
