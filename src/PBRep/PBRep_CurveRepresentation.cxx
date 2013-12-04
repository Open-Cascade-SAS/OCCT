// Created on: 1993-07-26
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
