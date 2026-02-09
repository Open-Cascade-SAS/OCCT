// Created on: 1992-03-13
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <HLRAlgo.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_CLProps.hxx>
#include <HLRBRep_Curve.hxx>
#include <Precision.hxx>
#include <ProjLib.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_UndefinedDerivative.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Edge.hxx>

// OCC155 // jfa 05.03.2002 // bad vectors projection
//=================================================================================================

HLRBRep_Curve::HLRBRep_Curve() = default;

//=================================================================================================

void HLRBRep_Curve::Curve(const TopoDS_Edge& E)
{
  myCurve.Initialize(E);
}

//=================================================================================================

double HLRBRep_Curve::Parameter2d(const double P3d) const
{
  // Mathematical formula for lines

  //        myOF P3d (myOF myVX - myOZ myVX + myOX myVZ)
  // Res -> --------------------------------------------
  //        (-myOF + myOZ) (-myOF + myOZ + P3d myVZ)

  switch (myType)
  {
    case GeomAbs_Line:
      if (((HLRAlgo_Projector*)myProj)->Perspective())
      {
        const double FmOZ = myOF - myOZ;
        return myOF * P3d * (myVX * FmOZ + myOX * myVZ) / (FmOZ * (FmOZ - P3d * myVZ));
      }
      return P3d * myVX;

    case GeomAbs_Ellipse:
      return P3d + myOX;

    default: // implemented to avoid gcc compiler warnings
      break;
  }
  return P3d;
}

//=================================================================================================

double HLRBRep_Curve::Parameter3d(const double P2d) const
{
  // Mathematical formula for lines

  //                                 2
  //                   (-myOF + myOZ)  P2d
  // P3d -> -----------------------------------------------------
  //        (myOF - myOZ) (myOF myVX + P2d myVZ) + myOF myOX myVZ

  if (myType == GeomAbs_Line)
  {
    if (((HLRAlgo_Projector*)myProj)->Perspective())
    {
      const double FmOZ = myOF - myOZ;
      return P2d * FmOZ * FmOZ / (FmOZ * (myOF * myVX + P2d * myVZ) + myOF * myOX * myVZ);
    }
    return ((myVX <= gp::Resolution()) ? P2d : (P2d / myVX));
  }

  else if (myType == GeomAbs_Ellipse)
  {
    return P2d - myOX;
  }

  return P2d;
}

//=================================================================================================

double HLRBRep_Curve::Update(double TotMin[16], double TotMax[16])
{
  GeomAbs_CurveType typ = HLRBRep_BCurveTool::GetType(myCurve);
  myType                = GeomAbs_OtherCurve;

  switch (typ)
  {

    case GeomAbs_Line:
      myType = typ;
      break;

    case GeomAbs_Circle:
      if (!((HLRAlgo_Projector*)myProj)->Perspective())
      {
        gp_Dir D1 = HLRBRep_BCurveTool::Circle(myCurve).Axis().Direction();
        D1.Transform(((HLRAlgo_Projector*)myProj)->Transformation());
        if (D1.IsParallel(gp::DZ(), Precision::Angular()))
          myType = GeomAbs_Circle;
        else if (std::abs(D1.Dot(gp::DZ()))
                 < Precision::Angular()
                     * 10) //*10: The minor radius of ellipse should not be too small.
          myType = GeomAbs_OtherCurve;
        else
        {
          myType = GeomAbs_Ellipse;
          // compute the angle offset
          gp_Dir D3 = D1.Crossed(gp::DZ());
          gp_Dir D2 = HLRBRep_BCurveTool::Circle(myCurve).XAxis().Direction();
          D2.Transform(((HLRAlgo_Projector*)myProj)->Transformation());
          myOX = D3.AngleWithRef(D2, D1);
        }
      }
      break;

    case GeomAbs_Ellipse:
      if (!((HLRAlgo_Projector*)myProj)->Perspective())
      {
        gp_Dir D1 = HLRBRep_BCurveTool::Ellipse(myCurve).Axis().Direction();
        D1.Transform(((HLRAlgo_Projector*)myProj)->Transformation());
        if (D1.IsParallel(gp::DZ(), Precision::Angular()))
        {
          myOX   = 0.; // no offset on the angle
          myType = GeomAbs_Ellipse;
        }
      }
      break;

    case GeomAbs_BezierCurve:
      if (HLRBRep_BCurveTool::Degree(myCurve) == 1)
        myType = GeomAbs_Line;
      else if (!((HLRAlgo_Projector*)myProj)->Perspective())
        myType = typ;
      break;

    case GeomAbs_BSplineCurve:
      if (!((HLRAlgo_Projector*)myProj)->Perspective())
        myType = typ;
      break;

    default:
      break;
  }

  if (myType == GeomAbs_Line)
  {
    // compute the values for a line
    gp_Lin L;
    double l3d = 1.; // length of the 3d bezier curve
    if (HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_Line)
    {
      L = HLRBRep_BCurveTool::Line(myCurve);
    }
    else
    { // bezier degree 1
      gp_Pnt PL;
      gp_Vec VL;
      HLRBRep_BCurveTool::D1(myCurve, 0, PL, VL);
      L   = gp_Lin(PL, VL);
      l3d = PL.Distance(HLRBRep_BCurveTool::Value(myCurve, 1.));
    }
    gp_Pnt P = L.Location();
    gp_Vec V = L.Direction();
    P.Transform(((HLRAlgo_Projector*)myProj)->Transformation());
    V.Transform(((HLRAlgo_Projector*)myProj)->Transformation());
    if (((HLRAlgo_Projector*)myProj)->Perspective())
    {
      gp_Pnt2d F;
      gp_Vec2d VFX;
      D1(0., F, VFX);
      VFX.Normalize();
      myVX     = (VFX.X() * V.X() + VFX.Y() * V.Y()) * l3d;
      double l = -(VFX.X() * F.X() + VFX.Y() * F.Y());
      F.SetCoord(F.X() + VFX.X() * l, F.Y() + VFX.Y() * l);
      myOX = VFX.X() * (P.X() - F.X()) + VFX.Y() * (P.Y() - F.Y());
      gp_Vec VFZ(-F.X(), -F.Y(), ((HLRAlgo_Projector*)myProj)->Focus());
      myOF = VFZ.Magnitude();
      VFZ /= myOF;
      myVZ = VFZ * V;
      myVZ *= l3d;
      myOZ = VFZ * gp_Vec(P.X() - F.X(), P.Y() - F.Y(), P.Z());
    }
    else
      myVX = std::sqrt(V.X() * V.X() + V.Y() * V.Y()) * l3d;
  }
  return (UpdateMinMax(TotMin, TotMax));
}

//=================================================================================================

double HLRBRep_Curve::UpdateMinMax(double TotMin[16], double TotMax[16])
{
  double a = HLRBRep_BCurveTool::FirstParameter(myCurve);
  double b = HLRBRep_BCurveTool::LastParameter(myCurve);
  double x, y, z, tolMinMax = 0;
  ((HLRAlgo_Projector*)myProj)->Project(Value3D(a), x, y, z);
  HLRAlgo::UpdateMinMax(x, y, z, TotMin, TotMax);

  if (myType != GeomAbs_Line)
  {
    int    nbPnt = 30;
    int    i;
    double step = (b - a) / (nbPnt + 1);
    double xa, ya, za, xb = 0., yb = 0., zb = 0.;
    double dx1, dy1, dz1, dd1;
    double dx2, dy2, dz2, dd2;

    for (i = 1; i <= nbPnt; i++)
    {
      a += step;
      xa = xb;
      ya = yb;
      za = zb;
      xb = x;
      yb = y;
      zb = z;
      ((HLRAlgo_Projector*)myProj)->Project(Value3D(a), x, y, z);
      HLRAlgo::UpdateMinMax(x, y, z, TotMin, TotMax);
      if (i >= 2)
      {
        dx1 = x - xa;
        dy1 = y - ya;
        dz1 = z - za;
        dd1 = sqrt(dx1 * dx1 + dy1 * dy1 + dz1 * dz1);
        if (dd1 > 0)
        {
          dx2 = xb - xa;
          dy2 = yb - ya;
          dz2 = zb - za;
          dd2 = sqrt(dx2 * dx2 + dy2 * dy2 + dz2 * dz2);
          if (dd2 > 0)
          {
            double p = (dx1 * dx2 + dy1 * dy2 + dz1 * dz2) / (dd1 * dd2);
            dx1      = xa + p * dx1 - xb;
            dy1      = ya + p * dy1 - yb;
            dz1      = za + p * dz1 - zb;
            dd1      = sqrt(dx1 * dx1 + dy1 * dy1 + dz1 * dz1);
            if (dd1 > tolMinMax)
              tolMinMax = dd1;
          }
        }
      }
    }
  }
  ((HLRAlgo_Projector*)myProj)->Project(Value3D(b), x, y, z);
  HLRAlgo::UpdateMinMax(x, y, z, TotMin, TotMax);
  return tolMinMax;
}

//=================================================================================================

double HLRBRep_Curve::Z(const double U) const
{
  gp_Pnt P3d;
  HLRBRep_BCurveTool::D0(myCurve, U, P3d);
  P3d.Transform(((HLRAlgo_Projector*)myProj)->Transformation());
  return P3d.Z();
}

//=================================================================================================

void HLRBRep_Curve::Tangent(const bool AtStart, gp_Pnt2d& P, gp_Dir2d& D) const
{
  double U = AtStart ? HLRBRep_BCurveTool::FirstParameter(myCurve)
                     : HLRBRep_BCurveTool::LastParameter(myCurve);

  D0(U, P);
  HLRBRep_CLProps      CLP(2, Epsilon(1.));
  const HLRBRep_Curve* aCurve = this;
  CLP.SetCurve(aCurve);
  CLP.SetParameter(U);
  StdFail_UndefinedDerivative_Raise_if(!CLP.IsTangentDefined(), "HLRBRep_Curve::Tangent");
  CLP.Tangent(D);
}

//=================================================================================================

void HLRBRep_Curve::D0(const double U, gp_Pnt2d& P) const
{
  /* gp_Pnt P3d;
  HLRBRep_BCurveTool::D0(myCurve,U,P3d);
  P3d.Transform(((HLRAlgo_Projector*) myProj)->Transformation());
  if (((HLRAlgo_Projector*) myProj)->Perspective()) {
    double R = 1.-P3d.Z()/((HLRAlgo_Projector*) myProj)->Focus();
    P.SetCoord(P3d.X()/R,P3d.Y()/R);
  }
  else P.SetCoord(P3d.X(),P3d.Y()); */
  gp_Pnt P3d;
  HLRBRep_BCurveTool::D0(myCurve, U, P3d);
  ((HLRAlgo_Projector*)myProj)->Project(P3d, P);
}

//=================================================================================================

void HLRBRep_Curve::D1(const double U, gp_Pnt2d& P, gp_Vec2d& V) const
{
  // Mathematical formula for lines

  //        X'[t]      X[t] Z'[t]
  // D1 =  -------- + -------------
  //           Z[t]          Z[t] 2
  //       1 - ----   f (1 - ----)
  //            f             f

  gp_Pnt P3D;
  gp_Vec V13D;
  HLRBRep_BCurveTool::D1(myCurve, U, P3D, V13D);
  if (myProj->Perspective())
  {
    P3D.Transform(myProj->Transformation());
    V13D.Transform(myProj->Transformation());

    double f = myProj->Focus();
    double R = 1. - P3D.Z() / f;
    double e = V13D.Z() / (f * R * R);
    P.SetCoord(P3D.X() / R, P3D.Y() / R);
    V.SetCoord(V13D.X() / R + P3D.X() * e, V13D.Y() / R + P3D.Y() * e);
  }
  else
  {
    // OCC155
    myProj->Project(P3D, V13D, P, V);
  }
}

//=================================================================================================

void HLRBRep_Curve::D2(const double U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
{
  // Mathematical formula for lines

  //                                   2
  //       2 X'[t] Z'[t]   2 X[t] Z'[t]      X''[t]     X[t] Z''[t]
  // D2 =  ------------- + -------------- + -------- + -------------
  //              Z[t] 2    2      Z[t] 3       Z[t]          Z[t] 2
  //       f (1 - ----)    f  (1 - ----)    1 - ----   f (1 - ----)
  //               f                f            f             f

  gp_Pnt P3D;
  gp_Vec V13D, V23D;
  HLRBRep_BCurveTool::D2(myCurve, U, P3D, V13D, V23D);
  P3D.Transform(myProj->Transformation());
  V13D.Transform(myProj->Transformation());
  V23D.Transform(myProj->Transformation());
  if (myProj->Perspective())
  {
    double f = myProj->Focus();
    double R = 1. - P3D.Z() / f;
    double q = f * R * R;
    double e = V13D.Z() / q;
    double c = e * V13D.Z() / (f * R);
    P.SetCoord(P3D.X() / R, P3D.Y() / R);
    V1.SetCoord(V13D.X() / R + P3D.X() * e, V13D.Y() / R + P3D.Y() * e);
    V2.SetCoord(V23D.X() / R + 2 * V13D.X() * e + P3D.X() * V23D.Z() / q + 2 * P3D.X() * c,
                V23D.Y() / R + 2 * V13D.Y() * e + P3D.Y() * V23D.Z() / q + 2 * P3D.Y() * c);
  }
  else
  {
    P.SetCoord(P3D.X(), P3D.Y());
    V1.SetCoord(V13D.X(), V13D.Y());
    V2.SetCoord(V23D.X(), V23D.Y());
  }
}

//=================================================================================================

void HLRBRep_Curve::D3(const double, gp_Pnt2d&, gp_Vec2d&, gp_Vec2d&, gp_Vec2d&) const {}

//=================================================================================================

gp_Vec2d HLRBRep_Curve::DN(const double, const int) const
{
  return gp_Vec2d();
}

//=================================================================================================

gp_Lin2d HLRBRep_Curve::Line() const
{
  gp_Pnt2d P;
  gp_Vec2d V;
  D1(0., P, V);
  return gp_Lin2d(P, V);
}

//=================================================================================================

gp_Circ2d HLRBRep_Curve::Circle() const
{
  gp_Circ C = HLRBRep_BCurveTool::Circle(myCurve);
  C.Transform(myProj->Transformation());
  return ProjLib::Project(gp_Pln(gp::XOY()), C);
}

//=================================================================================================

gp_Elips2d HLRBRep_Curve::Ellipse() const
{
  if (HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_Ellipse)
  {
    gp_Elips E = HLRBRep_BCurveTool::Ellipse(myCurve);
    E.Transform(myProj->Transformation());
    return ProjLib::Project(gp_Pln(gp::XOY()), E);
  }
  // this is a circle
  gp_Circ C = HLRBRep_BCurveTool::Circle(myCurve);
  C.Transform(myProj->Transformation());
  const gp_Dir& D1  = C.Axis().Direction();
  const gp_Dir& D3  = D1.Crossed(gp::DZ());
  const gp_Dir& D2  = D1.Crossed(D3);
  double        rap = sqrt(D2.X() * D2.X() + D2.Y() * D2.Y());
  gp_Dir2d      d(D1.Y(), -D1.X());
  gp_Pnt2d      p(C.Location().X(), C.Location().Y());
  gp_Elips2d    El(gp_Ax2d(p, d), C.Radius(), C.Radius() * rap);
  if (D1.Z() < 0)
    El.Reverse();
  return El;
}

//=================================================================================================

gp_Hypr2d HLRBRep_Curve::Hyperbola() const
{
  return gp_Hypr2d();
}

//=================================================================================================

gp_Parab2d HLRBRep_Curve::Parabola() const
{
  return gp_Parab2d();
}

//=================================================================================================

void HLRBRep_Curve::Poles(NCollection_Array1<gp_Pnt2d>& TP) const
{
  int                        i1 = TP.Lower();
  int                        i2 = TP.Upper();
  NCollection_Array1<gp_Pnt> TP3(i1, i2);
  //-- HLRBRep_BCurveTool::Poles(myCurve,TP3);
  if (HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom_BSplineCurve>    aBSpl     = HLRBRep_BCurveTool::BSpline(myCurve);
    const NCollection_Array1<gp_Pnt>& aSrcPoles = aBSpl->Poles();
    for (int i = i1; i <= i2; i++)
      TP3(i) = aSrcPoles(i);
  }
  else
  {
    occ::handle<Geom_BezierCurve>     aBez      = HLRBRep_BCurveTool::Bezier(myCurve);
    const NCollection_Array1<gp_Pnt>& aSrcPoles = aBez->Poles();
    for (int i = i1; i <= i2; i++)
      TP3(i) = aSrcPoles(i);
  }
  for (int i = i1; i <= i2; i++)
  {
    myProj->Transform(TP3(i));
    TP(i).SetCoord(TP3(i).X(), TP3(i).Y());
  }
}

//=================================================================================================

void HLRBRep_Curve::Poles(const occ::handle<Geom_BSplineCurve>& aCurve,
                          NCollection_Array1<gp_Pnt2d>&         TP) const
{
  int                        i1 = TP.Lower();
  int                        i2 = TP.Upper();
  NCollection_Array1<gp_Pnt> TP3(i1, i2);
  //-- HLRBRep_BCurveTool::Poles(myCurve,TP3);
  const NCollection_Array1<gp_Pnt>& aSrcPoles = aCurve->Poles();
  for (int i = i1; i <= i2; i++)
    TP3(i) = aSrcPoles(i);

  for (int i = i1; i <= i2; i++)
  {
    ((HLRAlgo_Projector*)myProj)->Transform(TP3(i));
    TP(i).SetCoord(TP3(i).X(), TP3(i).Y());
  }
}

//=================================================================================================

void HLRBRep_Curve::PolesAndWeights(NCollection_Array1<gp_Pnt2d>& TP,
                                    NCollection_Array1<double>&   TW) const
{
  int                        i1 = TP.Lower();
  int                        i2 = TP.Upper();
  NCollection_Array1<gp_Pnt> TP3(i1, i2);
  //-- HLRBRep_BCurveTool::PolesAndWeights(myCurve,TP3,TW);

  if (HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom_BSplineCurve>    HB        = (HLRBRep_BCurveTool::BSpline(myCurve));
    const NCollection_Array1<gp_Pnt>& aSrcPoles = HB->Poles();
    for (int i = i1; i <= i2; i++)
      TP3(i) = aSrcPoles(i);
    const NCollection_Array1<double>* aWPtr = HB->Weights();
    if (aWPtr != nullptr)
      for (int i = i1; i <= i2; i++)
        TW(i) = (*aWPtr)(i);
    //-- (HLRBRep_BCurveTool::BSpline(myCurve))->PolesAndWeights(TP3,TW);
  }
  else
  {
    occ::handle<Geom_BezierCurve>     HB        = (HLRBRep_BCurveTool::Bezier(myCurve));
    const NCollection_Array1<gp_Pnt>& aSrcPoles = HB->Poles();
    for (int i = i1; i <= i2; i++)
      TP3(i) = aSrcPoles(i);
    const NCollection_Array1<double>* aWPtr = HB->Weights();
    if (aWPtr != nullptr)
      for (int i = i1; i <= i2; i++)
        TW(i) = (*aWPtr)(i);
    //-- (HLRBRep_BCurveTool::Bezier(myCurve))->PolesAndWeights(TP3,TW);
  }
  for (int i = i1; i <= i2; i++)
  {
    ((HLRAlgo_Projector*)myProj)->Transform(TP3(i));
    TP(i).SetCoord(TP3(i).X(), TP3(i).Y());
  }
}

//=================================================================================================

void HLRBRep_Curve::PolesAndWeights(const occ::handle<Geom_BSplineCurve>& aCurve,
                                    NCollection_Array1<gp_Pnt2d>&         TP,
                                    NCollection_Array1<double>&           TW) const
{
  int                        i1 = TP.Lower();
  int                        i2 = TP.Upper();
  NCollection_Array1<gp_Pnt> TP3(i1, i2);
  //-- HLRBRep_BCurveTool::PolesAndWeights(myCurve,TP3,TW);

  const NCollection_Array1<gp_Pnt>& aSrcPoles = aCurve->Poles();
  for (int i = i1; i <= i2; i++)
    TP3(i) = aSrcPoles(i);
  const NCollection_Array1<double>* aWPtr = aCurve->Weights();
  if (aWPtr != nullptr)
    for (int i = i1; i <= i2; i++)
      TW(i) = (*aWPtr)(i);
  //-- (HLRBRep_BCurveTool::BSpline(myCurve))->PolesAndWeights(TP3,TW);

  for (int i = i1; i <= i2; i++)
  {
    ((HLRAlgo_Projector*)myProj)->Transform(TP3(i));
    TP(i).SetCoord(TP3(i).X(), TP3(i).Y());
  }
}

//=================================================================================================

void HLRBRep_Curve::Knots(NCollection_Array1<double>& kn) const
{
  if (HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom_BSplineCurve>     aBSpl     = HLRBRep_BCurveTool::BSpline(myCurve);
    const NCollection_Array1<double>& aSrcKnots = aBSpl->Knots();
    for (int i = kn.Lower(); i <= kn.Upper(); i++)
      kn(i) = aSrcKnots(i);
  }
}

//=================================================================================================

void HLRBRep_Curve::Multiplicities(NCollection_Array1<int>& mu) const
{
  if (HLRBRep_BCurveTool::GetType(myCurve) == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom_BSplineCurve>  aBSpl     = HLRBRep_BCurveTool::BSpline(myCurve);
    const NCollection_Array1<int>& aSrcMults = aBSpl->Multiplicities();
    for (int i = mu.Lower(); i <= mu.Upper(); i++)
      mu(i) = aSrcMults(i);
  }
}
