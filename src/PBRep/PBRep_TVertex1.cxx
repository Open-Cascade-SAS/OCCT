// File:	PBRep_TVertex1.cxx
// Created:	Mon Jul 26 10:29:14 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_TVertex1.ixx>


//=======================================================================
//function : PBRep_TVertex1
//purpose  : 
//=======================================================================

PBRep_TVertex1::PBRep_TVertex1() 
{
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  PBRep_TVertex1::Tolerance()const 
{
  return myTolerance;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  PBRep_TVertex1::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt  PBRep_TVertex1::Pnt()const 
{
  return myPnt;
}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

void  PBRep_TVertex1::Pnt(const gp_Pnt& P)
{
  myPnt = P;
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

Handle(PBRep_PointRepresentation)  PBRep_TVertex1::Points()const 
{
  return myPoints;
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void  PBRep_TVertex1::Points(const Handle(PBRep_PointRepresentation)& P)
{
  myPoints = P;
}


