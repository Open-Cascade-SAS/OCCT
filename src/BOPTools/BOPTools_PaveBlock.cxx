// File:	BOPTools_PaveBlock.cxx
// Created:	Thu Feb 15 11:32:13 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_PaveBlock.ixx>

//=======================================================================
// function: BOPTools_PaveBlock::BOPTools_PaveBlock
// purpose: 
//=======================================================================
  BOPTools_PaveBlock::BOPTools_PaveBlock()
  : 
  myEdge(0), 
  myOriginalEdge(0),
  myFace1(0),
  myFace2(0)
{}
//=======================================================================
// function:  BOPTools_PaveBlock::BOPTools_PaveBlock
// purpose: 
//=======================================================================
  BOPTools_PaveBlock::BOPTools_PaveBlock(const Standard_Integer theEdge,
					 const BOPTools_Pave& thePave1,
					 const BOPTools_Pave& thePave2)
{
  myOriginalEdge=theEdge;
  myPave1=thePave1;
  myPave2=thePave2;
  myFace1=0;
  myFace2=0;
}

//=======================================================================
// function: SetCurve
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetCurve(const IntTools_Curve& aC)
{
   myCurve=aC;
}
//=======================================================================
// function: SetFace1
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetFace1(const Standard_Integer theFace1)
{
   myFace1=theFace1;
}
//=======================================================================
// function: SetFace2
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetFace2(const Standard_Integer theFace2)
{
   myFace2=theFace2;
}

//=======================================================================
// function: Face1
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_PaveBlock::Face1() const 
{
   return myFace1;
}
//=======================================================================
// function: Face2
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_PaveBlock::Face2() const 
{
   return myFace2;
}
//=======================================================================
// function: Curve
// purpose: 
//=======================================================================
  const IntTools_Curve& BOPTools_PaveBlock::Curve() const
{
   return myCurve;
}

//=======================================================================
// function: SetEdge
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetEdge(const Standard_Integer theEdge)
{
   myEdge=theEdge;
}
//=======================================================================
// function: SetOriginalEdge
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetOriginalEdge(const Standard_Integer theEdge)
{
   myOriginalEdge=theEdge;
}

//=======================================================================
// function: SetPave1
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetPave1(const BOPTools_Pave& thePave1)
{
   myPave1=thePave1;
  
}
//=======================================================================
// function: SetPave2
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetPave2(const BOPTools_Pave& thePave2)
{
   myPave2=thePave2;
}

//=======================================================================
// function: SetShrunkRange
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetShrunkRange(const IntTools_ShrunkRange& theSR)
{
   myShrunkRange=theSR;
}

//=======================================================================
// function: SetPointBetween
// purpose: 
//=======================================================================
  void BOPTools_PaveBlock::SetPointBetween (const BOPTools_PointBetween& thePB)
{
   myPointBetween=thePB;
}

//=======================================================================
// function: PointBetween
// purpose: 
//=======================================================================
  const BOPTools_PointBetween& BOPTools_PaveBlock::PointBetween ()const 
{
   return myPointBetween;
}

//=======================================================================
// function: Pave1
// purpose: 
//=======================================================================
  const BOPTools_Pave& BOPTools_PaveBlock::Pave1() const 
{
  Standard_Real t1, t2;

  t1=myPave1.Param();
  t2=myPave2.Param();

  if (t1<t2) {
   return myPave1; 
  }
  return myPave2;
}
//=======================================================================
// function: Pave2
// purpose: 
//=======================================================================
  const BOPTools_Pave& BOPTools_PaveBlock::Pave2() const 
{
  Standard_Real t1, t2;

  t1=myPave1.Param();
  t2=myPave2.Param();

  if (t1>t2) {
   return myPave1; 
  }
  return myPave2;
}

//=======================================================================
// function: Edge
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_PaveBlock::Edge() const
{
  return myEdge;
}

//=======================================================================
// function: OriginalEdge
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_PaveBlock::OriginalEdge() const
{
  return myOriginalEdge;
}

//=======================================================================
// function: IsValid
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_PaveBlock::IsValid() const
{
  Standard_Integer anInd1, anInd2;
  anInd1=myPave1.Index();
  anInd2=myPave2.Index();

  return (anInd1 && anInd2);
}
//=======================================================================
// function: IsEqual
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_PaveBlock::IsEqual(const BOPTools_PaveBlock& Other) const
{
  if (myOriginalEdge==Other.myOriginalEdge) {

    const BOPTools_Pave& aP11=Pave1();
    const BOPTools_Pave& aP12=Pave2();
    const BOPTools_Pave& aP21=Other.Pave1();
    const BOPTools_Pave& aP22=Other.Pave2();
  
    if (aP11.IsEqual(aP21)) {
      if (aP12.IsEqual(aP22)) {
	return Standard_True;
      }
    }
  }
  return Standard_False;
}
//=======================================================================
// function: Parameters
// purpose: 
//=======================================================================
  void  BOPTools_PaveBlock::Parameters(Standard_Real& aT1, 
				       Standard_Real& aT2) const 
{
  Standard_Real t1, t2;
  t1=myPave1.Param();
  t2=myPave2.Param();
  aT1= (t1<t2) ? t1 : t2;
  aT2= (t1>t2) ? t1 : t2;
}
//=======================================================================
// function: Range
// purpose: 
//=======================================================================
  const IntTools_Range& BOPTools_PaveBlock::Range() const 
{
  Standard_Real t1, t2;
  t1=myPave1.Param();
  t2=myPave2.Param();

  IntTools_Range* pRange=(IntTools_Range*) &myRange;

  if (t1<t2) {
   pRange->SetFirst (t1); 
   pRange->SetLast  (t2); 
  }
  else {
    pRange->SetFirst (t2); 
    pRange->SetLast  (t1);
  }
  return myRange;
}

//=======================================================================
// function: ShrunkRange
// purpose: 
//=======================================================================
  const IntTools_ShrunkRange& BOPTools_PaveBlock::ShrunkRange()const 
{
   return myShrunkRange;
}
//=======================================================================
// function:  IsInBlock
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_PaveBlock::IsInBlock(const BOPTools_Pave& thePaveX) const
{
  Standard_Boolean bIsInBlock;
  Standard_Real aT1, aT2, aTx;
  //
  aTx=thePaveX.Param();
  Parameters(aT1, aT2);
  bIsInBlock=(aTx > aT1 && aTx < aT2);
  return bIsInBlock;
}
