// Created on: 1995-04-24
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



