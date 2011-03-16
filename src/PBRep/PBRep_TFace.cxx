// File:	PBRep_TFace.cxx
// Created:	Mon Jul 26 10:29:13 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_TFace.ixx>


//=======================================================================
//function : PBRep_TFace
//purpose  : 
//=======================================================================

PBRep_TFace::PBRep_TFace() :
       myNaturalRestriction(Standard_False)
{
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_TFace::Surface() const
{
  return mySurface;
}

//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

Handle(PPoly_Triangulation)  PBRep_TFace::Triangulation() const
{
  return myTriangulation;
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

PTopLoc_Location  PBRep_TFace::Location() const
{
  return myLocation;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  PBRep_TFace::Tolerance() const
{
  return myTolerance;
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

void  PBRep_TFace::Surface(const Handle(PGeom_Surface)& S)
{
  mySurface = S;
}


//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

void  PBRep_TFace::Triangulation(const Handle(PPoly_Triangulation)& T)
{
  myTriangulation = T;
}

//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

void  PBRep_TFace::Location(const PTopLoc_Location& L)
{
  myLocation = L;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  PBRep_TFace::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}

//=======================================================================
//function : NaturalRestriction
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TFace::NaturalRestriction()const 
{
  return myNaturalRestriction;
}


//=======================================================================
//function : NaturalRestriction
//purpose  : 
//=======================================================================

void  PBRep_TFace::NaturalRestriction(const Standard_Boolean N)
{
  myNaturalRestriction = N;
}


