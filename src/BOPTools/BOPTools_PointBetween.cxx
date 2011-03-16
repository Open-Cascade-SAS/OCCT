// File:	BOPTools_PointBetween.cxx
// Created:	Thu Apr 19 11:19:20 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_PointBetween.ixx>

//=======================================================================
// function:  BOPTools_PointBetween:: BOPTools_PointBetween
// purpose: 
//=======================================================================
  BOPTools_PointBetween::BOPTools_PointBetween()
:
  myT(0.),
  myU(0.),
  myV(0.)
{}
  
//=======================================================================
// function:  SetParameter
// purpose: 
//=======================================================================
  void BOPTools_PointBetween::SetParameter(const Standard_Real aT)
{
  myT=aT;
}

//=======================================================================
// function:  Parameter
// purpose: 
//=======================================================================
  Standard_Real BOPTools_PointBetween::Parameter()const 
{
  return myT;
}

//=======================================================================
// function:  SetUV
// purpose: 
//=======================================================================
  void BOPTools_PointBetween::SetUV(const Standard_Real aU, 
				    const Standard_Real aV)
{
  myU=aU;
  myV=aV;
}
//=======================================================================
// function:  UV
// purpose: 
//=======================================================================
  void BOPTools_PointBetween::UV(Standard_Real& aU, 
				 Standard_Real& aV) const
{
  aU=myU;
  aV=myV;
}

//=======================================================================
// function:  SetPnt
// purpose: 
//=======================================================================
  void BOPTools_PointBetween::SetPnt (const gp_Pnt& aP) 
{
  myPnt=aP;
}

//=======================================================================
// function:  Pnt
// purpose: 
//=======================================================================
  const gp_Pnt& BOPTools_PointBetween::Pnt () const 
{
  return myPnt;
}
