// Created on: 1993-07-26
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <PBRep_TFace1.ixx>


//=======================================================================
//function : PBRep_TFace1
//purpose  : 
//=======================================================================

PBRep_TFace1::PBRep_TFace1() :
    myTolerance(0.),
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


