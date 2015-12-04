// Created on: 1993-03-10
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


#include <ElCLib.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Line,Geom_Curve)

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================
Handle(Geom_Geometry) Geom_Line::Copy() const {

   Handle(Geom_Line) L;
   L = new Geom_Line (pos);
   return L;
}

//=======================================================================
//function : Geom_Line
//purpose  : 
//=======================================================================

Geom_Line::Geom_Line (const gp_Ax1& A) : pos (A) { }

//=======================================================================
//function : Geom_Line
//purpose  : 
//=======================================================================

Geom_Line::Geom_Line (const gp_Lin& L) : pos (L.Position()) { }

//=======================================================================
//function : Geom_Line
//purpose  : 
//=======================================================================

Geom_Line::Geom_Line (const gp_Pnt& P, const gp_Dir& V) : pos (P, V) { }

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom_Line::Reverse () { pos.Reverse(); }

//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Line::ReversedParameter( const Standard_Real U) const 
{ return (-U);}

//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void Geom_Line::SetDirection (const gp_Dir& V) { pos.SetDirection (V); }

//=======================================================================
//function : SetLin
//purpose  : 
//=======================================================================

void Geom_Line::SetLin (const gp_Lin& L) { pos = L.Position(); }

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Geom_Line::SetLocation (const gp_Pnt& P) { pos.SetLocation (P); }

//=======================================================================
//function : SetPosition
//purpose  : 
//=======================================================================

void Geom_Line::SetPosition (const gp_Ax1& A1) { pos = A1; }

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

const gp_Ax1& Geom_Line::Position () const { return pos; }

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Line::IsClosed () const { return Standard_False; }

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Line::IsPeriodic () const { return Standard_False;  }

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_Line::Continuity () const { return GeomAbs_CN; }

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Line::FirstParameter () const 
{ return -Precision::Infinite(); }

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Line::LastParameter () const 
{ return Precision::Infinite(); }

//=======================================================================
//function : Lin
//purpose  : 
//=======================================================================

gp_Lin Geom_Line::Lin () const { return gp_Lin (pos); }


//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Line::IsCN (const Standard_Integer ) const 
{ return Standard_True; }

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_Line::Transform (const gp_Trsf& T) { pos.Transform (T); }

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_Line::D0 (const Standard_Real U, gp_Pnt& P) const  
{ 
  P = ElCLib::LineValue (U, pos);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_Line::D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const {
 
  ElCLib::LineD1 (U, pos, P, V1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_Line::D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const {

  ElCLib::LineD1 (U, pos, P, V1);
  V2.SetCoord (0.0, 0.0, 0.0);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_Line::D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const{

  ElCLib::LineD1 (U, pos, P, V1);
  V2.SetCoord (0.0, 0.0, 0.0);
  V3.SetCoord (0.0, 0.0, 0.0);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Geom_Line::DN (const Standard_Real , const Standard_Integer N) const {

  Standard_RangeError_Raise_if (N <= 0, " ");
  if (N == 1) return gp_Vec (pos.Direction ());
  else        return gp_Vec (0.0, 0.0, 0.0);
}

//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Line::TransformedParameter(const Standard_Real U,
					      const gp_Trsf& T) const
{
  if (Precision::IsInfinite(U)) return U;
  return U * Abs(T.ScaleFactor());
}


//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Line::ParametricTransformation(const gp_Trsf& T) const
{
  return Abs(T.ScaleFactor());
}


