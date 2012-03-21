// Created on: 1995-11-03
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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



#include <GeomFill_Boundary.ixx>


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
  Standard_Failure::Raise("GeomFill_Boundary::Norm : Undefined normals");
  return gp_Vec();
}


//=======================================================================
//function : D1Norm
//purpose  : 
//=======================================================================

void GeomFill_Boundary::D1Norm(const Standard_Real , gp_Vec& , gp_Vec& ) const 
{
  Standard_Failure::Raise("GeomFill_Boundary::Norm : Undefined normals");
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


