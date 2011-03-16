// File:	ChFiDS_CommonPoint.cxx
// Created:	Tue Nov 30 17:48:00 1993
// Author:	Isabelle GRIGNON
//		<isg@zerox>


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


