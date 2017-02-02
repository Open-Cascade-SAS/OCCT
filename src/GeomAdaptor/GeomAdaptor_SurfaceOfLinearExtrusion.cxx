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

#include <GeomAdaptor_SurfaceOfLinearExtrusion.hxx>

#include <Adaptor3d_HCurve.hxx>
#include <GeomAdaptor_HSurfaceOfLinearExtrusion.hxx>
#include <GeomEvaluator_SurfaceOfExtrusion.hxx>
#include <Standard_NoSuchObject.hxx>

//=======================================================================
//function : GeomAdaptor_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================
GeomAdaptor_SurfaceOfLinearExtrusion::GeomAdaptor_SurfaceOfLinearExtrusion()
  : myHaveDir(Standard_False)
{}

//=======================================================================
//function : GeomAdaptor_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================

GeomAdaptor_SurfaceOfLinearExtrusion::GeomAdaptor_SurfaceOfLinearExtrusion
(const Handle(Adaptor3d_HCurve)& C)
  : myHaveDir(Standard_False)
{
  Load(C);
}

//=======================================================================
//function : GeomAdaptor_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================

GeomAdaptor_SurfaceOfLinearExtrusion::GeomAdaptor_SurfaceOfLinearExtrusion
(const Handle(Adaptor3d_HCurve)& C,
 const gp_Dir&        V)
  : myHaveDir(Standard_False)
{
  Load(C);
  Load(V);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void GeomAdaptor_SurfaceOfLinearExtrusion::Load(const Handle(Adaptor3d_HCurve)& C)
{
  myBasisCurve = C;
  if (myHaveDir)
    Load(myDirection);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void GeomAdaptor_SurfaceOfLinearExtrusion::Load(const gp_Dir& V)
{
  myHaveDir = Standard_True;
  myDirection = V;

  mySurfaceType = GeomAbs_SurfaceOfExtrusion;
  myNestedEvaluator = new GeomEvaluator_SurfaceOfExtrusion(myBasisCurve, myDirection);
}

//=======================================================================
//function : FirstUParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfLinearExtrusion::FirstUParameter() const 
{
  return myBasisCurve->FirstParameter();
}

//=======================================================================
//function : LastUParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfLinearExtrusion::LastUParameter() const 
{
  return myBasisCurve->LastParameter();
}

//=======================================================================
//function : FirstVParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfLinearExtrusion::FirstVParameter() const 
{
  return RealFirst();
}

//=======================================================================
//function : LastVParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfLinearExtrusion::LastVParameter() const 
{
  return RealLast();
}

//=======================================================================
//function : UContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomAdaptor_SurfaceOfLinearExtrusion::UContinuity() const 
{
  return myBasisCurve->Continuity();
}

//=======================================================================
//function : VContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomAdaptor_SurfaceOfLinearExtrusion::VContinuity() const 
{
  return GeomAbs_CN;
}

//=======================================================================
//function : NbUIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfLinearExtrusion::NbUIntervals
(const GeomAbs_Shape S)  const 
{
  return   myBasisCurve->NbIntervals(S);
}

//=======================================================================
//function : NbVIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfLinearExtrusion::NbVIntervals
(const GeomAbs_Shape ) const 
{
  return 1;
}

//=======================================================================
//function : UIntervals
//purpose  : 
//=======================================================================

void GeomAdaptor_SurfaceOfLinearExtrusion::UIntervals
(TColStd_Array1OfReal&  T, const GeomAbs_Shape S) const 
{
  myBasisCurve->Intervals(T,S);
}

//=======================================================================
//function : VIntervals
//purpose  : 
//=======================================================================

void GeomAdaptor_SurfaceOfLinearExtrusion::VIntervals
(TColStd_Array1OfReal&  T, const GeomAbs_Shape ) const 
{
 T(T.Lower()) = FirstVParameter() ;
 T(T.Lower() + 1) = LastVParameter() ;
}

//=======================================================================
//function : VTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface)  GeomAdaptor_SurfaceOfLinearExtrusion::VTrim
(const Standard_Real First,
 const Standard_Real Last,
 const Standard_Real Tol) const 
{
  Handle(Adaptor3d_HCurve) HC = BasisCurve()->Trim(First,Last,Tol);
  Handle(GeomAdaptor_HSurfaceOfLinearExtrusion) HR = new GeomAdaptor_HSurfaceOfLinearExtrusion(
      GeomAdaptor_SurfaceOfLinearExtrusion(HC, myDirection));
  return HR;
}

//=======================================================================
//function : UTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface)  GeomAdaptor_SurfaceOfLinearExtrusion::UTrim
(const Standard_Real ,
 const Standard_Real ,
 const Standard_Real ) const 
{
  Handle(GeomAdaptor_HSurfaceOfLinearExtrusion) HR = new GeomAdaptor_HSurfaceOfLinearExtrusion(
      GeomAdaptor_SurfaceOfLinearExtrusion(myBasisCurve, myDirection));
  return HR;
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfLinearExtrusion::IsUClosed() const 
{
  return myBasisCurve->IsClosed();
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfLinearExtrusion::IsVClosed() const 
{
  return Standard_False;
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfLinearExtrusion::IsUPeriodic() const 
{
  return myBasisCurve->IsPeriodic();
}

//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfLinearExtrusion::UPeriod() const 
{
  return myBasisCurve->Period() ;
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfLinearExtrusion::IsVPeriodic() const 
{
  return Standard_False;
}

//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfLinearExtrusion::VPeriod() const 
{
  throw Standard_DomainError("GeomAdaptor_SurfaceOfLinearExtrusion::VPeriod");
}

//=======================================================================
//function : UResolution
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfLinearExtrusion::UResolution
(const Standard_Real R3d) const 
{
  return myBasisCurve->Resolution(R3d);
}

//=======================================================================
//function : VResolution
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_SurfaceOfLinearExtrusion::VResolution
(const Standard_Real R3d) const 
{
  return R3d;
}

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_SurfaceType GeomAdaptor_SurfaceOfLinearExtrusion::GetType() const 
{
  switch ( myBasisCurve->GetType()) {
    
  case GeomAbs_Line:
    {
      gp_Dir D = myBasisCurve->Line().Direction();
      if (!myDirection.IsParallel( D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }
    
  case GeomAbs_Circle:
    {
      gp_Dir D = (myBasisCurve->Circle()).Axis().Direction();
      if ( myDirection.IsParallel( D, Precision::Angular()))
        return GeomAbs_Cylinder;
      else if (myDirection.IsNormal(D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }
    
  case GeomAbs_Ellipse:
    {
      gp_Dir D = (myBasisCurve->Ellipse()).Axis().Direction();
      if (myDirection.IsNormal(D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }
    
  case GeomAbs_Parabola:
    {
      gp_Dir D = (myBasisCurve->Parabola()).Axis().Direction();
      if (myDirection.IsNormal(D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }
    
  case GeomAbs_Hyperbola:
    {
      gp_Dir D = (myBasisCurve->Hyperbola()).Axis().Direction();
      if (myDirection.IsNormal(D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }

  default:
    break;
  }

  return GeomAbs_SurfaceOfExtrusion;
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln GeomAdaptor_SurfaceOfLinearExtrusion::Plane() const 
{ 
  Standard_NoSuchObject_Raise_if (GetType() != GeomAbs_Plane,
                  "GeomAdaptor_SurfaceOfLinearExtrusion::Plane");

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

gp_Cylinder GeomAdaptor_SurfaceOfLinearExtrusion::Cylinder() const 
{
  Standard_NoSuchObject_Raise_if 
    (GetType() != GeomAbs_Cylinder,
     "GeomAdaptor_SurfaceOfLinearExtrusion::Cylinder");

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

gp_Cone GeomAdaptor_SurfaceOfLinearExtrusion::Cone() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Cone");
}

//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

gp_Sphere GeomAdaptor_SurfaceOfLinearExtrusion::Sphere() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Sphere");
}

//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus GeomAdaptor_SurfaceOfLinearExtrusion::Torus() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Torus");
}


//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

gp_Ax1 GeomAdaptor_SurfaceOfLinearExtrusion::AxeOfRevolution() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Axes");
}

//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfLinearExtrusion::UDegree() const 
{
  return myBasisCurve->Degree();
}
//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_SurfaceOfLinearExtrusion::NbUPoles() const 
{
  return myBasisCurve->NbPoles();
}

//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfLinearExtrusion::IsURational() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::IsURational");
}
//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_SurfaceOfLinearExtrusion::IsVRational() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::IsVRational");
}
//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================


Handle(Geom_BezierSurface)  GeomAdaptor_SurfaceOfLinearExtrusion::Bezier() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Bezier");
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface)  GeomAdaptor_SurfaceOfLinearExtrusion::BSpline() const 
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::BSpline");
}

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir GeomAdaptor_SurfaceOfLinearExtrusion::Direction() const
{
  return myDirection;
}

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) GeomAdaptor_SurfaceOfLinearExtrusion::BasisCurve() const 
{
  return myBasisCurve;
}
