// File:	PBRep_CurveOn2Surfaces.cxx
// Created:	Mon Jul 26 10:28:53 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_CurveOn2Surfaces.ixx>


//=======================================================================
//function : PBRep_CurveOn2Surfaces
//purpose  : 
//=======================================================================

PBRep_CurveOn2Surfaces::PBRep_CurveOn2Surfaces
  (const Handle(PGeom_Surface)& S1,
   const Handle(PGeom_Surface)& S2,
   const PTopLoc_Location& L1, 
   const PTopLoc_Location& L2, 
   const GeomAbs_Shape C) :
  PBRep_CurveRepresentation(L1),
  mySurface(S1),
  mySurface2(S2),
  myLocation2(L2),
  myContinuity(C)
{
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_CurveOn2Surfaces::Surface()const 
{
  return mySurface;
}


//=======================================================================
//function : Surface2
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_CurveOn2Surfaces::Surface2()const 
{
  return mySurface2;
}


//=======================================================================
//function : Location2
//purpose  : 
//=======================================================================

PTopLoc_Location  PBRep_CurveOn2Surfaces::Location2()const 
{
  return myLocation2;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape  PBRep_CurveOn2Surfaces::Continuity()const 
{
  return myContinuity;
}

//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveOn2Surfaces::IsRegularity()const 
{
  return Standard_True;
}


