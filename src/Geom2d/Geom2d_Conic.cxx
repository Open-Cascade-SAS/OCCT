// Created on: 1993-03-24
// Created by: JCV
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


#include <Geom2d_Conic.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Conic,Geom2d_Curve)

typedef Geom2d_Conic         Conic;

typedef gp_Ax2d  Ax2d;
typedef gp_Dir2d Dir2d;
typedef gp_Pnt2d Pnt2d;
typedef gp_Vec2d Vec2d;

//=======================================================================
//function : SetAxis
//purpose  : 
//=======================================================================

void Geom2d_Conic::SetAxis(const gp_Ax22d& A)
{
  pos.SetAxis(A); 
}

//=======================================================================
//function : SetXAxis
//purpose  : 
//=======================================================================

void Geom2d_Conic::SetXAxis (const Ax2d& A) 
{ 
  pos.SetXAxis(A); 
}

//=======================================================================
//function : SetYAxis
//purpose  : 
//=======================================================================

void Geom2d_Conic::SetYAxis (const Ax2d& A)
{ 
  pos.SetYAxis(A);
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Geom2d_Conic::SetLocation (const Pnt2d& P) 
{
  pos.SetLocation (P); 
}

//=======================================================================
//function : XAxis
//purpose  : 
//=======================================================================

Ax2d Geom2d_Conic::XAxis () const 
{ 
  return gp_Ax2d(pos.Location(), pos.XDirection()); 
}

//=======================================================================
//function : YAxis
//purpose  : 
//=======================================================================

Ax2d Geom2d_Conic::YAxis () const 
{
   return gp_Ax2d(pos.Location(), pos.YDirection());
}

//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

Pnt2d Geom2d_Conic::Location () const 
{
 return pos.Location(); 
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

const gp_Ax22d& Geom2d_Conic::Position () const 
{
  return pos; 
}


//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom2d_Conic::Reverse () { 

  Dir2d Temp = pos.YDirection ();
  Temp.Reverse ();
  pos.SetAxis(gp_Ax22d(pos.Location(), pos.XDirection(), Temp));
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom2d_Conic::Continuity () const 
{
  return GeomAbs_CN; 
}

//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Conic::IsCN (const Standard_Integer ) const 
{
  return Standard_True; 
}
