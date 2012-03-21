// Created on: 1993-03-24
// Created by: JCV
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





#include <Geom2d_Line.ixx>

#include <Precision.hxx>
#include <ElCLib.hxx>
#include <gp_XY.hxx>
#include <Standard_RangeError.hxx>


typedef Geom2d_Line         Line;
typedef Handle(Geom2d_Line) Handle(Line);
typedef gp_Ax2d   Ax2d;
typedef gp_Dir2d  Dir2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Vec2d  Vec2d;
typedef gp_Trsf2d Trsf2d;
typedef gp_XY     XY;





//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom2d_Geometry) Geom2d_Line::Copy() const 
{
  Handle(Line) L;
  L = new Line (pos);
  return L;
}


//=======================================================================
//function : Geom2d_Line
//purpose  : 
//=======================================================================

Geom2d_Line::Geom2d_Line (const Ax2d& A)           : pos (A) { }

//=======================================================================
//function : Geom2d_Line
//purpose  : 
//=======================================================================

Geom2d_Line::Geom2d_Line (const gp_Lin2d& L)       : pos (L.Position()) { }

//=======================================================================
//function : Geom2d_Line
//purpose  : 
//=======================================================================

Geom2d_Line::Geom2d_Line (const Pnt2d& P, const Dir2d& V)  : pos (P, V) { }

//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void Geom2d_Line::SetDirection (const Dir2d& V) { pos.SetDirection (V); }

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

const gp_Dir2d& Geom2d_Line::Direction () const { return pos.Direction (); }

//=======================================================================
//function : SetLin2d
//purpose  : 
//=======================================================================

void Geom2d_Line::SetLin2d (const gp_Lin2d& L)  { pos = L.Position(); }

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Geom2d_Line::SetLocation (const Pnt2d& P)  { pos.SetLocation (P); }

//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

const gp_Pnt2d& Geom2d_Line::Location () const  { return pos.Location (); }

//=======================================================================
//function : SetPosition
//purpose  : 
//=======================================================================

void Geom2d_Line::SetPosition (const Ax2d& A)   { pos = A; }

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

const gp_Ax2d& Geom2d_Line::Position () const   { return pos; }

//=======================================================================
//function : Lin2d
//purpose  : 
//=======================================================================

gp_Lin2d Geom2d_Line::Lin2d () const            { return gp_Lin2d (pos); }

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom2d_Line::Reverse ()                    { pos.Reverse(); }

//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Line::ReversedParameter( const Standard_Real U) const  { return (-U); }

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Line::FirstParameter () const       
{ return -Precision::Infinite(); }

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Line::LastParameter () const        
{ return Precision::Infinite(); }

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Line::IsClosed () const          { return Standard_False; }

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Line::IsPeriodic () const        { return Standard_False;  }

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom2d_Line::Continuity () const   { return GeomAbs_CN; }

//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Line::IsCN (const Standard_Integer ) const      { return Standard_True; }

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2d_Line::D0 (const Standard_Real U, Pnt2d& P) const  
{
  P = ElCLib::LineValue (U, pos);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom2d_Line::D1 (const Standard_Real U, Pnt2d& P, Vec2d& V1) const 
{
  ElCLib::LineD1 (U, pos, P, V1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_Line::D2 (const Standard_Real U, 
		            Pnt2d& P, 
		            Vec2d& V1, Vec2d& V2) const 
{
  ElCLib::LineD1 (U, pos, P, V1);
  V2.SetCoord (0.0, 0.0);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_Line::D3 (const Standard_Real U, 
		            Pnt2d& P, 
		            Vec2d& V1, Vec2d& V2, Vec2d& V3) const
{
  ElCLib::LineD1 (U, pos, P, V1);
  V2.SetCoord (0.0, 0.0);
  V3.SetCoord (0.0, 0.0);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec2d Geom2d_Line::DN
  (const Standard_Real     ,
   const Standard_Integer N ) const
{
  Standard_RangeError_Raise_if (N <= 0, " ");
  if (N == 1) 
    return Vec2d (pos.Direction ());
  else        
    return Vec2d (0.0, 0.0);
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom2d_Line::Transform (const Trsf2d& T)          { pos.Transform (T); }

//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Line::TransformedParameter(const Standard_Real U,
						const gp_Trsf2d& T) const
{
  if (Precision::IsInfinite(U)) return U;
  return U * Abs(T.ScaleFactor());
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Line::ParametricTransformation(const gp_Trsf2d& T) const
{
  return Abs(T.ScaleFactor());
}


//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Line::Distance (const gp_Pnt2d& P) const {

  gp_Lin2d L (pos);   
  return L.Distance (P);
}

