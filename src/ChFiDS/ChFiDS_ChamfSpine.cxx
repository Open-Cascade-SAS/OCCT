// Created on: 1995-04-24
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <ChFiDS_ChamfSpine.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfBoolean.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ChFiDS_ChamfSpine,ChFiDS_Spine)

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
  if (mChamf != ChFiDS_Sym)   throw Standard_Failure("Chamfer is not symetric");
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
  if (mChamf != ChFiDS_TwoDist)  throw Standard_Failure("Chamfer is not a Two Dists Chamfer");
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
  if (mChamf != ChFiDS_DistAngle)  throw Standard_Failure("Chamfer is not a Two Dists Chamfer");
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



