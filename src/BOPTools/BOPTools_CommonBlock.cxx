// File:	BOPTools_CommonBlock.cxx
// Created:	Thu Feb 15 12:19:13 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


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
