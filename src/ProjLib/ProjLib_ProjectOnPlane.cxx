// Created on: 1994-09-05
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

// 09-Aug-95 : xab : changed the ProjLib_ProjectOnPlane in the case
//                   of the line and the parameteriation is kept
#include <ProjLib_ProjectOnPlane.hxx>
#include <Approx_FitAndDivide.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Precision.hxx>
#include <BSplCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <ElCLib.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Line.hxx>
#include <GeomConvert.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Ellipse.hxx>
#include <GeomLib_Tool.hxx>
#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <gce_MakeParab.hxx>
#include <gce_MakeDir.hxx>
#include <LProp3d_CLProps.hxx>
#include <math_Function.hxx>
#include <math_BrentMinimum.hxx>

const Standard_Real aParabolaLimit  = 20000.;
const Standard_Real aHyperbolaLimit = 10.;

//=======================================================================
// function : OnPlane_Value
// purpose  : Evaluate current point of the projected curve
//=======================================================================

static gp_Pnt OnPlane_Value(const Standard_Real            U,
                            const Handle(Adaptor3d_Curve)& aCurvePtr,
                            const gp_Ax3&                  Pl,
                            const gp_Dir&                  D)
{
  //                   PO . Z                /  Z = Pl.Direction()
  // Proj(u) = P(u) + -------  * D     avec  \  O = Pl.Location()
  //                   D  . Z

  gp_Pnt Point = aCurvePtr->Value(U);

  gp_Vec PO(Point, Pl.Location());

  Standard_Real Alpha = PO * gp_Vec(Pl.Direction());
  Alpha /= D * Pl.Direction();
  Point.SetXYZ(Point.XYZ() + Alpha * D.XYZ());

  return Point;
}

//=======================================================================
// function : OnPlane_DN
// purpose  : Evaluate current point of the projected curve
//=======================================================================

static gp_Vec OnPlane_DN(const Standard_Real            U,
                         const Standard_Integer         DerivativeRequest,
                         const Handle(Adaptor3d_Curve)& aCurvePtr,
                         const gp_Ax3&                  Pl,
                         const gp_Dir&                  D)
{
  //                   PO . Z                /  Z = Pl.Direction()
  // Proj(u) = P(u) + -------  * D     avec  \  O = Pl.Location()
  //                   D  . Z

  gp_Vec Vector = aCurvePtr->DN(U, DerivativeRequest);

  gp_Dir Z = Pl.Direction();

  Standard_Real Alpha = Vector * gp_Vec(Z);
  Alpha /= D * Z;

  Vector.SetXYZ(Vector.XYZ() - Alpha * D.XYZ());
  return Vector;
}

//=================================================================================================

static Standard_Boolean OnPlane_D1(const Standard_Real            U,
                                   gp_Pnt&                        P,
                                   gp_Vec&                        V,
                                   const Handle(Adaptor3d_Curve)& aCurvePtr,
                                   const gp_Ax3&                  Pl,
                                   const gp_Dir&                  D)
{
  Standard_Real Alpha;
  gp_Pnt        Point;
  gp_Vec        Vector;

  gp_Dir Z = Pl.Direction();

  aCurvePtr->D1(U, Point, Vector);

  // evaluate the point as in `OnPlane_Value`
  gp_Vec PO(Point, Pl.Location());
  Alpha = PO * gp_Vec(Z);
  Alpha /= D * Z;
  P.SetXYZ(Point.XYZ() + Alpha * D.XYZ());

  // evaluate the derivative.
  //
  //   d(Proj)  d(P)       1        d(P)
  //   ------ = ---  - -------- * ( --- . Z ) * D
  //     dU     dU     ( D . Z)     dU
  //

  Alpha = Vector * gp_Vec(Z);
  Alpha /= D * Z;

  V.SetXYZ(Vector.XYZ() - Alpha * D.XYZ());

  return Standard_True;
}

//=================================================================================================

static Standard_Boolean OnPlane_D2(const Standard_Real            U,
                                   gp_Pnt&                        P,
                                   gp_Vec&                        V1,
                                   gp_Vec&                        V2,
                                   const Handle(Adaptor3d_Curve)& aCurvePtr,
                                   const gp_Ax3&                  Pl,
                                   const gp_Dir&                  D)
{
  Standard_Real Alpha;
  gp_Pnt        Point;
  gp_Vec        Vector1, Vector2;

  gp_Dir Z = Pl.Direction();

  aCurvePtr->D2(U, Point, Vector1, Vector2);

  // evaluate the point as in `OnPlane_Value`
  gp_Vec PO(Point, Pl.Location());
  Alpha = PO * gp_Vec(Z);
  Alpha /= D * Z;
  P.SetXYZ(Point.XYZ() + Alpha * D.XYZ());

  // evaluate the derivative.
  //
  //   d(Proj)  d(P)       1        d(P)
  //   ------ = ---  - -------- * ( --- . Z ) * D
  //     dU     dU     ( D . Z)     dU
  //

  Alpha = Vector1 * gp_Vec(Z);
  Alpha /= D * Z;

  V1.SetXYZ(Vector1.XYZ() - Alpha * D.XYZ());

  Alpha = Vector2 * gp_Vec(Z);
  Alpha /= D * Z;

  V2.SetXYZ(Vector2.XYZ() - Alpha * D.XYZ());
  return Standard_True;
}

//=================================================================================================

static Standard_Boolean OnPlane_D3(const Standard_Real            U,
                                   gp_Pnt&                        P,
                                   gp_Vec&                        V1,
                                   gp_Vec&                        V2,
                                   gp_Vec&                        V3,
                                   const Handle(Adaptor3d_Curve)& aCurvePtr,
                                   const gp_Ax3&                  Pl,
                                   const gp_Dir&                  D)
{
  Standard_Real Alpha;
  gp_Pnt        Point;
  gp_Vec        Vector1, Vector2, Vector3;

  gp_Dir Z = Pl.Direction();

  aCurvePtr->D3(U, Point, Vector1, Vector2, Vector3);

  // evaluate the point as in `OnPlane_Value`
  gp_Vec PO(Point, Pl.Location());
  Alpha = PO * gp_Vec(Z);
  Alpha /= D * Z;
  P.SetXYZ(Point.XYZ() + Alpha * D.XYZ());

  // evaluate the derivative.
  //
  //   d(Proj)  d(P)       1        d(P)
  //   ------ = ---  - -------- * ( --- . Z ) * D
  //     dU     dU     ( D . Z)     dU
  //

  Alpha = Vector1 * gp_Vec(Z);
  Alpha /= D * Z;

  V1.SetXYZ(Vector1.XYZ() - Alpha * D.XYZ());

  Alpha = Vector2 * gp_Vec(Z);
  Alpha /= D * Z;

  V2.SetXYZ(Vector2.XYZ() - Alpha * D.XYZ());
  Alpha = Vector3 * gp_Vec(Z);
  Alpha /= D * Z;

  V3.SetXYZ(Vector3.XYZ() - Alpha * D.XYZ());
  return Standard_True;
}

//=======================================================================
//  class  : ProjLib_OnPlane
// purpose  : Use to approximate the projection on a plane
//=======================================================================

class ProjLib_OnPlane : public AppCont_Function

{
  Handle(Adaptor3d_Curve) myCurve;
  gp_Ax3                  myPlane;
  gp_Dir                  myDirection;

public:
  ProjLib_OnPlane(const Handle(Adaptor3d_Curve)& C, const gp_Ax3& Pl, const gp_Dir& D)
      : myCurve(C),
        myPlane(Pl),
        myDirection(D)
  {
    myNbPnt   = 1;
    myNbPnt2d = 0;
  }

  Standard_Real FirstParameter() const { return myCurve->FirstParameter(); }

  Standard_Real LastParameter() const { return myCurve->LastParameter(); }

  Standard_Boolean Value(const Standard_Real theT,
                         NCollection_Array1<gp_Pnt2d>& /*thePnt2d*/,
                         NCollection_Array1<gp_Pnt>& thePnt) const
  {
    thePnt(1) = OnPlane_Value(theT, myCurve, myPlane, myDirection);
    return Standard_True;
  }

  Standard_Boolean D1(const Standard_Real theT,
                      NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
                      NCollection_Array1<gp_Vec>& theVec) const
  {
    gp_Pnt aDummyPnt;
    return OnPlane_D1(theT, aDummyPnt, theVec(1), myCurve, myPlane, myDirection);
  }
};

//=======================================================================
//  class  : ProjLib_MaxCurvature
// purpose  : Use to search apex of parabola or hyperbola, which is its projection
//           on a plane. Apex is point with maximal curvature
//=======================================================================

class ProjLib_MaxCurvature : public math_Function

{

public:
  ProjLib_MaxCurvature(LProp3d_CLProps& theProps)
      : myProps(&theProps)
  {
  }

  virtual Standard_Boolean Value(const Standard_Real X, Standard_Real& F)
  {
    myProps->SetParameter(X);
    F = -myProps->Curvature();
    return Standard_True;
  }

private:
  LProp3d_CLProps* myProps;
};

//=====================================================================//
//                                                                     //
//  D E S C R I P T I O N   O F   T H E   C L A S S  :                 //
//                                                                     //
//         P r o j L i b _ A p p r o x P r o j e c t O n P l a n e     //
//                                                                     //
//=====================================================================//

//=================================================================================================

static void PerformApprox(const Handle(Adaptor3d_Curve)& C,
                          const gp_Ax3&                  Pl,
                          const gp_Dir&                  D,
                          Handle(Geom_BSplineCurve)&     BSplineCurvePtr)

{
  ProjLib_OnPlane F(C, Pl, D);

  Standard_Integer Deg1, Deg2;
  Deg1 = 8;
  Deg2 = 8;
  if (C->GetType() == GeomAbs_Parabola)
  {
    Deg1 = 2;
    Deg2 = 2;
  }
  Standard_Integer aNbSegm = 100;
  if (C->GetType() == GeomAbs_Hyperbola)
  {
    Deg1    = 14;
    Deg2    = 14;
    aNbSegm = 1000;
  }
  Approx_FitAndDivide Fit(Deg1,
                          Deg2,
                          Precision::Approximation(),
                          Precision::PApproximation(),
                          Standard_True);
  Fit.SetMaxSegments(aNbSegm);
  Fit.Perform(F);
  if (!Fit.IsAllApproximated())
  {
    return;
  }
  Standard_Integer i;
  Standard_Integer NbCurves = Fit.NbMultiCurves();
  Standard_Integer MaxDeg   = 0;

  // Pour transformer la MultiCurve en BSpline, il faut que toutes
  // les Bezier la constituant aient le meme degre -> Calcul de MaxDeg
  Standard_Integer NbPoles = 1;
  for (i = 1; i <= NbCurves; i++)
  {
    Standard_Integer Deg = Fit.Value(i).Degree();
    MaxDeg               = Max(MaxDeg, Deg);
  }
  NbPoles = MaxDeg * NbCurves + 1; // Poles sur la BSpline

  TColgp_Array1OfPnt Poles(1, NbPoles);

  TColgp_Array1OfPnt TempPoles(1, MaxDeg + 1); // pour augmentation du degre

  TColStd_Array1OfReal Knots(1, NbCurves + 1); // Noeuds de la BSpline

  Standard_Integer Compt    = 1;
  Standard_Real    anErrMax = 0., anErr3d, anErr2d;
  for (i = 1; i <= Fit.NbMultiCurves(); i++)
  {
    Fit.Parameters(i, Knots(i), Knots(i + 1));
    Fit.Error(i, anErr3d, anErr2d);
    anErrMax                   = Max(anErrMax, anErr3d);
    AppParCurves_MultiCurve MC = Fit.Value(i);              // Charge la Ieme Curve
    TColgp_Array1OfPnt      LocalPoles(1, MC.Degree() + 1); // Recupere les poles
    MC.Curve(1, LocalPoles);

    // Augmentation eventuelle du degre
    if (MaxDeg > MC.Degree())
    {
      BSplCLib::IncreaseDegree(MaxDeg,
                               LocalPoles,
                               BSplCLib::NoWeights(),
                               TempPoles,
                               BSplCLib::NoWeights());
      // mise a jour des poles de la PCurve
      for (Standard_Integer j = 1; j <= MaxDeg + 1; j++)
      {
        Poles.SetValue(Compt, TempPoles(j));
        Compt++;
      }
    }
    else
    {
      // mise a jour des poles de la PCurve
      for (Standard_Integer j = 1; j <= MaxDeg + 1; j++)
      {
        Poles.SetValue(Compt, LocalPoles(j));
        Compt++;
      }
    }

    Compt--;
  }

  // mise a jour des fields de ProjLib_Approx

  Standard_Integer NbKnots = NbCurves + 1;

  TColStd_Array1OfInteger Mults(1, NbKnots);
  Mults.SetValue(1, MaxDeg + 1);
  for (i = 2; i <= NbCurves; i++)
  {
    Mults.SetValue(i, MaxDeg);
  }
  Mults.SetValue(NbKnots, MaxDeg + 1);
  BSplineCurvePtr = new Geom_BSplineCurve(Poles, Knots, Mults, MaxDeg, Standard_False);

  // Try to smooth
  Standard_Integer m1 = MaxDeg - 1;
  for (i = 2; i < NbKnots; ++i)
  {
    if (BSplineCurvePtr->Multiplicity(i) == MaxDeg)
    {
      BSplineCurvePtr->RemoveKnot(i, m1, anErrMax);
    }
  }
}

//=================================================================================================

ProjLib_ProjectOnPlane::ProjLib_ProjectOnPlane()
    : myKeepParam(Standard_False),
      myFirstPar(0.),
      myLastPar(0.),
      myTolerance(0.),
      myType(GeomAbs_OtherCurve),
      myIsApprox(Standard_False)
{
}

//=================================================================================================

ProjLib_ProjectOnPlane::ProjLib_ProjectOnPlane(const gp_Ax3& Pl)
    : myPlane(Pl),
      myDirection(Pl.Direction()),
      myKeepParam(Standard_False),
      myFirstPar(0.),
      myLastPar(0.),
      myTolerance(0.),
      myType(GeomAbs_OtherCurve),
      myIsApprox(Standard_False)
{
}

//=================================================================================================

ProjLib_ProjectOnPlane::ProjLib_ProjectOnPlane(const gp_Ax3& Pl, const gp_Dir& D)
    : myPlane(Pl),
      myDirection(D),
      myKeepParam(Standard_False),
      myFirstPar(0.),
      myLastPar(0.),
      myTolerance(0.),
      myType(GeomAbs_OtherCurve),
      myIsApprox(Standard_False)
{
  //  if ( Abs(D * Pl.Direction()) < Precision::Confusion()) {
  //    throw Standard_ConstructionError
  //      ("ProjLib_ProjectOnPlane:  The Direction and the Plane are parallel");
  //  }
}

//=================================================================================================

Handle(Adaptor3d_Curve) ProjLib_ProjectOnPlane::ShallowCopy() const
{
  Handle(ProjLib_ProjectOnPlane) aCopy = new ProjLib_ProjectOnPlane();

  if (!myCurve.IsNull())
  {
    aCopy->myCurve = myCurve->ShallowCopy();
  }
  aCopy->myPlane     = myPlane;
  aCopy->myDirection = myDirection;
  aCopy->myKeepParam = myKeepParam;
  aCopy->myFirstPar  = myFirstPar;
  aCopy->myLastPar   = myLastPar;
  aCopy->myTolerance = myTolerance;
  aCopy->myType      = myType;
  if (!myResult.IsNull())
  {
    aCopy->myResult = Handle(GeomAdaptor_Curve)::DownCast(myResult->ShallowCopy());
  }
  aCopy->myIsApprox = myIsApprox;

  return aCopy;
}

//=======================================================================
// function : Project
// purpose  : Returns the projection of a point <Point> on a plane
//           <ThePlane>  along a direction <TheDir>.
//=======================================================================

static gp_Pnt ProjectPnt(const gp_Ax3& ThePlane, const gp_Dir& TheDir, const gp_Pnt& Point)
{
  gp_Vec PO(Point, ThePlane.Location());

  Standard_Real Alpha = PO * gp_Vec(ThePlane.Direction());
  Alpha /= TheDir * ThePlane.Direction();

  gp_Pnt P;
  P.SetXYZ(Point.XYZ() + Alpha * TheDir.XYZ());

  return P;
}

//=======================================================================
// function : Project
// purpose  : Returns the projection of a Vector <Vec> on a plane
//           <ThePlane> along a direction <TheDir>.
//=======================================================================

static gp_Vec ProjectVec(const gp_Ax3& ThePlane, const gp_Dir& TheDir, const gp_Vec& Vec)
{
  gp_Vec D = Vec;
  gp_Vec Z = ThePlane.Direction();

  D -= ((Vec * Z) / (TheDir * Z)) * TheDir;

  return D;
}

//=================================================================================================

void ProjLib_ProjectOnPlane::Load(const Handle(Adaptor3d_Curve)& C,
                                  const Standard_Real            Tolerance,
                                  const Standard_Boolean         KeepParametrization)

{
  myCurve     = C;
  myType      = GeomAbs_OtherCurve;
  myIsApprox  = Standard_False;
  myTolerance = Tolerance;

  Handle(Geom_BSplineCurve) ApproxCurve;
  Handle(GeomAdaptor_Curve) aGAHCurve;

  Handle(Geom_Line)      GeomLinePtr;
  Handle(Geom_Circle)    GeomCirclePtr;
  Handle(Geom_Ellipse)   GeomEllipsePtr;
  Handle(Geom_Hyperbola) GeomHyperbolaPtr;
  Handle(Geom_Parabola)  GeomParabolaPtr;

  gp_Lin   aLine;
  gp_Elips Elips;
  //  gp_Hypr  Hypr ;

  Standard_Integer  num_knots;
  GeomAbs_CurveType Type = C->GetType();

  gp_Ax2        Axis;
  Standard_Real R1 = 0., R2 = 0.;

  myKeepParam = KeepParametrization;

  switch (Type)
  {
    case GeomAbs_Line: {
      //     P(u) = O + u * Xc
      // ==> Q(u) = f(P(u))
      //          = f(O) + u * f(Xc)

      gp_Lin L  = myCurve->Line();
      gp_Vec Xc = ProjectVec(myPlane, myDirection, gp_Vec(L.Direction()));

      if (Xc.Magnitude() < Precision::Confusion())
      { // line orthog au plan
        myType                    = GeomAbs_BSplineCurve;
        gp_Pnt                  P = ProjectPnt(myPlane, myDirection, L.Location());
        TColStd_Array1OfInteger Mults(1, 2);
        Mults.Init(2);
        TColgp_Array1OfPnt Poles(1, 2);
        Poles.Init(P);
        TColStd_Array1OfReal Knots(1, 2);
        Knots(1)                      = myCurve->FirstParameter();
        Knots(2)                      = myCurve->LastParameter();
        Handle(Geom_BSplineCurve) BSP = new Geom_BSplineCurve(Poles, Knots, Mults, 1);

        //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
        GeomAdaptor_Curve aGACurve(BSP);
        myResult = new GeomAdaptor_Curve(aGACurve);
        //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
      else if (Abs(Xc.Magnitude() - 1.) < Precision::Confusion())
      {
        myType      = GeomAbs_Line;
        gp_Pnt P    = ProjectPnt(myPlane, myDirection, L.Location());
        myFirstPar  = myCurve->FirstParameter();
        myLastPar   = myCurve->LastParameter();
        aLine       = gp_Lin(P, gp_Dir(Xc));
        GeomLinePtr = new Geom_Line(aLine);

        //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
        GeomAdaptor_Curve aGACurve(GeomLinePtr,
                                   myCurve->FirstParameter(),
                                   myCurve->LastParameter());
        myResult = new GeomAdaptor_Curve(aGACurve);
        //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
      else
      {
        myType   = GeomAbs_Line;
        gp_Pnt P = ProjectPnt(myPlane, myDirection, L.Location());
        aLine    = gp_Lin(P, gp_Dir(Xc));
        Standard_Real Udeb, Ufin;

        // eval the first and last parameters of the projected curve
        Udeb        = myCurve->FirstParameter();
        Ufin        = myCurve->LastParameter();
        gp_Pnt P1   = ProjectPnt(myPlane, myDirection, myCurve->Value(Udeb));
        gp_Pnt P2   = ProjectPnt(myPlane, myDirection, myCurve->Value(Ufin));
        myFirstPar  = gp_Vec(aLine.Direction()).Dot(gp_Vec(P, P1));
        myLastPar   = gp_Vec(aLine.Direction()).Dot(gp_Vec(P, P2));
        GeomLinePtr = new Geom_Line(aLine);
        if (!myKeepParam)
        {
          //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
          GeomAdaptor_Curve aGACurve(GeomLinePtr, myFirstPar, myLastPar);
          myResult = new GeomAdaptor_Curve(aGACurve);
          //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
        }
        else
        {
          myType = GeomAbs_BSplineCurve;
          //
          // make a linear BSpline of degree 1 between the end points of
          // the projected line
          //
          Handle(Geom_TrimmedCurve) NewTrimCurvePtr =
            new Geom_TrimmedCurve(GeomLinePtr, myFirstPar, myLastPar);

          Handle(Geom_BSplineCurve) NewCurvePtr = GeomConvert::CurveToBSplineCurve(NewTrimCurvePtr);
          num_knots                             = NewCurvePtr->NbKnots();
          TColStd_Array1OfReal BsplineKnots(1, num_knots);
          NewCurvePtr->Knots(BsplineKnots);

          BSplCLib::Reparametrize(myCurve->FirstParameter(),
                                  myCurve->LastParameter(),
                                  BsplineKnots);

          NewCurvePtr->SetKnots(BsplineKnots);
          //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
          GeomAdaptor_Curve aGACurve(NewCurvePtr);
          myResult = new GeomAdaptor_Curve(aGACurve);
          //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
        }
      }
      break;
    }
    case GeomAbs_Circle: {
      // Pour le cercle et l ellipse on a les relations suivantes:
      // ( Rem : pour le cercle R1 = R2 = R)
      //     P(u) = O + R1 * Cos(u) * Xc + R2 * Sin(u) * Yc
      // ==> Q(u) = f(P(u))
      //          = f(O) + R1 * Cos(u) * f(Xc) + R2 * Sin(u) * f(Yc)

      gp_Circ Circ = myCurve->Circle();
      Axis         = Circ.Position();
      R1 = R2 = Circ.Radius();
    }
      Standard_FALLTHROUGH
    case GeomAbs_Ellipse: {
      if (Type == GeomAbs_Ellipse)
      {
        gp_Elips E = myCurve->Ellipse();
        Axis       = E.Position();
        R1         = E.MajorRadius();
        R2         = E.MinorRadius();
      }

      // Common Code  for CIRCLE & ELLIPSE begin here
      gp_Dir X   = Axis.XDirection();
      gp_Dir Y   = Axis.YDirection();
      gp_Vec VDx = ProjectVec(myPlane, myDirection, X);
      gp_Vec VDy = ProjectVec(myPlane, myDirection, Y);
      gp_Dir Dx, Dy;

      Standard_Real Tol2 = myTolerance * myTolerance;
      if (VDx.SquareMagnitude() < Tol2 || VDy.SquareMagnitude() < Tol2
          || VDx.CrossSquareMagnitude(VDy) < Tol2)
      {
        myIsApprox = Standard_True;
      }

      if (!myIsApprox)
      {
        Dx                  = gp_Dir(VDx);
        Dy                  = gp_Dir(VDy);
        gp_Pnt        O     = Axis.Location();
        gp_Pnt        P     = ProjectPnt(myPlane, myDirection, O);
        gp_Pnt        Px    = ProjectPnt(myPlane, myDirection, O.Translated(R1 * gp_Vec(X)));
        gp_Pnt        Py    = ProjectPnt(myPlane, myDirection, O.Translated(R2 * gp_Vec(Y)));
        Standard_Real Major = P.Distance(Px);
        Standard_Real Minor = P.Distance(Py);

        if (myKeepParam)
        {
          myIsApprox = !gp_Dir(VDx).IsNormal(gp_Dir(VDy), Precision::Angular());
        }
        else
        {
          // Since it is not necessary to keep the same parameter for the point on the original and
          // on the projected curves, we will use the following approach to find axes of the
          // projected ellipse and provide the canonical curve:
          // https://www.geometrictools.com/Documentation/ParallelProjectionEllipse.pdf
          math_Matrix aMatrA(1, 2, 1, 2);
          // A = Jp^T * Pr(Je), where
          //   Pr(Je) - projection of axes of original ellipse to the target plane
          //   Jp - X and Y axes of the target plane
          aMatrA(1, 1) = myPlane.XDirection().XYZ().Dot(VDx.XYZ());
          aMatrA(1, 2) = myPlane.XDirection().XYZ().Dot(VDy.XYZ());
          aMatrA(2, 1) = myPlane.YDirection().XYZ().Dot(VDx.XYZ());
          aMatrA(2, 2) = myPlane.YDirection().XYZ().Dot(VDy.XYZ());

          math_Matrix aMatrDelta2(1, 2, 1, 2, 0.0);
          //           | 1/MajorRad^2       0       |
          // Delta^2 = |                            |
          //           |      0        1/MajorRad^2 |
          aMatrDelta2(1, 1) = 1.0 / (R1 * R1);
          aMatrDelta2(2, 2) = 1.0 / (R2 * R2);

          math_Matrix aMatrAInv = aMatrA.Inverse();
          math_Matrix aMatrM    = aMatrAInv.Transposed() * aMatrDelta2 * aMatrAInv;

          // perform eigenvalues calculation
          math_Jacobi anEigenCalc(aMatrM);
          if (anEigenCalc.IsDone())
          {
            // radii of the projected ellipse
            Minor = 1.0 / Sqrt(anEigenCalc.Value(1));
            Major = 1.0 / Sqrt(anEigenCalc.Value(2));

            // calculate the rotation angle for the plane axes to meet the correct axes of the
            // projected ellipse (swap eigenvectors in respect to major and minor axes)
            const math_Matrix& anEigenVec = anEigenCalc.Vectors();
            gp_Trsf2d          aTrsfInPlane;
            aTrsfInPlane.SetValues(anEigenVec(1, 2),
                                   anEigenVec(1, 1),
                                   0.0,
                                   anEigenVec(2, 2),
                                   anEigenVec(2, 1),
                                   0.0);
            gp_Trsf aRot;
            aRot.SetRotation(gp_Ax1(P, myPlane.Direction()), aTrsfInPlane.RotationPart());

            Dx = myPlane.XDirection().Transformed(aRot);
            Dy = myPlane.YDirection().Transformed(aRot);
          }
          else
          {
            myIsApprox = Standard_True;
          }
        }

        if (!myIsApprox)
        {
          gp_Ax2 Axe(P, Dx ^ Dy, Dx);

          if (Abs(Major - Minor) < Precision::Confusion())
          {
            myType = GeomAbs_Circle;
            gp_Circ Circ(Axe, Major);
            GeomCirclePtr = new Geom_Circle(Circ);
            //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
            GeomAdaptor_Curve aGACurve(GeomCirclePtr);
            myResult = new GeomAdaptor_Curve(aGACurve);
            //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
          }
          else if (Major > Minor)
          {
            myType = GeomAbs_Ellipse;
            Elips  = gp_Elips(Axe, Major, Minor);

            GeomEllipsePtr = new Geom_Ellipse(Elips);
            //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
            GeomAdaptor_Curve aGACurve(GeomEllipsePtr);
            myResult = new GeomAdaptor_Curve(aGACurve);
            //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
          }
          else
          {
            myIsApprox = Standard_True;
          }
        }
      }

      // No way to build the canonical curve, approximate as B-spline
      if (myIsApprox)
      {
        myType = GeomAbs_BSplineCurve;
        PerformApprox(myCurve, myPlane, myDirection, ApproxCurve);
        //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
        GeomAdaptor_Curve aGACurve(ApproxCurve);
        myResult = new GeomAdaptor_Curve(aGACurve);
        //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
      }
      else if (GeomCirclePtr || GeomEllipsePtr)
      {
        Handle(Geom_Curve) aResultCurve = GeomCirclePtr;
        if (aResultCurve.IsNull())
          aResultCurve = GeomEllipsePtr;
        // start and end parameters of the projected curve
        Standard_Real aParFirst = myCurve->FirstParameter();
        Standard_Real aParLast  = myCurve->LastParameter();
        gp_Pnt        aPntFirst = ProjectPnt(myPlane, myDirection, myCurve->Value(aParFirst));
        gp_Pnt        aPntLast  = ProjectPnt(myPlane, myDirection, myCurve->Value(aParLast));
        GeomLib_Tool::Parameter(aResultCurve, aPntFirst, Precision::Confusion(), myFirstPar);
        GeomLib_Tool::Parameter(aResultCurve, aPntLast, Precision::Confusion(), myLastPar);
        while (myLastPar <= myFirstPar)
          myLastPar += myResult->Period();
      }
    }
    break;
    case GeomAbs_Parabola: {
      //     P(u) = O + (u*u)/(4*f) * Xc + u * Yc
      // ==> Q(u) = f(P(u))
      //          = f(O) + (u*u)/(4*f) * f(Xc) + u * f(Yc)

      gp_Parab Parab  = myCurve->Parabola();
      gp_Ax2   AxeRef = Parab.Position();
      gp_Vec   Xc     = ProjectVec(myPlane, myDirection, gp_Vec(AxeRef.XDirection()));
      gp_Vec   Yc     = ProjectVec(myPlane, myDirection, gp_Vec(AxeRef.YDirection()));
      gp_Pnt   P      = ProjectPnt(myPlane, myDirection, AxeRef.Location());

      myIsApprox = Standard_False;

      if ((Abs(Yc.Magnitude() - 1.) < Precision::Confusion())
          && (Xc.Magnitude() < Precision::Confusion()))
      {
        myType      = GeomAbs_Line;
        aLine       = gp_Lin(P, gp_Dir(Yc));
        GeomLinePtr = new Geom_Line(aLine);
      }
      else if (Xc.IsNormal(Yc, Precision::Angular()))
      {
        myType                   = GeomAbs_Parabola;
        Standard_Real F          = Parab.Focal() / Xc.Magnitude();
        gp_Parab      aProjParab = gp_Parab(gp_Ax2(P, Xc ^ Yc, Xc), F);
        GeomParabolaPtr          = new Geom_Parabola(aProjParab);
      }
      else if (Yc.Magnitude() < Precision::Confusion() || Yc.IsParallel(Xc, Precision::Angular()))
      {
        myIsApprox = Standard_True;
      }
      else if (!myKeepParam)
      {
        // Try building parabola with help of apex position
        myIsApprox = !BuildParabolaByApex(GeomParabolaPtr);
      }
      else
      {
        myIsApprox = Standard_True;
      }

      if (!myIsApprox)
      {
        GetTrimmedResult(GeomParabolaPtr);
      }
      else
      {
        BuildByApprox(aParabolaLimit);
      }
    }
    break;
    case GeomAbs_Hyperbola: {
      //     P(u) = O + R1 * Cosh(u) * Xc + R2 * Sinh(u) * Yc
      // ==> Q(u) = f(P(u))
      //          = f(O) + R1 * Cosh(u) * f(Xc) + R2 * Sinh(u) * f(Yc)

      gp_Hypr       Hypr   = myCurve->Hyperbola();
      gp_Ax2        AxeRef = Hypr.Position();
      gp_Vec        Xc     = ProjectVec(myPlane, myDirection, gp_Vec(AxeRef.XDirection()));
      gp_Vec        Yc     = ProjectVec(myPlane, myDirection, gp_Vec(AxeRef.YDirection()));
      gp_Pnt        P      = ProjectPnt(myPlane, myDirection, AxeRef.Location());
      Standard_Real aR1    = Hypr.MajorRadius();
      Standard_Real aR2    = Hypr.MinorRadius();
      gp_Dir        Z      = myPlane.Direction();
      myIsApprox           = Standard_False;

      if (Xc.Magnitude() < Precision::Confusion())
      {
        myType           = GeomAbs_Hyperbola;
        gp_Dir X         = gp_Dir(Yc) ^ Z;
        Hypr             = gp_Hypr(gp_Ax2(P, Z, X), 0., aR2 * Yc.Magnitude());
        GeomHyperbolaPtr = new Geom_Hyperbola(Hypr);
      }
      else if (Yc.Magnitude() < Precision::Confusion())
      {
        myType           = GeomAbs_Hyperbola;
        Hypr             = gp_Hypr(gp_Ax2(P, Z, gp_Dir(Xc)), aR1 * Xc.Magnitude(), 0.);
        GeomHyperbolaPtr = new Geom_Hyperbola(Hypr);
      }
      else if (Xc.IsNormal(Yc, Precision::Angular()))
      {
        myType           = GeomAbs_Hyperbola;
        Hypr             = gp_Hypr(gp_Ax2(P, gp_Dir(Xc ^ Yc), gp_Dir(Xc)),
                       aR1 * Xc.Magnitude(),
                       aR2 * Yc.Magnitude());
        GeomHyperbolaPtr = new Geom_Hyperbola(Hypr);
      }
      else if (Yc.Magnitude() < Precision::Confusion() || Yc.IsParallel(Xc, Precision::Angular()))
      {
        myIsApprox = Standard_True;
      }
      else if (!myKeepParam)
      {
        myIsApprox = !BuildHyperbolaByApex(GeomHyperbolaPtr);
      }
      else
      {
        myIsApprox = Standard_True;
      }
      if (!myIsApprox)
      {
        GetTrimmedResult(GeomHyperbolaPtr);
      }
      else
      {
        BuildByApprox(aHyperbolaLimit);
      }
    }
    break;
    case GeomAbs_BezierCurve: {
      Handle(Geom_BezierCurve) BezierCurvePtr = myCurve->Bezier();
      Standard_Integer         NbPoles        = BezierCurvePtr->NbPoles();

      Handle(Geom_BezierCurve) ProjCu = Handle(Geom_BezierCurve)::DownCast(BezierCurvePtr->Copy());

      myKeepParam = Standard_True;
      myIsApprox  = Standard_False;
      myType      = Type;
      for (Standard_Integer i = 1; i <= NbPoles; i++)
      {
        ProjCu->SetPole(i, ProjectPnt(myPlane, myDirection, BezierCurvePtr->Pole(i)));
      }

      //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
      GeomAdaptor_Curve aGACurve(ProjCu);
      myResult = new GeomAdaptor_Curve(aGACurve);
      //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
    }
    break;
    case GeomAbs_BSplineCurve: {
      Handle(Geom_BSplineCurve) BSplineCurvePtr = myCurve->BSpline();
      //
      //    make a copy of the curve and projects its poles
      //
      Handle(Geom_BSplineCurve) ProjectedBSplinePtr =
        Handle(Geom_BSplineCurve)::DownCast(BSplineCurvePtr->Copy());

      myKeepParam = Standard_True;
      myIsApprox  = Standard_False;
      myType      = Type;
      for (Standard_Integer i = 1; i <= BSplineCurvePtr->NbPoles(); i++)
      {
        ProjectedBSplinePtr->SetPole(i, ProjectPnt(myPlane, myDirection, BSplineCurvePtr->Pole(i)));
      }

      //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
      GeomAdaptor_Curve aGACurve(ProjectedBSplinePtr);
      myResult = new GeomAdaptor_Curve(aGACurve);
      //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
    }
    break;
    default: {
      myKeepParam = Standard_True;
      myIsApprox  = Standard_True;
      myType      = GeomAbs_BSplineCurve;
      PerformApprox(myCurve, myPlane, myDirection, ApproxCurve);
      //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:29 2002 Begin
      GeomAdaptor_Curve aGACurve(ApproxCurve);
      myResult = new GeomAdaptor_Curve(aGACurve);
      //  Modified by Sergey KHROMOV - Tue Jan 29 16:57:30 2002 End
    }
    break;
  }
}

//=================================================================================================

const gp_Ax3& ProjLib_ProjectOnPlane::GetPlane() const
{
  return myPlane;
}

//=================================================================================================

const gp_Dir& ProjLib_ProjectOnPlane::GetDirection() const
{
  return myDirection;
}

//=================================================================================================

const Handle(Adaptor3d_Curve)& ProjLib_ProjectOnPlane::GetCurve() const
{
  return myCurve;
}

//=================================================================================================

const Handle(GeomAdaptor_Curve)& ProjLib_ProjectOnPlane::GetResult() const
{
  return myResult;
}

//=================================================================================================

Standard_Real ProjLib_ProjectOnPlane::FirstParameter() const
{
  if (myKeepParam || myIsApprox)
    return myCurve->FirstParameter();
  else
    return myFirstPar;
}

//=================================================================================================

Standard_Real ProjLib_ProjectOnPlane::LastParameter() const
{
  if (myKeepParam || myIsApprox)
    return myCurve->LastParameter();
  else
    return myLastPar;
}

//=================================================================================================

GeomAbs_Shape ProjLib_ProjectOnPlane::Continuity() const
{
  return myCurve->Continuity();
}

//=================================================================================================

Standard_Integer ProjLib_ProjectOnPlane::NbIntervals(const GeomAbs_Shape S) const
{
  return myCurve->NbIntervals(S);
}

//=================================================================================================

void ProjLib_ProjectOnPlane::Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  myCurve->Intervals(T, S);
}

//=================================================================================================

Handle(Adaptor3d_Curve) ProjLib_ProjectOnPlane::Trim(const Standard_Real First,
                                                     const Standard_Real Last,
                                                     const Standard_Real Tolerance) const
{
  if (myType != GeomAbs_OtherCurve)
  {
    return myResult->Trim(First, Last, Tolerance);
  }
  else
  {
    throw Standard_NotImplemented("ProjLib_ProjectOnPlane::Trim() - curve of unsupported type");
  }
}

//=================================================================================================

Standard_Boolean ProjLib_ProjectOnPlane::IsClosed() const
{
  return myCurve->IsClosed();
}

//=================================================================================================

Standard_Boolean ProjLib_ProjectOnPlane::IsPeriodic() const
{
  if (myIsApprox)
    return Standard_False;
  else
    return myCurve->IsPeriodic();
}

//=================================================================================================

Standard_Real ProjLib_ProjectOnPlane::Period() const
{
  if (!IsPeriodic())
  {
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane::Period");
  }

  if (myIsApprox)
    return Standard_False;
  else
    return myCurve->Period();
}

//=================================================================================================

gp_Pnt ProjLib_ProjectOnPlane::Value(const Standard_Real U) const
{
  if (myType != GeomAbs_OtherCurve)
  {
    return myResult->Value(U);
  }
  else
  {
    return OnPlane_Value(U, myCurve, myPlane, myDirection);
  }
}

//=================================================================================================

void ProjLib_ProjectOnPlane::D0(const Standard_Real U, gp_Pnt& P) const
{
  if (myType != GeomAbs_OtherCurve)
  {
    myResult->D0(U, P);
  }
  else
  {
    P = OnPlane_Value(U, myCurve, myPlane, myDirection);
  }
}

//=================================================================================================

void ProjLib_ProjectOnPlane::D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V) const
{
  if (myType != GeomAbs_OtherCurve)
  {
    myResult->D1(U, P, V);
  }
  else
  {
    OnPlane_D1(U, P, V, myCurve, myPlane, myDirection);
  }
}

//=================================================================================================

void ProjLib_ProjectOnPlane::D2(const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{
  if (myType != GeomAbs_OtherCurve)
  {
    myResult->D2(U, P, V1, V2);
  }
  else
  {
    OnPlane_D2(U, P, V1, V2, myCurve, myPlane, myDirection);
  }
}

//=================================================================================================

void ProjLib_ProjectOnPlane::D3(const Standard_Real U,
                                gp_Pnt&             P,
                                gp_Vec&             V1,
                                gp_Vec&             V2,
                                gp_Vec&             V3) const
{
  if (myType != GeomAbs_OtherCurve)
  {
    myResult->D3(U, P, V1, V2, V3);
  }
  else
  {
    OnPlane_D3(U, P, V1, V2, V3, myCurve, myPlane, myDirection);
  }
}

//=================================================================================================

gp_Vec ProjLib_ProjectOnPlane::DN(const Standard_Real    U,
                                  const Standard_Integer DerivativeRequest) const
{
  if (myType != GeomAbs_OtherCurve)
  {
    return myResult->DN(U, DerivativeRequest);
  }
  else
  {
    return OnPlane_DN(U, DerivativeRequest, myCurve, myPlane, myDirection);
  }
}

//=================================================================================================

Standard_Real ProjLib_ProjectOnPlane::Resolution(const Standard_Real Tolerance) const
{
  if (myType != GeomAbs_OtherCurve)
  {
    return myResult->Resolution(Tolerance);
  }
  else
  {
    return 0;
  }
}

//=================================================================================================

GeomAbs_CurveType ProjLib_ProjectOnPlane::GetType() const
{
  return myType;
}

//=================================================================================================

gp_Lin ProjLib_ProjectOnPlane::Line() const
{
  if (myType != GeomAbs_Line)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Line");

  return myResult->Line();
}

//=================================================================================================

gp_Circ ProjLib_ProjectOnPlane::Circle() const
{
  if (myType != GeomAbs_Circle)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Circle");

  return myResult->Circle();
}

//=================================================================================================

gp_Elips ProjLib_ProjectOnPlane::Ellipse() const
{
  if (myType != GeomAbs_Ellipse)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Ellipse");

  return myResult->Ellipse();
}

//=================================================================================================

gp_Hypr ProjLib_ProjectOnPlane::Hyperbola() const
{
  if (myType != GeomAbs_Hyperbola)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Hyperbola");

  return myResult->Hyperbola();
}

//=================================================================================================

gp_Parab ProjLib_ProjectOnPlane::Parabola() const
{
  if (myType != GeomAbs_Parabola)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Parabola");

  return myResult->Parabola();
}

//=================================================================================================

Standard_Integer ProjLib_ProjectOnPlane::Degree() const
{
  if ((GetType() != GeomAbs_BSplineCurve) && (GetType() != GeomAbs_BezierCurve))
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Degree");

  if (myIsApprox)
    return myResult->Degree();
  else
    return myCurve->Degree();
}

//=================================================================================================

Standard_Boolean ProjLib_ProjectOnPlane::IsRational() const
{
  if ((GetType() != GeomAbs_BSplineCurve) && (GetType() != GeomAbs_BezierCurve))
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:IsRational");

  if (myIsApprox)
    return myResult->IsRational();
  else
    return myCurve->IsRational();
}

//=================================================================================================

Standard_Integer ProjLib_ProjectOnPlane::NbPoles() const
{
  if ((GetType() != GeomAbs_BSplineCurve) && (GetType() != GeomAbs_BezierCurve))
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:NbPoles");

  if (myIsApprox)
    return myResult->NbPoles();
  else
    return myCurve->NbPoles();
}

//=================================================================================================

Standard_Integer ProjLib_ProjectOnPlane::NbKnots() const
{
  if (GetType() != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:NbKnots");

  if (myIsApprox)
    return myResult->NbKnots();
  else
    return myCurve->NbKnots();
}

//=================================================================================================

Handle(Geom_BezierCurve) ProjLib_ProjectOnPlane::Bezier() const
{
  if (myType != GeomAbs_BezierCurve)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:Bezier");

  return myResult->Bezier();
}

//=================================================================================================

Handle(Geom_BSplineCurve) ProjLib_ProjectOnPlane::BSpline() const
{
  if (myType != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("ProjLib_ProjectOnPlane:BSpline");

  return myResult->BSpline();
}

//=================================================================================================

void ProjLib_ProjectOnPlane::GetTrimmedResult(const Handle(Geom_Curve)& theProjCurve)
{
  gp_Lin   aLin;
  gp_Parab aParab;
  gp_Hypr  aHypr;
  if (myType == GeomAbs_Line)
  {
    aLin = Handle(Geom_Line)::DownCast(theProjCurve)->Lin();
  }
  else if (myType == GeomAbs_Parabola)
  {
    aParab = Handle(Geom_Parabola)::DownCast(theProjCurve)->Parab();
  }
  else if (myType == GeomAbs_Hyperbola)
  {
    aHypr = Handle(Geom_Hyperbola)::DownCast(theProjCurve)->Hypr();
  }

  myFirstPar = theProjCurve->FirstParameter();
  myLastPar  = theProjCurve->LastParameter();
  if (!Precision::IsInfinite(myCurve->FirstParameter()))
  {
    gp_Pnt aP = myCurve->Value(myCurve->FirstParameter());
    aP        = ProjectPnt(myPlane, myDirection, aP);
    if (myType == GeomAbs_Line)
    {
      myFirstPar = ElCLib::Parameter(aLin, aP);
    }
    else if (myType == GeomAbs_Parabola)
    {
      myFirstPar = ElCLib::Parameter(aParab, aP);
    }
    else if (myType == GeomAbs_Hyperbola)
    {
      myFirstPar = ElCLib::Parameter(aHypr, aP);
    }
    else
    {
      GeomLib_Tool::Parameter(theProjCurve, aP, Precision::Confusion(), myFirstPar);
    }
  }
  if (!Precision::IsInfinite(myCurve->LastParameter()))
  {
    gp_Pnt aP = myCurve->Value(myCurve->LastParameter());
    aP        = ProjectPnt(myPlane, myDirection, aP);
    if (myType == GeomAbs_Line)
    {
      myLastPar = ElCLib::Parameter(aLin, aP);
    }
    else if (myType == GeomAbs_Parabola)
    {
      myLastPar = ElCLib::Parameter(aParab, aP);
    }
    else if (myType == GeomAbs_Hyperbola)
    {
      myLastPar = ElCLib::Parameter(aHypr, aP);
    }
    else
    {
      GeomLib_Tool::Parameter(theProjCurve, aP, Precision::Confusion(), myLastPar);
    }
  }
  myResult = new GeomAdaptor_Curve(theProjCurve, myFirstPar, myLastPar);
}

//=================================================================================================

Standard_Boolean ProjLib_ProjectOnPlane::BuildParabolaByApex(Handle(Geom_Curve)& theGeomParabolaPtr)
{
  //
  // Searching parabola apex as point with maximal curvature
  Standard_Real     aF       = myCurve->Parabola().Focal();
  GeomAbs_CurveType aCurType = myType;
  myType = GeomAbs_OtherCurve; // To provide correct calculation of derivativesb by projection for
                               // copy of instance;
  Handle(Adaptor3d_Curve) aProjCrv = ShallowCopy();
  myType                           = aCurType;
  LProp3d_CLProps      aProps(aProjCrv, 2, Precision::Confusion());
  ProjLib_MaxCurvature aMaxCur(aProps);
  math_BrentMinimum    aSolver(Precision::PConfusion());
  aSolver.Perform(aMaxCur, -10. * aF, 0., 10. * aF);

  if (!aSolver.IsDone())
  {
    return Standard_False;
  }

  Standard_Real aT;
  aT = aSolver.Location();
  aProps.SetParameter(aT);
  gp_Pnt        aP0 = aProps.Value();
  gp_Vec        aDY = aProps.D1();
  gp_Dir        anYDir(aDY);
  gp_Dir        anXDir;
  Standard_Real aCurv = aProps.Curvature();
  if (Precision::IsInfinite(aCurv) || aCurv < Precision::Confusion())
  {
    return Standard_False;
  }
  aProps.Normal(anXDir);
  //
  gp_Lin anXLine(aP0, anXDir);
  gp_Pnt aP1 = Value(aT + 10. * aF);
  //
  Standard_Real anX   = ElCLib::LineParameter(anXLine.Position(), aP1);
  Standard_Real anY   = anXLine.Distance(aP1);
  Standard_Real aNewF = anY * anY / 4. / anX;
  gp_Dir        anN   = anXDir ^ anYDir;
  gp_Ax2        anA2(aP0, anN, anXDir);
  gce_MakeParab aMkParab(anA2, aNewF);
  if (!aMkParab.IsDone())
  {
    return Standard_False;
  }

  gp_Parab aProjParab = aMkParab.Value();

  myType             = GeomAbs_Parabola;
  theGeomParabolaPtr = new Geom_Parabola(aProjParab);
  // GetTrimmedResult(theGeomParabolaPtr);

  return Standard_True;
}

//=================================================================================================

Standard_Boolean ProjLib_ProjectOnPlane::BuildHyperbolaByApex(
  Handle(Geom_Curve)& theGeomHyperbolaPtr)
{
  // Try to build hyperbola with help of apex position
  GeomAbs_CurveType aCurType = myType;
  myType = GeomAbs_OtherCurve; // To provide correct calculation of derivativesb by projection for
                               // copy of instance;
  Handle(Adaptor3d_Curve) aProjCrv = ShallowCopy();
  myType                           = aCurType;
  // Searching hyperbola apex as point with maximal curvature
  LProp3d_CLProps      aProps(aProjCrv, 2, Precision::Confusion());
  ProjLib_MaxCurvature aMaxCur(aProps);
  math_BrentMinimum    aSolver(Precision::PConfusion());
  aSolver.Perform(aMaxCur, -5., 0., 5.);

  if (aSolver.IsDone())
  {
    Standard_Real aT;
    aT = aSolver.Location();
    aProps.SetParameter(aT);
    Standard_Real aCurv = aProps.Curvature();
    if (Precision::IsInfinite(aCurv) || aCurv < Precision::Confusion())
    {
      return Standard_False;
    }
    else
    {
      gp_Hypr Hypr   = myCurve->Hyperbola();
      gp_Ax2  AxeRef = Hypr.Position();
      gp_Pnt  P      = ProjectPnt(myPlane, myDirection, AxeRef.Location());
      gp_Dir  Z      = myPlane.Direction();
      gp_Pnt  aP0    = aProps.Value();
      gp_Dir  anXDir = gce_MakeDir(P, aP0);
      gp_Dir  anYDir = gce_MakeDir(aProps.D1());
      //
      Standard_Real aMajRad = P.Distance(aP0);
      gp_Pnt        aP1     = Value(aT + 1.);
      gp_Vec        aV(P, aP1);
      Standard_Real anX     = aV * anXDir;
      Standard_Real anY     = aV * anYDir;
      Standard_Real aMinRad = anY / Sqrt(anX * anX / aMajRad / aMajRad - 1.);
      gp_Ax2        anA2(P, Z, anXDir);
      gp_Hypr       anHypr(anA2, aMajRad, aMinRad);
      theGeomHyperbolaPtr = new Geom_Hyperbola(anHypr);
      myType              = GeomAbs_Hyperbola;
    }
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//=================================================================================================

void ProjLib_ProjectOnPlane::BuildByApprox(const Standard_Real theLimitParameter)
{
  myType = GeomAbs_BSplineCurve;
  Handle(Geom_BSplineCurve) anApproxCurve;
  if (Precision::IsInfinite(myCurve->FirstParameter())
      || Precision::IsInfinite(myCurve->LastParameter()))
  {
    // To avoid exception in approximation
    Standard_Real           f          = Max(-theLimitParameter, myCurve->FirstParameter());
    Standard_Real           l          = Min(theLimitParameter, myCurve->LastParameter());
    Handle(Adaptor3d_Curve) aTrimCurve = myCurve->Trim(f, l, Precision::Confusion());
    PerformApprox(aTrimCurve, myPlane, myDirection, anApproxCurve);
  }
  else
  {
    PerformApprox(myCurve, myPlane, myDirection, anApproxCurve);
  }
  myFirstPar = anApproxCurve->FirstParameter();
  myLastPar  = anApproxCurve->LastParameter();
  GeomAdaptor_Curve aGACurve(anApproxCurve);
  myResult = new GeomAdaptor_Curve(aGACurve);
}