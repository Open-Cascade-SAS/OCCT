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

#include <GeomAdaptor_SurfaceOfRevolution.hxx>

#include <Adaptor3d_Curve.hxx>
#include <ElCLib.hxx>
#include <Standard_NoSuchObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_SurfaceOfRevolution, GeomAdaptor_Surface)

//=================================================================================================

GeomAdaptor_SurfaceOfRevolution::GeomAdaptor_SurfaceOfRevolution()
    : myHaveAxis(false)
{
}

//=================================================================================================

GeomAdaptor_SurfaceOfRevolution::GeomAdaptor_SurfaceOfRevolution(
  const occ::handle<Adaptor3d_Curve>& C)
    : myHaveAxis(false)
{
  Load(C);
}

//=================================================================================================

GeomAdaptor_SurfaceOfRevolution::GeomAdaptor_SurfaceOfRevolution(
  const occ::handle<Adaptor3d_Curve>& C,
  const gp_Ax1&                       V)
    : myHaveAxis(false)
{
  Load(C);
  Load(V);
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_SurfaceOfRevolution::ShallowCopy() const
{
  occ::handle<GeomAdaptor_SurfaceOfRevolution> aCopy = new GeomAdaptor_SurfaceOfRevolution();

  if (!myBasisCurve.IsNull())
  {
    aCopy->myBasisCurve = myBasisCurve->ShallowCopy();
  }
  aCopy->myAxis     = myAxis;
  aCopy->myHaveAxis = myHaveAxis;
  aCopy->myAxeRev   = myAxeRev;

  aCopy->mySurface     = mySurface;
  aCopy->myUFirst      = myUFirst;
  aCopy->myULast       = myULast;
  aCopy->myVFirst      = myVFirst;
  aCopy->myVLast       = myVLast;
  aCopy->myTolU        = myTolU;
  aCopy->myTolV        = myTolV;
  aCopy->mySurfaceType = mySurfaceType;

  // Copy surface data variant
  if (auto* aRevData = std::get_if<GeomAdaptor_Surface::RevolutionData>(&mySurfaceData))
  {
    GeomAdaptor_Surface::RevolutionData aNewData;
    aNewData.BasisCurve  = aRevData->BasisCurve->ShallowCopy();
    aNewData.Axis        = aRevData->Axis;
    aCopy->mySurfaceData = aNewData;
  }

  return aCopy;
}

//=================================================================================================

void GeomAdaptor_SurfaceOfRevolution::Load(const occ::handle<Adaptor3d_Curve>& C)
{
  myBasisCurve = C;
  if (myHaveAxis)
    Load(myAxis); // to evaluate the new myAxeRev.
}

//=================================================================================================

void GeomAdaptor_SurfaceOfRevolution::Load(const gp_Ax1& V)
{
  myHaveAxis = true;
  myAxis     = V;

  mySurfaceType = GeomAbs_SurfaceOfRevolution;

  // Populate revolution surface data for fast evaluation
  GeomAdaptor_Surface::RevolutionData aRevData;
  aRevData.BasisCurve = myBasisCurve;
  aRevData.Axis       = myAxis;
  mySurfaceData       = aRevData;

  // Eval myAxeRev : axe of revolution ( Determination de Ox).
  gp_Pnt P, Q;
  gp_Pnt O = myAxis.Location();
  gp_Dir Ox;
  gp_Dir Oz   = myAxis.Direction();
  bool   yrev = false;
  if (myBasisCurve->GetType() == GeomAbs_Line)
  {
    if ((myBasisCurve->Line().Direction()).Dot(Oz) < 0.)
    {
      yrev = true;
      Oz.Reverse();
    }
  }

  if (myBasisCurve->GetType() == GeomAbs_Circle)
  {
    Q = P = (myBasisCurve->Circle()).Location();
  }
  else
  {
    double First = myBasisCurve->FirstParameter();
    P            = Value(0., 0.); // ce qui ne veut pas dire grand chose
    if (GetType() == GeomAbs_Cone)
    {
      if (gp_Lin(myAxis).Distance(P) <= Precision::Confusion())
        Q = ElCLib::Value(1., myBasisCurve->Line());
      else
        Q = P;
    }
    else if (Precision::IsInfinite(First))
      Q = P;
    else
      Q = Value(0., First);
  }

  gp_Dir DZ = myAxis.Direction();
  O.SetXYZ(O.XYZ() + (gp_Vec(O, P) * DZ) * DZ.XYZ());
  if (gp_Lin(myAxis).Distance(Q) > Precision::Confusion())
  {
    Ox = gp_Dir(Q.XYZ() - O.XYZ());
  }
  else
  {
    double First = myBasisCurve->FirstParameter();
    double Last  = myBasisCurve->LastParameter();
    int    Ratio = 1;
    double Dist;
    gp_Pnt PP;
    do
    {
      PP   = myBasisCurve->Value(First + (Last - First) / Ratio);
      Dist = gp_Lin(myAxis).Distance(PP);
      Ratio++;
    } while (Dist < Precision::Confusion() && Ratio < 100);

    if (Ratio >= 100)
    {
      throw Standard_ConstructionError(
        "Adaptor3d_SurfaceOfRevolution : Axe and meridian are confused");
    }
    Ox = ((Oz ^ gp_Vec(PP.XYZ() - O.XYZ())) ^ Oz);
  }

  myAxeRev = gp_Ax3(O, Oz, Ox);

  if (yrev)
  {
    myAxeRev.YReverse();
  }
  else if (myBasisCurve->GetType() == GeomAbs_Circle)
  {
    gp_Dir DC = (myBasisCurve->Circle()).Axis().Direction();
    if ((Ox.Crossed(Oz)).Dot(DC) < 0.)
      myAxeRev.ZReverse();
  }
}

//=================================================================================================

gp_Ax1 GeomAdaptor_SurfaceOfRevolution::AxeOfRevolution() const
{
  return myAxis;
}

//=================================================================================================

double GeomAdaptor_SurfaceOfRevolution::FirstUParameter() const
{
  return 0.;
}

//=================================================================================================

double GeomAdaptor_SurfaceOfRevolution::LastUParameter() const
{
  return 2 * M_PI;
}

//=================================================================================================

double GeomAdaptor_SurfaceOfRevolution::FirstVParameter() const
{
  return myBasisCurve->FirstParameter();
}

//=================================================================================================

double GeomAdaptor_SurfaceOfRevolution::LastVParameter() const
{
  return myBasisCurve->LastParameter();
}

//=================================================================================================

GeomAbs_Shape GeomAdaptor_SurfaceOfRevolution::UContinuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

GeomAbs_Shape GeomAdaptor_SurfaceOfRevolution::VContinuity() const
{
  return myBasisCurve->Continuity();
}

//=================================================================================================

int GeomAdaptor_SurfaceOfRevolution::NbUIntervals(const GeomAbs_Shape) const
{
  return 1;
}

//=================================================================================================

int GeomAdaptor_SurfaceOfRevolution::NbVIntervals(const GeomAbs_Shape S) const
{
  return myBasisCurve->NbIntervals(S);
}

//=================================================================================================

void GeomAdaptor_SurfaceOfRevolution::UIntervals(NCollection_Array1<double>& T,
                                                 const GeomAbs_Shape) const
{
  T(T.Lower())     = 0.;
  T(T.Lower() + 1) = 2 * M_PI;
}

//=================================================================================================

void GeomAdaptor_SurfaceOfRevolution::VIntervals(NCollection_Array1<double>& T,
                                                 const GeomAbs_Shape         S) const
{
  myBasisCurve->Intervals(T, S);
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_SurfaceOfRevolution::UTrim(const double First,
                                                                      const double Last,
                                                                      const double Tol) const
{
  constexpr double Eps = Precision::PConfusion();
  (void)Eps;
  (void)First;
  (void)Last;
  (void)Tol;
  Standard_OutOfRange_Raise_if(std::abs(First) > Eps || std::abs(Last - 2. * M_PI) > Eps,
                               "GeomAdaptor_SurfaceOfRevolution : UTrim : Parameters out of range");

  occ::handle<GeomAdaptor_SurfaceOfRevolution> HR =
    new GeomAdaptor_SurfaceOfRevolution(GeomAdaptor_SurfaceOfRevolution(myBasisCurve, myAxis));
  return HR;
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_SurfaceOfRevolution::VTrim(const double First,
                                                                      const double Last,
                                                                      const double Tol) const
{
  occ::handle<Adaptor3d_Curve>                 HC = BasisCurve()->Trim(First, Last, Tol);
  occ::handle<GeomAdaptor_SurfaceOfRevolution> HR =
    new GeomAdaptor_SurfaceOfRevolution(GeomAdaptor_SurfaceOfRevolution(HC, myAxis));
  return HR;
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfRevolution::IsUClosed() const
{
  return true;
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfRevolution::IsVClosed() const
{
  return myBasisCurve->IsClosed();
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfRevolution::IsUPeriodic() const
{
  return true;
}

//=================================================================================================

double GeomAdaptor_SurfaceOfRevolution::UPeriod() const
{
  return 2 * M_PI;
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfRevolution::IsVPeriodic() const
{
  return myBasisCurve->IsPeriodic();
}

//=================================================================================================

double GeomAdaptor_SurfaceOfRevolution::VPeriod() const
{
  return myBasisCurve->Period();
}

//=================================================================================================

double GeomAdaptor_SurfaceOfRevolution::UResolution(const double R3d) const
{
  return Precision::Parametric(R3d);
}

//=================================================================================================

double GeomAdaptor_SurfaceOfRevolution::VResolution(const double R3d) const
{
  return myBasisCurve->Resolution(R3d);
}

//=================================================================================================

GeomAbs_SurfaceType GeomAdaptor_SurfaceOfRevolution::GetType() const
{
  constexpr double TolConf        = Precision::Confusion();
  constexpr double TolAng         = Precision::Angular();
  constexpr double TolConeSemiAng = Precision::Confusion();

  switch (myBasisCurve->GetType())
  {
    case GeomAbs_Line: {
      gp_Ax1 Axe = myBasisCurve->Line().Position();

      if (myAxis.IsParallel(Axe, TolAng))
      {
        gp_Pnt P = Value(0., 0.);
        double R = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
        if (R > TolConf)
        {
          return GeomAbs_Cylinder;
        }
      }
      else if (myAxis.IsNormal(Axe, TolAng))
        return GeomAbs_Plane;
      else
      {
        double uf     = myBasisCurve->FirstParameter();
        double ul     = myBasisCurve->LastParameter();
        bool   istrim = (!Precision::IsInfinite(uf) && !Precision::IsInfinite(ul));
        if (istrim)
        {
          gp_Pnt pf  = myBasisCurve->Value(uf);
          gp_Pnt pl  = myBasisCurve->Value(ul);
          double len = pf.Distance(pl);
          // on calcule la distance projetee sur l axe.
          gp_Vec vlin(pf, pl);
          gp_Vec vaxe(myAxis.Direction());
          double projlen = std::abs(vaxe.Dot(vlin));
          if ((len - projlen) <= TolConf)
          {
            gp_Pnt P = Value(0., 0.);
            double R = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
            if (R > TolConf)
            {
              return GeomAbs_Cylinder;
            }
          }
          else if (projlen <= TolConf)
            return GeomAbs_Plane;
        }
        gp_Vec V(myAxis.Location(), myBasisCurve->Line().Location());
        gp_Vec W(Axe.Direction());
        gp_Vec AxisDir(myAxis.Direction());
        double proj = std::abs(W.Dot(AxisDir));
        if (std::abs(V.DotCross(AxisDir, W)) <= TolConf
            && (proj >= TolConeSemiAng && proj <= 1. - TolConeSemiAng))
        {
          return GeomAbs_Cone;
        }
      }
      break;
    } // case GeomAbs_Line:
    //
    case GeomAbs_Circle: {
      double MajorRadius, aR;
      gp_Lin aLin(myAxis);
      //
      gp_Circ       C   = myBasisCurve->Circle();
      const gp_Pnt& aLC = C.Location();
      aR                = C.Radius();
      //

      if (!C.Position().IsCoplanar(myAxis, TolConf, TolAng))
        return GeomAbs_SurfaceOfRevolution;
      else if (aLin.Distance(aLC) <= TolConf)
        return GeomAbs_Sphere;
      else
      {
        MajorRadius = aLin.Distance(aLC);
        if (MajorRadius > aR)
        {
          return GeomAbs_Torus;
        }
      }
      break;
    }
    //
    default:
      break;
  }

  return GeomAbs_SurfaceOfRevolution;
}

//=================================================================================================

gp_Pln GeomAdaptor_SurfaceOfRevolution::Plane() const
{
  Standard_NoSuchObject_Raise_if(GetType() != GeomAbs_Plane,
                                 "GeomAdaptor_SurfaceOfRevolution:Plane");

  gp_Ax3 Axe       = myAxeRev;
  gp_Pnt aPonCurve = Value(0., 0.);
  double aDot      = (aPonCurve.XYZ() - myAxis.Location().XYZ()).Dot(myAxis.Direction().XYZ());

  gp_Pnt P(myAxis.Location().XYZ() + aDot * myAxis.Direction().XYZ());
  Axe.SetLocation(P);
  if (Axe.XDirection().Dot(myBasisCurve->Line().Direction()) >= -Precision::Confusion())
    Axe.XReverse();

  return gp_Pln(Axe);
}

//=================================================================================================

gp_Cylinder GeomAdaptor_SurfaceOfRevolution::Cylinder() const
{
  Standard_NoSuchObject_Raise_if(GetType() != GeomAbs_Cylinder,
                                 "GeomAdaptor_SurfaceOfRevolution::Cylinder");

  gp_Pnt P = Value(0., 0.);
  double R = gp_Vec(myAxeRev.Location(), P) * myAxeRev.XDirection();
  return gp_Cylinder(myAxeRev, R);
}

//=================================================================================================

gp_Cone GeomAdaptor_SurfaceOfRevolution::Cone() const
{
  Standard_NoSuchObject_Raise_if(GetType() != GeomAbs_Cone, "GeomAdaptor_SurfaceOfRevolution:Cone");

  gp_Ax3 Axe   = myAxeRev;
  gp_Dir ldir  = (myBasisCurve->Line()).Direction();
  double Angle = (Axe.Direction()).Angle(ldir);
  gp_Pnt P0    = Value(0., 0.);
  double R     = (Axe.Location()).Distance(P0);
  if (R >= Precision::Confusion())
  {
    gp_Pnt O = Axe.Location();
    gp_Vec OP0(O, P0);
    double t = OP0.Dot(Axe.XDirection());
    t /= ldir.Dot(Axe.XDirection());
    OP0.Add(-t * gp_Vec(ldir));
    if (OP0.Dot(Axe.Direction()) > 0.)
      Angle = -Angle;
  }
  return gp_Cone(Axe, Angle, R);
}

//=================================================================================================

gp_Sphere GeomAdaptor_SurfaceOfRevolution::Sphere() const
{
  Standard_NoSuchObject_Raise_if(GetType() != GeomAbs_Sphere,
                                 "GeomAdaptor_SurfaceOfRevolution:Sphere");

  gp_Circ C   = myBasisCurve->Circle();
  gp_Ax3  Axe = myAxeRev;
  Axe.SetLocation(C.Location());
  return gp_Sphere(Axe, C.Radius());
}

//=================================================================================================

gp_Torus GeomAdaptor_SurfaceOfRevolution::Torus() const
{
  Standard_NoSuchObject_Raise_if(GetType() != GeomAbs_Torus,
                                 "GeomAdaptor_SurfaceOfRevolution:Torus");

  gp_Circ C           = myBasisCurve->Circle();
  double  MajorRadius = gp_Lin(myAxis).Distance(C.Location());
  return gp_Torus(myAxeRev, MajorRadius, C.Radius());
}

//=================================================================================================

int GeomAdaptor_SurfaceOfRevolution::VDegree() const
{
  return myBasisCurve->Degree();
}

//=================================================================================================

int GeomAdaptor_SurfaceOfRevolution::NbVPoles() const
{
  return myBasisCurve->NbPoles();
}

//=================================================================================================

int GeomAdaptor_SurfaceOfRevolution::NbVKnots() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::NbVKnots");
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfRevolution::IsURational() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::IsURational");
}

//=================================================================================================

bool GeomAdaptor_SurfaceOfRevolution::IsVRational() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::IsVRational");
}

//=================================================================================================

occ::handle<Geom_BezierSurface> GeomAdaptor_SurfaceOfRevolution::Bezier() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::Bezier");
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomAdaptor_SurfaceOfRevolution::BSpline() const
{
  throw Standard_NoSuchObject("GeomAdaptor_SurfaceOfRevolution::BSpline");
}

//=================================================================================================

const gp_Ax3& GeomAdaptor_SurfaceOfRevolution::Axis() const
{
  return myAxeRev;
}

//=================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_SurfaceOfRevolution::BasisCurve() const
{
  return myBasisCurve;
}
