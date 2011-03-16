// File:	ChFiDS_ChamfSpine.cxx
// Created:	Mon Apr 24 17:14:07 1995
// Author:	Modelistation
//		<model@phylox>


#include <ChFiDS_ChamfSpine.ixx>
#include <TColStd_HArray1OfBoolean.hxx>

//=======================================================================
//function : ChFiDS_ChamfSpine
//purpose  : 
//=======================================================================

ChFiDS_ChamfSpine::ChFiDS_ChamfSpine(){}

ChFiDS_ChamfSpine::ChFiDS_ChamfSpine(const Standard_Real Tol):
ChFiDS_Spine(Tol)
{}


//=======================================================================
//function : GetDist
//purpose  : 
//=======================================================================

void ChFiDS_ChamfSpine::GetDist(Standard_Real& Dis) const
{
  if (mChamf != ChFiDS_Sym)   Standard_Failure::Raise("Chamfer is not symetric");
  Dis = d1;
}

//=======================================================================
//function : SetDist
//purpose  : 
//=======================================================================

void ChFiDS_ChamfSpine::SetDist(const Standard_Real Dis)
{
  //isconstant->Init(Standard_True);
  mChamf = ChFiDS_Sym;  
  d1     = Dis;
}




//=======================================================================
//function : Dists
//purpose  : 
//=======================================================================

void ChFiDS_ChamfSpine::Dists(Standard_Real& Dis1,
			      Standard_Real& Dis2)const
{
  if (mChamf != ChFiDS_TwoDist)  Standard_Failure::Raise("Chamfer is not a Two Dists Chamfer");
  Dis1 = d1;
  Dis2 = d2;
}

//=======================================================================
//function : SetDists
//purpose  : 
//=======================================================================

void ChFiDS_ChamfSpine::SetDists(const Standard_Real Dis1,
				 const Standard_Real Dis2)
{
  //isconstant->Init(Standard_True);
  mChamf = ChFiDS_TwoDist;
  d1     = Dis1;
  d2     = Dis2;
}


//=======================================================================
//function : GetDistAngle
//purpose  : 
//=======================================================================

void ChFiDS_ChamfSpine::GetDistAngle(Standard_Real& Dis,
				     Standard_Real& Angle,
				     Standard_Boolean& DisOnF1)const
{
  if (mChamf != ChFiDS_DistAngle)  Standard_Failure::Raise("Chamfer is not a Two Dists Chamfer");
  Dis     = d1;
  Angle   = angle;
  DisOnF1 = dison1;
}

//=======================================================================
//function : SetDistAngle
//purpose  : 
//=======================================================================

void ChFiDS_ChamfSpine::SetDistAngle(const Standard_Real Dis,
				     const Standard_Real Angle,
				     const Standard_Boolean DisOnF1)
{
  //isconstant->Init(Standard_True);
  mChamf = ChFiDS_DistAngle;
  d1     = Dis;
  angle  = Angle;
  dison1 = DisOnF1;
}


//=======================================================================
//function : IsChamfer
//purpose  : 
//=======================================================================

ChFiDS_ChamfMethod  ChFiDS_ChamfSpine::IsChamfer() const
{

  return mChamf;
}



