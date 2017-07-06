// Created on: 1995-11-03
// Created by: Laurent BOURESCHE
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


#include <GeomFill_Boundary.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_Boundary,Standard_Transient)

//=======================================================================
//function : GeomFill_Boundary
//purpose  : 
//=======================================================================
GeomFill_Boundary::GeomFill_Boundary(const Standard_Real Tol3d, 
				     const Standard_Real Tolang):
 myT3d(Tol3d), myTang(Tolang)
{
}


//=======================================================================
//function : HasNormals
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_Boundary::HasNormals() const 
{
  return Standard_False;
}


//=======================================================================
//function : Norm
//purpose  : 
//=======================================================================

gp_Vec GeomFill_Boundary::Norm(const Standard_Real ) const 
{
  throw Standard_Failure("GeomFill_Boundary::Norm : Undefined normals");
}


//=======================================================================
//function : D1Norm
//purpose  : 
//=======================================================================

void GeomFill_Boundary::D1Norm(const Standard_Real , gp_Vec& , gp_Vec& ) const 
{
  throw Standard_Failure("GeomFill_Boundary::Norm : Undefined normals");
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void GeomFill_Boundary::Points(gp_Pnt& PFirst, gp_Pnt& PLast) const
{
  Standard_Real f,l;
  Bounds(f,l);
  PFirst = Value(f);
  PLast  = Value(l);
}


//=======================================================================
//function : Tol3d
//purpose  : 
//=======================================================================

Standard_Real GeomFill_Boundary::Tol3d() const 
{
  return myT3d;
}


//=======================================================================
//function : Tol3d
//purpose  : 
//=======================================================================

void GeomFill_Boundary::Tol3d(const Standard_Real Tol)
{
  myT3d = Tol;
}


//=======================================================================
//function : Tolang
//purpose  : 
//=======================================================================

Standard_Real GeomFill_Boundary::Tolang() const 
{
  return myTang;
}


//=======================================================================
//function : Tolang
//purpose  : 
//=======================================================================

void GeomFill_Boundary::Tolang(const Standard_Real Tol)
{
  myTang = Tol;
}


