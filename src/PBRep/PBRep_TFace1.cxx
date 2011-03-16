// File:	PBRep_TFace1.cxx
// Created:	Mon Jul 26 10:29:13 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_TFace1.ixx>


//=======================================================================
//function : PBRep_TFace1
//purpose  : 
//=======================================================================

PBRep_TFace1::PBRep_TFace1() :
       myNaturalRestriction(Standard_False)
{
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_TFace1::Surface() const
{
  return mySurface;
}

//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

Handle(PPoly_Triangulation)  PBRep_TFace1::Triangulation() const
{
  return myTriangulation;
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

PTopLoc_Location  PBRep_TFace1::Location() const
{
  return myLocation;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  PBRep_TFace1::Tolerance() const
{
  return myTolerance;
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

void  PBRep_TFace1::Surface(const Handle(PGeom_Surface)& S)
{
  mySurface = S;
}


//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

void  PBRep_TFace1::Triangulation(const Handle(PPoly_Triangulation)& T)
{
  myTriangulation = T;
}

//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

void  PBRep_TFace1::Location(const PTopLoc_Location& L)
{
  myLocation = L;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  PBRep_TFace1::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}

//=======================================================================
//function : NaturalRestriction
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TFace1::NaturalRestriction()const 
{
  return myNaturalRestriction;
}


//=======================================================================
//function : NaturalRestriction
//purpose  : 
//=======================================================================

void  PBRep_TFace1::NaturalRestriction(const Standard_Boolean N)
{
  myNaturalRestriction = N;
}


