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
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Hyperbola,Geom2d_Conic)

typedef Geom2d_Hyperbola         Hyperbola;
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

Handle(Geom2d_Geometry) Geom2d_Hyperbola::Copy() const 
{
  Handle(Geom2d_Hyperbola) H;
  H = new Hyperbola (pos, majorRadius, minorRadius);
  return H;
}



//=======================================================================
//function : Geom2d_Hyperbola
//purpose  : 
//=======================================================================

Geom2d_Hyperbola::Geom2d_Hyperbola (const gp_Hypr2d& H) 
{
  majorRadius = H.MajorRadius();
  minorRadius = H.MinorRadius();
  pos = H.Axis();
}


//=======================================================================
//function : Geom2d_Hyperbola
//purpose  : 
//=======================================================================

Geom2d_Hyperbola::Geom2d_Hyperbola (const Ax2d& A, 
				    const Standard_Real MajorRadius, 
				    const Standard_Real MinorRadius, 
				    const Standard_Boolean Sense) 
: majorRadius (MajorRadius), minorRadius (MinorRadius) 
{
  if( MajorRadius < 0.0|| MinorRadius < 0.0) 
    throw Standard_ConstructionError();
  pos = gp_Ax22d(A, Sense);
}

//=======================================================================
//function : Geom2d_Hyperbola
//purpose  : 
//=======================================================================

Geom2d_Hyperbola::Geom2d_Hyperbola (const gp_Ax22d& Axis, 
				    const Standard_Real MajorRadius, 
				    const Standard_Real MinorRadius) 
: majorRadius (MajorRadius), minorRadius (MinorRadius) 
{
  if( MajorRadius < 0.0|| MinorRadius < 0.0)
    throw Standard_ConstructionError();
  pos = Axis;
}


//=======================================================================
//function : SetHypr2d
//purpose  : 
//=======================================================================

void Geom2d_Hyperbola::SetHypr2d (const gp_Hypr2d& H) 
{
  majorRadius = H.MajorRadius();
  minorRadius = H.MinorRadius();
  pos = H.Axis();
}


//=======================================================================
//function : SetMajorRadius
//purpose  : 
//=======================================================================

void Geom2d_Hyperbola::SetMajorRadius (const Standard_Real MajorRadius) 
{
  if (MajorRadius < 0.0) 
    throw Standard_ConstructionError();
  else                   
    majorRadius = MajorRadius;
}


//=======================================================================
//function : SetMinorRadius
//purpose  : 
//=======================================================================

void Geom2d_Hyperbola::SetMinorRadius (const Standard_Real MinorRadius) 
{
  if (MinorRadius < 0.0 ) 
    throw Standard_ConstructionError();
  else
    minorRadius = MinorRadius;
}


//=======================================================================
//function : Hypr2d
//purpose  : 
//=======================================================================

gp_Hypr2d Geom2d_Hyperbola::Hypr2d () const 
{
  return gp_Hypr2d (pos, majorRadius, minorRadius);
}


//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Hyperbola::ReversedParameter( const Standard_Real U) const
{
  return ( -U);
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Hyperbola::FirstParameter () const   
{
  return -Precision::Infinite(); 
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Hyperbola::LastParameter () const    
{
  return Precision::Infinite(); 
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Hyperbola::IsClosed () const      
{
  return Standard_False; 
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Hyperbola::IsPeriodic () const    
{
  return Standard_False; 
}
 
//=======================================================================
//function : Asymptote1
//purpose  : 
//=======================================================================

Ax2d Geom2d_Hyperbola::Asymptote1 () const 
{
  gp_Hypr2d Hv (pos, majorRadius, minorRadius);
  return Hv.Asymptote1();
}

//=======================================================================
//function : Asymptote2
//purpose  : 
//=======================================================================

Ax2d Geom2d_Hyperbola::Asymptote2 () const 
{
  gp_Hypr2d Hv (pos, majorRadius, minorRadius);
  return Hv.Asymptote2();
}

//=======================================================================
//function : ConjugateBranch1
//purpose  : 
//=======================================================================

gp_Hypr2d Geom2d_Hyperbola::ConjugateBranch1 () const 
{
  gp_Hypr2d Hv (pos, majorRadius, minorRadius);
  return Hv.ConjugateBranch1 ();
}

//=======================================================================
//function : ConjugateBranch2
//purpose  : 
//=======================================================================

gp_Hypr2d Geom2d_Hyperbola::ConjugateBranch2 () const 
{
  gp_Hypr2d Hv (pos, majorRadius, minorRadius);
  return Hv.ConjugateBranch2 ();
}

//=======================================================================
//function : Directrix1
//purpose  : 
//=======================================================================

Ax2d Geom2d_Hyperbola::Directrix1 () const 
{
  gp_Hypr2d Hv (pos, majorRadius, minorRadius);
  return Hv.Directrix1 ();
}

//=======================================================================
//function : Directrix2
//purpose  : 
//=======================================================================

Ax2d Geom2d_Hyperbola::Directrix2 () const 
{
  gp_Hypr2d Hv (pos, majorRadius, minorRadius);
  return Hv.Directrix2 ();
}

//=======================================================================
//function : Eccentricity
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Hyperbola::Eccentricity () const 
{
  Standard_DomainError_Raise_if (majorRadius <= gp::Resolution(), " ");
  return 
   (Sqrt(majorRadius*majorRadius+minorRadius*minorRadius))/majorRadius;
}

//=======================================================================
//function : Focal
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Hyperbola::Focal () const 
{
  return 2.0 * Sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
}

//=======================================================================
//function : Focus1
//purpose  : 
//=======================================================================

Pnt2d Geom2d_Hyperbola::Focus1 () const 
{
  Standard_Real C = Sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
  XY Pxy = pos.XDirection().XY();
  Pxy.Multiply (C);
  Pxy.Add (pos.Location().XY());  
  return Pnt2d (Pxy);
}

//=======================================================================
//function : Focus2
//purpose  : 
//=======================================================================

Pnt2d Geom2d_Hyperbola::Focus2 () const 
{
  Standard_Real C = Sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
  XY Pxy = pos.XDirection().XY();
  Pxy.Multiply (-C);
  Pxy.Add (pos.Location().XY());  
  return Pnt2d (Pxy);
}

//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Hyperbola::MajorRadius () const      
{
  return majorRadius; 
}

//=======================================================================
//function : MinorRadius
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Hyperbola::MinorRadius () const      
{
  return minorRadius; 
}

//=======================================================================
//function : OtherBranch
//purpose  : 
//=======================================================================

gp_Hypr2d Geom2d_Hyperbola::OtherBranch () const 
{
  gp_Hypr2d Hv (pos, majorRadius, minorRadius);
  return Hv.OtherBranch ();
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Hyperbola::Parameter () const 
{
  Standard_DomainError_Raise_if (majorRadius <= gp::Resolution(), " ");
  return (minorRadius * minorRadius) / majorRadius;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2d_Hyperbola::D0 (const Standard_Real   U,  
			         Pnt2d& P ) const 
{
  P = ElCLib::HyperbolaValue (U, pos, majorRadius, minorRadius);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom2d_Hyperbola::D1 (const Standard_Real   U, 
			         Pnt2d& P, 
			         Vec2d& V1) const 
{
  ElCLib::HyperbolaD1 (U, pos, majorRadius, minorRadius, P, V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_Hyperbola::D2 (const Standard_Real   U, 
			         Pnt2d& P, 
                                 Vec2d& V1, Vec2d& V2) const
{
  ElCLib::HyperbolaD2 (U, pos, majorRadius, minorRadius, P, V1, V2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_Hyperbola::D3 (const Standard_Real U, 
			         Pnt2d& P, 
			         Vec2d& V1, Vec2d& V2, Vec2d& V3) const 
{
  ElCLib::HyperbolaD3 (U, pos, majorRadius, minorRadius, P, V1, V2, V3);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec2d Geom2d_Hyperbola::DN (const Standard_Real U, const Standard_Integer N) const 
{
  Standard_RangeError_Raise_if (N < 1, " ");
  return ElCLib::HyperbolaDN (U, pos, majorRadius, minorRadius, N);
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom2d_Hyperbola::Transform (const Trsf2d& T) 
{
  majorRadius = majorRadius * Abs (T.ScaleFactor());
  minorRadius = minorRadius * Abs (T.ScaleFactor());
  pos.Transform(T);
}
