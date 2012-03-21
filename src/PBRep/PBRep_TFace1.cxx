// Created on: 1993-07-26
// Created by: Remi LEQUETTE
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


