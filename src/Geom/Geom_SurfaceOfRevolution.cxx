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


#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <GeomEvaluator_SurfaceOfRevolution.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_SurfaceOfRevolution,Geom_SweptSurface)

#define  POLES    (poles->Array2())
#define  WEIGHTS  (weights->Array2())
#define  UKNOTS   (uknots->Array1())
#define  VKNOTS   (vknots->Array1())
#define  UFKNOTS  (ufknots->Array1())
#define  VFKNOTS  (vfknots->Array1())
#define  FMULTS   (BSplCLib::NoMults())

typedef Geom_SurfaceOfRevolution         SurfaceOfRevolution;
typedef Geom_Curve                       Curve;
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Dir  Dir;
typedef gp_Lin  Lin;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;





//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_SurfaceOfRevolution::Copy () const {

  return new Geom_SurfaceOfRevolution (basisCurve, Axis());
}


//=======================================================================
//function : Geom_SurfaceOfRevolution
//purpose  : 
//=======================================================================

Geom_SurfaceOfRevolution::Geom_SurfaceOfRevolution 
  (const Handle(Geom_Curve)& C , 
   const Ax1&           A1 ) : loc (A1.Location()) {

  direction  = A1.Direction();
  SetBasisCurve(C);
}


//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::UReverse () { 

  direction.Reverse();
  myEvaluator->SetDirection(direction);
}


//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_SurfaceOfRevolution::UReversedParameter (const Standard_Real U) const {

  return ( 2.*M_PI - U);
}


//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::VReverse () { 

  basisCurve->Reverse(); 
}


//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_SurfaceOfRevolution::VReversedParameter (const Standard_Real V) const {

  return basisCurve->ReversedParameter(V);
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

const gp_Pnt& Geom_SurfaceOfRevolution::Location () const { 

  return loc; 
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsUPeriodic () const {

  return Standard_True; 
}

//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsCNu (const Standard_Integer ) const  {

  return Standard_True;
}

//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

Ax1 Geom_SurfaceOfRevolution::Axis () const  { 

  return Ax1 (loc, direction); 
}

//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsCNv (const Standard_Integer N) const {

  Standard_RangeError_Raise_if (N < 0, " ");
  return basisCurve->IsCN(N);
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsUClosed () const { 

  return Standard_True; 
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsVClosed () const 
{ 
  return basisCurve->IsClosed();
}


//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsVPeriodic () const { 

  return basisCurve->IsPeriodic(); 
}


//=======================================================================
//function : SetAxis
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::SetAxis (const Ax1& A1) {

   direction = A1.Direction();
   loc = A1.Location();
   myEvaluator->SetAxis(A1);
}


//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::SetDirection (const Dir& V) {

   direction = V;
   myEvaluator->SetDirection(direction);
}


//=======================================================================
//function : SetBasisCurve
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::SetBasisCurve (const Handle(Geom_Curve)& C) {

   basisCurve = Handle(Geom_Curve)::DownCast(C->Copy());
   smooth     = C->Continuity();
   myEvaluator = new GeomEvaluator_SurfaceOfRevolution(basisCurve, direction, loc);
}


//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::SetLocation (const Pnt& P) {

   loc = P;
   myEvaluator->SetLocation(loc);
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::Bounds ( Standard_Real& U1, 
				        Standard_Real& U2, 
				        Standard_Real& V1, 
				        Standard_Real& V2 ) const {

  U1 = 0.0; 
  U2 = 2.0 * M_PI; 
  V1 = basisCurve->FirstParameter();  
  V2 = basisCurve->LastParameter();
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::D0
(const Standard_Real U, const Standard_Real V, Pnt& P) const
{
  myEvaluator->D0(U, V, P);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::D1 
  (const Standard_Real U, const Standard_Real V, 
         Pnt& P, 
         Vec& D1U, Vec& D1V   ) const
{
  myEvaluator->D1(U, V, P, D1U, D1V);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::D2 
  (const Standard_Real   U, const Standard_Real V,
         Pnt&   P, 
         Vec& D1U, Vec& D1V, 
         Vec& D2U, Vec& D2V, Vec& D2UV ) const
{
  myEvaluator->D2(U, V, P, D1U, D1V, D2U, D2V, D2UV);
}



//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::D3 
  (const Standard_Real U, const Standard_Real V,
         Pnt& P,
         Vec& D1U, Vec& D1V, 
         Vec& D2U, Vec& D2V, Vec& D2UV,
         Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV ) const
{
  myEvaluator->D3(U, V, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_SurfaceOfRevolution::DN (const Standard_Real    U , const Standard_Real    V, 
                                  const Standard_Integer Nu, const Standard_Integer Nv) const
{
  return myEvaluator->DN(U, V, Nu, Nv);
}


//=======================================================================
//function : ReferencePlane
//purpose  : 
//=======================================================================

Ax2 Geom_SurfaceOfRevolution::ReferencePlane() const {
        
   throw Standard_NotImplemented();
}


//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SurfaceOfRevolution::UIso (const Standard_Real U) const {

   Handle(Geom_Curve) C = Handle(Geom_Curve)::DownCast(basisCurve->Copy());
   Ax1 RotAxis = Ax1 (loc, direction);
   C->Rotate (RotAxis, U);
   return C;
}


//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SurfaceOfRevolution::VIso (const Standard_Real V) const {

  Handle(Geom_Circle) Circ;
  Pnt Pc = basisCurve->Value (V);
  gp_Lin L1(loc,direction);
  Standard_Real Rad= L1.Distance(Pc);

  Ax2 Rep ;
  if ( Rad > gp::Resolution()) { 
    XYZ P  = Pc.XYZ(); 
    XYZ C;
    C.SetLinearForm((P-loc.XYZ()).Dot(direction.XYZ()), 
		    direction.XYZ(), loc.XYZ() );
    P = P-C;
    if(P.Modulus() > gp::Resolution()) {
      gp_Dir D = P.Normalized();
      Rep = gp_Ax2(C, direction, D);
    }
    else 
      Rep = gp_Ax2(C, direction);
  }
  else
    Rep = gp_Ax2(Pc, direction);

  Circ   = new Geom_Circle (Rep, Rad);
  return Circ;
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::Transform (const Trsf& T) {

  loc.Transform (T);
  direction.Transform (T);
  basisCurve->Transform (T);
  if(T.ScaleFactor()*T.HVectorialPart().Determinant() < 0.) UReverse(); 
  myEvaluator->SetDirection(direction);
  myEvaluator->SetLocation(loc);
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_SurfaceOfRevolution::TransformParameters(Standard_Real& ,
						   Standard_Real& V,
						   const gp_Trsf& T) 
const
{
  V = basisCurve->TransformedParameter(V,T);
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_SurfaceOfRevolution::ParametricTransformation
(const gp_Trsf& T) const
{
  gp_GTrsf2d T2;
  gp_Ax2d Axis(gp::Origin2d(),gp::DX2d());
  T2.SetAffinity(Axis, basisCurve->ParametricTransformation(T));
  return T2;
}

