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


#include <ElCLib.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Ellipse,Geom2d_Conic)

typedef Geom2d_Ellipse         Ellipse;
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

Handle(Geom2d_Geometry) Geom2d_Ellipse::Copy() const 
{
  Handle(Geom2d_Ellipse) E;
  E = new Ellipse (pos, majorRadius, minorRadius);
  return E;
}


//=======================================================================
//function : Geom2d_Ellipse
//purpose  : 
//=======================================================================

Geom2d_Ellipse::Geom2d_Ellipse (const gp_Elips2d& E) {
 
  majorRadius = E.MajorRadius();
  minorRadius = E.MinorRadius();
  pos = E.Axis();
}



//=======================================================================
//function : Geom2d_Ellipse
//purpose  : 
//=======================================================================

Geom2d_Ellipse::Geom2d_Ellipse (const Ax2d& MajorAxis,
				const Standard_Real MajorRadius,
				const Standard_Real MinorRadius,
				const Standard_Boolean Sense) 
:majorRadius (MajorRadius), minorRadius (MinorRadius){

  if (MajorRadius < MinorRadius || MinorRadius < 0.0 ) {
    throw Standard_ConstructionError();
  }
  pos = gp_Ax22d(MajorAxis, Sense);
}


//=======================================================================
//function : Geom2d_Ellipse
//purpose  : 
//=======================================================================

Geom2d_Ellipse::Geom2d_Ellipse (const gp_Ax22d& Axis, 
				const Standard_Real MajorRadius, 
				const Standard_Real MinorRadius) 
: majorRadius (MajorRadius), minorRadius (MinorRadius)
{
  if (MajorRadius < MinorRadius || MinorRadius < 0.0 ) {
    throw Standard_ConstructionError();
  }
  pos = Axis;
}


//=======================================================================
//function : SetElips2d
//purpose  : 
//=======================================================================

void Geom2d_Ellipse::SetElips2d (const gp_Elips2d& E) 
{
  majorRadius = E.MajorRadius();
  minorRadius = E.MinorRadius();
  pos = E.Axis();
}


//=======================================================================
//function : SetMajorRadius
//purpose  : 
//=======================================================================

void Geom2d_Ellipse::SetMajorRadius (const Standard_Real MajorRadius) 
{
  if (MajorRadius < minorRadius) 
    throw Standard_ConstructionError();
  else                           
    majorRadius = MajorRadius; 
}


//=======================================================================
//function : SetMinorRadius
//purpose  : 
//=======================================================================

void Geom2d_Ellipse::SetMinorRadius (const Standard_Real MinorRadius) 
{
   if (MinorRadius < 0 || majorRadius < MinorRadius) 
     { throw Standard_ConstructionError(); }
   else 
     { minorRadius = MinorRadius; }
}


//=======================================================================
//function : Elips2d
//purpose  : 
//=======================================================================

gp_Elips2d Geom2d_Ellipse::Elips2d () const 
{
  return gp_Elips2d (pos, majorRadius, minorRadius);
}

//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Ellipse::ReversedParameter( const Standard_Real U) const 
{
  return (2. * M_PI - U);
}

//=======================================================================
//function : Directrix1
//purpose  : 
//=======================================================================

Ax2d Geom2d_Ellipse::Directrix1 () const 
{
  gp_Elips2d Ev (pos, majorRadius, minorRadius);
  return Ev.Directrix1();
}


//=======================================================================
//function : Directrix2
//purpose  : 
//=======================================================================

Ax2d Geom2d_Ellipse::Directrix2 () const 
{
  gp_Elips2d Ev (pos, majorRadius, minorRadius);
  return Ev.Directrix2();
}


//=======================================================================
//function : Eccentricity
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Ellipse::Eccentricity () const 
{
  if (majorRadius == 0.0) 
    { return 0.0; }
  else 
    { return 
	(Sqrt(majorRadius*majorRadius-minorRadius*minorRadius))/majorRadius;
    }
}


//=======================================================================
//function : Focal
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Ellipse::Focal () const 
{
  return 2.0 * Sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
}


//=======================================================================
//function : Focus1
//purpose  : 
//=======================================================================

Pnt2d Geom2d_Ellipse::Focus1 () const 
{
  Standard_Real C = Sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  return Pnt2d (pos.Location().X() + C * pos.XDirection().X(),
                pos.Location().Y() + C * pos.XDirection().Y());
}


//=======================================================================
//function : Focus2
//purpose  : 
//=======================================================================

Pnt2d Geom2d_Ellipse::Focus2 () const 
{
  Standard_Real C = Sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  return Pnt2d (pos.Location().X() - C * pos.XDirection().X(),
                pos.Location().Y() - C * pos.XDirection().Y());
}

//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Ellipse::MajorRadius () const       
{
  return majorRadius; 
}

//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Ellipse::MinorRadius () const       
{
  return minorRadius; 
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Ellipse::Parameter () const 
{
  if (majorRadius == 0.0) 
    return 0.0;
  else                    
    return (minorRadius * minorRadius)/majorRadius;
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Ellipse::FirstParameter () const    
{
  return 0.0; 
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Ellipse::LastParameter () const     
{
  return 2.0 * M_PI; 
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Ellipse::IsClosed () const       
{
  return Standard_True; 
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Ellipse::IsPeriodic () const     
{
  return Standard_True; 
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2d_Ellipse::D0 (const Standard_Real U, Pnt2d& P) const 
{
  P = ElCLib::EllipseValue (U, pos, majorRadius, minorRadius);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom2d_Ellipse::D1 (const Standard_Real U, Pnt2d& P, Vec2d& V1) const 
{
  ElCLib::EllipseD1 (U, pos, majorRadius, minorRadius, P, V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_Ellipse::D2 (const Standard_Real U, 
			       Pnt2d& P, 
			       Vec2d& V1, Vec2d& V2) const 
{
  ElCLib::EllipseD2 (U, pos, majorRadius, minorRadius, P, V1, V2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_Ellipse::D3 (const Standard_Real U, 
			       Pnt2d& P, 
			       Vec2d& V1, Vec2d& V2, Vec2d& V3) const 
{
  ElCLib::EllipseD3 (U, pos, majorRadius, minorRadius, P, V1, V2, V3);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec2d Geom2d_Ellipse::DN (const Standard_Real U, const Standard_Integer N) const 
{
  Standard_RangeError_Raise_if (N < 1, " ");
  return ElCLib::EllipseDN (U, pos, majorRadius, minorRadius, N);
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom2d_Ellipse::Transform (const Trsf2d& T) 
{
  majorRadius = majorRadius * Abs(T.ScaleFactor());
  minorRadius = minorRadius * Abs(T.ScaleFactor());
  pos.Transform(T);
}
