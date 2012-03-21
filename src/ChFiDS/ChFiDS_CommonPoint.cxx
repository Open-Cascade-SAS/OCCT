// Created on: 1993-11-30
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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



#include <ChFiDS_CommonPoint.ixx>

//=======================================================================
//function : ChFiDS_CommonPoint
//purpose  : 
//=======================================================================

ChFiDS_CommonPoint::ChFiDS_CommonPoint() : 
tol(0.),
isonarc(Standard_False),
isvtx(Standard_False),
hasvector(Standard_False)
{
}


//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void ChFiDS_CommonPoint::Reset()
{
  tol = 0.;
  isvtx  = Standard_False;
  isonarc = Standard_False;
  hasvector = Standard_False;
}


//=======================================================================
//function : SetArc
//purpose  : 
// 30/09/1997 : PMN On n'ecrabouille plus la tolerance
//=======================================================================

void  ChFiDS_CommonPoint::SetArc(const Standard_Real Tol, 
				const TopoDS_Edge& A, 
				const Standard_Real Param, 
				const TopAbs_Orientation TArc)
{
  isonarc = Standard_True;
  if (Tol > tol) tol = Tol;
  arc     = A;
  prmarc     = Param;
  traarc  = TArc;
}

//=======================================================================
//function : SetParameter
//purpose  : 
//=======================================================================

void  ChFiDS_CommonPoint::SetParameter(const Standard_Real Param)
{
  prmtg     = Param;
}

//=======================================================================
//function : Arc
//purpose  : 
//=======================================================================

const TopoDS_Edge&  ChFiDS_CommonPoint::Arc()const 
{
  if (!isonarc){
    Standard_DomainError::Raise("CommonPoint not on Arc"); 
  }
  return arc;
}


//=======================================================================
//function : TransitionOnArc
//purpose  : 
//=======================================================================

TopAbs_Orientation  ChFiDS_CommonPoint::TransitionOnArc()const 
{
  if (!isonarc){
    Standard_DomainError::Raise("CommonPoint not on Arc"); 
  }
  return traarc;
}


//=======================================================================
//function : ParameterOnArc
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_CommonPoint::ParameterOnArc()const 
{
  if (!isonarc){
    Standard_DomainError::Raise("CommonPoint not on Arc"); 
  }
  return prmarc;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_CommonPoint::Parameter()const 
{
  return prmtg;
}


