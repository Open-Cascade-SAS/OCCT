// File:	BOPTools_Curve.cxx
// Created:	Tue May  8 13:13:58 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_Curve.ixx>

//=======================================================================
// function: BOPTools_Curve::BOPTools_Curve
// purpose: 
//=======================================================================
  BOPTools_Curve::BOPTools_Curve()
{}
//=======================================================================
// function: BOPTools_Curve::BOPTools_Curve
// purpose: 
//=======================================================================
  BOPTools_Curve::BOPTools_Curve (const IntTools_Curve& aIC)
{
  myCurve=aIC;
}
//=======================================================================
// function: SetCurve
// purpose: 
//=======================================================================
  void BOPTools_Curve::SetCurve (const IntTools_Curve& aIC)
{
   myCurve=aIC;
}
//=======================================================================
// function: Curve
// purpose: 
//=======================================================================
  const IntTools_Curve& BOPTools_Curve::Curve () const 
{
   return myCurve;
}
//=======================================================================
// function: Set
// purpose: 
//=======================================================================
  BOPTools_PaveSet& BOPTools_Curve::Set()
{
  return myPaveSet;
}

//=======================================================================
//function :  AppendNewBlock
//purpose  : 
//=======================================================================
  void BOPTools_Curve::AppendNewBlock(const BOPTools_PaveBlock& aPB) 
{
  myNewPBs.Append(aPB);
}

//=======================================================================
//function :  NewPaveBlocks
//purpose  : 
//=======================================================================
  const BOPTools_ListOfPaveBlock& BOPTools_Curve::NewPaveBlocks() const
{
  return myNewPBs;
}
//=======================================================================
//function :  TechnoVertices
//purpose  : 
//=======================================================================
  TColStd_ListOfInteger& BOPTools_Curve::TechnoVertices() 
{
  return myTechnoVertices;
}
