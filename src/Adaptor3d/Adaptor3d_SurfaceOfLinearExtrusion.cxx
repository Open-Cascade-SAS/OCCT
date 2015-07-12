// Created on: 1993-04-21
// Created by: Bruno DUMORTIER
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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceOfLinearExtrusion.hxx>
#include <Adaptor3d_SurfaceOfLinearExtrusion.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : Adaptor3d_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================
Adaptor3d_SurfaceOfLinearExtrusion::Adaptor3d_SurfaceOfLinearExtrusion()
{}

//=======================================================================
//function : Adaptor3d_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================

Adaptor3d_SurfaceOfLinearExtrusion::Adaptor3d_SurfaceOfLinearExtrusion
(const Handle(Adaptor3d_HCurve)& C)
{
  Load( C);
}

//=======================================================================
//function : Adaptor3d_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================

Adaptor3d_SurfaceOfLinearExtrusion::Adaptor3d_SurfaceOfLinearExtrusion
(const Handle(Adaptor3d_HCurve)& C,
 const gp_Dir&        V)
{
  Load( C);
  Load( V);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfLinearExtrusion::Load( const Handle(Adaptor3d_HCurve)& C)
{
  myBasisCurve = C;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfLinearExtrusion::Load( const gp_Dir& V)
{
  myDirection = V;
}

//=======================================================================
//function : FirstUParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfLinearExtrusion::FirstUParameter() const 
{
  return myBasisCurve->FirstParameter();
}

//=======================================================================
//function : LastUParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfLinearExtrusion::LastUParameter() const 
{
  return myBasisCurve->LastParameter();
}

//=======================================================================
//function : FirstVParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfLinearExtrusion::FirstVParameter() const 
{
  return RealFirst();
}

//=======================================================================
//function : LastVParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfLinearExtrusion::LastVParameter() const 
{
  return RealLast();
}

//=======================================================================
//function : UContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_SurfaceOfLinearExtrusion::UContinuity() const 
{
  return myBasisCurve->Continuity();
}

//=======================================================================
//function : VContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_SurfaceOfLinearExtrusion::VContinuity() const 
{
  return GeomAbs_CN;
}

//=======================================================================
//function : NbUIntervals
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfLinearExtrusion::NbUIntervals
(const GeomAbs_Shape S)  const 
{
  return   myBasisCurve->NbIntervals(S);
}

//=======================================================================
//function : NbVIntervals
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfLinearExtrusion::NbVIntervals
(const GeomAbs_Shape ) const 
{
  return 1;
}

//=======================================================================
//function : UIntervals
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfLinearExtrusion::UIntervals
(TColStd_Array1OfReal&  T, const GeomAbs_Shape S) const 
{
  myBasisCurve->Intervals(T,S);
}

//=======================================================================
//function : VIntervals
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfLinearExtrusion::VIntervals
//(TColStd_Array1OfReal&  T, const GeomAbs_Shape S) const 
(TColStd_Array1OfReal&  T, const GeomAbs_Shape ) const 
{
 T(T.Lower()) = FirstVParameter() ;
 T(T.Lower() + 1) = LastVParameter() ;
}

//=======================================================================
//function : VTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface)  Adaptor3d_SurfaceOfLinearExtrusion::VTrim
(const Standard_Real First ,
 const Standard_Real Last,
 const Standard_Real Tol) const 
{
  Handle(Adaptor3d_HCurve) newBasisCurve =
  myBasisCurve->Trim(First, Last, Tol);
  
  Adaptor3d_SurfaceOfLinearExtrusion * SurfacePtr =
  new  Adaptor3d_SurfaceOfLinearExtrusion(newBasisCurve, myDirection) ;

  return new Adaptor3d_HSurfaceOfLinearExtrusion(*SurfacePtr);
}

//=======================================================================
//function : UTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface)  Adaptor3d_SurfaceOfLinearExtrusion::UTrim
//(const Standard_Real First ,
// const Standard_Real Last,
// const Standard_Real Tol) const 
(const Standard_Real ,
 const Standard_Real ,
 const Standard_Real ) const 
{
  Adaptor3d_SurfaceOfLinearExtrusion * SurfacePtr = 
  new  Adaptor3d_SurfaceOfLinearExtrusion(myBasisCurve,myDirection);

  return new Adaptor3d_HSurfaceOfLinearExtrusion(*SurfacePtr) ;
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfLinearExtrusion::IsUClosed() const 
{
  return myBasisCurve->IsClosed();
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfLinearExtrusion::IsVClosed() const 
{
  return Standard_True;
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfLinearExtrusion::IsUPeriodic() const 
{
  return myBasisCurve->IsPeriodic();
}

//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfLinearExtrusion::UPeriod() const 
{
  return myBasisCurve->Period() ;
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfLinearExtrusion::IsVPeriodic() const 
{
  return Standard_False;
}

//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfLinearExtrusion::VPeriod() const 
{
  Standard_DomainError::Raise("Adaptor3d_SurfaceOfLinearExtrusion::VPeriod");
  return 0.0e0 ;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt Adaptor3d_SurfaceOfLinearExtrusion::Value(const Standard_Real U, 
                                                 const Standard_Real V)
     const 
{
  gp_Pnt P;
  P = myBasisCurve->Value(U);
  P.Translate( V * gp_Vec(myDirection));
  return P;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfLinearExtrusion::D0(const Standard_Real U,
                                            const Standard_Real V, 
                                            gp_Pnt&             P) const 
{
  myBasisCurve->D0(U,P);
  P.Translate( V * gp_Vec(myDirection));
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfLinearExtrusion::D1(const Standard_Real U, 
                                            const Standard_Real V, 
                                                  gp_Pnt& P, 
                                                  gp_Vec& D1U, 
                                                  gp_Vec& D1V) const 
{
  myBasisCurve->D1(U,P,D1U);
  D0(U,V,P);
  D1V = gp_Vec(myDirection);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfLinearExtrusion::D2(const Standard_Real U, 
                                            const Standard_Real V,
                                            gp_Pnt& P, 
                                            gp_Vec& D1U, gp_Vec& D1V,
                                            gp_Vec& D2U, gp_Vec& D2V, 
                                            gp_Vec& D2UV) const 
{
  myBasisCurve->D2(U,P,D1U,D2U);
  D1V = gp_Vec(myDirection);
  D2V.SetCoord( 0., 0., 0.);
  D2UV.SetCoord( 0., 0., 0.);
  D0(U,V,P);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Adaptor3d_SurfaceOfLinearExtrusion::D3(const Standard_Real U, 
                                            const Standard_Real V,
                                            gp_Pnt& P,
                                            gp_Vec& D1U, gp_Vec& D1V,
                                            gp_Vec& D2U, gp_Vec& D2V, 
                                            gp_Vec& D2UV,
                                            gp_Vec& D3U, gp_Vec& D3V, 
                                            gp_Vec& D3UUV, gp_Vec& D3UVV) const 
{
  myBasisCurve->D3(U,P,D1U,D2U,D3U);
  D1V = gp_Vec(myDirection);
  D2V.SetCoord( 0., 0., 0.);
  D2UV.SetCoord( 0., 0., 0.);
  D3V.SetCoord( 0., 0., 0.);
  D3UUV.SetCoord( 0., 0., 0.);
  D3UVV.SetCoord( 0., 0., 0.);
  D0(U,V,P);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Adaptor3d_SurfaceOfLinearExtrusion::DN
(const Standard_Real    U, 
// const Standard_Real    V,
 const Standard_Real    ,
 const Standard_Integer NU, 
 const Standard_Integer NV) const 
{
  if ( (NU+NV)<1 || NU<0 || NV<0) {
    Standard_DomainError::Raise("Adaptor3d_SurfaceOfLinearExtrusion::DN");
    return gp_Vec();
  }
  else {
    if (NU == 0 && NV ==1) return gp_Vec( myDirection);
    else if (NV == 0)      return myBasisCurve->DN(U,NU);
    else                   return gp_Vec( 0., 0., 0.);
  }   
}

//=======================================================================
//function : UResolution
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfLinearExtrusion::UResolution
(const Standard_Real R3d) const 
{
  return myBasisCurve->Resolution(R3d);
}

//=======================================================================
//function : VResolution
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_SurfaceOfLinearExtrusion::VResolution
(const Standard_Real R3d) const 
{
  return R3d;
}

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_SurfaceType Adaptor3d_SurfaceOfLinearExtrusion::GetType() const 
{
  switch ( myBasisCurve->GetType()) {
    
  case GeomAbs_Line:
    {
      gp_Dir D = myBasisCurve->Line().Direction();
      if (myDirection.IsParallel( D, Precision::Angular())) {
        return GeomAbs_SurfaceOfExtrusion;
      }
      else {
        return GeomAbs_Plane;
      }
    }
    
  case GeomAbs_Circle:
    {
      gp_Dir D = (myBasisCurve->Circle()).Axis().Direction();
      if ( myDirection.IsParallel( D, Precision::Angular())) {
        return GeomAbs_Cylinder;
      }
      // JAG 10.11.95
      else if (myDirection.IsNormal(D, Precision::Angular())) {
        return GeomAbs_Plane;
      }
      else {
        return GeomAbs_SurfaceOfExtrusion;
      }
    }
    // JAG 10.11.95
    
  case GeomAbs_Ellipse:
    {
      gp_Dir D = (myBasisCurve->Ellipse()).Axis().Direction();
      if (myDirection.IsNormal(D, Precision::Angular())) {
        return GeomAbs_Plane;
      }
      else {
        return GeomAbs_SurfaceOfExtrusion;
      }
    }
    
  case GeomAbs_Parabola:
    {
      gp_Dir D = (myBasisCurve->Parabola()).Axis().Direction();
      if (myDirection.IsNormal(D, Precision::Angular())) {
        return GeomAbs_Plane;
      }
      else {
        return GeomAbs_SurfaceOfExtrusion;
      }
    }
    
  case GeomAbs_Hyperbola:
    {
      gp_Dir D = (myBasisCurve->Hyperbola()).Axis().Direction();
      if (myDirection.IsNormal(D, Precision::Angular())) {
        return GeomAbs_Plane;
      }
      else {
        return GeomAbs_SurfaceOfExtrusion;
      }
    }

  default:
    return GeomAbs_SurfaceOfExtrusion;

  }
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln Adaptor3d_SurfaceOfLinearExtrusion::Plane() const 
{ 
  Standard_NoSuchObject_Raise_if (GetType() != GeomAbs_Plane,
                  "Adaptor3d_SurfaceOfLinearExtrusion::Plane");
/*
  gp_Pnt P;
  gp_Vec Ox, Oy;
  D1( 0., 0., P, Ox, Oy);
  gp_Ax3 Ax3(P,gp_Dir(Ox^Oy),gp_Dir(Ox));
  if (gp_Dir(Oy).Dot(Ax3.YDirection())<0.){
    Ax3.YReverse();
  }
  return gp_Pln(Ax3);
*/

  gp_Pnt P;
  gp_Vec D1u, newZ;
  Standard_Real UFirst = myBasisCurve->FirstParameter();
  Standard_Real ULast  = myBasisCurve->LastParameter();
  if (Precision::IsNegativeInfinite(UFirst) &&
      Precision::IsPositiveInfinite(ULast)) {
    UFirst = -100.;
    ULast  = 100.;
  }
  else if (Precision::IsNegativeInfinite(UFirst)) {
    UFirst = ULast - 200.;
  }
  else if (Precision::IsPositiveInfinite(ULast)) {
    ULast = UFirst + 200.;
  }
  Standard_Real deltau = (ULast-UFirst)/20.;
  for (Standard_Integer i =1; i<=21; i++) {
    Standard_Real prm = UFirst + (i-1)*deltau;
    myBasisCurve->D1(prm,P,D1u);
    newZ = D1u.Normalized().Crossed(myDirection);
    if (newZ.Magnitude() > 1.e-12) break;
  }
  gp_Ax3 Ax3(P,gp_Dir(newZ),gp_Dir(D1u));
  if (myDirection.Dot(Ax3.YDirection())<0.){
    Ax3.YReverse();
  }
  return gp_Pln(Ax3);
}


//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

gp_Cylinder Adaptor3d_SurfaceOfLinearExtrusion::Cylinder() const 
{
  Standard_NoSuchObject_Raise_if 
    (GetType() != GeomAbs_Cylinder,
     "Adaptor3d_SurfaceOfLinearExtrusion::Cylinder");

  gp_Circ C =  myBasisCurve->Circle() ;
  gp_Ax3 Ax3(C.Position());
  if(myDirection.Dot((C.Axis()).Direction())<0.){
    Ax3.ZReverse();
  }
  return gp_Cylinder(Ax3,C.Radius());
}

//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

gp_Cone Adaptor3d_SurfaceOfLinearExtrusion::Cone() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::Cone");
  return gp_Cone();
}

//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

gp_Sphere Adaptor3d_SurfaceOfLinearExtrusion::Sphere() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::Sphere");
  return gp_Sphere();
}

//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus Adaptor3d_SurfaceOfLinearExtrusion::Torus() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::Torus");
  return gp_Torus();
}


//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

gp_Ax1 Adaptor3d_SurfaceOfLinearExtrusion::AxeOfRevolution() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::Axes");
  return gp_Ax1();
}

//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfLinearExtrusion::UDegree() const 
{
  return myBasisCurve -> Degree();
}
//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfLinearExtrusion::NbUPoles() const 
{
  return myBasisCurve->NbPoles();
}
//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfLinearExtrusion::VDegree() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::VDegree");
  return 0;
}

//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfLinearExtrusion::NbVPoles() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::NbVPoles");
  return 0;
}

//=======================================================================
//function : NbUKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfLinearExtrusion::NbUKnots() const 
{
  Standard_NoSuchObject::Raise
    ("Adaptor3d_SurfaceOfLinearExtrusion::NbUKnots");
  return 0;
}
//=======================================================================
//function : NbVKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_SurfaceOfLinearExtrusion::NbVKnots() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::NbVKnots");
  return 0;
}
//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfLinearExtrusion::IsURational() const 
{
  Standard_NoSuchObject::Raise
    ("Adaptor3d_SurfaceOfLinearExtrusion::IsURational");
  return Standard_False;
}
//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_SurfaceOfLinearExtrusion::IsVRational() const 
{
  Standard_NoSuchObject::Raise
    ("Adaptor3d_SurfaceOfLinearExtrusion::IsVRational");
  return Standard_False;
}
//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================


Handle(Geom_BezierSurface)  Adaptor3d_SurfaceOfLinearExtrusion::Bezier() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::Axes");
  return Handle(Geom_BezierSurface)() ;
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface)  Adaptor3d_SurfaceOfLinearExtrusion::BSpline() const 
{
  Standard_NoSuchObject::Raise("Adaptor3d_SurfaceOfLinearExtrusion::Axes");
  return Handle(Geom_BSplineSurface)() ;
}

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir Adaptor3d_SurfaceOfLinearExtrusion::Direction() const
{
  return myDirection;
}

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) Adaptor3d_SurfaceOfLinearExtrusion::BasisCurve() const 
{
  return myBasisCurve;
}
