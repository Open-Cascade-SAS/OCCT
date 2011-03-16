// File:	PBRep_PointRepresentation.cxx
// Created:	Wed Aug 11 12:34:19 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <PBRep_PointRepresentation.ixx>

//=======================================================================
//function : PBRep_PointRepresentation
//purpose  : 
//=======================================================================

PBRep_PointRepresentation::PBRep_PointRepresentation
  (const Standard_Real P,
   const PTopLoc_Location& L) :
   myLocation(L),
   myParameter(P)
  
{
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

PTopLoc_Location  PBRep_PointRepresentation::Location()const 
{
  return myLocation;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real  PBRep_PointRepresentation::Parameter()const 
{
  return myParameter;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void  PBRep_PointRepresentation::Parameter(const Standard_Real P)
{
  myParameter = P;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

Handle(PBRep_PointRepresentation)  PBRep_PointRepresentation::Next()const 
{
  return myNext;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  PBRep_PointRepresentation::Next(const Handle(PBRep_PointRepresentation)& N)
{
  myNext = N;
}

//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointRepresentation::IsPointOnCurve() const
{
  return Standard_False;
}

//=======================================================================
//function : IsPointOnCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointRepresentation::IsPointOnCurveOnSurface() const
{
  return Standard_False;
}

//=======================================================================
//function : IsPointOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PointRepresentation::IsPointOnSurface() const
{
  return Standard_False;
}
