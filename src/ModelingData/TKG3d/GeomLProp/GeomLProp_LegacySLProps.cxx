#include <GeomLProp_LegacySLProps.hxx>

#include <CSLib.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_Surface.hxx>
#include <LProp_NotDefined.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_OutOfRange.hxx>
#include <math_DirectPolynomialRoots.hxx>

#include <algorithm>
#include <cmath>

namespace
{
constexpr double THE_MIN_STEP = 1.0e-7;

static int continuity(const occ::handle<Geom_Surface>& theSurface)
{
  switch (theSurface->Continuity())
  {
    case GeomAbs_C0: return 0;
    case GeomAbs_C1: return 1;
    case GeomAbs_C2: return 2;
    case GeomAbs_C3: return 3;
    case GeomAbs_CN: return 3;
    default: return 0;
  }
}

static bool isTangentDefined(GeomLProp_LegacySLProps& theProps,
                             int                      theCN,
                             double                   theLinTol,
                             int                      theDerivative,
                             int&                     theOrder,
                             LProp_Status&            theStatus)
{
  const double aTol = theLinTol * theLinTol;
  gp_Vec       aVec[2];
  theOrder = 0;
  while (theOrder < 3)
  {
    ++theOrder;
    if (theCN >= theOrder)
    {
      switch (theOrder)
      {
        case 1:
          aVec[0] = theProps.D1U();
          aVec[1] = theProps.D1V();
          break;
        case 2:
          aVec[0] = theProps.D2U();
          aVec[1] = theProps.D2V();
          break;
      }
      if (aVec[theDerivative].SquareMagnitude() > aTol)
      {
        theStatus = LProp_Defined;
        return true;
      }
    }
    else
    {
      theStatus = LProp_Undefined;
      return false;
    }
  }
  return false;
}
}

GeomLProp_LegacySLProps::GeomLProp_LegacySLProps(const occ::handle<Geom_Surface>& theSurface,
                                                 double                           theU,
                                                 double                           theV,
                                                 int                              theDerOrder,
                                                 double                           theResolution)
    : mySurf(theSurface),
      myDerOrder(theDerOrder),
      myCN(4),
      myLinTol(theResolution),
      myMinCurv(0.0),
      myMaxCurv(0.0),
      myMeanCurv(0.0),
      myGausCurv(0.0),
      mySignificantFirstDerivativeOrderU(0),
      mySignificantFirstDerivativeOrderV(0),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(theDerOrder < 0 || theDerOrder > 2, "GeomLProp_LegacySLProps()");
  SetParameters(theU, theV);
}

GeomLProp_LegacySLProps::GeomLProp_LegacySLProps(const occ::handle<Geom_Surface>& theSurface,
                                                 int                              theDerOrder,
                                                 double                           theResolution)
    : mySurf(theSurface),
      myU(RealLast()),
      myV(RealLast()),
      myDerOrder(theDerOrder),
      myCN(4),
      myLinTol(theResolution),
      myMinCurv(0.0),
      myMaxCurv(0.0),
      myMeanCurv(0.0),
      myGausCurv(0.0),
      mySignificantFirstDerivativeOrderU(0),
      mySignificantFirstDerivativeOrderV(0),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(theDerOrder < 0 || theDerOrder > 2, "GeomLProp_LegacySLProps()");
}

GeomLProp_LegacySLProps::GeomLProp_LegacySLProps(int theDerOrder, double theResolution)
    : myU(RealLast()),
      myV(RealLast()),
      myDerOrder(theDerOrder),
      myCN(0),
      myLinTol(theResolution),
      myMinCurv(0.0),
      myMaxCurv(0.0),
      myMeanCurv(0.0),
      myGausCurv(0.0),
      mySignificantFirstDerivativeOrderU(0),
      mySignificantFirstDerivativeOrderV(0),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(theDerOrder < 0 || theDerOrder > 2, "GeomLProp_LegacySLProps()");
}

void GeomLProp_LegacySLProps::SetSurface(const occ::handle<Geom_Surface>& theSurface)
{
  mySurf = theSurface;
  myCN   = continuity(theSurface);
}

void GeomLProp_LegacySLProps::SetParameters(const double theU, const double theV)
{
  myU = theU;
  myV = theV;
  switch (myDerOrder)
  {
    case 0: mySurf->D0(myU, myV, myPnt); break;
    case 1: mySurf->D1(myU, myV, myPnt, myD1u, myD1v); break;
    case 2: mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv); break;
  }
  myUTangentStatus  = LProp_Undecided;
  myVTangentStatus  = LProp_Undecided;
  myNormalStatus    = LProp_Undecided;
  myCurvatureStatus = LProp_Undecided;
}

const gp_Pnt& GeomLProp_LegacySLProps::Value() const { return myPnt; }

const gp_Vec& GeomLProp_LegacySLProps::D1U()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    mySurf->D1(myU, myV, myPnt, myD1u, myD1v);
  }
  return myD1u;
}

const gp_Vec& GeomLProp_LegacySLProps::D1V()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    mySurf->D1(myU, myV, myPnt, myD1u, myD1v);
  }
  return myD1v;
}

const gp_Vec& GeomLProp_LegacySLProps::D2U()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }
  return myD2u;
}

const gp_Vec& GeomLProp_LegacySLProps::D2V()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }
  return myD2v;
}

const gp_Vec& GeomLProp_LegacySLProps::DUV()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }
  return myDuv;
}

bool GeomLProp_LegacySLProps::IsTangentUDefined()
{
  if (myUTangentStatus == LProp_Undefined)
    return false;
  if (myUTangentStatus >= LProp_Defined)
    return true;
  return isTangentDefined(*this,
                          myCN,
                          myLinTol,
                          0,
                          mySignificantFirstDerivativeOrderU,
                          myUTangentStatus);
}

void GeomLProp_LegacySLProps::TangentU(gp_Dir& theDir)
{
  if (!IsTangentUDefined())
    throw LProp_NotDefined();
  if (mySignificantFirstDerivativeOrderU == 1)
  {
    theDir = gp_Dir(myD1u);
    return;
  }
  double aU1, aU2, aV1, aV2;
  mySurf->Bounds(aU1, aU2, aV1, aV2);
  const double aRange = (aU2 >= RealLast() || aU1 <= RealFirst()) ? 0.0 : aU2 - aU1;
  const double aDelta = std::max(aRange * 1.0e-3, THE_MIN_STEP);
  const double anOther = myU - aU1 < aDelta ? myU + aDelta : myU - aDelta;
  gp_Pnt       aP1;
  gp_Pnt       aP2;
  mySurf->D0(std::min(myU, anOther), myV, aP1);
  mySurf->D0(std::max(myU, anOther), myV, aP2);
  gp_Vec aVec = myD2u;
  if (aVec.Dot(gp_Vec(aP1, aP2)) < 0.0)
    aVec = -aVec;
  theDir = gp_Dir(aVec);
}

bool GeomLProp_LegacySLProps::IsTangentVDefined()
{
  if (myVTangentStatus == LProp_Undefined)
    return false;
  if (myVTangentStatus >= LProp_Defined)
    return true;
  return isTangentDefined(*this,
                          myCN,
                          myLinTol,
                          1,
                          mySignificantFirstDerivativeOrderV,
                          myVTangentStatus);
}

void GeomLProp_LegacySLProps::TangentV(gp_Dir& theDir)
{
  if (!IsTangentVDefined())
    throw LProp_NotDefined();
  if (mySignificantFirstDerivativeOrderV == 1)
  {
    theDir = gp_Dir(myD1v);
    return;
  }
  double aU1, aU2, aV1, aV2;
  mySurf->Bounds(aU1, aU2, aV1, aV2);
  const double aRange = (aV2 >= RealLast() || aV1 <= RealFirst()) ? 0.0 : aV2 - aV1;
  const double aDelta = std::max(aRange * 1.0e-3, THE_MIN_STEP);
  const double anOther = myV - aV1 < aDelta ? myV + aDelta : myV - aDelta;
  gp_Pnt       aP1;
  gp_Pnt       aP2;
  mySurf->D0(myU, std::min(myV, anOther), aP1);
  mySurf->D0(myU, std::max(myV, anOther), aP2);
  gp_Vec aVec = myD2v;
  if (aVec.Dot(gp_Vec(aP1, aP2)) < 0.0)
    aVec = -aVec;
  theDir = gp_Dir(aVec);
}

bool GeomLProp_LegacySLProps::IsNormalDefined()
{
  if (myNormalStatus == LProp_Undefined)
    return false;
  if (myNormalStatus >= LProp_Defined)
    return true;
  CSLib_DerivativeStatus aStatus = CSLib_Done;
  CSLib::Normal(myD1u, myD1v, myLinTol, aStatus, myNormal);
  if (aStatus == CSLib_Done)
  {
    myNormalStatus = LProp_Computed;
    return true;
  }
  myNormalStatus = LProp_Undefined;
  return false;
}

const gp_Dir& GeomLProp_LegacySLProps::Normal()
{
  if (!IsNormalDefined())
    throw LProp_NotDefined();
  return myNormal;
}

bool GeomLProp_LegacySLProps::IsCurvatureDefined()
{
  if (myCurvatureStatus == LProp_Undefined)
    return false;
  if (myCurvatureStatus >= LProp_Defined)
    return true;
  if (myCN < 2 || !IsNormalDefined() || !IsTangentUDefined() || !IsTangentVDefined())
  {
    myCurvatureStatus = LProp_Undefined;
    return false;
  }

  gp_Vec aNorm(myNormal);
  const double E = myD1u.SquareMagnitude();
  const double F = myD1u.Dot(myD1v);
  const double G = myD1v.SquareMagnitude();
  if (myDerOrder < 2)
    (void)D2U();
  const double L = aNorm.Dot(myD2u);
  const double M = aNorm.Dot(myDuv);
  const double N = aNorm.Dot(myD2v);

  double A = E * M - F * L;
  double B = E * N - G * L;
  double C = F * N - G * M;
  const double aMaxABC = std::max(std::max(std::abs(A), std::abs(B)), std::abs(C));
  if (aMaxABC < RealEpsilon())
  {
    myMinCurv         = N / G;
    myMaxCurv         = myMinCurv;
    myDirMinCurv      = gp_Dir(myD1u);
    myDirMaxCurv      = gp_Dir(myD1u.Crossed(aNorm));
    myMeanCurv        = myMinCurv;
    myGausCurv        = myMinCurv * myMinCurv;
    myCurvatureStatus = LProp_Computed;
    return true;
  }

  A /= aMaxABC;
  B /= aMaxABC;
  C /= aMaxABC;
  double Curv1, Curv2, Root1, Root2;
  gp_Vec VectCurv1, VectCurv2;
  if (std::abs(A) > RealEpsilon())
  {
    math_DirectPolynomialRoots aRoot(A, B, C);
    if (aRoot.NbSolutions() != 2)
    {
      myCurvatureStatus = LProp_Undefined;
      return false;
    }
    Root1 = aRoot.Value(1);
    Root2 = aRoot.Value(2);
    Curv1 = ((L * Root1 + 2.0 * M) * Root1 + N) / ((E * Root1 + 2.0 * F) * Root1 + G);
    Curv2 = ((L * Root2 + 2.0 * M) * Root2 + N) / ((E * Root2 + 2.0 * F) * Root2 + G);
    VectCurv1 = Root1 * myD1u + myD1v;
    VectCurv2 = Root2 * myD1u + myD1v;
  }
  else if (std::abs(C) > RealEpsilon())
  {
    math_DirectPolynomialRoots aRoot(C, B, A);
    if (aRoot.NbSolutions() != 2)
    {
      myCurvatureStatus = LProp_Undefined;
      return false;
    }
    Root1 = aRoot.Value(1);
    Root2 = aRoot.Value(2);
    Curv1 = ((N * Root1 + 2.0 * M) * Root1 + L) / ((G * Root1 + 2.0 * F) * Root1 + E);
    Curv2 = ((N * Root2 + 2.0 * M) * Root2 + L) / ((G * Root2 + 2.0 * F) * Root2 + E);
    VectCurv1 = myD1u + Root1 * myD1v;
    VectCurv2 = myD1u + Root2 * myD1v;
  }
  else
  {
    Curv1 = L / E;
    Curv2 = N / G;
    VectCurv1 = myD1u;
    VectCurv2 = myD1v;
  }

  if (Curv1 < Curv2)
  {
    myMinCurv = Curv1;
    myMaxCurv = Curv2;
    myDirMinCurv = gp_Dir(VectCurv1);
    myDirMaxCurv = gp_Dir(VectCurv2);
  }
  else
  {
    myMinCurv = Curv2;
    myMaxCurv = Curv1;
    myDirMinCurv = gp_Dir(VectCurv2);
    myDirMaxCurv = gp_Dir(VectCurv1);
  }

  myMeanCurv = ((N * E) - (2.0 * M * F) + (L * G)) / (2.0 * ((E * G) - (F * F)));
  myGausCurv = ((L * N) - (M * M)) / ((E * G) - (F * F));
  myCurvatureStatus = LProp_Computed;
  return true;
}

bool GeomLProp_LegacySLProps::IsUmbilic()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();
  return std::abs(myMaxCurv - myMinCurv) < std::abs(Epsilon(myMaxCurv));
}

double GeomLProp_LegacySLProps::MaxCurvature()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();
  return myMaxCurv;
}

double GeomLProp_LegacySLProps::MinCurvature()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();
  return myMinCurv;
}

void GeomLProp_LegacySLProps::CurvatureDirections(gp_Dir& theMax, gp_Dir& theMin)
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();
  theMax = myDirMaxCurv;
  theMin = myDirMinCurv;
}

double GeomLProp_LegacySLProps::MeanCurvature()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();
  return myMeanCurv;
}

double GeomLProp_LegacySLProps::GaussianCurvature()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();
  return myGausCurv;
}
