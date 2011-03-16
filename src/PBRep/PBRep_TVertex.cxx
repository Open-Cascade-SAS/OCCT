// File:	PBRep_TVertex.cxx
// Created:	Mon Jul 26 10:29:14 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_TVertex.ixx>


//=======================================================================
//function : PBRep_TVertex
//purpose  : 
//=======================================================================

PBRep_TVertex::PBRep_TVertex() 
{
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  PBRep_TVertex::Tolerance()const 
{
  return myTolerance;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  PBRep_TVertex::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt  PBRep_TVertex::Pnt()const 
{
  return myPnt;
}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

void  PBRep_TVertex::Pnt(const gp_Pnt& P)
{
  myPnt = P;
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

Handle(PBRep_PointRepresentation)  PBRep_TVertex::Points()const 
{
  return myPoints;
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void  PBRep_TVertex::Points(const Handle(PBRep_PointRepresentation)& P)
{
  myPoints = P;
}


