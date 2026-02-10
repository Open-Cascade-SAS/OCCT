// Created on: 1993-09-07
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

// modified by NIZHNY-OFV  Thu Jan 20 11:04:19 2005

#include <ProjLib_ComputeApprox.hxx>
#include <ProjLib.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Convert_CompBezierCurves2dToBSplineCurve2d.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Approx_FitAndDivide2d.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Integer.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <GCPnts_AbscissaPoint.hxx>

#ifdef OCCT_DEBUG
// static bool AffichValue = false;
#endif

namespace
{

//! Helper for optimized point-to-plane projection without gp_Trsf overhead.
//! Pre-computes plane basis for efficient batch projection of multiple points.
struct PlaneProjector
{
  double OX, OY, OZ;    //!< Plane origin
  double DXx, DXy, DXz; //!< X direction components
  double DYx, DYy, DYz; //!< Y direction components

  //! Initialize from plane position.
  PlaneProjector(const gp_Ax3& thePos)
  {
    const gp_Pnt& aLoc = thePos.Location();
    OX                 = aLoc.X();
    OY                 = aLoc.Y();
    OZ                 = aLoc.Z();

    const gp_Dir& aXDir = thePos.XDirection();
    DXx                 = aXDir.X();
    DXy                 = aXDir.Y();
    DXz                 = aXDir.Z();

    const gp_Dir& aYDir = thePos.YDirection();
    DYx                 = aYDir.X();
    DYy                 = aYDir.Y();
    DYz                 = aYDir.Z();
  }

  //! Project point onto plane.
  //! @return 2D point where X = (P - Origin) x XDirection, Y = (P - Origin) x YDirection
  gp_Pnt2d Project(const gp_Pnt& theP) const
  {
    const double dX = theP.X() - OX;
    const double dY = theP.Y() - OY;
    const double dZ = theP.Z() - OZ;
    return gp_Pnt2d(dX * DXx + dY * DXy + dZ * DXz, dX * DYx + dY * DYy + dZ * DYz);
  }
};

} // namespace

//=================================================================================================

// OFV:
static inline bool IsEqual(double Check, double With, double Toler)
{
  return (std::abs(Check - With) < Toler);
}

//=================================================================================================

static gp_Pnt2d Function_Value(const double                          U,
                               const occ::handle<Adaptor3d_Curve>&   myCurve,
                               const occ::handle<Adaptor3d_Surface>& mySurface,
                               const double                          U1,
                               const double                          U2,
                               const double                          V1,
                               const double                          V2,
                               const bool                            UCouture,
                               const bool                            VCouture)
{
  double S = 0., T = 0.;

  gp_Pnt              P3d   = myCurve->Value(U);
  GeomAbs_SurfaceType SType = mySurface->GetType();

  switch (SType)
  {

    case GeomAbs_Plane: {
      gp_Pln Plane = mySurface->Plane();
      ElSLib::Parameters(Plane, P3d, S, T);
      break;
    }
    case GeomAbs_Cylinder: {
      gp_Cylinder Cylinder = mySurface->Cylinder();
      ElSLib::Parameters(Cylinder, P3d, S, T);
      break;
    }
    case GeomAbs_Cone: {
      gp_Cone Cone = mySurface->Cone();
      ElSLib::Parameters(Cone, P3d, S, T);
      break;
    }
    case GeomAbs_Sphere: {
      gp_Sphere Sphere = mySurface->Sphere();
      ElSLib::Parameters(Sphere, P3d, S, T);
      break;
    }
    case GeomAbs_Torus: {
      gp_Torus Torus = mySurface->Torus();
      ElSLib::Parameters(Torus, P3d, S, T);
      break;
    }
    default:
      throw Standard_NoSuchObject("ProjLib_ComputeApprox::Value");
  }

  if (UCouture)
  {
    if (S < U1 || S > U2)
    {
      S = ElCLib::InPeriod(S, U1, U2);
    }
  }

  if (VCouture)
  {
    if (SType == GeomAbs_Sphere)
    {
      if (std::abs(S - U1) > M_PI)
      {
        T = M_PI - T;
        S = M_PI + S;
      }
      if (S > U1 || S < U2)
        S = ElCLib::InPeriod(S, U1, U2);
    }
    if (T < V1 || T > V2)
      T = ElCLib::InPeriod(T, V1, V2);
  }

  return gp_Pnt2d(S, T);
}

//=================================================================================================

static bool Function_D1(const double                          U,
                        gp_Pnt2d&                             P,
                        gp_Vec2d&                             D,
                        const occ::handle<Adaptor3d_Curve>&   myCurve,
                        const occ::handle<Adaptor3d_Surface>& mySurface,
                        const double                          U1,
                        const double                          U2,
                        const double                          V1,
                        const double                          V2,
                        const bool                            UCouture,
                        const bool                            VCouture)
{
  gp_Pnt P3d;
  double dU, dV;

  P = Function_Value(U, myCurve, mySurface, U1, U2, V1, V2, UCouture, VCouture);

  GeomAbs_SurfaceType Type = mySurface->GetType();

  switch (Type)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cone:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Torus: {
      gp_Vec D1U, D1V;
      gp_Vec T;
      myCurve->D1(U, P3d, T);
      mySurface->D1(P.X(), P.Y(), P3d, D1U, D1V);

      dU        = T.Dot(D1U);
      dV        = T.Dot(D1V);
      double Nu = D1U.SquareMagnitude();
      double Nv = D1V.SquareMagnitude();

      if (Nu < Epsilon(1.) || Nv < Epsilon(1.))
        return false;

      dU /= Nu;
      dV /= Nv;
      D = gp_Vec2d(dU, dV);
    }
    break;

    default:
      return false;
  }

  return true;
}

//=================================================================================================

static double Function_ComputeStep(const occ::handle<Adaptor3d_Curve>& myCurve, const double R)
{
  double Step0 = .1;
  double W1, W2;
  W1          = myCurve->FirstParameter();
  W2          = myCurve->LastParameter();
  double L    = GCPnts_AbscissaPoint::Length(*myCurve);
  int    nbp  = RealToInt(L / (R * M_PI_4)) + 1;
  nbp         = std::max(nbp, 3);
  double Step = (W2 - W1) / (nbp - 1);
  if (Step > Step0)
  {
    Step = Step0;
    nbp  = RealToInt((W2 - W1) / Step) + 1;
    nbp  = std::max(nbp, 3);
    Step = (W2 - W1) / (nbp - 1);
  }

  return Step;
}

//=================================================================================================

static void Function_SetUVBounds(double&                               myU1,
                                 double&                               myU2,
                                 double&                               myV1,
                                 double&                               myV2,
                                 bool&                                 UCouture,
                                 bool&                                 VCouture,
                                 const occ::handle<Adaptor3d_Curve>&   myCurve,
                                 const occ::handle<Adaptor3d_Surface>& mySurface)
{
  double W1, W2, W;
  gp_Pnt P1, P2, P;
  //
  W1 = myCurve->FirstParameter();
  W2 = myCurve->LastParameter();
  W  = 0.5 * (W1 + W2);
  // on ouvre l`intervalle
  // W1 += 1.0e-9;
  // W2 -= 1.0e-9;
  P1 = myCurve->Value(W1);
  P2 = myCurve->Value(W2);
  P  = myCurve->Value(W);

  switch (mySurface->GetType())
  {

    case GeomAbs_Cone: {
      double           tol  = Epsilon(1.);
      constexpr double ptol = Precision::PConfusion();
      gp_Cone          Cone = mySurface->Cone();
      VCouture              = false;
      // Calculation of cone parameters for P == ConeApex often produces wrong
      // values of U
      gp_Pnt ConeApex = Cone.Apex();
      if (ConeApex.XYZ().IsEqual(P1.XYZ(), tol))
      {
        W1 += ptol;
        P1 = myCurve->Value(W1);
      }
      if (ConeApex.XYZ().IsEqual(P2.XYZ(), tol))
      {
        W2 -= ptol;
        P2 = myCurve->Value(W2);
      }
      if (ConeApex.XYZ().IsEqual(P.XYZ(), tol))
      {
        W += ptol;
        P = myCurve->Value(W);
      }

      switch (myCurve->GetType())
      {
        case GeomAbs_Parabola:
        case GeomAbs_Hyperbola:
        case GeomAbs_Ellipse: {
          double U1, U2, V1, V2, U, V;
          ElSLib::Parameters(Cone, P1, U1, V1);
          ElSLib::Parameters(Cone, P2, U2, V2);
          ElSLib::Parameters(Cone, P, U, V);
          myU1 = std::min(U1, U2);
          myU2 = std::max(U1, U2);
          if ((U1 < U && U < U2) && !myCurve->IsClosed())
          {
            UCouture = false;
          }
          else
          {
            UCouture = true;
            myU2     = myU1 + 2 * M_PI;
          }
        }
        break;
        default: {
          double U1, V1, U, V, Delta = 0., d = 0., pmin = W1, pmax = W1, dmax = 0., Uf, Ul;
          ElSLib::Parameters(Cone, P1, U1, V1);
          ElSLib::Parameters(Cone, P2, Ul, V1);
          const gp_Ax1& anAx1 = Cone.Axis();
          gp_Lin        aLin(anAx1);
          double        R = (aLin.Distance(P1) + aLin.Distance(P2) + aLin.Distance(P)) / 3.;
          double        Step;
          myU1 = U1;
          myU2 = U1;
          Uf   = U1;
          if (myCurve->GetType() == GeomAbs_Line)
          {
            int nbp = 3;
            Step    = (W2 - W1) / (nbp - 1);
          }
          else
          {
            Step = Function_ComputeStep(myCurve, R);
          }
          //
          bool isclandper = (!(myCurve->IsClosed()) && !(myCurve->IsPeriodic()));
          bool isFirst    = true;
          for (double par = W1 + Step; par <= W2; par += Step)
          {
            if (!isclandper)
              par += Step;
            P = myCurve->Value(par);
            ElSLib::Parameters(Cone, P, U, V);
            U += Delta;
            d = U - U1;
            if (d > M_PI)
            {
              if (((IsEqual(U, (2 * M_PI), 1.e-10) && (U1 >= 0. && U1 <= M_PI))
                   && (IsEqual(U, Ul, 1.e-10) && !IsEqual(Uf, 0., 1.e-10)))
                  && isclandper)
                U = 0.0;
              else
              {
                // Protection against first-last point on seam.
                if (isFirst)
                  U1 = 2 * M_PI;
                else if (par + Step >= W2)
                  U = 0.0;
                else
                  Delta -= 2 * M_PI;
              }
              U += Delta;
              d = U - U1;
            }
            else if (d < -M_PI)
            {
              if (((IsEqual(U, 0., 1.e-10) && (U1 >= M_PI && U1 <= (2 * M_PI)))
                   && (IsEqual(U, Ul, 1.e-10) && !IsEqual(Uf, (2 * M_PI), 1.e-10)))
                  && isclandper)
                U = 2 * M_PI;
              else
              {
                // Protection against first-last point on seam.
                if (isFirst)
                  U1 = 0.0;
                else if (par + Step >= W2)
                  U = 2 * M_PI;
                else
                  Delta += 2 * M_PI;
              }
              U += Delta;
              d = U - U1;
            }
            dmax = std::max(dmax, std::abs(d));
            if (U < myU1)
            {
              myU1 = U;
              pmin = par;
            }
            if (U > myU2)
            {
              myU2 = U;
              pmax = par;
            }
            U1      = U;
            isFirst = false;
          } // for(double par = W1 + Step; par <= W2; par += Step)

          if (std::abs(pmin - W1) > Precision::PConfusion()
              && std::abs(pmin - W2) > Precision::PConfusion())
            myU1 -= dmax * .5;
          if (std::abs(pmax - W1) > Precision::PConfusion()
              && std::abs(pmax - W2) > Precision::PConfusion())
            myU2 += dmax * .5;

          if ((myU1 >= 0. && myU1 <= 2 * M_PI) && (myU2 >= 0. && myU2 <= 2 * M_PI))
            UCouture = false;
          else
          {
            U        = (myU1 + myU2) / 2.;
            myU1     = U - M_PI;
            myU2     = U + M_PI;
            UCouture = true;
          }
        }
        break;
      } // switch curve type
    } // case Cone
    break;

    case GeomAbs_Cylinder: {
      gp_Cylinder Cylinder = mySurface->Cylinder();
      VCouture             = false;

      if (myCurve->GetType() == GeomAbs_Ellipse)
      {

        double U1, U2, V1, V2, U, V;
        ElSLib::Parameters(Cylinder, P1, U1, V1);
        ElSLib::Parameters(Cylinder, P2, U2, V2);
        ElSLib::Parameters(Cylinder, P, U, V);
        myU1 = std::min(U1, U2);
        myU2 = std::max(U1, U2);

        if (!myCurve->IsClosed())
        {
          if (myU1 < U && U < myU2)
          {
            U    = (myU1 + myU2) / 2.;
            myU1 = U - M_PI;
            myU2 = U + M_PI;
          }
          else
          {
            U = (myU1 + myU2) / 2.;
            if (myU1 < U)
            {
              myU1 = U - 2 * M_PI;
              myU2 = U;
            }
            else
            {
              myU1 = U;
              myU2 = U + 2 * M_PI;
            }
          }
          UCouture = true;
        }
        else
        {
          gp_Vec D1U, D1V;
          gp_Vec T;
          gp_Pnt P3d;
          myCurve->D1(W1, P3d, T);
          mySurface->D1(U1, U2, P3d, D1U, D1V);
          double dU = T.Dot(D1U);

          UCouture = true;
          if (dU > 0.)
          {
            myU2 = myU1 + 2 * M_PI;
          }
          else
          {
            myU2 = myU1;
            myU1 -= 2 * M_PI;
          }
        }
      }
      else
      {
        double U1, V1, U, V;
        ElSLib::Parameters(Cylinder, P1, U1, V1);
        double R     = Cylinder.Radius();
        double Delta = 0., Step;
        double eps = M_PI, dmax = 0., d = 0.;
        Step        = Function_ComputeStep(myCurve, R);
        myU1        = U1;
        myU2        = U1;
        double pmin = W1, pmax = W1, plim = W2 + .1 * Step;
        for (double par = W1 + Step; par <= plim; par += Step)
        {
          P = myCurve->Value(par);
          ElSLib::Parameters(Cylinder, P, U, V);
          U += Delta;
          d = U - U1;
          if (d > eps)
          {
            U -= Delta;
            Delta -= 2 * M_PI;
            U += Delta;
            d = U - U1;
          }
          else if (d < -eps)
          {
            U -= Delta;
            Delta += 2 * M_PI;
            U += Delta;
            d = U - U1;
          }
          dmax = std::max(dmax, std::abs(d));
          if (U < myU1)
          {
            myU1 = U;
            pmin = par;
          }
          if (U > myU2)
          {
            myU2 = U;
            pmax = par;
          }
          U1 = U;
        }

        if (std::abs(pmin - W1) > Precision::PConfusion()
            && std::abs(pmin - W2) > Precision::PConfusion())
          myU1 -= dmax * .5;
        if (std::abs(pmax - W1) > Precision::PConfusion()
            && std::abs(pmax - W2) > Precision::PConfusion())
          myU2 += dmax * .5;

        if ((myU1 >= 0. && myU1 <= 2 * M_PI) && (myU2 >= 0. && myU2 <= 2 * M_PI))
        {
          UCouture = false;
        }
        else
        {
          U        = (myU1 + myU2) / 2.;
          myU1     = U - M_PI;
          myU2     = U + M_PI;
          UCouture = true;
        }
      }
    }
    break;
    //
    case GeomAbs_Sphere: {
      VCouture     = false;
      gp_Sphere SP = mySurface->Sphere();
      if (myCurve->GetType() == GeomAbs_Circle)
      {
        UCouture = true;

        // we seek to know how many times the seam is
        // crossed.
        // if 0 or 2 times: the PCurve is closed and in the interval
        //                  [Uc-PI, Uc+PI] (Uc: U of circle center)
        // if 1 time      : the PCurve is open and in the interval
        //                  [U1, U1 +/- 2*PI]

        // to determine the number of solutions, we solve the system
        // x^2 + y^2 + z^2     = R^2  (1)
        // A x + B y + C z + D = 0    (2)
        // x > 0                      (3)
        // y = 0                      (4)
        // REM : (1) (2)     : equation du cercle
        //       (1) (3) (4) : equation de la couture.
        int     NbSolutions = 0;
        double  A, B, C, D, R, Tol = 1.e-10;
        double  U1, U2, V1, V2;
        gp_Trsf Trsf;
        //
        gp_Circ Circle = myCurve->Circle();
        Trsf.SetTransformation(SP.Position());
        Circle.Transform(Trsf);
        //
        R = SP.Radius();
        gp_Pln Plane(gp_Ax3(Circle.Position()));
        Plane.Coefficients(A, B, C, D);
        //
        if (std::abs(C) < Tol)
        {
          if (std::abs(A) > Tol)
          {
            if ((D / A) < 0.)
            {
              if ((R - std::abs(D / A)) > Tol)
                NbSolutions = 2;
              else if (std::abs(R - std::abs(D / A)) < Tol)
                NbSolutions = 1;
              else
                NbSolutions = 0;
            }
          }
        }
        else
        {
          double delta = R * R * (A * A + C * C) - D * D;
          delta *= C * C;
          if (std::abs(delta) < Tol * Tol)
          {
            if (A * D > 0.)
              NbSolutions = 1;
          }
          else if (delta > 0)
          {
            double xx;
            delta = std::sqrt(delta);
            xx    = -A * D + delta;
            //
            if (xx > Tol)
              NbSolutions++;
            xx = -A * D - delta;
            //
            if (xx > Tol)
              NbSolutions++;
          }
        }
        //

        // box+sphere >>
        double UU = 0.;
        ElSLib::Parameters(SP, P1, U1, V1);
        double eps = 10. * Epsilon(1.);
        double dt  = std::max(Precision::PConfusion(), 0.01 * (W2 - W1));
        if (std::abs(U1) < eps)
        {
          // May be U1 must be equal 2*PI?
          gp_Pnt Pd = myCurve->Value(W1 + dt);
          double ud, vd;
          ElSLib::Parameters(SP, Pd, ud, vd);
          if (std::abs(U1 - ud) > M_PI)
          {
            U1 = 2. * M_PI;
          }
        }
        else if (std::abs(2. * M_PI - U1) < eps)
        {
          // maybe U1 = 0.?
          gp_Pnt Pd = myCurve->Value(W1 + dt);
          double ud, vd;
          ElSLib::Parameters(SP, Pd, ud, vd);
          if (std::abs(U1 - ud) > M_PI)
          {
            U1 = 0.;
          }
        }
        //
        ElSLib::Parameters(SP, P2, U2, V1);
        if (std::abs(U2) < eps)
        {
          // May be U2 must be equal 2*PI?
          gp_Pnt Pd = myCurve->Value(W2 - dt);
          double ud, vd;
          ElSLib::Parameters(SP, Pd, ud, vd);
          if (std::abs(U2 - ud) > M_PI)
          {
            U2 = 2. * M_PI;
          }
        }
        else if (std::abs(2. * M_PI - U2) < eps)
        {
          // maybe U2 = 0.?
          gp_Pnt Pd = myCurve->Value(W2 - dt);
          double ud, vd;
          ElSLib::Parameters(SP, Pd, ud, vd);
          if (std::abs(U2 - ud) > M_PI)
          {
            U2 = 0.;
          }
        }
        //
        ElSLib::Parameters(SP, P, UU, V1);
        P2 = myCurve->Value(W1 + M_PI / 8);
        ElSLib::Parameters(SP, P2, U2, V2);
        //
        if (NbSolutions == 1)
        {
          if (std::abs(U1 - U2) > M_PI)
          { // on traverse la couture
            if (U1 > M_PI)
            {
              myU1 = U1;
              myU2 = U1 + 2 * M_PI;
            }
            else
            {
              myU2 = U1;
              myU1 = U1 - 2 * M_PI;
            }
          }
          else
          { // on ne traverse pas la couture
            if (U1 > U2)
            {
              myU2 = U1;
              myU1 = U1 - 2 * M_PI;
            }
            else
            {
              myU1 = U1;
              myU2 = U1 + 2 * M_PI;
            }
          }
        }
        else
        { // 0 ou 2 solutions
          gp_Pnt Center = Circle.Location();
          double U, V;
          ElSLib::SphereParameters(gp_Ax3(gp::XOY()), 1, Center, U, V);
          myU1 = U - M_PI;
          myU2 = U + M_PI;
        }
        //
        // eval the VCouture.
        if ((C == 0) || std::abs(std::abs(D / C) - R) > 1.e-10)
        {
          VCouture = false;
        }
        else
        {
          VCouture = true;
          UCouture = true;

          if (D / C < 0.)
          {
            myV1 = -M_PI / 2.;
            myV2 = 3 * M_PI / 2.;
          }
          else
          {
            myV1 = -3 * M_PI / 2.;
            myV2 = M_PI / 2.;
          }

          // if P1.Z() equals +/- R we are at the vertex: not significant.
          gp_Pnt pp = P1.Transformed(Trsf);

          if (std::abs(pp.X() * pp.X() + pp.Y() * pp.Y() + pp.Z() * pp.Z() - R * R) < Tol)
          {
            gp_Pnt Center = Circle.Location();
            double U, V;
            ElSLib::SphereParameters(gp_Ax3(gp::XOY()), 1, Center, U, V);
            myU1     = U - M_PI;
            myU2     = U + M_PI;
            VCouture = false;
          }
        }

        // box+sphere >>
        myV1 = -1.e+100;
        myV2 = 1.e+100;
        // box+sphere <<

      } // if ( myCurve->GetType() == GeomAbs_Circle)

      else
      {
        double U1, V1, U, V;
        ElSLib::Parameters(SP, P1, U1, V1);
        double R     = SP.Radius();
        double Delta = 0., Step;
        double eps = M_PI, dmax = 0., d = 0.;
        Step        = Function_ComputeStep(myCurve, R);
        myU1        = U1;
        myU2        = U1;
        double pmin = W1, pmax = W1, plim = W2 + .1 * Step;
        for (double par = W1 + Step; par <= plim; par += Step)
        {
          P = myCurve->Value(par);
          ElSLib::Parameters(SP, P, U, V);
          U += Delta;
          d = U - U1;
          if (d > eps)
          {
            U -= Delta;
            Delta -= 2 * M_PI;
            U += Delta;
            d = U - U1;
          }
          else if (d < -eps)
          {
            U -= Delta;
            Delta += 2 * M_PI;
            U += Delta;
            d = U - U1;
          }
          dmax = std::max(dmax, std::abs(d));
          if (U < myU1)
          {
            myU1 = U;
            pmin = par;
          }
          if (U > myU2)
          {
            myU2 = U;
            pmax = par;
          }
          U1 = U;
        }

        if (std::abs(pmin - W1) > Precision::PConfusion()
            && std::abs(pmin - W2) > Precision::PConfusion())
          myU1 -= dmax * .5;
        if (std::abs(pmax - W1) > Precision::PConfusion()
            && std::abs(pmax - W2) > Precision::PConfusion())
          myU2 += dmax * .5;

        if ((myU1 >= 0. && myU1 <= 2 * M_PI) && (myU2 >= 0. && myU2 <= 2 * M_PI))
        {
          myU1     = 0.;
          myU2     = 2. * M_PI;
          UCouture = false;
        }
        else
        {
          U        = (myU1 + myU2) / 2.;
          myU1     = U - M_PI;
          myU2     = U + M_PI;
          UCouture = true;
        }

        VCouture = false;
      }
    }
    break;
    //
    case GeomAbs_Torus: {
      gp_Torus TR = mySurface->Torus();
      double   U1, V1, U, V, dU, dV;
      ElSLib::Parameters(TR, P1, U1, V1);
      double R      = TR.MinorRadius();
      double DeltaU = 0., DeltaV = 0., Step;
      double eps = M_PI, dmaxU = 0., dmaxV = 0.;
      Step         = Function_ComputeStep(myCurve, R);
      myU1         = U1;
      myU2         = U1;
      myV1         = V1;
      myV2         = V1;
      double pminU = W1, pmaxU = W1, pminV = W1, pmaxV = W1, plim = W2 + .1 * Step;
      for (double par = W1 + Step; par <= plim; par += Step)
      {
        P = myCurve->Value(par);
        ElSLib::Parameters(TR, P, U, V);
        U += DeltaU;
        V += DeltaV;
        dU = U - U1;
        dV = V - V1;
        if (dU > eps)
        {
          U -= DeltaU;
          DeltaU -= 2 * M_PI;
          U += DeltaU;
          dU = U - U1;
        }
        else if (dU < -eps)
        {
          U -= DeltaU;
          DeltaU += 2 * M_PI;
          U += DeltaU;
          dU = U - U1;
        }
        if (dV > eps)
        {
          V -= DeltaV;
          DeltaV -= 2 * M_PI;
          V += DeltaV;
          dV = V - V1;
        }
        else if (dV < -eps)
        {
          V -= DeltaV;
          DeltaV += 2 * M_PI;
          V += DeltaV;
          dV = V - V1;
        }
        dmaxU = std::max(dmaxU, std::abs(dU));
        dmaxV = std::max(dmaxV, std::abs(dV));
        if (U < myU1)
        {
          myU1  = U;
          pminU = par;
        }
        if (U > myU2)
        {
          myU2  = U;
          pmaxU = par;
        }
        if (V < myV1)
        {
          myV1  = V;
          pminV = par;
        }
        if (V > myV2)
        {
          myV2  = V;
          pmaxV = par;
        }
        U1 = U;
        V1 = V;
      }

      if (std::abs(pminU - W1) > Precision::PConfusion()
          && std::abs(pminU - W2) > Precision::PConfusion())
        myU1 -= dmaxU * .5;
      if (std::abs(pmaxU - W1) > Precision::PConfusion()
          && std::abs(pmaxU - W2) > Precision::PConfusion())
        myU2 += dmaxU * .5;
      if (std::abs(pminV - W1) > Precision::PConfusion()
          && std::abs(pminV - W2) > Precision::PConfusion())
        myV1 -= dmaxV * .5;
      if (std::abs(pmaxV - W1) > Precision::PConfusion()
          && std::abs(pmaxV - W2) > Precision::PConfusion())
        myV2 += dmaxV * .5;

      if ((myU1 >= 0. && myU1 <= 2 * M_PI) && (myU2 >= 0. && myU2 <= 2 * M_PI))
      {
        myU1     = 0.;
        myU2     = 2. * M_PI;
        UCouture = false;
      }
      else
      {
        U        = (myU1 + myU2) / 2.;
        myU1     = U - M_PI;
        myU2     = U + M_PI;
        UCouture = true;
      }
      if ((myV1 >= 0. && myV1 <= 2 * M_PI) && (myV2 >= 0. && myV2 <= 2 * M_PI))
      {
        VCouture = false;
      }
      else
      {
        V        = (myV1 + myV2) / 2.;
        myV1     = V - M_PI;
        myV2     = V + M_PI;
        VCouture = true;
      }
    }
    break;

    default: {
      UCouture = false;
      VCouture = false;
    }
    break;
  }
}

//
//

//=================================================================================================

class ProjLib_Function : public AppCont_Function
{
  occ::handle<Adaptor3d_Curve>   myCurve;
  occ::handle<Adaptor3d_Surface> mySurface;
  bool                           myIsPeriodic[2];
  double                         myPeriod[2];

public:
  double myU1, myU2, myV1, myV2;
  bool   UCouture, VCouture;

  ProjLib_Function(const occ::handle<Adaptor3d_Curve>& C, const occ::handle<Adaptor3d_Surface>& S)
      : myCurve(C),
        mySurface(S),
        myU1(0.0),
        myU2(0.0),
        myV1(0.0),
        myV2(0.0),
        UCouture(false),
        VCouture(false)
  {
    myNbPnt   = 0;
    myNbPnt2d = 1;
    Function_SetUVBounds(myU1, myU2, myV1, myV2, UCouture, VCouture, myCurve, mySurface);
    myIsPeriodic[0] = mySurface->IsUPeriodic();
    myIsPeriodic[1] = mySurface->IsVPeriodic();

    if (myIsPeriodic[0])
      myPeriod[0] = mySurface->UPeriod();
    else
      myPeriod[0] = 0.0;

    if (myIsPeriodic[1])
      myPeriod[1] = mySurface->VPeriod();
    else
      myPeriod[1] = 0.0;
  }

  void PeriodInformation(const int theDimIdx, bool& IsPeriodic, double& thePeriod) const override
  {
    IsPeriodic = myIsPeriodic[theDimIdx - 1];
    thePeriod  = myPeriod[theDimIdx - 1];
  }

  double FirstParameter() const override { return (myCurve->FirstParameter()); }

  double LastParameter() const override { return (myCurve->LastParameter()); }

  bool Value(const double                  theT,
             NCollection_Array1<gp_Pnt2d>& thePnt2d,
             NCollection_Array1<gp_Pnt>& /*thePnt*/) const override
  {
    thePnt2d(1) =
      Function_Value(theT, myCurve, mySurface, myU1, myU2, myV1, myV2, UCouture, VCouture);
    return true;
  }

  gp_Pnt2d Value(const double theT) const
  {
    return Function_Value(theT, myCurve, mySurface, myU1, myU2, myV1, myV2, UCouture, VCouture);
  }

  bool D1(const double                  theT,
          NCollection_Array1<gp_Vec2d>& theVec2d,
          NCollection_Array1<gp_Vec>& /*theVec*/) const override
  {
    gp_Pnt2d aPnt2d;
    gp_Vec2d aVec2d;
    bool     isOk = Function_D1(theT,
                            aPnt2d,
                            aVec2d,
                            myCurve,
                            mySurface,
                            myU1,
                            myU2,
                            myV1,
                            myV2,
                            UCouture,
                            VCouture);
    theVec2d(1)   = aVec2d;
    return isOk;
  }
};

//=================================================================================================

static double ComputeTolU(const occ::handle<Adaptor3d_Surface>& theSurf, const double theTolerance)
{
  double aTolU = theSurf->UResolution(theTolerance);
  if (theSurf->IsUPeriodic())
  {
    aTolU = std::min(aTolU, 0.01 * theSurf->UPeriod());
  }

  return aTolU;
}

//=================================================================================================

static double ComputeTolV(const occ::handle<Adaptor3d_Surface>& theSurf, const double theTolerance)
{
  double aTolV = theSurf->VResolution(theTolerance);
  if (theSurf->IsVPeriodic())
  {
    aTolV = std::min(aTolV, 0.01 * theSurf->VPeriod());
  }

  return aTolV;
}

//=================================================================================================

ProjLib_ComputeApprox::ProjLib_ComputeApprox()
    : myTolerance(Precision::PApproximation()),
      myDegMin(-1),
      myDegMax(-1),
      myMaxSegments(-1),
      myBndPnt(AppParCurves_TangencyPoint)
{
}

//=================================================================================================

ProjLib_ComputeApprox::ProjLib_ComputeApprox(const occ::handle<Adaptor3d_Curve>&   C,
                                             const occ::handle<Adaptor3d_Surface>& S,
                                             const double                          Tol)
    : myTolerance(std::max(Tol, Precision::PApproximation())),
      myDegMin(-1),
      myDegMax(-1),
      myMaxSegments(-1),
      myBndPnt(AppParCurves_TangencyPoint)
{
  Perform(C, S);
}

//=================================================================================================

void ProjLib_ComputeApprox::Perform(const occ::handle<Adaptor3d_Curve>&   C,
                                    const occ::handle<Adaptor3d_Surface>& S)
{
  // if the surface is a plane and the curve a BSpline or a BezierCurve,
  // don`t make an Approx but only the projection of the poles.

  int                 NbKnots, NbPoles;
  GeomAbs_CurveType   CType = C->GetType();
  GeomAbs_SurfaceType SType = S->GetType();

  bool SurfIsAnal = ProjLib::IsAnaSurf(S);

  bool CurvIsAnal = (CType != GeomAbs_BSplineCurve) && (CType != GeomAbs_BezierCurve)
                    && (CType != GeomAbs_OffsetCurve) && (CType != GeomAbs_OtherCurve);

  bool simplecase = SurfIsAnal && CurvIsAnal;
  if (CType == GeomAbs_BSplineCurve || CType == GeomAbs_BezierCurve)
  {
    int aNbKnots = 1;
    if (CType == GeomAbs_BSplineCurve)
    {
      aNbKnots = C->NbKnots();
    }
    simplecase = simplecase && C->Degree() <= 2 && aNbKnots <= 2;
  }

  if (CType == GeomAbs_BSplineCurve && SType == GeomAbs_Plane)
  {

    // get the poles and eventually the weights
    occ::handle<Geom_BSplineCurve> BS     = C->BSpline();
    NbPoles                               = BS->NbPoles();
    const NCollection_Array1<gp_Pnt>& P3d = BS->Poles();
    NCollection_Array1<gp_Pnt2d>      Poles(1, NbPoles);

    // Project poles onto plane using optimized projector (avoids gp_Trsf per point)
    const PlaneProjector aProj(S->Plane().Position());
    for (int i = 1; i <= NbPoles; i++)
    {
      Poles.SetValue(i, aProj.Project(P3d(i)));
    }
    const NCollection_Array1<double>& Knots = BS->Knots();
    const NCollection_Array1<int>&    Mults = BS->Multiplicities();
    // get the knots and mults if BSplineCurve
    if (BS->IsRational())
    {
      myBSpline = new Geom2d_BSplineCurve(Poles,
                                          BS->WeightsArray(),
                                          Knots,
                                          Mults,
                                          BS->Degree(),
                                          BS->IsPeriodic());
    }
    else
    {
      myBSpline = new Geom2d_BSplineCurve(Poles, Knots, Mults, BS->Degree(), BS->IsPeriodic());
    }
  }
  else if (CType == GeomAbs_BezierCurve && SType == GeomAbs_Plane)
  {

    // get the poles and eventually the weights
    occ::handle<Geom_BezierCurve> BezierCurvePtr = C->Bezier();
    NbPoles                                      = BezierCurvePtr->NbPoles();
    const NCollection_Array1<gp_Pnt>& P3d        = BezierCurvePtr->Poles();
    NCollection_Array1<gp_Pnt2d>      Poles(1, NbPoles);

    // Project poles onto plane using optimized projector (avoids gp_Trsf per point)
    const PlaneProjector aProj(S->Plane().Position());
    for (int i = 1; i <= NbPoles; i++)
    {
      Poles.SetValue(i, aProj.Project(P3d(i)));
    }
    if (BezierCurvePtr->IsRational())
    {
      myBezier = new Geom2d_BezierCurve(Poles, BezierCurvePtr->WeightsArray());
    }
    else
    {
      myBezier = new Geom2d_BezierCurve(Poles);
    }
  }
  else
  {
    ProjLib_Function F(C, S);

#ifdef OCCT_DEBUG
    // if ( AffichValue) {
    //   int Nb = 20;
    //   double U1, U2, dU, U;
    //   U1 = F.FirstParameter();
    //   U2 = F.LastParameter();
    //   dU = ( U2 - U1) / Nb;
    //   NCollection_Array1<int> Mults(1,Nb+1);
    //   NCollection_Array1<double>    Knots(1,Nb+1);
    //   NCollection_Array1<gp_Pnt2d>    Poles(1,Nb+1);
    //   for ( int i = 1; i <= Nb+1; i++) {
    //    U = U1 + (i-1)*dU;
    //    Poles(i) = F.Value(U);
    //    std::cout << "i = " << i << ": U = " << U <<
    //       ", p(" << Poles(i).X() << ", " << Poles(i).Y() << ");" << std::endl;
    //    Knots(i) = i;
    //    Mults(i) = 1;
    //   }
    //   Mults(1)    = 2;
    //   Mults(Nb+1) = 2;

    // 2D-curve for showing in DRAW
    //   occ::handle<Geom2d_Curve> aCC = new Geom2d_BSplineCurve(Poles,Knots,Mults,1);
    //   AffichValue = false;
    // }
#endif

    //-----------
    int Deg1 = 5, Deg2;
    if (simplecase)
    {
      Deg2 = 8;
    }
    else
    {
      Deg2 = 10;
    }
    if (myDegMin > 0)
    {
      Deg1 = myDegMin;
    }
    //
    if (myDegMax > 0)
    {
      Deg2 = myDegMax;
    }
    //
    int aMaxSegments = 1000;
    if (myMaxSegments > 0)
    {
      aMaxSegments = myMaxSegments;
    }
    AppParCurves_Constraint aFistC = AppParCurves_TangencyPoint,
                            aLastC = AppParCurves_TangencyPoint;
    if (myBndPnt != AppParCurves_TangencyPoint)
    {
      aFistC = myBndPnt;
      aLastC = myBndPnt;
    }

    //-------------
    const double aTolU = ComputeTolU(S, myTolerance);
    const double aTolV = ComputeTolV(S, myTolerance);
    const double aTol2d =
      std::max(std::sqrt(aTolU * aTolU + aTolV * aTolV), Precision::PConfusion());

    Approx_FitAndDivide2d Fit(Deg1, Deg2, myTolerance, aTol2d, true, aFistC, aLastC);
    Fit.SetMaxSegments(aMaxSegments);
    if (simplecase)
    {
      Fit.SetHangChecking(false);
    }
    Fit.Perform(F);

    double aNewTol2d = 0;
    if (Fit.IsAllApproximated())
    {
      int i;
      int NbCurves = Fit.NbMultiCurves();

      // on essaie de rendre la courbe au moins C1
      Convert_CompBezierCurves2dToBSplineCurve2d Conv;

      double Tol3d, Tol2d;
      for (i = 1; i <= NbCurves; i++)
      {
        Fit.Error(i, Tol3d, Tol2d);
        aNewTol2d                       = std::max(aNewTol2d, Tol2d);
        AppParCurves_MultiCurve      MC = Fit.Value(i);           // Charge la Ieme Curve
        NCollection_Array1<gp_Pnt2d> Poles2d(1, MC.Degree() + 1); // Recupere les poles
        MC.Curve(1, Poles2d);
        Conv.AddCurve(Poles2d);
      }

      // mise a jour des fields de ProjLib_Approx
      Conv.Perform();
      NbPoles = Conv.NbPoles();
      NbKnots = Conv.NbKnots();

      if (NbPoles <= 0 || NbPoles > 100000)
        return;
      if (NbKnots <= 0 || NbKnots > 100000)
        return;

      NCollection_Array1<gp_Pnt2d> NewPoles(1, NbPoles);
      NCollection_Array1<double>   NewKnots(1, NbKnots);
      NCollection_Array1<int>      NewMults(1, NbKnots);

      Conv.KnotsAndMults(NewKnots, NewMults);
      Conv.Poles(NewPoles);

      BSplCLib::Reparametrize(C->FirstParameter(), C->LastParameter(), NewKnots);

      // Set NewKnots(NbKnots) exactly C->LastParameter()
      // to avoid problems if trim is used.
      NewKnots(NbKnots) = C->LastParameter();

      // il faut recadrer les poles de debut et de fin:
      // ( Car pour les problemes de couture, on a du ouvrir l`intervalle
      // de definition de la courbe.)
      // On choisit de calculer ces poles par prolongement de la courbe
      // approximee.
      myBSpline = new Geom2d_BSplineCurve(NewPoles, NewKnots, NewMults, Conv.Degree());

      if (aFistC == AppParCurves_PassPoint || aLastC == AppParCurves_PassPoint)
      {
        // try to smoother the Curve GeomAbs_C1.
        int    aDeg       = myBSpline->Degree();
        bool   OK         = true;
        double aSmoothTol = std::max(Precision::Confusion(), aNewTol2d);
        for (int ij = 2; ij < NbKnots; ij++)
        {
          OK = OK && myBSpline->RemoveKnot(ij, aDeg - 1, aSmoothTol);
        }
      }
    }
    else
    {
      int NbCurves = Fit.NbMultiCurves();
      if (NbCurves != 0)
      {
        double Tol3d, Tol2d;
        Fit.Error(NbCurves, Tol3d, Tol2d);
        aNewTol2d = Tol2d;
      }
    }

    // restore tolerance 3d from 2d

    // Here we consider that
    //    aTolU(new)/aTolV(new) = aTolU(old)/aTolV(old)
    //(it is assumption indeed).
    // Then,
    //   Tol3D(new)/Tol3D(old) = Tol2D(new)/Tol2D(old).
    myTolerance *= (aNewTol2d / aTol2d);

    // Return curve home
    double UFirst = F.FirstParameter();
    double ULast  = F.LastParameter();
    double Umid   = (UFirst + ULast) / 2;
    gp_Pnt P3d    = C->Value(Umid);
    double u = 0., v = 0.;
    switch (SType)
    {
      case GeomAbs_Plane: {
        gp_Pln Plane = S->Plane();
        ElSLib::Parameters(Plane, P3d, u, v);
        break;
      }
      case GeomAbs_Cylinder: {
        gp_Cylinder Cylinder = S->Cylinder();
        ElSLib::Parameters(Cylinder, P3d, u, v);
        break;
      }
      case GeomAbs_Cone: {
        gp_Cone Cone = S->Cone();
        ElSLib::Parameters(Cone, P3d, u, v);
        break;
      }
      case GeomAbs_Sphere: {
        gp_Sphere Sphere = S->Sphere();
        ElSLib::Parameters(Sphere, P3d, u, v);
        break;
      }
      case GeomAbs_Torus: {
        gp_Torus Torus = S->Torus();
        ElSLib::Parameters(Torus, P3d, u, v);
        break;
      }
      default:
        throw Standard_NoSuchObject("ProjLib_ComputeApprox::Value");
    }
    bool   ToMirror = false;
    double du = 0., dv = 0.;
    int    number;
    if (F.VCouture)
    {
      if (SType == GeomAbs_Sphere && std::abs(u - F.myU1) > M_PI)
      {
        ToMirror = true;
        dv       = -M_PI;
        v        = M_PI - v;
      }
      double newV = ElCLib::InPeriod(v, F.myV1, F.myV2);
      number      = (int)(std::floor((newV - v) / (F.myV2 - F.myV1)));
      dv -= number * (F.myV2 - F.myV1);
    }
    if (F.UCouture || (F.VCouture && SType == GeomAbs_Sphere))
    {
      double   aNbPer;
      gp_Pnt2d P2d = F.Value(Umid);
      du           = u - P2d.X();
      du           = (du < 0) ? (du - Precision::PConfusion()) : (du + Precision::PConfusion());
      modf(du / M_PI, &aNbPer);
      number = (int)aNbPer;
      du     = number * M_PI;
    }

    if (!myBSpline.IsNull())
    {
      if (du != 0. || dv != 0.)
        myBSpline->Translate(gp_Vec2d(du, dv));
      if (ToMirror)
      {
        gp_Ax2d Axe(gp_Pnt2d(0., 0.), gp_Dir2d(gp_Dir2d::D::X));
        myBSpline->Mirror(Axe);
      }
    }
  }
}

//=================================================================================================

void ProjLib_ComputeApprox::SetTolerance(const double theTolerance)
{
  myTolerance = theTolerance;
}

//=================================================================================================

void ProjLib_ComputeApprox::SetDegree(const int theDegMin, const int theDegMax)
{
  myDegMin = theDegMin;
  myDegMax = theDegMax;
}

//=================================================================================================

void ProjLib_ComputeApprox::SetMaxSegments(const int theMaxSegments)
{
  myMaxSegments = theMaxSegments;
}

//=================================================================================================

void ProjLib_ComputeApprox::SetBndPnt(const AppParCurves_Constraint theBndPnt)
{
  myBndPnt = theBndPnt;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> ProjLib_ComputeApprox::BSpline() const

{
  return myBSpline;
}

//=================================================================================================

occ::handle<Geom2d_BezierCurve> ProjLib_ComputeApprox::Bezier() const

{
  return myBezier;
}

//=================================================================================================

double ProjLib_ComputeApprox::Tolerance() const
{
  return myTolerance;
}
