// Created on: 1996-11-08
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1996-1999 Matra Datavision
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

#include <GCPnts_TangentialDeflection.hxx>

#include <GCPnts_TCurveTypes.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XYZ.hxx>
#include <math_BrentMinimum.hxx>
#include <math_PSO.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <NCollection_Array1.hxx>

namespace
{
static const double Us3 = 0.3333333333333333333333333333;

inline static void D0(const Adaptor3d_Curve& C, const double U, gp_Pnt& P)
{
  C.D0(U, P);
}

inline static void D2(const Adaptor3d_Curve& C,
                      const double    U,
                      gp_Pnt&                P,
                      gp_Vec&                V1,
                      gp_Vec&                V2)
{
  C.D2(U, P, V1, V2);
}

static void D0(const Adaptor2d_Curve2d& C, const double U, gp_Pnt& PP)
{
  double X, Y;
  gp_Pnt2d      P;
  C.D0(U, P);
  P.Coord(X, Y);
  PP.SetCoord(X, Y, 0.0);
}

static void D2(const Adaptor2d_Curve2d& C,
               const double      U,
               gp_Pnt&                  PP,
               gp_Vec&                  VV1,
               gp_Vec&                  VV2)
{
  double X, Y;
  gp_Pnt2d      P;
  gp_Vec2d      V1, V2;
  C.D2(U, P, V1, V2);
  P.Coord(X, Y);
  PP.SetCoord(X, Y, 0.0);
  V1.Coord(X, Y);
  VV1.SetCoord(X, Y, 0.0);
  V2.Coord(X, Y);
  VV2.SetCoord(X, Y, 0.0);
}

static double EstimAngl(const gp_Pnt& P1, const gp_Pnt& Pm, const gp_Pnt& P2)
{
  gp_Vec        V1(P1, Pm), V2(Pm, P2);
  double L = V1.Magnitude() * V2.Magnitude();
  if (L > gp::Resolution())
  {
    return V1.CrossMagnitude(V2) / L;
  }
  else
  {
    return 0.;
  }
}

// Return number of interval of continuity on which theParam is located.
// Last parameter is used to increase search speed.
static int getIntervalIdx(const double    theParam,
                                       NCollection_Array1<double>&  theIntervs,
                                       const int thePreviousIdx)
{
  int anIdx;
  for (anIdx = thePreviousIdx; anIdx < theIntervs.Upper(); anIdx++)
  {
    if (theParam >= theIntervs(anIdx)
        && theParam <= theIntervs(anIdx + 1)) // Inside of anIdx interval.
    {
      break;
    }
  }
  return anIdx;
}
} // namespace

//=================================================================================================

GCPnts_TangentialDeflection::GCPnts_TangentialDeflection()
    : myAngularDeflection(0.0),
      myCurvatureDeflection(0.0),
      myUTol(0.0),
      myMinNbPnts(0),
      myMinLen(0.0),
      myLastU(0.0),
      myFirstu(0.0)
{
}

//=================================================================================================

GCPnts_TangentialDeflection::GCPnts_TangentialDeflection(const Adaptor3d_Curve& theC,
                                                         const double theAngularDeflection,
                                                         const double theCurvatureDeflection,
                                                         const int theMinimumOfPoints,
                                                         const double    theUTol,
                                                         const double    theMinLen)
    : myAngularDeflection(0.0),
      myCurvatureDeflection(0.0),
      myUTol(0.0),
      myMinNbPnts(0),
      myMinLen(0.0),
      myLastU(0.0),
      myFirstu(0.0)
{
  Initialize(theC,
             theAngularDeflection,
             theCurvatureDeflection,
             theMinimumOfPoints,
             theUTol,
             theMinLen);
}

//=================================================================================================

GCPnts_TangentialDeflection::GCPnts_TangentialDeflection(const Adaptor3d_Curve& theC,
                                                         const double    theFirstParameter,
                                                         const double    theLastParameter,
                                                         const double theAngularDeflection,
                                                         const double theCurvatureDeflection,
                                                         const int theMinimumOfPoints,
                                                         const double    theUTol,
                                                         const double    theMinLen)
    : myAngularDeflection(0.0),
      myCurvatureDeflection(0.0),
      myUTol(0.0),
      myMinNbPnts(0),
      myMinLen(0.0),
      myLastU(0.0),
      myFirstu(0.0)
{
  Initialize(theC,
             theFirstParameter,
             theLastParameter,
             theAngularDeflection,
             theCurvatureDeflection,
             theMinimumOfPoints,
             theUTol,
             theMinLen);
}

//=================================================================================================

GCPnts_TangentialDeflection::GCPnts_TangentialDeflection(const Adaptor2d_Curve2d& theC,
                                                         const double theAngularDeflection,
                                                         const double theCurvatureDeflection,
                                                         const int theMinimumOfPoints,
                                                         const double    theUTol,
                                                         const double    theMinLen)
    : myAngularDeflection(0.0),
      myCurvatureDeflection(0.0),
      myUTol(0.0),
      myMinNbPnts(0),
      myMinLen(0.0),
      myLastU(0.0),
      myFirstu(0.0)
{
  Initialize(theC,
             theAngularDeflection,
             theCurvatureDeflection,
             theMinimumOfPoints,
             theUTol,
             theMinLen);
}

//=================================================================================================

GCPnts_TangentialDeflection::GCPnts_TangentialDeflection(const Adaptor2d_Curve2d& theC,
                                                         const double      theFirstParameter,
                                                         const double      theLastParameter,
                                                         const double theAngularDeflection,
                                                         const double theCurvatureDeflection,
                                                         const int theMinimumOfPoints,
                                                         const double    theUTol,
                                                         const double    theMinLen)
    : myAngularDeflection(0.0),
      myCurvatureDeflection(0.0),
      myUTol(0.0),
      myMinNbPnts(0),
      myMinLen(0.0),
      myLastU(0.0),
      myFirstu(0.0)
{
  Initialize(theC,
             theFirstParameter,
             theLastParameter,
             theAngularDeflection,
             theCurvatureDeflection,
             theMinimumOfPoints,
             theUTol,
             theMinLen);
}

//=================================================================================================

void GCPnts_TangentialDeflection::Initialize(const Adaptor3d_Curve& theC,
                                             const double    theAngularDeflection,
                                             const double    theCurvatureDeflection,
                                             const int theMinimumOfPoints,
                                             const double    theUTol,
                                             const double    theMinLen)
{
  Initialize(theC,
             theC.FirstParameter(),
             theC.LastParameter(),
             theAngularDeflection,
             theCurvatureDeflection,
             theMinimumOfPoints,
             theUTol,
             theMinLen);
}

//=================================================================================================

void GCPnts_TangentialDeflection::Initialize(const Adaptor2d_Curve2d& theC,
                                             const double      theAngularDeflection,
                                             const double      theCurvatureDeflection,
                                             const int   theMinimumOfPoints,
                                             const double      theUTol,
                                             const double      theMinLen)
{
  Initialize(theC,
             theC.FirstParameter(),
             theC.LastParameter(),
             theAngularDeflection,
             theCurvatureDeflection,
             theMinimumOfPoints,
             theUTol,
             theMinLen);
}

//=================================================================================================

void GCPnts_TangentialDeflection::Initialize(const Adaptor3d_Curve& theC,
                                             const double    theFirstParameter,
                                             const double    theLastParameter,
                                             const double    theAngularDeflection,
                                             const double    theCurvatureDeflection,
                                             const int theMinimumOfPoints,
                                             const double    theUTol,
                                             const double    theMinLen)
{
  initialize(theC,
             theFirstParameter,
             theLastParameter,
             theAngularDeflection,
             theCurvatureDeflection,
             theMinimumOfPoints,
             theUTol,
             theMinLen);
}

//=================================================================================================

void GCPnts_TangentialDeflection::Initialize(const Adaptor2d_Curve2d& theC,
                                             const double      theFirstParameter,
                                             const double      theLastParameter,
                                             const double      theAngularDeflection,
                                             const double      theCurvatureDeflection,
                                             const int   theMinimumOfPoints,
                                             const double      theUTol,
                                             const double      theMinLen)
{
  initialize(theC,
             theFirstParameter,
             theLastParameter,
             theAngularDeflection,
             theCurvatureDeflection,
             theMinimumOfPoints,
             theUTol,
             theMinLen);
}

//=================================================================================================

template <class TheCurve>
void GCPnts_TangentialDeflection::EvaluateDu(const TheCurve&     theC,
                                             const double theU,
                                             gp_Pnt&             theP,
                                             double&      theDu,
                                             bool&   theNotDone) const
{
  gp_Vec T, N;
  D2(theC, theU, theP, T, N);
  double           Lt   = T.Magnitude();
  constexpr double LTol = Precision::Confusion();
  if (Lt > LTol && N.Magnitude() > LTol)
  {
    double Lc = N.CrossMagnitude(T);
    double Ln = Lc / Lt;
    if (Ln > LTol)
    {
      theDu      = sqrt(8.0 * std::max(myCurvatureDeflection, myMinLen) / Ln);
      theNotDone = false;
    }
  }
}

//=================================================================================================

template <class TheCurve>
void GCPnts_TangentialDeflection::PerformLinear(const TheCurve& theC)
{
  gp_Pnt P;
  D0(theC, myFirstu, P);
  myParameters.Append(myFirstu);
  myPoints.Append(P);
  if (myMinNbPnts > 2)
  {
    double Du = (myLastU - myFirstu) / myMinNbPnts;
    double U  = myFirstu + Du;
    for (int i = 2; i < myMinNbPnts; i++)
    {
      D0(theC, U, P);
      myParameters.Append(U);
      myPoints.Append(P);
      U += Du;
    }
  }
  D0(theC, myLastU, P);
  myParameters.Append(myLastU);
  myPoints.Append(P);
}

//=================================================================================================

template <class TheCurve>
void GCPnts_TangentialDeflection::PerformCircular(const TheCurve& theC)
{
  // akm 8/01/02 : check the radius before divide by it
  double dfR = theC.Circle().Radius();
  double Du  = GCPnts_TangentialDeflection::ArcAngularStep(dfR,
                                                                 myCurvatureDeflection,
                                                                 myAngularDeflection,
                                                                 myMinLen);

  const double aDiff = myLastU - myFirstu;
  // Round up number of points to satisfy curvatureDeflection more precisely
  int NbPoints = (int)std::min(std::ceil(aDiff / Du), 1.0e+6);
  NbPoints                  = std::max(NbPoints, myMinNbPnts - 1);
  Du                        = aDiff / NbPoints;

  gp_Pnt        P;
  double U = myFirstu;
  for (int i = 1; i <= NbPoints; i++)
  {
    D0(theC, U, P);
    myParameters.Append(U);
    myPoints.Append(P);
    U += Du;
  }

  D0(theC, myLastU, P);
  myParameters.Append(myLastU);
  myPoints.Append(P);
}

//=================================================================================================

template <class TheCurve>
void GCPnts_TangentialDeflection::initialize(const TheCurve&        theC,
                                             const double    theFirstParameter,
                                             const double    theLastParameter,
                                             const double    theAngularDeflection,
                                             const double    theCurvatureDeflection,
                                             const int theMinimumOfPoints,
                                             const double    theUTol,
                                             const double    theMinLen)
{
  Standard_ConstructionError_Raise_if(
    theCurvatureDeflection < Precision::Confusion() || theAngularDeflection < Precision::Angular(),
    "GCPnts_TangentialDeflection::Initialize - Zero Deflection") myParameters.Clear();
  myPoints.Clear();
  if (theFirstParameter < theLastParameter)
  {
    myFirstu = theFirstParameter;
    myLastU  = theLastParameter;
  }
  else
  {
    myLastU  = theFirstParameter;
    myFirstu = theLastParameter;
  }
  myUTol                = theUTol;
  myAngularDeflection   = theAngularDeflection;
  myCurvatureDeflection = theCurvatureDeflection;
  myMinNbPnts           = std::max(theMinimumOfPoints, 2);
  myMinLen              = std::max(theMinLen, Precision::Confusion());

  switch (theC.GetType())
  {
    case GeomAbs_Line: {
      PerformLinear(theC);
      break;
    }
    case GeomAbs_Circle: {
      PerformCircular(theC);
      break;
    }
    case GeomAbs_BSplineCurve: {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BSplineCurve) aBS = theC.BSpline();
      if (aBS->NbPoles() == 2)
        PerformLinear(theC);
      else
        PerformCurve(theC);
      break;
    }
    case GeomAbs_BezierCurve: {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BezierCurve) aBZ = theC.Bezier();
      if (aBZ->NbPoles() == 2)
        PerformLinear(theC);
      else
        PerformCurve(theC);
      break;
    }
    default: {
      PerformCurve(theC);
      break;
    }
  }
}

//=================================================================================================

int GCPnts_TangentialDeflection::AddPoint(const gp_Pnt&          thePnt,
                                                       const double    theParam,
                                                       const bool theIsReplace)
{
  constexpr double tol   = Precision::PConfusion();
  int        index = -1;
  const int  nb    = myParameters.Length();
  for (int i = 1; index == -1 && i <= nb; i++)
  {
    double dist = myParameters.Value(i) - theParam;
    if (fabs(dist) <= tol)
    {
      index = i;
      if (theIsReplace)
      {
        myPoints.ChangeValue(i)     = thePnt;
        myParameters.ChangeValue(i) = theParam;
      }
    }
    else if (dist > tol)
    {
      myPoints.InsertBefore(i, thePnt);
      myParameters.InsertBefore(i, theParam);
      index = i;
    }
  }
  if (index == -1)
  {
    myPoints.Append(thePnt);
    myParameters.Append(theParam);
    index = myParameters.Length();
  }
  return index;
}

//=================================================================================================

double GCPnts_TangentialDeflection::ArcAngularStep(const double theRadius,
                                                          const double theLinearDeflection,
                                                          const double theAngularDeflection,
                                                          const double theMinLength)
{
  Standard_ConstructionError_Raise_if(theRadius < 0.0, "Negative radius");

  constexpr double aPrecision = Precision::Confusion();

  double Du = 0.0, aMinSizeAng = 0.0;
  if (theRadius > aPrecision)
  {
    Du = std::max(1.0 - (theLinearDeflection / theRadius), 0.0);

    // It is not suitable to consider min size greater than 1/4 arc len.
    if (theMinLength > aPrecision)
      aMinSizeAng = std::min(theMinLength / theRadius, M_PI_2);
  }
  Du = 2.0 * std::acos(Du);
  Du = std::max(std::min(Du, theAngularDeflection), aMinSizeAng);
  return Du;
}

//=================================================================================================

template <class TheCurve>
void GCPnts_TangentialDeflection::PerformCurve(const TheCurve& theC)
{
  int i, j;
  gp_XYZ           V1, V2;
  gp_Pnt           MiddlePoint, CurrentPoint, LastPoint;
  double    Du, Dusave, MiddleU, L1, L2;

  double           U1   = myFirstu;
  constexpr double LTol = Precision::Confusion(); // protection longueur nulle
  double           ATol = 1.e-2 * myAngularDeflection;
  if (ATol > 1.e-2)
  {
    ATol = 1.e-2;
  }
  else if (ATol < 1.e-7)
  {
    ATol = 1.e-7;
  }

  D0(theC, myLastU, LastPoint);

  // Initialization du calcul

  bool NotDone = true;
  Dusave                   = (myLastU - myFirstu) * Us3;
  Du                       = Dusave;
  EvaluateDu(theC, U1, CurrentPoint, Du, NotDone);
  myParameters.Append(U1);
  myPoints.Append(CurrentPoint);

  // Used to detect "isLine" current bspline and in Du computation in general handling.
  const int NbInterv = theC.NbIntervals(GeomAbs_CN);
  NCollection_Array1<double>   Intervs(1, NbInterv + 1);
  theC.Intervals(Intervs, GeomAbs_CN);

  if (NotDone || Du > 5. * Dusave)
  {
    // C'est soit une droite, soit une singularite :
    V1 = (LastPoint.XYZ() - CurrentPoint.XYZ());
    L1 = V1.Modulus();
    if (L1 > LTol)
    {
      // Si c'est une droite on verifie en calculant minNbPoints :
      bool IsLine   = true;
      int NbPoints = (myMinNbPnts > 3) ? myMinNbPnts : 3;
      switch (theC.GetType())
      {
        case GeomAbs_BSplineCurve: {
          Handle(typename GCPnts_TCurveTypes<TheCurve>::BSplineCurve) BS = theC.BSpline();
          NbPoints = std::max(BS->Degree() + 1, NbPoints);
          break;
        }
        case GeomAbs_BezierCurve: {
          Handle(typename GCPnts_TCurveTypes<TheCurve>::BezierCurve) BZ = theC.Bezier();
          NbPoints = std::max(BZ->Degree() + 1, NbPoints);
          break;
        }
        default: {
          break;
        }
      }
      ////
      double param = 0.;
      for (i = 1; i <= NbInterv && IsLine; ++i)
      {
        // Avoid usage intervals out of [myFirstu, myLastU].
        if ((Intervs(i + 1) < myFirstu) || (Intervs(i) > myLastU))
        {
          continue;
        }

        // Fix border points in applicable intervals, to avoid be out of target interval.
        if ((Intervs(i) < myFirstu) && (Intervs(i + 1) > myFirstu))
        {
          Intervs(i) = myFirstu;
        }
        if ((Intervs(i) < myLastU) && (Intervs(i + 1) > myLastU))
        {
          Intervs(i + 1) = myLastU;
        }

        const double delta = (Intervs(i + 1) - Intervs(i)) / NbPoints;
        for (j = 1; j <= NbPoints && IsLine; ++j)
        {
          param = Intervs(i) + j * delta;
          D0(theC, param, MiddlePoint);
          V2 = (MiddlePoint.XYZ() - CurrentPoint.XYZ());
          L2 = V2.Modulus();
          if (L2 > LTol)
          {
            const double aAngle = V2.CrossMagnitude(V1) / (L1 * L2);
            IsLine                     = (aAngle < ATol);
          }
        }
      }

      if (IsLine)
      {
        myParameters.Clear();
        myPoints.Clear();

        PerformLinear(theC);
        return;
      }
      else
      {
        // c'etait une singularite on continue:
        // Du = Dusave;
        EvaluateDu(theC, param, MiddlePoint, Du, NotDone);
      }
    }
    else
    {
      Du      = (myLastU - myFirstu) / 2.1;
      MiddleU = myFirstu + Du;
      D0(theC, MiddleU, MiddlePoint);
      V1 = (MiddlePoint.XYZ() - CurrentPoint.XYZ());
      L1 = V1.Modulus();
      if (L1 < LTol)
      {
        // L1 < LTol C'est une courbe de longueur nulle, calcul termine :
        // on renvoi un segment de 2 points   (protection)
        myParameters.Append(myLastU);
        myPoints.Append(LastPoint);
        return;
      }
    }
  }

  if (Du > Dusave)
    Du = Dusave;
  else
    Dusave = Du;

  if (Du < myUTol)
  {
    Du = myLastU - myFirstu;
    if (Du < myUTol)
    {
      myParameters.Append(myLastU);
      myPoints.Append(LastPoint);
      return;
    }
  }

  // Traitement normal pour une courbe
  bool MorePoints = true;
  double    U2         = myFirstu;
  double    AngleMax   = myAngularDeflection * 0.5; // because we take the midpoint
  // Indexes of intervals of U1 and U2, used to handle non-uniform case.
  int aIdx[2]       = {Intervs.Lower(), Intervs.Lower()};
  bool isNeedToCheck = false;
  gp_Pnt           aPrevPoint    = myPoints.Last();

  while (MorePoints)
  {
    aIdx[0] = getIntervalIdx(U1, Intervs, aIdx[0]);
    U2 += Du;

    if (U2 >= myLastU) // Bout de courbe
    {
      U2           = myLastU;
      CurrentPoint = LastPoint;
      Du           = U2 - U1;
      Dusave       = Du;
    }
    else
    {
      D0(theC, U2, CurrentPoint); // Point suivant
    }

    double    Coef = 0.0, ACoef = 0., FCoef = 0.;
    bool Correction, TooLarge, TooSmall;
    TooLarge   = false;
    Correction = true;
    TooSmall   = false;

    while (Correction) // Ajustement Du
    {
      if (isNeedToCheck)
      {
        aIdx[1] = getIntervalIdx(U2, Intervs, aIdx[0]);
        if (aIdx[1] > aIdx[0]) // Jump to another polynom.
        {
          // Set Du to the smallest value and check deflection on it.
          if (Du > (Intervs(aIdx[0] + 1) - Intervs(aIdx[0])) * Us3)
          {
            Du = (Intervs(aIdx[0] + 1) - Intervs(aIdx[0])) * Us3;
            U2 = U1 + Du;
            if (U2 > myLastU)
            {
              U2 = myLastU;
            }
            D0(theC, U2, CurrentPoint);
          }
        }
      }
      MiddleU = (U1 + U2) * 0.5; // Verif / au point milieu
      D0(theC, MiddleU, MiddlePoint);

      V1 = (CurrentPoint.XYZ() - aPrevPoint.XYZ()); // Critere de fleche
      V2 = (MiddlePoint.XYZ() - aPrevPoint.XYZ());
      L1 = V1.Modulus();

      FCoef = (L1 > myMinLen) ? V1.CrossMagnitude(V2) / (L1 * myCurvatureDeflection) : 0.0;

      V1 = (CurrentPoint.XYZ() - MiddlePoint.XYZ()); // Critere d'angle
      L1 = V1.Modulus();
      L2 = V2.Modulus();
      if (L1 > myMinLen && L2 > myMinLen)
      {
        double angg = V1.CrossMagnitude(V2) / (L1 * L2);
        ACoef              = angg / AngleMax;
      }
      else
      {
        ACoef = 0.0;
      }

      // On retient le plus penalisant
      Coef = std::max(ACoef, FCoef);

      if (isNeedToCheck && Coef < 0.55)
      {
        isNeedToCheck = false;
        Du            = Dusave;
        U2            = U1 + Du;
        if (U2 > myLastU)
        {
          U2 = myLastU;
        }
        D0(theC, U2, CurrentPoint);
        continue;
      }

      if (Coef <= 1.0)
      {
        if (std::abs(myLastU - U2) < myUTol)
        {
          myParameters.Append(myLastU);
          myPoints.Append(LastPoint);
          MorePoints = false;
          Correction = false;
        }
        else
        {
          if (Coef >= 0.55 || TooLarge)
          {
            myParameters.Append(U2);
            myPoints.Append(CurrentPoint);
            aPrevPoint    = CurrentPoint;
            Correction    = false;
            isNeedToCheck = true;
          }
          else if (TooSmall)
          {
            Correction = false;
            aPrevPoint = CurrentPoint;
          }
          else
          {
            TooSmall = true;
            // double UUU2 = U2;
            Du += std::min((U2 - U1) * (1. - Coef), Du * Us3);

            U2 = U1 + Du;
            if (U2 > myLastU)
            {
              U2 = myLastU;
            }
            D0(theC, U2, CurrentPoint);
          }
        }
      }
      else
      {
        if (Coef >= 1.5)
        {
          if (!aPrevPoint.IsEqual(myPoints.Last(), Precision::Confusion()))
          {
            myParameters.Append(U1);
            myPoints.Append(aPrevPoint);
          }
          U2           = MiddleU;
          Du           = U2 - U1;
          CurrentPoint = MiddlePoint;
        }
        else
        {
          Du *= 0.9;
          U2 = U1 + Du;
          D0(theC, U2, CurrentPoint);
          TooLarge = true;
        }
      }
    }

    Du = U2 - U1;

    if (MorePoints)
    {
      if (U1 > myFirstu)
      {
        if (FCoef > ACoef)
        {
          // La fleche est critere de decoupage
          EvaluateDu(theC, U2, CurrentPoint, Du, NotDone);
          if (NotDone)
          {
            Du += (Du - Dusave) * (Du / Dusave);
            if (Du > 1.5 * Dusave)
              Du = 1.5 * Dusave;
            if (Du < 0.75 * Dusave)
              Du = 0.75 * Dusave;
          }
        }
        else
        {
          // L'angle est le critere de decoupage
          Du += (Du - Dusave) * (Du / Dusave);
          if (Du > 1.5 * Dusave)
            Du = 1.5 * Dusave;
          if (Du < 0.75 * Dusave)
            Du = 0.75 * Dusave;
        }
      }

      if (Du < myUTol)
      {
        Du = myLastU - U2;
        if (Du < myUTol)
        {
          myParameters.Append(myLastU);
          myPoints.Append(LastPoint);
          MorePoints = false;
        }
        else if (Du * Us3 > myUTol)
        {
          Du *= Us3;
        }
      }
      U1     = U2;
      Dusave = Du;
    }
  }
  // Recalage avant dernier point :
  i = myPoints.Length() - 1;
  //  Real d = myPoints (i).Distance (myPoints (i+1));
  // if (std::abs(myParameters (i) - myParameters (i+1))<= 0.000001 || d < Precision::Confusion()) {
  //    cout<<"deux points confondus"<<endl;
  //    myParameters.Remove (i+1);
  //    myPoints.Remove (i+1);
  //    i--;
  //  }
  if (i >= 2)
  {
    MiddleU = myParameters(i - 1);
    MiddleU = (myLastU + MiddleU) * 0.5;
    D0(theC, MiddleU, MiddlePoint);
    myParameters.SetValue(i, MiddleU);
    myPoints.SetValue(i, MiddlePoint);
  }

  //-- On rajoute des points aux milieux des segments si le nombre
  //-- mini de points n'est pas atteint
  //--
  int Nbp = myPoints.Length();

  // std::cout << "GCPnts_TangentialDeflection: Number of Points (" << Nbp << " " << myMinNbPnts <<
  // " )" << std::endl;

  while (Nbp < myMinNbPnts)
  {
    for (i = 2; i <= Nbp; i += 2)
    {
      MiddleU = (myParameters.Value(i - 1) + myParameters.Value(i)) * 0.5;
      D0(theC, MiddleU, MiddlePoint);
      myParameters.InsertBefore(i, MiddleU);
      myPoints.InsertBefore(i, MiddlePoint);
      Nbp++;
    }
  }
  // Additional check for intervals
  const double    MinLen2 = myMinLen * myMinLen;
  const int MaxNbp  = 10 * Nbp;
  for (i = 1; i < Nbp; ++i)
  {
    U1 = myParameters(i);
    U2 = myParameters(i + 1);

    if (U2 - U1 <= myUTol)
    {
      continue;
    }

    // Check maximal deflection on interval;
    double dmax = 0.;
    double umax = 0.;
    double amax = 0.;
    EstimDefl(theC, U1, U2, dmax, umax);
    const gp_Pnt& P1 = myPoints(i);
    const gp_Pnt& P2 = myPoints(i + 1);
    D0(theC, umax, MiddlePoint);
    amax = EstimAngl(P1, MiddlePoint, P2);
    if (dmax > myCurvatureDeflection || amax > AngleMax)
    {
      if (umax - U1 > myUTol && U2 - umax > myUTol)
      {
        if (P1.SquareDistance(MiddlePoint) > MinLen2 && P2.SquareDistance(MiddlePoint) > MinLen2)
        {
          myParameters.InsertAfter(i, umax);
          myPoints.InsertAfter(i, MiddlePoint);
          ++Nbp;
          --i; // To compensate ++i in loop header: i must point to first part of split interval
          if (Nbp > MaxNbp)
          {
            break;
          }
        }
      }
    }
  }
}

//=================================================================================================

template <class TheCurve>
void GCPnts_TangentialDeflection::EstimDefl(const TheCurve&     theC,
                                            const double theU1,
                                            const double theU2,
                                            double&      theMaxDefl,
                                            double&      theUMax)
{
  const double Du = (myLastU - myFirstu);
  //
  typename GCPnts_TCurveTypes<TheCurve>::DistFunction aFunc(theC, theU1, theU2);
  //
  const int aNbIter = 100;
  const double aRelTol = std::max(1.e-3, 2. * myUTol / (std::abs(theU1) + std::abs(theU2)));
  //
  math_BrentMinimum anOptLoc(aRelTol, aNbIter, myUTol);
  anOptLoc.Perform(aFunc, theU1, (theU1 + theU2) / 2., theU2);
  if (anOptLoc.IsDone())
  {
    theMaxDefl = std::sqrt(-anOptLoc.Minimum());
    theUMax    = anOptLoc.Location();
    return;
  }
  //
  math_Vector aLowBorder(1, 1), aUppBorder(1, 1), aSteps(1, 1);
  aSteps(1)                           = std::max(0.1 * Du, 100. * myUTol);
  const int aNbParticles = std::max(8, RealToInt(32 * (theU2 - theU1) / Du));
  aLowBorder(1)                       = theU1;
  aUppBorder(1)                       = theU2;
  //
  //
  double                                         aValue = 0.0;
  math_Vector                                           aT(1, 1);
  typename GCPnts_TCurveTypes<TheCurve>::DistFunctionMV aFuncMV(aFunc);

  math_PSO aFinder(&aFuncMV, aLowBorder, aUppBorder, aSteps, aNbParticles);
  aFinder.Perform(aSteps, aValue, aT);
  //
  anOptLoc.Perform(aFunc,
                   std::max(aT(1) - aSteps(1), theU1),
                   aT(1),
                   std::min(aT(1) + aSteps(1), theU2));
  if (anOptLoc.IsDone())
  {
    theMaxDefl = std::sqrt(-anOptLoc.Minimum());
    theUMax    = anOptLoc.Location();
    return;
  }

  theMaxDefl = std::sqrt(-aValue);
  theUMax    = aT(1);
}
