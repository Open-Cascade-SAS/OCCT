// Copyright (c) 2008-2015 OPEN CASCADE SAS
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

#include <BRepGProp_Gauss.hxx>

#include <math.hxx>
#include <Standard_Assert.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp_Domain.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_LocalArray.hxx>
#include <math_Vector.hxx>

#include <algorithm>

namespace
{
// Minimal value of interval's range for computation | minimal value of "dim" | ...
static const double EPS_PARAM          = 1.e-12;
static const double EPS_DIM            = 1.e-30;
static const double ERROR_ALGEBR_RATIO = 2.0 / 3.0;

// Maximum of GaussPoints on a subinterval and maximum of subintervals
static const int    GPM        = math::GaussPointsMax();
static const int    SUBS_POWER = 32;
static const size_t SM         = static_cast<size_t>(SUBS_POWER * GPM + 1);

static constexpr int THE_GAUSS_INLINE_SIZE = 64;
static constexpr int THE_KNOT_INLINE_SIZE  = 2;

template <int theInlineSize>
static NCollection_Array1<double> prepareArray(
  NCollection_LocalArray<double, theInlineSize>& theStorage,
  const size_t                                   theSize)
{
  if (theStorage.Size() < theSize)
  {
    theStorage.Reallocate(theSize);
  }
  return NCollection_Array1<double>(theStorage.begin(), theSize);
}

static void initGaussPoints(const int                                              theNbPoints,
                            NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>& theStorage)
{
  NCollection_Array1<double> aPointArray =
    prepareArray(theStorage, static_cast<size_t>(theNbPoints));
  math_Vector aPoints(aPointArray.Data(), 1, aPointArray.Length());
  math::GaussPoints(theNbPoints, aPoints);
}

static void initGaussWeights(const int                                              theNbPoints,
                             NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>& theStorage)
{
  NCollection_Array1<double> aWeightArray =
    prepareArray(theStorage, static_cast<size_t>(theNbPoints));
  math_Vector aWeights(aWeightArray.Data(), 1, aWeightArray.Length());
  math::GaussWeights(theNbPoints, aWeights);
}

// Auxiliary inner functions to perform arithmetic operations.
static double Add(const double theA, const double theB)
{
  return theA + theB;
}

static double AddInf(const double theA, const double theB)
{
  if (Precision::IsPositiveInfinite(theA))
  {
    if (Precision::IsNegativeInfinite(theB))
    {
      return 0.0;
    }
    else
    {
      return Precision::Infinite();
    }
  }

  if (Precision::IsPositiveInfinite(theB))
  {
    if (Precision::IsNegativeInfinite(theA))
    {
      return 0.0;
    }
    else
    {
      return Precision::Infinite();
    }
  }

  if (Precision::IsNegativeInfinite(theA))
  {
    if (Precision::IsPositiveInfinite(theB))
    {
      return 0.0;
    }
    else
    {
      return -Precision::Infinite();
    }
  }

  if (Precision::IsNegativeInfinite(theB))
  {
    if (Precision::IsPositiveInfinite(theA))
    {
      return 0.0;
    }
    else
    {
      return -Precision::Infinite();
    }
  }

  return theA + theB;
}

static double Mult(const double theA, const double theB)
{
  return theA * theB;
}

static double MultInf(const double theA, const double theB)
{
  if ((theA == 0.0) || (theB == 0.0))
  { // strictly zerro (without any tolerances)
    return 0.0;
  }

  if (Precision::IsPositiveInfinite(theA))
  {
    if (theB < 0.0)
    {
      return -Precision::Infinite();
    }
    else
    {
      return Precision::Infinite();
    }
  }

  if (Precision::IsPositiveInfinite(theB))
  {
    if (theA < 0.0)
    {
      return -Precision::Infinite();
    }
    else
    {
      return Precision::Infinite();
    }
  }

  if (Precision::IsNegativeInfinite(theA))
  {
    if (theB < 0.0)
    {
      return +Precision::Infinite();
    }
    else
    {
      return -Precision::Infinite();
    }
  }

  if (Precision::IsNegativeInfinite(theB))
  {
    if (theA < 0.0)
    {
      return +Precision::Infinite();
    }
    else
    {
      return -Precision::Infinite();
    }
  }

  return theA * theB;
}

//=================================================================================================

static size_t maxSubs(const size_t theN, const size_t theCoeff = 32)
{
  return static_cast<size_t>(IntegerLast()) / theCoeff < theN ? static_cast<size_t>(IntegerLast())
                                                              : theN * theCoeff + 1;
}

//=================================================================================================

template <typename theInertia>
static size_t fillIntervalBounds(const double                      theA,
                                 const double                      theB,
                                 const NCollection_Array1<double>& theKnots,
                                 const size_t                      theNumSubs,
                                 NCollection_Array1<theInertia>&   theInerts,
                                 NCollection_Array1<double>&       theParam1,
                                 NCollection_Array1<double>&       theParam2,
                                 NCollection_Array1<double>&       theError,
                                 NCollection_Array1<double>*       theCommonError)
{
  const size_t aSize = std::max(theKnots.Size(), maxSubs(theKnots.Size() - 1, theNumSubs));

  if (aSize > theParam1.Size())
  {
    theInerts.Resize(aSize, false);
    theParam1.Resize(aSize, false);
    theParam2.Resize(aSize, false);
    theError.Resize(aSize, false);
    theError.Init(0.0);

    if (theCommonError != nullptr)
    {
      theCommonError->Resize(aSize, false);
      theCommonError->Init(0.0);
    }
  }

  const auto aFirstKnot = std::upper_bound(theKnots.cbegin(), theKnots.cend(), theA);
  const auto aLastKnot  = std::lower_bound(aFirstKnot, theKnots.cend(), theB);
  const auto aNbKnots   = static_cast<size_t>(aLastKnot - aFirstKnot);

  theParam1.ChangeAt(0) = theA;
  std::copy(aFirstKnot, aLastKnot, theParam1.begin() + 1);
  std::copy(aFirstKnot, aLastKnot, theParam2.begin());
  theParam2.ChangeAt(aNbKnots) = theB;
  return aNbKnots + 1;
}
} // namespace

BRepGProp_Gauss::BRepGProp_Gauss(const GaussType theType)
    : myType(theType),
      add(::Add),
      mult(::Mult)
{
}

//=================================================================================================

void BRepGProp_Gauss::computeVInertiaOfElementaryPart(const gp_Pnt&             thePoint,
                                                      const gp_Vec&             theNormal,
                                                      const gp_Pnt&             theLocation,
                                                      const double              theWeight,
                                                      const double              theCoeff[],
                                                      const bool                theIsByPoint,
                                                      BRepGProp_Gauss::Inertia& theOutInertia)
{
  double x = thePoint.X() - theLocation.X();
  double y = thePoint.Y() - theLocation.Y();
  double z = thePoint.Z() - theLocation.Z();

  const double xn = theNormal.X() * theWeight;
  const double yn = theNormal.Y() * theWeight;
  const double zn = theNormal.Z() * theWeight;

  if (theIsByPoint)
  {
    /////////////////////                        ///////////////////////
    //    OFV code     //                        //    Initial code   //
    /////////////////////                        ///////////////////////
    // modified by APO

    double dv = x * xn + y * yn + z * zn; // xyz  = x * y * z;
    theOutInertia.Mass += dv / 3.0;       // Ixyi += zn * xyz;
    theOutInertia.Ix += 0.25 * x * dv;    // Iyzi += xn * xyz;
    theOutInertia.Iy += 0.25 * y * dv;    // Ixzi += yn * xyz;
    theOutInertia.Iz += 0.25 * z * dv;    // xi = x * x * x * xn / 3.0;
    x -= theCoeff[0];                     // yi = y * y * y * yn / 3.0;
    y -= theCoeff[1];                     // zi = z * z * z * zn / 3.0;
    z -= theCoeff[2];                     // Ixxi += (yi + zi);
    dv *= 0.2;                            // Iyyi += (xi + zi);
    theOutInertia.Ixy -= x * y * dv;      // Izzi += (xi + yi);
    theOutInertia.Iyz -= y * z * dv;      // x -= Coeff[0];
    theOutInertia.Ixz -= x * z * dv;      // y -= Coeff[1];
    x *= x;                               // z -= Coeff[2];
    y *= y;                               // dv = x * xn + y * yn + z * zn;
    z *= z;                               // dvi +=  dv;
    theOutInertia.Ixx += (y + z) * dv;    // Ixi += x * dv;
    theOutInertia.Iyy += (x + z) * dv;    // Iyi += y * dv;
    theOutInertia.Izz += (x + y) * dv;    // Izi += z * dv;
  }
  else
  { // By plane
    const double s = xn * theCoeff[0] + yn * theCoeff[1] + zn * theCoeff[2];

    double d1 = theCoeff[0] * x + theCoeff[1] * y + theCoeff[2] * z - theCoeff[3];
    double d2 = d1 * d1;
    double d3 = d1 * d2 / 3.0;
    double dv = s * d1;

    theOutInertia.Mass += dv;
    theOutInertia.Ix += (x - (theCoeff[0] * d1 * 0.5)) * dv;
    theOutInertia.Iy += (y - (theCoeff[1] * d1 * 0.5)) * dv;
    theOutInertia.Iz += (z - (theCoeff[2] * d1 * 0.5)) * dv;

    const double px = x - theCoeff[0] * d1;
    const double py = y - theCoeff[1] * d1;
    const double pz = z - theCoeff[2] * d1;

    x = px * px * d1 + px * theCoeff[0] * d2 + theCoeff[0] * theCoeff[0] * d3;
    y = py * py * d1 + py * theCoeff[1] * d2 + theCoeff[1] * theCoeff[1] * d3;
    z = pz * pz * d1 + pz * theCoeff[2] * d2 + theCoeff[2] * theCoeff[2] * d3;

    theOutInertia.Ixx += (y + z) * s;
    theOutInertia.Iyy += (x + z) * s;
    theOutInertia.Izz += (x + y) * s;

    d2 *= 0.5;
    x = (py * pz * d1) + (py * theCoeff[2] * d2) + (pz * theCoeff[1] * d2)
        + (theCoeff[1] * theCoeff[2] * d3);
    y = (px * pz * d1) + (pz * theCoeff[0] * d2) + (px * theCoeff[2] * d2)
        + (theCoeff[0] * theCoeff[2] * d3);
    z = (px * py * d1) + (px * theCoeff[1] * d2) + (py * theCoeff[0] * d2)
        + (theCoeff[0] * theCoeff[1] * d3);

    theOutInertia.Ixy -= z * s;
    theOutInertia.Iyz -= x * s;
    theOutInertia.Ixz -= y * s;
  }
}

//=================================================================================================

void BRepGProp_Gauss::computeSInertiaOfElementaryPart(const gp_Pnt&             thePoint,
                                                      const gp_Vec&             theNormal,
                                                      const gp_Pnt&             theLocation,
                                                      const double              theWeight,
                                                      BRepGProp_Gauss::Inertia& theOutInertia)
{
  // ds - Jacobien (x, y, z) -> (u, v) = ||n||
  const double ds = mult(theNormal.Magnitude(), theWeight);
  const double x  = add(thePoint.X(), -theLocation.X());
  const double y  = add(thePoint.Y(), -theLocation.Y());
  const double z  = add(thePoint.Z(), -theLocation.Z());

  theOutInertia.Mass = add(theOutInertia.Mass, ds);

  const double XdS = mult(x, ds);
  const double YdS = mult(y, ds);
  const double ZdS = mult(z, ds);

  theOutInertia.Ix  = add(theOutInertia.Ix, XdS);
  theOutInertia.Iy  = add(theOutInertia.Iy, YdS);
  theOutInertia.Iz  = add(theOutInertia.Iz, ZdS);
  theOutInertia.Ixy = add(theOutInertia.Ixy, mult(x, YdS));
  theOutInertia.Iyz = add(theOutInertia.Iyz, mult(y, ZdS));
  theOutInertia.Ixz = add(theOutInertia.Ixz, mult(x, ZdS));

  const double XXdS = mult(x, XdS);
  const double YYdS = mult(y, YdS);
  const double ZZdS = mult(z, ZdS);

  theOutInertia.Ixx = add(theOutInertia.Ixx, add(YYdS, ZZdS));
  theOutInertia.Iyy = add(theOutInertia.Iyy, add(XXdS, ZZdS));
  theOutInertia.Izz = add(theOutInertia.Izz, add(XXdS, YYdS));
}

//=================================================================================================

void BRepGProp_Gauss::checkBounds(const double theU1,
                                  const double theU2,
                                  const double theV1,
                                  const double theV2)
{
  if (Precision::IsInfinite(theU1) || Precision::IsInfinite(theU2) || Precision::IsInfinite(theV1)
      || Precision::IsInfinite(theV2))
  {
    add  = (::AddInf);
    mult = (::MultInf);
  }
}

//=================================================================================================

void BRepGProp_Gauss::addAndRestoreInertia(const BRepGProp_Gauss::Inertia& theInInertia,
                                           BRepGProp_Gauss::Inertia&       theOutInertia)
{
  theOutInertia.Mass = add(theOutInertia.Mass, theInInertia.Mass);
  theOutInertia.Ix   = add(theOutInertia.Ix, theInInertia.Ix);
  theOutInertia.Iy   = add(theOutInertia.Iy, theInInertia.Iy);
  theOutInertia.Iz   = add(theOutInertia.Iz, theInInertia.Iz);
  theOutInertia.Ixx  = add(theOutInertia.Ixx, theInInertia.Ixx);
  theOutInertia.Iyy  = add(theOutInertia.Iyy, theInInertia.Iyy);
  theOutInertia.Izz  = add(theOutInertia.Izz, theInInertia.Izz);
  theOutInertia.Ixy  = add(theOutInertia.Ixy, theInInertia.Ixy);
  theOutInertia.Ixz  = add(theOutInertia.Ixz, theInInertia.Ixz);
  theOutInertia.Iyz  = add(theOutInertia.Iyz, theInInertia.Iyz);
}

//=================================================================================================

void BRepGProp_Gauss::multAndRestoreInertia(const double              theValue,
                                            BRepGProp_Gauss::Inertia& theInOutInertia)
{
  theInOutInertia.Mass = mult(theInOutInertia.Mass, theValue);
  theInOutInertia.Ix   = mult(theInOutInertia.Ix, theValue);
  theInOutInertia.Iy   = mult(theInOutInertia.Iy, theValue);
  theInOutInertia.Iz   = mult(theInOutInertia.Iz, theValue);
  theInOutInertia.Ixx  = mult(theInOutInertia.Ixx, theValue);
  theInOutInertia.Iyy  = mult(theInOutInertia.Iyy, theValue);
  theInOutInertia.Izz  = mult(theInOutInertia.Izz, theValue);
  theInOutInertia.Ixy  = mult(theInOutInertia.Ixy, theValue);
  theInOutInertia.Ixz  = mult(theInOutInertia.Ixz, theValue);
  theInOutInertia.Iyz  = mult(theInOutInertia.Iyz, theValue);
}

//=================================================================================================

void BRepGProp_Gauss::convert(const BRepGProp_Gauss::Inertia& theInertia,
                              gp_Pnt&                         theOutGravityCenter,
                              gp_Mat&                         theOutMatrixOfInertia,
                              double&                         theOutMass)
{
  if (std::abs(theInertia.Mass) >= EPS_DIM)
  {
    const double anInvMass = 1.0 / theInertia.Mass;
    theOutGravityCenter.SetX(theInertia.Ix * anInvMass);
    theOutGravityCenter.SetY(theInertia.Iy * anInvMass);
    theOutGravityCenter.SetZ(theInertia.Iz * anInvMass);

    theOutMass = theInertia.Mass;
  }
  else
  {
    theOutMass = 0.0;
    theOutGravityCenter.SetCoord(0.0, 0.0, 0.0);
  }

  theOutMatrixOfInertia = gp_Mat(gp_XYZ(theInertia.Ixx, -theInertia.Ixy, -theInertia.Ixz),
                                 gp_XYZ(-theInertia.Ixy, theInertia.Iyy, -theInertia.Iyz),
                                 gp_XYZ(-theInertia.Ixz, -theInertia.Iyz, theInertia.Izz));
}

//=================================================================================================

void BRepGProp_Gauss::convert(const BRepGProp_Gauss::Inertia& theInertia,
                              const double                    theCoeff[],
                              const bool                      theIsByPoint,
                              gp_Pnt&                         theOutGravityCenter,
                              gp_Mat&                         theOutMatrixOfInertia,
                              double&                         theOutMass)
{
  convert(theInertia, theOutGravityCenter, theOutMatrixOfInertia, theOutMass);
  if (std::abs(theInertia.Mass) >= EPS_DIM && theIsByPoint)
  {
    const double anInvMass = 1.0 / theInertia.Mass;
    if (theIsByPoint)
    {
      theOutGravityCenter.SetX(theCoeff[0] + theInertia.Ix * anInvMass);
      theOutGravityCenter.SetY(theCoeff[1] + theInertia.Iy * anInvMass);
      theOutGravityCenter.SetZ(theCoeff[2] + theInertia.Iz * anInvMass);
    }
    else
    {
      theOutGravityCenter.SetX(theInertia.Ix * anInvMass);
      theOutGravityCenter.SetY(theInertia.Iy * anInvMass);
      theOutGravityCenter.SetZ(theInertia.Iz * anInvMass);
    }

    theOutMass = theInertia.Mass;
  }
  else
  {
    theOutMass = 0.0;
    theOutGravityCenter.SetCoord(0.0, 0.0, 0.0);
  }

  theOutMatrixOfInertia = gp_Mat(gp_XYZ(theInertia.Ixx, theInertia.Ixy, theInertia.Ixz),
                                 gp_XYZ(theInertia.Ixy, theInertia.Iyy, theInertia.Iyz),
                                 gp_XYZ(theInertia.Ixz, theInertia.Iyz, theInertia.Izz));
}

//=================================================================================================

double BRepGProp_Gauss::Compute(BRepGProp_Face&   theSurface,
                                BRepGProp_Domain& theDomain,
                                const gp_Pnt&     theLocation,
                                const double      theEps,
                                const double      theCoeff[],
                                const bool        theIsByPoint,
                                double&           theOutMass,
                                gp_Pnt&           theOutGravityCenter,
                                gp_Mat&           theOutInertia)
{
  const bool isErrorCalculation  = 0.0 > theEps || theEps < 0.001;
  const bool isVerifyComputation = 0.0 < theEps && theEps < 0.001;

  double anEpsilon = std::abs(theEps);

  BRepGProp_Gauss::Inertia    anInertia;
  NCollection_Array1<Inertia> anInertiaL(SM);
  NCollection_Array1<Inertia> anInertiaU(SM);

  // Prepare Gauss points and weights
  const int aNbGaussPoint = RealToInt(std::ceil(ERROR_ALGEBR_RATIO * GPM));

  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> LGaussP[2] = {
    NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>(static_cast<size_t>(GPM)),
    NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>(static_cast<size_t>(aNbGaussPoint))};
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> LGaussW[2] = {
    NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>(static_cast<size_t>(GPM)),
    NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>(static_cast<size_t>(aNbGaussPoint))};
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> UGaussP[2] = {
    NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>(static_cast<size_t>(GPM)),
    NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>(static_cast<size_t>(aNbGaussPoint))};
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> UGaussW[2] = {
    NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>(static_cast<size_t>(GPM)),
    NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE>(static_cast<size_t>(aNbGaussPoint))};

  NCollection_Array1<double> L1(SM);
  NCollection_Array1<double> L2(SM);
  NCollection_Array1<double> U1(SM);
  NCollection_Array1<double> U2(SM);

  NCollection_Array1<double> ErrL(SM);
  NCollection_Array1<double> ErrU(SM);
  NCollection_Array1<double> ErrUL(SM);
  ErrL.Init(0.0);
  ErrU.Init(0.0);
  ErrUL.Init(0.0);

  // Face parametrization in U and V direction
  double BV1, BV2, BU1, BU2;
  theSurface.Bounds(BU1, BU2, BV1, BV2);
  checkBounds(BU1, BU2, BV1, BV2);

  //
  const size_t       NumSubs = SUBS_POWER;
  const TopoDS_Face& aF      = theSurface.GetFace();
  // clang-format off
  const bool isNaturalRestriction = (aF.NbChildren () == 0); //theSurface.NaturalRestriction();
  // clang-format on

  double CIx, CIy, CIz, CIxy, CIxz, CIyz;
  double CDim[2], CIxx[2], CIyy[2], CIzz[2];

  // Boundary curve parametrization
  double u1 = BU1, u2, l1, l2, lm, lr, l, v;

  // On the boundary curve u-v
  gp_Pnt2d Puv;
  gp_Vec2d Vuv;
  double   Dul; // Dul = Du / Dl

  size_t iLS, iLSubEnd, LRange[2], iL, kL, IL, JL;
  size_t iUSubEnd, URange[2], kU, IU, JU;
  size_t UMaxSubs, LMaxSubs;
  int    iGL, iGLEnd, NbLGaussP[2];
  int    NbUGaussP[2];
  size_t kLEnd, kUEnd;

  double ErrorU, ErrorL, ErrorLMax = 0.0, Eps = 0.0, EpsL = 0.0, EpsU = 0.0;
  iGLEnd = isErrorCalculation ? 2 : 1;

  NbUGaussP[0] = theSurface.SIntOrder(anEpsilon);
  NbUGaussP[1] = RealToInt(std::ceil(ERROR_ALGEBR_RATIO * NbUGaussP[0]));

  initGaussPoints(NbUGaussP[0], UGaussP[0]);
  initGaussWeights(NbUGaussP[0], UGaussW[0]);
  initGaussPoints(NbUGaussP[1], UGaussP[1]);
  initGaussWeights(NbUGaussP[1], UGaussW[1]);

  const int                                            aNbUSubs = theSurface.SUIntSubs();
  NCollection_LocalArray<double, THE_KNOT_INLINE_SIZE> aUKnotStorage(
    static_cast<size_t>(aNbUSubs + 1));
  NCollection_Array1<double> UKnots = aUKnotStorage.ToArray1();
  UKnots.UpdateLowerBound(1);
  theSurface.UKnots(UKnots);

  NCollection_LocalArray<double, THE_KNOT_INLINE_SIZE> aLKnotStorage;
  while (isNaturalRestriction || theDomain.More())
  {
    if (isNaturalRestriction)
    {
      NbLGaussP[0] = std::min(2 * NbUGaussP[0], math::GaussPointsMax());
    }
    else
    {
      if (!theSurface.Load(theDomain.Value()))
      {
        return Precision::Infinite();
      }
      NbLGaussP[0] = theSurface.LIntOrder(anEpsilon);
    }

    NbLGaussP[1] = RealToInt(std::ceil(ERROR_ALGEBR_RATIO * NbLGaussP[0]));

    initGaussPoints(NbLGaussP[0], LGaussP[0]);
    initGaussWeights(NbLGaussP[0], LGaussW[0]);
    initGaussPoints(NbLGaussP[1], LGaussP[1]);
    initGaussWeights(NbLGaussP[1], LGaussW[1]);

    const int aNbLSubs = isNaturalRestriction ? theSurface.SVIntSubs() : theSurface.LIntSubs();
    NCollection_Array1<double> LKnots =
      prepareArray(aLKnotStorage, static_cast<size_t>(aNbLSubs + 1));
    LKnots.UpdateLowerBound(1);

    if (isNaturalRestriction)
    {
      theSurface.VKnots(LKnots);
      l1 = BV1;
      l2 = BV2;
    }
    else
    {
      theSurface.LKnots(LKnots);
      l1 = theSurface.FirstParameter();
      l2 = theSurface.LastParameter();
    }
    ErrorL = 0.0;
    kLEnd  = 1;
    JL     = 0;

    if (std::abs(l2 - l1) > EPS_PARAM)
    {
      iLSubEnd = fillIntervalBounds(l1, l2, LKnots, NumSubs, anInertiaL, L1, L2, ErrL, &ErrUL);
      LMaxSubs = maxSubs(iLSubEnd);

      if (LMaxSubs > SM)
      {
        LMaxSubs = SM;
      }

      std::for_each(anInertiaL.begin(), anInertiaL.end(), [](Inertia& theInertia) {
        theInertia.Mass = 0.0;
      });
      ErrL.Init(0.0);
      ErrUL.Init(0.0);

      do // while: L
      {
        if (++JL > iLSubEnd)
        {
          LRange[0] = IL =
            static_cast<size_t>(std::max_element(ErrL.cbegin(), ErrL.cend()) - ErrL.cbegin());
          LRange[1]           = JL - 1;
          L1.ChangeAt(JL - 1) = (L1.At(IL) + L2.At(IL)) * 0.5;
          L2.ChangeAt(JL - 1) = L2.At(IL);
          L2.ChangeAt(IL)     = L1.At(JL - 1);
        }
        else
        {
          LRange[0] = IL = JL - 1;
        }

        if (JL == LMaxSubs || std::abs(L2.At(JL - 1) - L1.At(JL - 1)) < EPS_PARAM)
        {
          if (kLEnd == 1)
          {
            anInertiaL.ChangeAt(JL - 1) = Inertia();
            ErrL.ChangeAt(JL - 1)       = 0.0;
          }
          else
          {
            --JL;
            EpsL = ErrorL;
            Eps  = EpsL / 0.9;
            break;
          }
        }
        else
        {
          for (kL = 0; kL < kLEnd; kL++)
          {
            iLS = LRange[kL];
            lm  = 0.5 * (L2.At(iLS) + L1.At(iLS));
            lr  = 0.5 * (L2.At(iLS) - L1.At(iLS));

            CIx = CIy = CIz = CIxy = CIxz = CIyz = 0.0;

            for (iGL = 0; iGL < iGLEnd; ++iGL)
            {
              CDim[iGL] = CIxx[iGL] = CIyy[iGL] = CIzz[iGL] = 0.0;

              for (iL = 0; iL < static_cast<size_t>(NbLGaussP[iGL]); ++iL)
              {
                l = lm + lr * LGaussP[iGL].begin()[iL];
                if (isNaturalRestriction)
                {
                  v   = l;
                  u2  = BU2;
                  Dul = LGaussW[iGL].begin()[iL];
                }
                else
                {
                  theSurface.D12d(l, Puv, Vuv);
                  Dul = Vuv.Y() * LGaussW[iGL].begin()[iL]; // Dul = Du / Dl

                  if (std::abs(Dul) < EPS_PARAM)
                  {
                    continue;
                  }

                  v  = Puv.Y();
                  u2 = Puv.X();

                  // Check on cause out off bounds of value current parameter
                  if (v < BV1)
                  {
                    v = BV1;
                  }
                  else if (v > BV2)
                  {
                    v = BV2;
                  }

                  if (u2 < BU1)
                  {
                    u2 = BU1;
                  }
                  else if (u2 > BU2)
                  {
                    u2 = BU2;
                  }
                }

                ErrUL.ChangeAt(iLS) = 0.0;
                kUEnd               = 1;
                JU                  = 0;

                if (std::abs(u2 - u1) < EPS_PARAM)
                {
                  continue;
                }

                iUSubEnd =
                  fillIntervalBounds(u1, u2, UKnots, NumSubs, anInertiaU, U1, U2, ErrU, nullptr);
                UMaxSubs = maxSubs(iUSubEnd);

                if (UMaxSubs > SM)
                {
                  UMaxSubs = SM;
                }

                std::for_each(anInertiaU.begin(), anInertiaU.end(), [](Inertia& theInertia) {
                  theInertia.Mass = 0.0;
                });
                ErrU.Init(0.0);
                ErrorU = 0.0;

                do
                { // while: U
                  if (++JU > iUSubEnd)
                  {
                    URange[0] = IU = static_cast<size_t>(
                      std::max_element(ErrU.cbegin(), ErrU.cend()) - ErrU.cbegin());
                    URange[1] = JU - 1;

                    U1.ChangeAt(JU - 1) = (U1.At(IU) + U2.At(IU)) * 0.5;
                    U2.ChangeAt(JU - 1) = U2.At(IU);
                    U2.ChangeAt(IU)     = U1.At(JU - 1);
                  }
                  else
                  {
                    URange[0] = IU = JU - 1;
                  }

                  if (JU == UMaxSubs || std::abs(U2.At(JU - 1) - U1.At(JU - 1)) < EPS_PARAM)
                  {
                    if (kUEnd == 1)
                    {
                      ErrU.ChangeAt(JU - 1)       = 0.0;
                      anInertiaU.ChangeAt(JU - 1) = Inertia();
                    }
                    else
                    {
                      --JU;
                      EpsU = ErrorU;
                      Eps  = 10. * EpsU * std::abs((u2 - u1) * Dul);
                      EpsL = 0.9 * Eps;
                      break;
                    }
                  }
                  else
                  {
                    gp_Pnt aPoint;
                    gp_Vec aNormal;

                    for (kU = 0; kU < kUEnd; ++kU)
                    {
                      BRepGProp_Gauss::Inertia aLocal[2];

                      const size_t iUS     = URange[kU];
                      const int    aLength = iGLEnd - iGL;

                      const double um = 0.5 * (U2.At(iUS) + U1.At(iUS));
                      const double ur = 0.5 * (U2.At(iUS) - U1.At(iUS));

                      for (size_t iGU = 0; iGU < static_cast<size_t>(aLength); ++iGU)
                      {
                        for (size_t iU = 0; iU < static_cast<size_t>(NbUGaussP[iGU]); ++iU)
                        {
                          double       w = UGaussW[iGU].begin()[iU];
                          const double u = um + ur * UGaussP[iGU].begin()[iU];

                          theSurface.Normal(u, v, aPoint, aNormal);

                          if (myType == GaussType::Vinert)
                          {
                            computeVInertiaOfElementaryPart(aPoint,
                                                            aNormal,
                                                            theLocation,
                                                            w,
                                                            theCoeff,
                                                            theIsByPoint,
                                                            aLocal[iGU]);
                          }
                          else
                          {
                            if (iGU > 0)
                            {
                              aLocal[iGU].Mass += (w * aNormal.Magnitude());
                            }
                            else
                            {
                              computeSInertiaOfElementaryPart(aPoint,
                                                              aNormal,
                                                              theLocation,
                                                              w,
                                                              aLocal[iGU]);
                            }
                          }
                        }
                      }

                      BRepGProp_Gauss::Inertia& anUI = anInertiaU.ChangeAt(iUS);

                      anUI.Mass = mult(aLocal[0].Mass, ur);

                      if (myType == GaussType::Vinert)
                      {
                        anUI.Ixx = mult(aLocal[0].Ixx, ur);
                        anUI.Iyy = mult(aLocal[0].Iyy, ur);
                        anUI.Izz = mult(aLocal[0].Izz, ur);
                      }

                      if (iGL > 0)
                      {
                        continue;
                      }

                      double aDMass = std::abs(aLocal[1].Mass - aLocal[0].Mass);

                      if (myType == GaussType::Vinert)
                      {
                        aLocal[1].Ixx = std::abs(aLocal[1].Ixx - aLocal[0].Ixx);
                        aLocal[1].Iyy = std::abs(aLocal[1].Iyy - aLocal[0].Iyy);
                        aLocal[1].Izz = std::abs(aLocal[1].Izz - aLocal[0].Izz);

                        anUI.Ix = mult(aLocal[0].Ix, ur);
                        anUI.Iy = mult(aLocal[0].Iy, ur);
                        anUI.Iz = mult(aLocal[0].Iz, ur);

                        anUI.Ixy = mult(aLocal[0].Ixy, ur);
                        anUI.Ixz = mult(aLocal[0].Ixz, ur);
                        anUI.Iyz = mult(aLocal[0].Iyz, ur);

                        ErrU.ChangeAt(iUS) = mult(aDMass, ur);
                      }
                      else
                      {
                        anUI.Ix  = mult(aLocal[0].Ix, ur);
                        anUI.Iy  = mult(aLocal[0].Iy, ur);
                        anUI.Iz  = mult(aLocal[0].Iz, ur);
                        anUI.Ixx = mult(aLocal[0].Ixx, ur);
                        anUI.Iyy = mult(aLocal[0].Iyy, ur);
                        anUI.Izz = mult(aLocal[0].Izz, ur);
                        anUI.Ixy = mult(aLocal[0].Ixy, ur);
                        anUI.Ixz = mult(aLocal[0].Ixz, ur);
                        anUI.Iyz = mult(aLocal[0].Iyz, ur);

                        ErrU.ChangeAt(iUS) = mult(aDMass, ur);
                      }
                    }
                  }

                  if (JU == iUSubEnd)
                  {
                    kUEnd  = 2;
                    ErrorU = *std::max_element(ErrU.cbegin(), ErrU.cend());
                  }
                } while ((ErrorU - EpsU > 0.0 && EpsU != 0.0) || kUEnd == 1);

                for (size_t anIndex = 0; anIndex < JU; ++anIndex)
                {
                  const BRepGProp_Gauss::Inertia& anIU = anInertiaU.At(anIndex);

                  CDim[iGL] = add(CDim[iGL], mult(anIU.Mass, Dul));
                  CIxx[iGL] = add(CIxx[iGL], mult(anIU.Ixx, Dul));
                  CIyy[iGL] = add(CIyy[iGL], mult(anIU.Iyy, Dul));
                  CIzz[iGL] = add(CIzz[iGL], mult(anIU.Izz, Dul));
                }

                if (iGL > 0)
                {
                  continue;
                }

                ErrUL.ChangeAt(iLS) = ErrorU * std::abs((u2 - u1) * Dul);

                for (size_t anIndex = 0; anIndex < JU; ++anIndex)
                {
                  const BRepGProp_Gauss::Inertia& anIU = anInertiaU.At(anIndex);

                  CIx = add(CIx, mult(anIU.Ix, Dul));
                  CIy = add(CIy, mult(anIU.Iy, Dul));
                  CIz = add(CIz, mult(anIU.Iz, Dul));

                  CIxy = add(CIxy, mult(anIU.Ixy, Dul));
                  CIxz = add(CIxz, mult(anIU.Ixz, Dul));
                  CIyz = add(CIyz, mult(anIU.Iyz, Dul));
                }
              } // for: iL
            } // for: iGL

            BRepGProp_Gauss::Inertia& aLI = anInertiaL.ChangeAt(iLS);

            aLI.Mass = mult(CDim[0], lr);
            aLI.Ixx  = mult(CIxx[0], lr);
            aLI.Iyy  = mult(CIyy[0], lr);
            aLI.Izz  = mult(CIzz[0], lr);

            if (iGLEnd == 2)
            {
              double aSubDim = std::abs(CDim[1] - CDim[0]);

              if (myType == GaussType::Vinert)
              {
                ErrorU = ErrUL.At(iLS);

                CIxx[1] = std::abs(CIxx[1] - CIxx[0]);
                CIyy[1] = std::abs(CIyy[1] - CIyy[0]);
                CIzz[1] = std::abs(CIzz[1] - CIzz[0]);

                ErrL.ChangeAt(iLS) = add(mult(aSubDim, lr), ErrorU);
              }
              else
              {
                ErrL.ChangeAt(iLS) = add(mult(aSubDim, lr), ErrUL.At(iLS));
              }
            }

            aLI.Ix = mult(CIx, lr);
            aLI.Iy = mult(CIy, lr);
            aLI.Iz = mult(CIz, lr);

            aLI.Ixy = mult(CIxy, lr);
            aLI.Ixz = mult(CIxz, lr);
            aLI.Iyz = mult(CIyz, lr);
          } // for: (kL)iLS
        }

        // Calculate/correct epsilon of computation by current value of dim
        // That is need for not spend time for
        if (JL == iLSubEnd)
        {
          kLEnd = 2;

          double DDim = 0.0;
          for (size_t anIndex = 0; anIndex < JL; ++anIndex)
          {
            DDim += anInertiaL.At(anIndex).Mass;
          }

          DDim = std::abs(DDim * anEpsilon);

          if (DDim > Eps)
          {
            Eps  = DDim;
            EpsL = 0.9 * Eps;
          }
        }
        if (kLEnd == 2)
        {
          ErrorL = *std::max_element(ErrL.cbegin(), ErrL.cend());
        }
      } while ((ErrorL - EpsL > 0.0 && isVerifyComputation) || kLEnd == 1);

      for (size_t anIndex = 0; anIndex < JL; ++anIndex)
      {
        addAndRestoreInertia(anInertiaL.At(anIndex), anInertia);
      }

      ErrorLMax = std::max(ErrorLMax, ErrorL);
    }

    if (isNaturalRestriction)
    {
      break;
    }

    theDomain.Next();
  }

  if (myType == GaussType::Vinert)
  {
    convert(anInertia, theCoeff, theIsByPoint, theOutGravityCenter, theOutInertia, theOutMass);
  }
  else
  {
    convert(anInertia, theOutGravityCenter, theOutInertia, theOutMass);
  }

  if (iGLEnd == 2)
  {
    if (theOutMass != 0.0)
    {
      Eps = ErrorLMax / std::abs(theOutMass);
    }
    else
    {
      Eps = 0.0;
    }
  }
  else
  {
    Eps = anEpsilon;
  }

  return Eps;
}

//=================================================================================================

double BRepGProp_Gauss::Compute(BRepGProp_Face&   theSurface,
                                BRepGProp_Domain& theDomain,
                                const gp_Pnt&     theLocation,
                                const double      theEps,
                                double&           theOutMass,
                                gp_Pnt&           theOutGravityCenter,
                                gp_Mat&           theOutInertia)
{
  Standard_ASSERT_RAISE(myType == GaussType::Sinert, "BRepGProp_Gauss: Incorrect type");

  return Compute(theSurface,
                 theDomain,
                 theLocation,
                 theEps,
                 nullptr,
                 true,
                 theOutMass,
                 theOutGravityCenter,
                 theOutInertia);
}

//=================================================================================================

void BRepGProp_Gauss::Compute(BRepGProp_Face&   theSurface,
                              BRepGProp_Domain& theDomain,
                              const gp_Pnt&     theLocation,
                              double&           theOutMass,
                              gp_Pnt&           theOutGravityCenter,
                              gp_Mat&           theOutInertia)
{
  Standard_ASSERT_RAISE(myType == GaussType::Sinert, "BRepGProp_Gauss: Incorrect type");

  double u1, u2, v1, v2;
  theSurface.Bounds(u1, u2, v1, v2);
  checkBounds(u1, u2, v1, v2);

  const int NbUGaussgp_Pnts = std::min(theSurface.UIntegrationOrder(), math::GaussPointsMax());

  const int NbVGaussgp_Pnts = std::min(theSurface.VIntegrationOrder(), math::GaussPointsMax());

  const int NbGaussgp_Pnts = std::max(NbUGaussgp_Pnts, NbVGaussgp_Pnts);

  // Number of Gauss points for the integration on the face
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussSPVStorage(
    static_cast<size_t>(NbGaussgp_Pnts));
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussSWVStorage(
    static_cast<size_t>(NbGaussgp_Pnts));
  NCollection_Array1<double> GaussSPV = aGaussSPVStorage.ToArray1();
  NCollection_Array1<double> GaussSWV = aGaussSWVStorage.ToArray1();
  initGaussPoints(NbGaussgp_Pnts, aGaussSPVStorage);
  initGaussWeights(NbGaussgp_Pnts, aGaussSWVStorage);

  BRepGProp_Gauss::Inertia                              anInertia;
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussCPStorage;
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussCWStorage;
  for (; theDomain.More(); theDomain.Next())
  {
    if (!theSurface.Load(theDomain.Value()))
    {
      return;
    }

    int NbCGaussgp_Pnts = std::min(theSurface.IntegrationOrder(), math::GaussPointsMax());

    NbCGaussgp_Pnts = std::max(NbCGaussgp_Pnts, NbGaussgp_Pnts);

    initGaussPoints(NbCGaussgp_Pnts, aGaussCPStorage);
    initGaussWeights(NbCGaussgp_Pnts, aGaussCWStorage);
    NCollection_Array1<double> GaussCP =
      prepareArray(aGaussCPStorage, static_cast<size_t>(NbCGaussgp_Pnts));
    NCollection_Array1<double> GaussCW =
      prepareArray(aGaussCWStorage, static_cast<size_t>(NbCGaussgp_Pnts));

    const double l1 = theSurface.FirstParameter();
    const double l2 = theSurface.LastParameter();
    const double lm = 0.5 * (l2 + l1);
    const double lr = 0.5 * (l2 - l1);

    BRepGProp_Gauss::Inertia aCInertia;
    for (size_t i = 0; i < static_cast<size_t>(NbCGaussgp_Pnts); ++i)
    {
      const double l = lm + lr * GaussCP.At(i);

      gp_Pnt2d Puv;
      gp_Vec2d Vuv;
      theSurface.D12d(l, Puv, Vuv);

      const double v = Puv.Y();
      u2             = Puv.X();

      const double Dul = Vuv.Y() * GaussCW.At(i);
      const double um  = 0.5 * (u2 + u1);
      const double ur  = 0.5 * (u2 - u1);

      BRepGProp_Gauss::Inertia aLocalInertia;
      for (size_t j = 0; j < static_cast<size_t>(NbGaussgp_Pnts); ++j)
      {
        const double u       = add(um, mult(ur, GaussSPV.At(j)));
        const double aWeight = Dul * GaussSWV.At(j);

        gp_Pnt aPoint;
        gp_Vec aNormal;
        theSurface.Normal(u, v, aPoint, aNormal);

        computeSInertiaOfElementaryPart(aPoint, aNormal, theLocation, aWeight, aLocalInertia);
      }

      multAndRestoreInertia(ur, aLocalInertia);
      addAndRestoreInertia(aLocalInertia, aCInertia);
    }

    multAndRestoreInertia(lr, aCInertia);
    addAndRestoreInertia(aCInertia, anInertia);
  }

  convert(anInertia, theOutGravityCenter, theOutInertia, theOutMass);
}

//=================================================================================================

void BRepGProp_Gauss::Compute(BRepGProp_Face&   theSurface,
                              BRepGProp_Domain& theDomain,
                              const gp_Pnt&     theLocation,
                              const double      theCoeff[],
                              const bool        theIsByPoint,
                              double&           theOutMass,
                              gp_Pnt&           theOutGravityCenter,
                              gp_Mat&           theOutInertia)
{
  Standard_ASSERT_RAISE(myType == GaussType::Vinert, "BRepGProp_Gauss: Incorrect type");

  double u1, v1, u2, v2;
  theSurface.Bounds(u1, u2, v1, v2);
  checkBounds(u1, u2, v1, v2);

  double _u2 = u2; // OCC104

  BRepGProp_Gauss::Inertia                              anInertia;
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussPStorage;
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussWStorage;
  for (; theDomain.More(); theDomain.Next())
  {
    if (!theSurface.Load(theDomain.Value()))
    {
      return;
    }

    const int aVNbCGaussgp_Pnts = theSurface.VIntegrationOrder();

    const int aNbGaussgp_Pnts =
      std::min(std::max(theSurface.IntegrationOrder(), aVNbCGaussgp_Pnts), math::GaussPointsMax());

    initGaussPoints(aNbGaussgp_Pnts, aGaussPStorage);
    initGaussWeights(aNbGaussgp_Pnts, aGaussWStorage);
    NCollection_Array1<double> GaussP =
      prepareArray(aGaussPStorage, static_cast<size_t>(aNbGaussgp_Pnts));
    NCollection_Array1<double> GaussW =
      prepareArray(aGaussWStorage, static_cast<size_t>(aNbGaussgp_Pnts));

    const double l1 = theSurface.FirstParameter();
    const double l2 = theSurface.LastParameter();
    const double lm = 0.5 * (l2 + l1);
    const double lr = 0.5 * (l2 - l1);

    BRepGProp_Gauss::Inertia aCInertia;
    for (size_t i = 0; i < static_cast<size_t>(aNbGaussgp_Pnts); ++i)
    {
      const double l = lm + lr * GaussP.At(i);

      gp_Pnt2d Puv;
      gp_Vec2d Vuv;

      theSurface.D12d(l, Puv, Vuv);

      u2             = Puv.X();
      u2             = std::min(std::max(u1, u2), _u2); // OCC104
      const double v = std::min(std::max(Puv.Y(), v1), v2);

      const double Dul = Vuv.Y() * GaussW.At(i);
      const double um  = 0.5 * (u2 + u1);
      const double ur  = 0.5 * (u2 - u1);

      BRepGProp_Gauss::Inertia aLocalInertia;
      for (size_t j = 0; j < static_cast<size_t>(aNbGaussgp_Pnts); ++j)
      {
        const double u       = um + ur * GaussP.At(j);
        const double aWeight = Dul * GaussW.At(j);

        gp_Pnt aPoint;
        gp_Vec aNormal;

        theSurface.Normal(u, v, aPoint, aNormal);

        computeVInertiaOfElementaryPart(aPoint,
                                        aNormal,
                                        theLocation,
                                        aWeight,
                                        theCoeff,
                                        theIsByPoint,
                                        aLocalInertia);
      }

      multAndRestoreInertia(ur, aLocalInertia);
      addAndRestoreInertia(aLocalInertia, aCInertia);
    }

    multAndRestoreInertia(lr, aCInertia);
    addAndRestoreInertia(aCInertia, anInertia);
  }

  convert(anInertia, theCoeff, theIsByPoint, theOutGravityCenter, theOutInertia, theOutMass);
}

//=================================================================================================

void BRepGProp_Gauss::Compute(const BRepGProp_Face& theSurface,
                              const gp_Pnt&         theLocation,
                              const double          theCoeff[],
                              const bool            theIsByPoint,
                              double&               theOutMass,
                              gp_Pnt&               theOutGravityCenter,
                              gp_Mat&               theOutInertia)
{
  double LowerU, UpperU, LowerV, UpperV;
  theSurface.Bounds(LowerU, UpperU, LowerV, UpperV);
  checkBounds(LowerU, UpperU, LowerV, UpperV);

  const int UOrder = std::min(theSurface.UIntegrationOrder(), math::GaussPointsMax());
  const int VOrder = std::min(theSurface.VIntegrationOrder(), math::GaussPointsMax());

  // Gauss points and weights
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussPUStorage(
    static_cast<size_t>(UOrder));
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussWUStorage(
    static_cast<size_t>(UOrder));
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussPVStorage(
    static_cast<size_t>(VOrder));
  NCollection_LocalArray<double, THE_GAUSS_INLINE_SIZE> aGaussWVStorage(
    static_cast<size_t>(VOrder));
  NCollection_Array1<double> GaussPU = aGaussPUStorage.ToArray1();
  NCollection_Array1<double> GaussWU = aGaussWUStorage.ToArray1();
  NCollection_Array1<double> GaussPV = aGaussPVStorage.ToArray1();
  NCollection_Array1<double> GaussWV = aGaussWVStorage.ToArray1();

  initGaussPoints(UOrder, aGaussPUStorage);
  initGaussWeights(UOrder, aGaussWUStorage);
  initGaussPoints(VOrder, aGaussPVStorage);
  initGaussWeights(VOrder, aGaussWVStorage);

  const double um = 0.5 * add(UpperU, LowerU);
  const double vm = 0.5 * add(UpperV, LowerV);
  double       ur = 0.5 * add(UpperU, -LowerU);
  double       vr = 0.5 * add(UpperV, -LowerV);

  gp_Pnt aPoint;
  gp_Vec aNormal;

  BRepGProp_Gauss::Inertia anInertia;
  for (size_t j = 0; j < static_cast<size_t>(VOrder); ++j)
  {
    BRepGProp_Gauss::Inertia anInertiaOfElementaryPart;
    const double             v = add(vm, mult(vr, GaussPV.At(j)));

    for (size_t i = 0; i < static_cast<size_t>(UOrder); ++i)
    {
      const double aWeight = GaussWU.At(i);
      const double u       = add(um, mult(ur, GaussPU.At(i)));
      theSurface.Normal(u, v, aPoint, aNormal);

      if (myType == GaussType::Vinert)
      {
        computeVInertiaOfElementaryPart(aPoint,
                                        aNormal,
                                        theLocation,
                                        aWeight,
                                        theCoeff,
                                        theIsByPoint,
                                        anInertiaOfElementaryPart);
      }
      else // Sinert
      {
        computeSInertiaOfElementaryPart(aPoint,
                                        aNormal,
                                        theLocation,
                                        aWeight,
                                        anInertiaOfElementaryPart);
      }
    }

    multAndRestoreInertia(GaussWV.At(j), anInertiaOfElementaryPart);
    addAndRestoreInertia(anInertiaOfElementaryPart, anInertia);
  }
  vr            = mult(vr, ur);
  anInertia.Ixx = mult(vr, anInertia.Ixx);
  anInertia.Iyy = mult(vr, anInertia.Iyy);
  anInertia.Izz = mult(vr, anInertia.Izz);
  anInertia.Ixy = mult(vr, anInertia.Ixy);
  anInertia.Ixz = mult(vr, anInertia.Ixz);
  anInertia.Iyz = mult(vr, anInertia.Iyz);

  if (myType == GaussType::Vinert)
  {
    convert(anInertia, theCoeff, theIsByPoint, theOutGravityCenter, theOutInertia, theOutMass);
  }
  else // Sinert
  {
    convert(anInertia, theOutGravityCenter, theOutInertia, theOutMass);
  }

  theOutMass *= vr;
}

//=================================================================================================

void BRepGProp_Gauss::Compute(const BRepGProp_Face& theSurface,
                              const gp_Pnt&         theLocation,
                              double&               theOutMass,
                              gp_Pnt&               theOutGravityCenter,
                              gp_Mat&               theOutInertia)
{
  Standard_ASSERT_RAISE(myType == GaussType::Sinert, "BRepGProp_Gauss: Incorrect type");

  Compute(theSurface, theLocation, nullptr, true, theOutMass, theOutGravityCenter, theOutInertia);
}
