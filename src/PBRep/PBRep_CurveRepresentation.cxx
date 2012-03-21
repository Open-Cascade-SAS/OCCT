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



#include <PBRep_CurveRepresentation.ixx>

//=======================================================================
//function : PBRep_CurveRepresentation
//purpose  : 
//=======================================================================

PBRep_CurveRepresentation::PBRep_CurveRepresentation(const PTopLoc_Location& L) :
  myLocation(L)
{
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

PTopLoc_Location  PBRep_CurveRepresentation::Location()const 
{
  return myLocation;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

Handle(PBRep_CurveRepresentation)  PBRep_CurveRepresentation::Next()const 
{
  return myNext;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  PBRep_CurveRepresentation::Next
  (const Handle(PBRep_CurveRepresentation)& N)
{
  myNext = N;
}

//=======================================================================
//function : IsGCurve
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsGCurve()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsCurve3D
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsCurve3D()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsCurveOnSurface()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsCurveOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsCurveOnClosedSurface()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsRegularity()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygon3D
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygon3D()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygonOnTriangulation()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsPolygonOnClosedTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygonOnClosedTriangulation()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygonOnSurface()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygonOnClosedSurface()const 
{
  return Standard_False;
}
