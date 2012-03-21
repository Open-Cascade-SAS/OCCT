// Created on: 1993-03-10
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



#include <Geom_Parabola.ixx>

#include <Precision.hxx>
#include <ElCLib.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>

typedef Geom_Parabola         Parabola;
typedef Handle(Geom_Parabola) Handle(Parabola);
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;






//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_Parabola::Copy() const {

   Handle(Parabola) Prb;
   Prb = new Parabola (pos, focalLength);
   return Prb;
}



//=======================================================================
//function : Geom_Parabola
//purpose  : 
//=======================================================================

Geom_Parabola::Geom_Parabola (const gp_Parab& Prb) 
 : focalLength (Prb.Focal()) 
{ pos = Prb.Position(); }


//=======================================================================
//function : Geom_Parabola
//purpose  : 
//=======================================================================

Geom_Parabola::Geom_Parabola (const Ax2& A2, const Standard_Real Focal) 
 : focalLength (Focal) {
   
  if(Focal < 0.0)  Standard_ConstructionError::Raise();
  pos = A2;
}


//=======================================================================
//function : Geom_Parabola
//purpose  : 
//=======================================================================

Geom_Parabola::Geom_Parabola (const Ax1& D, const Pnt& F) {

   gp_Parab Prb (D, F);
   pos = Prb.Position();
   focalLength = Prb.Focal();
}

//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Parabola::ReversedParameter( const Standard_Real U) const
{
  return (-U);
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Parabola::IsClosed () const { return Standard_False; }

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Parabola::IsPeriodic () const     {  return Standard_False; }

//=======================================================================
//function : Eccentricity
//purpose  : 
//=======================================================================

Standard_Real Geom_Parabola::Eccentricity () const      { return 1.0; }

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Parabola::FirstParameter () const    
{ return -Precision::Infinite(); }

//=======================================================================
//function : Focal
//purpose  : 
//=======================================================================

Standard_Real Geom_Parabola::Focal () const             { return focalLength; }

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Parabola::LastParameter () const     
{ return Precision::Infinite(); }

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Parabola::Parameter () const         { return 2.0 * focalLength; }

//=======================================================================
//function : SetFocal
//purpose  : 
//=======================================================================

void Geom_Parabola::SetFocal (const Standard_Real Focal) {

   if (Focal < 0.0)  Standard_ConstructionError::Raise();
   focalLength = Focal;
}


//=======================================================================
//function : SetParab
//purpose  : 
//=======================================================================

void Geom_Parabola::SetParab (const gp_Parab& Prb) {

   focalLength = Prb.Focal ();
   pos = Prb.Position ();
}


//=======================================================================
//function : Directrix
//purpose  : 
//=======================================================================

Ax1 Geom_Parabola::Directrix () const {

   gp_Parab Prb (pos, focalLength);
   return Prb.Directrix();
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_Parabola::D0 (const Standard_Real U, Pnt& P) const {

   P = ElCLib::ParabolaValue (U, pos, focalLength);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_Parabola::D1 (const Standard_Real U, Pnt& P, Vec& V1) const {

  ElCLib::ParabolaD1 (U, pos, focalLength, P, V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_Parabola::D2 (const Standard_Real U, Pnt& P, Vec& V1, Vec& V2) const {

   ElCLib::ParabolaD2 (U, pos, focalLength, P, V1, V2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_Parabola::D3 (const Standard_Real U, 
			Pnt& P, Vec& V1, Vec& V2, Vec& V3) const {

   ElCLib::ParabolaD2 (U, pos, focalLength, P, V1, V2);
   V3.SetCoord (0.0, 0.0, 0.0);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_Parabola::DN (const Standard_Real U, const Standard_Integer N) const {

  Standard_RangeError_Raise_if (N < 1, " ");
   return ElCLib::ParabolaDN (U, pos, focalLength, N);
}


//=======================================================================
//function : Focus
//purpose  : 
//=======================================================================

Pnt Geom_Parabola::Focus () const {

  Standard_Real Xp, Yp, Zp, Xd, Yd, Zd;
  pos.Location().Coord (Xp, Yp, Zp);
  pos.XDirection().Coord (Xd, Yd, Zd);
  return Pnt (Xp + focalLength*Xd, Yp + focalLength*Yd, Zp + focalLength*Zd);
}


//=======================================================================
//function : Parab
//purpose  : 
//=======================================================================

gp_Parab Geom_Parabola::Parab () const {

   return gp_Parab (pos, focalLength);
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_Parabola::Transform (const Trsf& T) {

  focalLength *= Abs(T.ScaleFactor());
  pos.Transform (T);
}



//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Parabola::TransformedParameter(const Standard_Real U,
						  const gp_Trsf& T) const
{
  if (Precision::IsInfinite(U)) return U;
  return U * Abs(T.ScaleFactor());
}


//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Parabola::ParametricTransformation(const gp_Trsf& T) const
{
  return Abs(T.ScaleFactor());
}


