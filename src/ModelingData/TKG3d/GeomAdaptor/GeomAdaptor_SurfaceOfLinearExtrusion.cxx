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

#include <Adaptor3d_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax3.hxx>
#include <Standard_NoSuchObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_SurfaceOfLinearExtrusion, GeomAdaptor_Surface)

//=================================================================================================

GeomAdaptor_SurfaceOfLinearExtrusion::GeomAdaptor_SurfaceOfLinearExtrusion()
    : myHaveDir(false)
{
}

//=================================================================================================

GeomAdaptor_SurfaceOfLinearExtrusion::GeomAdaptor_SurfaceOfLinearExtrusion(
  const occ::handle<Adaptor3d_Curve>& C)
    : myHaveDir(false)
{
  Load(C);
}

//=================================================================================================

GeomAdaptor_SurfaceOfLinearExtrusion::GeomAdaptor_SurfaceOfLinearExtrusion(
  const occ::handle<Adaptor3d_Curve>& C,
  const gp_Dir&                       V)
    : myHaveDir(false)
{
  Load(C);
  Load(V);
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_SurfaceOfLinearExtrusion::ShallowCopy() const
{
  occ::handle<GeomAdaptor_SurfaceOfLinearExtrusion> aCopy =
    new GeomAdaptor_SurfaceOfLinearExtrusion();

  if (!myBasisCurve.IsNull())
  {
    aCopy->myBasisCurve = myBasisCurve->ShallowCopy();
  }
  aCopy->myDirection = myDirection;
  aCopy->myHaveDir   = myHaveDir;

  aCopy->mySurface     = mySurface;
  aCopy->myUFirst      = myUFirst;
  aCopy->myULast       = myULast;
  aCopy->myVFirst      = myVFirst;
  aCopy->myVLast       = myVLast;
  aCopy->myTolU        = myTolU;
  aCopy->myTolV        = myTolV;
  aCopy->mySurfaceType = mySurfaceType;

  // Copy surface data variant
  if (auto* anExtData = std::get_if<GeomAdaptor_Surface::ExtrusionData>(&mySurfaceData))
  {
    GeomAdaptor_Surface::ExtrusionData aNewData;
    aNewData.BasisCurve  = anExtData->BasisCurve->ShallowCopy();
    aNewData.Direction   = anExtData->Direction;
    aCopy->mySurfaceData = aNewData;
  }

  return aCopy;
}

//=================================================================================================

void GeomAdaptor_SurfaceOfLinearExtrusion::Load(const occ::handle<Adaptor3d_Curve>& C)
{
  myBasisCurve = C;
  if (myHaveDir)
    Load(myDirection);
}

//=================================================================================================

void GeomAdaptor_SurfaceOfLinearExtrusion::Load(const gp_Dir& V)
{
  myHaveDir   = true;
  myDirection = V;

  mySurfaceType = GeomAbs_SurfaceOfExtrusion;

  // Populate extrusion surface data for fast evaluation
  GeomAdaptor_Surface::ExtrusionData anExtData;
  anExtData.BasisCurve = myBasisCurve;
  anExtData.Direction  = myDirection.XYZ();
  mySurfaceData        = anExtData;
}

//=================================================================================================

double GeomAdaptor_SurfaceOfLinearExtrusion::FirstUParameter() const
{
  return myBasisCurve->FirstParameter();
}

//=================================================================================================

double GeomAdaptor_SurfaceOfLinearExtrusion::LastUParameter() const
{
  return myBasisCurve->LastParameter();
}

//=================================================================================================

double GeomAdaptor_SurfaceOfLinearExtrusion::FirstVParameter() const
{
  return RealFirst();
}

//=================================================================================================

double GeomAdaptor_SurfaceOfLinearExtrusion::LastVParameter() const
{
  return RealLast();
}

//=================================================================================================

GeomAbs_Shape GeomAdaptor_SurfaceOfLinearExtrusion::UContinuity() const
{
  return myBasisCurve->Continuity();
}

//=================================================================================================

GeomAbs_Shape GeomAdaptor_SurfaceOfLinearExtrusion::VContinuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

int GeomAdaptor_SurfaceOfLinearExtrusion::NbUIntervals(const GeomAbs_Shape S) const
{
  return myBasisCurve->NbIntervals(S);
}

//=================================================================================================

int GeomAdaptor_SurfaceOfLinearExtrusion::NbVIntervals(const GeomAbs_Shape) const
{
  return 1;
}

//=================================================================================================

void GeomAdaptor_SurfaceOfLinearExtrusion::UIntervals(NCollection_Array1<double>& T,
                                                      const GeomAbs_Shape         S) const
{
  myBasisCurve->Intervals(T, S);
}

//=================================================================================================

void GeomAdaptor_SurfaceOfLinearExtrusion::VIntervals(NCollection_Array1<double>& T,
                                                      const GeomAbs_Shape) const
{
  T(T.Lower())     = FirstVParameter();
  T(T.Lower() + 1) = LastVParameter();
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_SurfaceOfLinearExtrusion::VTrim(const double First,
                                                                           const double Last,
                                                                           const double Tol) const
{
  occ::handle<Adaptor3d_Curve>                      HC = BasisCurve()->Trim(First, Last, Tol);
  occ::handle<GeomAdaptor_SurfaceOfLinearExtrusion> HR =
    new GeomAdaptor_SurfaceOfLinearExtrusion(GeomAdaptor_SurfaceOfLinearExtrusion(HC, myDirection));
  return HR;
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_SurfaceOfLinearExtrusion::UTrim(const double,
                                                                           const double,
                                                                           const double) const
{
  occ::handle<GeomAdaptor_SurfaceOfLinearExtrusion> HR = new GeomAdaptor_SurfaceOfLinearExtrusion(
    GeomAdaptor_SurfaceOfLinearExtrusion(myBasisCurve, myDirection));
  return HR;
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfLinearExtrusion::IsUClosed() const
{
  return myBasisCurve->IsClosed();
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfLinearExtrusion::IsVClosed() const
{
  return false;
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfLinearExtrusion::IsUPeriodic() const
{
  return myBasisCurve->IsPeriodic();
}

//=================================================================================================

double GeomAdaptor_SurfaceOfLinearExtrusion::UPeriod() const
{
  return myBasisCurve->Period();
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfLinearExtrusion::IsVPeriodic() const
{
  return false;
}

//=================================================================================================

double GeomAdaptor_SurfaceOfLinearExtrusion::VPeriod() const
{
  throw Standard_DomainError("GeomAdaptor_SurfaceOfLinearExtrusion::VPeriod");
}

//=================================================================================================

double GeomAdaptor_SurfaceOfLinearExtrusion::UResolution(const double R3d) const
{
  return myBasisCurve->Resolution(R3d);
}

//=================================================================================================

double GeomAdaptor_SurfaceOfLinearExtrusion::VResolution(const double R3d) const
{
  return R3d;
}

//=================================================================================================

GeomAbs_SurfaceType GeomAdaptor_SurfaceOfLinearExtrusion::GetType() const
{
  switch (myBasisCurve->GetType())
  {

    case GeomAbs_Line: {
      gp_Dir D = myBasisCurve->Line().Direction();
      if (!myDirection.IsParallel(D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }

    case GeomAbs_Circle: {
      gp_Dir D = (myBasisCurve->Circle()).Axis().Direction();
      if (myDirection.IsParallel(D, Precision::Angular()))
        return GeomAbs_Cylinder;
      else if (myDirection.IsNormal(D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }

    case GeomAbs_Ellipse: {
      gp_Dir D = (myBasisCurve->Ellipse()).Axis().Direction();
      if (myDirection.IsNormal(D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }

    case GeomAbs_Parabola: {
      gp_Dir D = (myBasisCurve->Parabola()).Axis().Direction();
      if (myDirection.IsNormal(D, Precision::Angular()))
        return GeomAbs_Plane;
      break;
    }

    case GeomAbs_Hyperbola: {
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

//=================================================================================================

gp_Pln GeomAdaptor_SurfaceOfLinearExtrusion::Plane() const
{
  Standard_NoSuchObject_Raise_if(GetType() != GeomAbs_Plane,
                                 "GeomAdaptor_SurfaceOfLinearExtrusion::Plane");

  gp_Pnt P;
  gp_Vec D1u, newZ;
  double UFirst = myBasisCurve->FirstParameter();
  double ULast  = myBasisCurve->LastParameter();
  if (Precision::IsNegativeInfinite(UFirst) && Precision::IsPositiveInfinite(ULast))
  {
    UFirst = -100.;
    ULast  = 100.;
  }
  else if (Precision::IsNegativeInfinite(UFirst))
  {
    UFirst = ULast - 200.;
  }
  else if (Precision::IsPositiveInfinite(ULast))
  {
    ULast = UFirst + 200.;
  }
  double deltau = (ULast - UFirst) / 20.;
  for (int i = 1; i <= 21; i++)
  {
    double prm = UFirst + (i - 1) * deltau;
    myBasisCurve->D1(prm, P, D1u);
    newZ = D1u.Normalized().Crossed(myDirection);
    if (newZ.Magnitude() > 1.e-12)
      break;
  }
  gp_Ax3 Ax3(P, gp_Dir(newZ), gp_Dir(D1u));
  if (myDirection.Dot(Ax3.YDirection()) < 0.)
  {
    Ax3.YReverse();
  }
  return gp_Pln(Ax3);
}

//=================================================================================================

gp_Cylinder GeomAdaptor_SurfaceOfLinearExtrusion::Cylinder() const
{
  Standard_NoSuchObject_Raise_if(GetType() != GeomAbs_Cylinder,
                                 "GeomAdaptor_SurfaceOfLinearExtrusion::Cylinder");

  gp_Circ C = myBasisCurve->Circle();
  gp_Ax3  Ax3(C.Position());
  if (myDirection.Dot((C.Axis()).Direction()) < 0.)
  {
    Ax3.ZReverse();
  }
  return gp_Cylinder(Ax3, C.Radius());
}

//=================================================================================================

gp_Cone GeomAdaptor_SurfaceOfLinearExtrusion::Cone() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Cone");
}

//=================================================================================================

gp_Sphere GeomAdaptor_SurfaceOfLinearExtrusion::Sphere() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Sphere");
}

//=================================================================================================

gp_Torus GeomAdaptor_SurfaceOfLinearExtrusion::Torus() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Torus");
}

//=================================================================================================

gp_Ax1 GeomAdaptor_SurfaceOfLinearExtrusion::AxeOfRevolution() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Axes");
}

//=================================================================================================

int GeomAdaptor_SurfaceOfLinearExtrusion::UDegree() const
{
  return myBasisCurve->Degree();
}

//=================================================================================================

int GeomAdaptor_SurfaceOfLinearExtrusion::NbUPoles() const
{
  return myBasisCurve->NbPoles();
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfLinearExtrusion::IsURational() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::IsURational");
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfLinearExtrusion::IsVRational() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::IsVRational");
}

//=================================================================================================

occ::handle<Geom_BezierSurface> GeomAdaptor_SurfaceOfLinearExtrusion::Bezier() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::Bezier");
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomAdaptor_SurfaceOfLinearExtrusion::BSpline() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfLinearExtrusion::BSpline");
}

//=================================================================================================

gp_Dir GeomAdaptor_SurfaceOfLinearExtrusion::Direction() const
{
  return myDirection;
}

//=================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_SurfaceOfLinearExtrusion::BasisCurve() const
{
  return myBasisCurve;
}
