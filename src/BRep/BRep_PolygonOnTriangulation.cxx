// Created on: 1995-03-15
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRep_CurveRepresentation.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_PolygonOnTriangulation,BRep_CurveRepresentation)

//=======================================================================
//function : BRep_PolygonOnTriangulation
//purpose  : 
//=======================================================================
BRep_PolygonOnTriangulation::BRep_PolygonOnTriangulation
(const Handle(Poly_PolygonOnTriangulation)& P, 
 const Handle(Poly_Triangulation)&          T,
 const TopLoc_Location&                     L):
 BRep_CurveRepresentation(L),
 myPolygon(P),
 myTriangulation(T)
{
}


//=======================================================================
//function : IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnTriangulation::IsPolygonOnTriangulation() const 
{
  return Standard_True;
}

//=======================================================================
//function : IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnTriangulation::IsPolygonOnTriangulation
  (const Handle(Poly_Triangulation)& T,
   const TopLoc_Location&            L) const 
{
  return (T == myTriangulation) && (L == myLocation);
}

//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

void BRep_PolygonOnTriangulation::PolygonOnTriangulation
  (const Handle(Poly_PolygonOnTriangulation)& P)
{
  myPolygon = P;
}

//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

const Handle(Poly_PolygonOnTriangulation)& BRep_PolygonOnTriangulation::PolygonOnTriangulation() const
{
  return myPolygon;
}

//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

const Handle(Poly_Triangulation)& BRep_PolygonOnTriangulation::Triangulation() const
{
  return myTriangulation;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_PolygonOnTriangulation::Copy() const 
{
  Handle(BRep_PolygonOnTriangulation) P = 
    new BRep_PolygonOnTriangulation(myPolygon, myTriangulation, Location());
  
  return P;
}
