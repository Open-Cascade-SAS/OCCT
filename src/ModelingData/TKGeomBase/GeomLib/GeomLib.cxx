// Created on: 1993-07-07
// Created by: Jean Claude VAUTHIER
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

#include <GeomLib.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <CSLib.hxx>
#include <CSLib_NormalStatus.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLib_DenominatorMultiplier.hxx>
#include <GeomLib_LogSample.hxx>
#include <GeomLib_MakeCurvefromApprox.hxx>
#include <GeomLib_PolyFunc.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_TrsfForm.hxx>
#include <gp_Vec.hxx>
#include <Hermit.hxx>
#include <math.hxx>
#include <math_FunctionAllRoots.hxx>
#include <math_FunctionSample.hxx>
#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <NCollection_Array1.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>

//

namespace
{
// Curve extension and lambda optimization.
constexpr int    THE_MAX_EXTENSION_CONTINUITY    = 3;
constexpr double THE_LAMBDA_MIN_FACTOR           = 1.0e-3;
constexpr double THE_LAMBDA_MAX_FACTOR           = 50.0;
constexpr int    THE_LAMBDA_SAMPLE_COUNT         = 100;
constexpr double THE_LAMBDA_ROOT_VALUE_TOLERANCE = 1.0e-15;
constexpr double THE_CURVE_EXTENSION_TOLERANCE   = 1.0e-6;
constexpr int    THE_CURVE_SPEED_SAMPLE_COUNT    = 9;
constexpr double THE_MIN_CURVE_SPEED_RATIO       = 0.75;
constexpr double THE_MAX_CURVE_SPEED_RATIO       = 1.5;

// Surface conversion and extension.
constexpr double THE_SURFACE_APPROX_TOLERANCE    = Precision::Confusion();
constexpr int    THE_SURFACE_APPROX_MAX_DEGREE   = 14;
constexpr int    THE_SURFACE_APPROX_MAX_SEGMENTS = 16;
constexpr int    THE_SURFACE_APPROX_PRECISION    = 1;
constexpr int    THE_MAX_SURFACE_DEGREE_RETRIES  = 2;
constexpr int    THE_MAX_SURFACE_WEIGHT_ATTEMPTS = 5;
constexpr double THE_SURFACE_TANGENT_TOLERANCE   = 1.0e-12;
constexpr double THE_TANGENT_REVERSAL_ANGLE      = 2.0;
constexpr double THE_SURFACE_WEIGHT_TOLERANCE    = 10.0 * Precision::PConfusion();

// Normal recovery near a singular surface point.
constexpr double THE_NORMAL_PARAMETER_STEP         = 1.0e-5;
constexpr double THE_NORMAL_SQUARED_MAGNITUDE_TOL  = 1.0e-16;
constexpr double THE_CONE_NORMAL_ANGULAR_TOLERANCE = 1.0e-4;

// Geometric sampling.
constexpr int THE_CLOSEDNESS_SAMPLE_COUNT = 23;
constexpr int THE_ISOLINE_CHECK_SEGMENTS  = 23;
} // namespace

static bool CompareWeightPoles(const NCollection_Array1<gp_Pnt>&       thePoles1,
                               const NCollection_Array1<double>* const theW1,
                               const NCollection_Array1<gp_Pnt>&       thePoles2,
                               const NCollection_Array1<double>* const theW2,
                               const double                            theTol);

//=================================================================================================
static void ComputeLambda(const math_Matrix& Constraint,
                          const math_Matrix& Hermit,
                          const double       Length,
                          double&            Lambda)
{
  const size_t aHermiteOrder = Hermit.RowSize();
  const int    Continuity    = static_cast<int>(aHermiteOrder) - 2;
  int          ip;

  // Minimization
  math_Matrix HDer(aHermiteOrder - 1, aHermiteOrder);
  for (size_t aColumn = 0; aColumn < aHermiteOrder; ++aColumn)
  {
    for (size_t aRow = 0; aRow + 1 < aHermiteOrder; ++aRow)
    {
      HDer.ChangeAt(aRow, aColumn) =
        static_cast<double>(aRow + 1) * Hermit.At(aColumn, aRow + 1);
    }
  }

  math_Vector V(1, static_cast<int>(aHermiteOrder));
  math_Vector Vec1(1, Constraint.RowNumber());
  math_Vector Vec2(1, Constraint.RowNumber());
  math_Vector Vec3(1, Constraint.RowNumber());
  math_Vector Vec4(1, Constraint.RowNumber());

  double* polynome = &HDer.ChangeAt(0, 0);
  double* valhder  = &V(1);
  Vec2             = Constraint.Col(2);
  Vec2 /= Length;
  double t, squared1 = Vec2.Norm2(), GW;

  int         GOrdre = 4 + 4 * Continuity, DDim = Continuity * (Continuity + 2);
  math_Vector GaussP(1, GOrdre), GaussW(1, GOrdre), pol2(1, 2 * Continuity + 1),
    pol4(1, 4 * Continuity + 1);
  math::GaussPoints(GOrdre, GaussP);
  math::GaussWeights(GOrdre, GaussW);
  pol4.Init(0.);

  for (ip = 1; ip <= GOrdre; ip++)
  {
    t  = (GaussP(ip) + 1.) / 2;
    GW = GaussW(ip);
    PLib::NoDerivativeEvalPolynomial(t, Continuity, Continuity + 2, DDim, polynome[0], valhder[0]);
    V /= Length; // Normalisation

    //                      i
    // C'(t) = SUM Vi*Lambda
    Vec1 = Constraint.Col(1);
    Vec1 *= V(1);
    Vec1 += V.At(aHermiteOrder - 1) * Constraint.Col(Constraint.UpperCol());
    Vec2 = Constraint.Col(2);
    Vec2 *= V(2);
    if (Continuity > 1)
    {
      Vec3 = Constraint.Col(3);
      Vec3 *= V(3);
      if (Continuity > 2)
      {
        Vec4 = Constraint.Col(4);
        Vec4 *= V(4);
      }
    }

    //   2          2
    // C'(t) - C'(0)

    pol2(1) = Vec1.Norm2();
    pol2(2) = 2 * (Vec1.Multiplied(Vec2));
    pol2(3) = Vec2.Norm2() - squared1;
    if (Continuity > 1)
    {
      pol2(3) += 2 * (Vec1.Multiplied(Vec3));
      pol2(4) = 2 * (Vec2.Multiplied(Vec3));
      pol2(5) = Vec3.Norm2();
      if (Continuity > 2)
      {
        pol2(4) += 2 * (Vec1.Multiplied(Vec4));
        pol2(5) += 2 * (Vec2.Multiplied(Vec4));
        pol2(6) = 2 * (Vec3.Multiplied(Vec4));
        pol2(7) = Vec4.Norm2();
      }
    }

    //                     2      2  2
    // Integrale de ( C'(t) - C'(0) )
    for (size_t aFirstIndex = 0; aFirstIndex < pol2.Size(); ++aFirstIndex)
    {
      size_t aResultIndex = aFirstIndex;
      for (size_t aSecondIndex = 0; aSecondIndex < aFirstIndex;
           ++aSecondIndex, ++aResultIndex)
      {
        pol4.ChangeAt(aResultIndex) +=
          2.0 * GW * pol2.At(aFirstIndex) * pol2.At(aSecondIndex);
      }
      pol4.ChangeAt(2 * aFirstIndex) += GW * std::pow(pol2.At(aFirstIndex), 2);
    }
  }

  double EMin, E;
  PLib::NoDerivativeEvalPolynomial(Lambda, pol4.Length() - 1, 1, pol4.Length() - 1, pol4(1), EMin);

  if (EMin > Precision::Confusion())
  {
    // Search for extrema of the function
    GeomLib_PolyFunc      FF(pol4);
    GeomLib_LogSample     S(Lambda * THE_LAMBDA_MIN_FACTOR,
                            Lambda * THE_LAMBDA_MAX_FACTOR,
                            THE_LAMBDA_SAMPLE_COUNT);
    math_FunctionAllRoots Solve(FF,
                                S,
                                Precision::Confusion(),
                                Precision::Confusion() * (Length + 1),
                                THE_LAMBDA_ROOT_VALUE_TOLERANCE);
    if (Solve.IsDone())
    {
      for (int aPointIndex = 1; aPointIndex <= Solve.NbPoints(); ++aPointIndex)
      {
        t = Solve.GetPoint(aPointIndex);
        PLib::NoDerivativeEvalPolynomial(t, pol4.Length() - 1, 1, pol4.Length() - 1, pol4(1), E);
        if (E < EMin)
        {
          Lambda = t;
          EMin   = E;
        }
      }
    }
  }
}

#include <Extrema_LocateExtPC.hxx>

//=================================================================================================

void GeomLib::RemovePointsFromArray(const int                         NumPoints,
                                    const NCollection_Array1<double>& InParameters,
                                    NCollection_Array1<double>&       OutParameters)
{
  NCollection_LinearVector<double> aSelectedParameters;
  aSelectedParameters.Append(InParameters.First());
  if (NumPoints <= 2)
  {
    aSelectedParameters.Append(InParameters.Last());
  }
  else
  {
    const int    aRequestedInteriorCount = std::max(0, NumPoints - 2);
    const double aDelta                  = (InParameters.Last() - InParameters.First())
                                           / static_cast<double>(aRequestedInteriorCount + 1);
    double       aCurrentParameter       = InParameters.First() + 0.5 * aDelta;
    size_t       anInputIndex            = 1;
    for (int aSegment = 0; anInputIndex + 1 < InParameters.Size() && aSegment < NumPoints;
         ++aSegment, aCurrentParameter += aDelta)
    {
      const size_t aPreviousIndex = anInputIndex;
      while (anInputIndex + 1 < InParameters.Size()
             && InParameters.At(anInputIndex) < aCurrentParameter)
      {
        ++anInputIndex;
      }
      if (anInputIndex != aPreviousIndex)
      {
        aSelectedParameters.Append(InParameters.At(anInputIndex - 1));
      }
    }
    aSelectedParameters.Append(InParameters.Last());
  }

  OutParameters.Resize(1, static_cast<int>(aSelectedParameters.Size()), false);
  for (size_t anIndex = 0; anIndex < aSelectedParameters.Size(); ++anIndex)
  {
    OutParameters.ChangeAt(anIndex) = aSelectedParameters.Value(anIndex);
  }
}

//=================================================================================================

void GeomLib::DensifyArray1OfReal(const int                         MinNumPoints,
                                  const NCollection_Array1<double>& InParameters,
                                  NCollection_Array1<double>&       OutParameters)
{
  const bool isInOrder = std::is_sorted(InParameters.begin(), InParameters.end());
  const int  aNumPoints =
    isInOrder ? std::max(MinNumPoints, InParameters.Length()) : InParameters.Length();
  OutParameters.Resize(1, aNumPoints, false);

  if (MinNumPoints <= InParameters.Length() || !isInOrder)
  {
    for (size_t anIndex = 0; anIndex < InParameters.Size(); ++anIndex)
    {
      OutParameters.ChangeAt(anIndex) = InParameters.At(anIndex);
    }
    return;
  }

  const int    aNumParametersToAdd = MinNumPoints - InParameters.Length();
  const double aDelta =
    (InParameters.Last() - InParameters.First()) / static_cast<double>(aNumParametersToAdd + 1);
  if (aDelta == 0.0)
  {
    OutParameters.Init(InParameters.First());
    return;
  }
  size_t anOutputIndex                    = 0;
  double aCurrentParameter                = InParameters.First();
  OutParameters.ChangeAt(anOutputIndex++) = aCurrentParameter;
  aCurrentParameter += aDelta;
  for (size_t anInputIndex = 1;
       anOutputIndex < OutParameters.Size() && anInputIndex < InParameters.Size();
       ++anInputIndex)
  {
    while (aCurrentParameter < InParameters.At(anInputIndex)
           && anOutputIndex < OutParameters.Size())
    {
      OutParameters.ChangeAt(anOutputIndex++) = aCurrentParameter;
      aCurrentParameter += aDelta;
    }
    if (anOutputIndex < OutParameters.Size())
    {
      OutParameters.ChangeAt(anOutputIndex++) = InParameters.At(anInputIndex);
    }
  }
  // Protect the last value from accumulated rounding error.
  OutParameters.ChangeLast() = InParameters.Last();
}

//=================================================================================================

void GeomLib::FuseIntervals(const NCollection_Array1<double>& theInterval1,
                            const NCollection_Array1<double>& theInterval2,
                            NCollection_LinearVector<double>& theFusion,
                            const double                      theConfusion,
                            const bool                        theAdjustToFirstInterval)
{
  theFusion.Clear(false);
  theFusion.Reserve(theInterval1.Size() + theInterval2.Size());

  size_t anIndex1 = 0;
  size_t anIndex2 = 0;
  while (anIndex1 < theInterval1.Size() && anIndex2 < theInterval2.Size())
  {
    const double aValue1 = theInterval1.At(anIndex1);
    const double aValue2 = theInterval2.At(anIndex2);
    if (std::abs(aValue1 - aValue2) <= theConfusion)
    {
      theFusion.Append(theAdjustToFirstInterval ? aValue1 : 0.5 * (aValue1 + aValue2));
      ++anIndex1;
      ++anIndex2;
    }
    else if (aValue1 < aValue2)
    {
      theFusion.Append(aValue1);
      ++anIndex1;
    }
    else
    {
      theFusion.Append(aValue2);
      ++anIndex2;
    }
  }

  for (; anIndex2 < theInterval2.Size(); ++anIndex2)
  {
    theFusion.Append(theInterval2.At(anIndex2));
  }

  for (; anIndex1 < theInterval1.Size(); ++anIndex1)
  {
    theFusion.Append(theInterval1.At(anIndex1));
  }
}

//=================================================================================================

void GeomLib::EvalMaxParametricDistance(const Adaptor3d_Curve& ACurve,
                                        const Adaptor3d_Curve& AReferenceCurve,
                                        //			       const double  Tolerance,
                                        const double,
                                        const NCollection_Array1<double>& Parameters,
                                        double&                           MaxDistance)
{
  double max_squared = 0.0;

  //  tolerance_squared = Tolerance * Tolerance ;
  for (const double aParameter : Parameters)
  {
    const gp_Pnt aPoint1 = ACurve.EvalD0(aParameter);
    const gp_Pnt aPoint2 = AReferenceCurve.EvalD0(aParameter);
    max_squared          = std::max(max_squared, aPoint1.SquareDistance(aPoint2));
  }
  MaxDistance = std::sqrt(max_squared);
}

//=================================================================================================

void GeomLib::EvalMaxDistanceAlongParameter(const Adaptor3d_Curve&            ACurve,
                                            const Adaptor3d_Curve&            AReferenceCurve,
                                            const double                      Tolerance,
                                            const NCollection_Array1<double>& Parameters,
                                            double&                           MaxDistance)
{
  double       max_squared       = 0.0;
  const double tolerance_squared = Tolerance * Tolerance;
  double       other_parameter   = Parameters.First();
  const double para_tolerance    = AReferenceCurve.Resolution(Tolerance);
  const gp_Pnt anInitialPoint = ACurve.EvalD0(other_parameter);
  Extrema_LocateExtPC a_projector(anInitialPoint,
                                  AReferenceCurve,
                                  other_parameter,
                                  para_tolerance);
  for (const double aParameter : Parameters)
  {
    const gp_Pnt aPoint = ACurve.EvalD0(aParameter);
    double       local_distance_squared =
      aPoint.SquareDistance(AReferenceCurve.EvalD0(aParameter));

    if (local_distance_squared > tolerance_squared)
    {

      a_projector.Perform(aPoint, other_parameter);
      if (a_projector.IsDone())
      {
        other_parameter = a_projector.Point().Parameter();
        local_distance_squared = aPoint.SquareDistance(AReferenceCurve.EvalD0(other_parameter));
      }
      else
      {
        local_distance_squared = 0.0e0;
        other_parameter        = aParameter;
      }
    }
    else
    {
      other_parameter = aParameter;
    }

    max_squared = std::max(max_squared, local_distance_squared);
  }
  if (max_squared > tolerance_squared)
  {
    MaxDistance = std::sqrt(max_squared);
  }
  else
  {
    MaxDistance = Tolerance;
  }
}

//=================================================================================================

occ::handle<Geom_Curve> GeomLib::To3d(const gp_Ax2&                    Position,
                                      const occ::handle<Geom2d_Curve>& Curve2d)
{
  occ::handle<Geom_Curve>    Curve3d;
  occ::handle<Standard_Type> KindOfCurve = Curve2d->DynamicType();

  if (KindOfCurve == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    occ::handle<Geom2d_TrimmedCurve> Ct       = occ::down_cast<Geom2d_TrimmedCurve>(Curve2d);
    double                           U1       = Ct->FirstParameter();
    double                           U2       = Ct->LastParameter();
    occ::handle<Geom2d_Curve>        CBasis2d = Ct->BasisCurve();
    occ::handle<Geom_Curve>          CC       = GeomLib::To3d(Position, CBasis2d);
    Curve3d                                   = new Geom_TrimmedCurve(CC, U1, U2);
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    occ::handle<Geom2d_OffsetCurve> Co       = occ::down_cast<Geom2d_OffsetCurve>(Curve2d);
    double                          Offset   = Co->Offset();
    occ::handle<Geom2d_Curve>       CBasis2d = Co->BasisCurve();
    occ::handle<Geom_Curve>         CC       = GeomLib::To3d(Position, CBasis2d);
    Curve3d = new Geom_OffsetCurve(CC, Offset, Position.Direction());
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    occ::handle<Geom2d_BezierCurve>     CBez2d  = occ::down_cast<Geom2d_BezierCurve>(Curve2d);
    const NCollection_Array1<gp_Pnt2d>& Poles2d = CBez2d->Poles();
    NCollection_Array1<gp_Pnt>          Poles3d(Poles2d.Size());
    for (size_t anIndex = 0; anIndex < Poles2d.Size(); ++anIndex)
    {
      Poles3d.ChangeAt(anIndex) = ElCLib::To3d(Position, Poles2d.At(anIndex));
    }
    occ::handle<Geom_BezierCurve> CBez3d;
    if (CBez2d->IsRational())
    {
      CBez3d = new Geom_BezierCurve(Poles3d, CBez2d->WeightsArray());
    }
    else
    {
      CBez3d = new Geom_BezierCurve(Poles3d);
    }
    Curve3d = CBez3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    occ::handle<Geom2d_BSplineCurve>    CBSpl2d    = occ::down_cast<Geom2d_BSplineCurve>(Curve2d);
    const int                           TheDegree  = CBSpl2d->Degree();
    const bool                          IsPeriodic = CBSpl2d->IsPeriodic();
    const NCollection_Array1<gp_Pnt2d>& Poles2d    = CBSpl2d->Poles();
    NCollection_Array1<gp_Pnt>          Poles3d(Poles2d.Size());
    for (size_t anIndex = 0; anIndex < Poles2d.Size(); ++anIndex)
    {
      Poles3d.ChangeAt(anIndex) = ElCLib::To3d(Position, Poles2d.At(anIndex));
    }
    const NCollection_Array1<double>& TheKnots = CBSpl2d->Knots();
    const NCollection_Array1<int>&    TheMults = CBSpl2d->Multiplicities();
    occ::handle<Geom_BSplineCurve>    CBSpl3d;
    if (CBSpl2d->IsRational())
    {
      CBSpl3d = new Geom_BSplineCurve(Poles3d,
                                      CBSpl2d->WeightsArray(),
                                      TheKnots,
                                      TheMults,
                                      TheDegree,
                                      IsPeriodic);
    }
    else
    {
      CBSpl3d = new Geom_BSplineCurve(Poles3d, TheKnots, TheMults, TheDegree, IsPeriodic);
    }
    Curve3d = CBSpl3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Line))
  {
    occ::handle<Geom2d_Line> Line2d  = occ::down_cast<Geom2d_Line>(Curve2d);
    gp_Lin2d                 L2d     = Line2d->Lin2d();
    gp_Lin                   L3d     = ElCLib::To3d(Position, L2d);
    occ::handle<Geom_Line>   GeomL3d = new Geom_Line(L3d);
    Curve3d                          = GeomL3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Circle))
  {
    occ::handle<Geom2d_Circle> Circle2d = occ::down_cast<Geom2d_Circle>(Curve2d);
    gp_Circ2d                  C2d      = Circle2d->Circ2d();
    gp_Circ                    C3d      = ElCLib::To3d(Position, C2d);
    occ::handle<Geom_Circle>   GeomC3d  = new Geom_Circle(C3d);
    Curve3d                             = GeomC3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Ellipse))
  {
    occ::handle<Geom2d_Ellipse> Ellipse2d = occ::down_cast<Geom2d_Ellipse>(Curve2d);
    gp_Elips2d                  E2d       = Ellipse2d->Elips2d();
    gp_Elips                    E3d       = ElCLib::To3d(Position, E2d);
    occ::handle<Geom_Ellipse>   GeomE3d   = new Geom_Ellipse(E3d);
    Curve3d                               = GeomE3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Parabola))
  {
    occ::handle<Geom2d_Parabola> Parabola2d = occ::down_cast<Geom2d_Parabola>(Curve2d);
    gp_Parab2d                   Prb2d      = Parabola2d->Parab2d();
    gp_Parab                     Prb3d      = ElCLib::To3d(Position, Prb2d);
    occ::handle<Geom_Parabola>   GeomPrb3d  = new Geom_Parabola(Prb3d);
    Curve3d                                 = GeomPrb3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    occ::handle<Geom2d_Hyperbola> Hyperbola2d = occ::down_cast<Geom2d_Hyperbola>(Curve2d);
    gp_Hypr2d                     H2d         = Hyperbola2d->Hypr2d();
    gp_Hypr                       H3d         = ElCLib::To3d(Position, H2d);
    occ::handle<Geom_Hyperbola>   GeomH3d     = new Geom_Hyperbola(H3d);
    Curve3d                                   = GeomH3d;
  }
  else
  {
    throw Standard_NotImplemented();
  }

  return Curve3d;
}

//=================================================================================================

occ::handle<Geom2d_Curve> GeomLib::GTransform(const occ::handle<Geom2d_Curve>& Curve,
                                              const gp_GTrsf2d&                GTrsf)
{
  gp_TrsfForm Form = GTrsf.Form();

  if (Form != gp_Other)
  {

    // Then, the GTrsf is actually a Trsf.
    // The curve geometry will then remain unchanged.

    occ::handle<Geom2d_Curve> C = occ::down_cast<Geom2d_Curve>(Curve->Transformed(GTrsf.Trsf2d()));
    return C;
  }
  else
  {

    // So, the GTrsf is an other Transformation.
    // The geometry of the curves is then changed, and the conics will have to
    // be converted to BSplines.

    occ::handle<Standard_Type> TheType = Curve->DynamicType();

    if (TheType == STANDARD_TYPE(Geom2d_TrimmedCurve))
    {

      // We will recurse on the BasisCurve

      occ::handle<Geom2d_TrimmedCurve> C = occ::down_cast<Geom2d_TrimmedCurve>(Curve->Copy());

      occ::handle<Standard_Type> TheBasisType = (C->BasisCurve())->DynamicType();

      if (TheBasisType == STANDARD_TYPE(Geom2d_BSplineCurve)
          || TheBasisType == STANDARD_TYPE(Geom2d_BezierCurve))
      {

        // In these cases the parameterization is preserved on the transformed curve,
        // so we can trim it with the parameters of the base curve.

        double U1 = C->FirstParameter();
        double U2 = C->LastParameter();

        occ::handle<Geom2d_TrimmedCurve> result =
          new Geom2d_TrimmedCurve(GTransform(C->BasisCurve(), GTrsf), U1, U2);
        return result;
      }
      else if (TheBasisType == STANDARD_TYPE(Geom2d_Line))
      {

        // In this case, the parameterization is no longer preserved.
        // We must recompute the trimming parameters on the resulting
        // curve (by projecting the transformed start and end points using ElCLib).

        occ::handle<Geom2d_Line> L =
          occ::down_cast<Geom2d_Line>(GTransform(C->BasisCurve(), GTrsf));
        gp_Lin2d Lin = L->Lin2d();

        gp_Pnt2d P1 = C->StartPoint();
        gp_Pnt2d P2 = C->EndPoint();
        P1.SetXY(GTrsf.Transformed(P1.XY()));
        P2.SetXY(GTrsf.Transformed(P2.XY()));
        double U1 = ElCLib::Parameter(Lin, P1);
        double U2 = ElCLib::Parameter(Lin, P2);

        occ::handle<Geom2d_TrimmedCurve> result = new Geom2d_TrimmedCurve(L, U1, U2);
        return result;
      }
      else if (TheBasisType == STANDARD_TYPE(Geom2d_Circle)
               || TheBasisType == STANDARD_TYPE(Geom2d_Ellipse)
               || TheBasisType == STANDARD_TYPE(Geom2d_Parabola)
               || TheBasisType == STANDARD_TYPE(Geom2d_Hyperbola))
      {

        // In these cases, the curve geometry is not preserved;
        // convert it to BSpline before applying the Trsf.

        occ::handle<Geom2d_BSplineCurve> BS = Geom2dConvert::CurveToBSplineCurve(C);
        return GTransform(BS, GTrsf);
      }
      else
      {
        // The transform of an OffsetCurve is not supported.
        return {};
      }
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Line))
    {

      occ::handle<Geom2d_Line> L         = occ::down_cast<Geom2d_Line>(Curve->Copy());
      const gp_Lin2d           Lin       = L->Lin2d();
      gp_Pnt2d                 aLocation = Lin.Location();
      aLocation.SetXY(GTrsf.Transformed(aLocation.XY()));
      const gp_XY aDirection = Lin.Direction().XY().Multiplied(GTrsf.VectorialPart());
      L->SetLocation(aLocation);
      L->SetDirection(gp_Dir2d(aDirection));
      return L;
    }
    else if (TheType == STANDARD_TYPE(Geom2d_BezierCurve))
    {

      // Since GTrsf is a linear operation, the transform of a pole-based curve
      // is the curve whose poles are the transforms of the base curve's poles.

      occ::handle<Geom2d_BezierCurve> C = occ::down_cast<Geom2d_BezierCurve>(Curve->Copy());
      for (size_t anIndex = 0; anIndex < C->Poles().Size(); ++anIndex)
      {
        gp_Pnt2d aPole = C->Poles().At(anIndex);
        aPole.SetXY(GTrsf.Transformed(aPole.XY()));
        C->SetPole(static_cast<int>(anIndex + 1), aPole);
      }
      return C;
    }
    else if (TheType == STANDARD_TYPE(Geom2d_BSplineCurve))
    {

      // See comment for Bezier curves above.

      occ::handle<Geom2d_BSplineCurve> C = occ::down_cast<Geom2d_BSplineCurve>(Curve->Copy());
      for (size_t anIndex = 0; anIndex < C->Poles().Size(); ++anIndex)
      {
        gp_Pnt2d aPole = C->Poles().At(anIndex);
        aPole.SetXY(GTrsf.Transformed(aPole.XY()));
        C->SetPole(static_cast<int>(anIndex + 1), aPole);
      }
      return C;
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Circle) || TheType == STANDARD_TYPE(Geom2d_Ellipse))
    {

      // In these cases, the curve geometry is not preserved;
      // convert it to BSpline before applying the Trsf.

      occ::handle<Geom2d_BSplineCurve> C = Geom2dConvert::CurveToBSplineCurve(Curve);
      return GTransform(C, GTrsf);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Parabola) || TheType == STANDARD_TYPE(Geom2d_Hyperbola)
             || TheType == STANDARD_TYPE(Geom2d_OffsetCurve))
    {
      // Not supported: return a null Handle;
      return {};
    }
  }

  return {};
}

//=================================================================================================

void GeomLib::SameRange(const double                     Tolerance,
                        const occ::handle<Geom2d_Curve>& CurvePtr,
                        const double                     FirstOnCurve,
                        const double                     LastOnCurve,
                        const double                     RequestedFirst,
                        const double                     RequestedLast,
                        occ::handle<Geom2d_Curve>&       NewCurvePtr)
{
  if (CurvePtr.IsNull())
  {
    throw Standard_Failure();
  }
  if (std::abs(LastOnCurve - RequestedLast) <= Tolerance
      && std::abs(FirstOnCurve - RequestedFirst) <= Tolerance)
  {
    NewCurvePtr = CurvePtr;
    return;
  }

  // the parametrisation length  must at least be the same.
  if (std::abs(LastOnCurve - FirstOnCurve - RequestedLast + RequestedFirst) <= Tolerance)
  {
    if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_Line)))
    {
      occ::handle<Geom2d_Line> Line = occ::down_cast<Geom2d_Line>(CurvePtr->Copy());
      double                   dU   = FirstOnCurve - RequestedFirst;
      gp_Dir2d                 D    = Line->Direction();
      Line->Translate(dU * gp_Vec2d(D));
      NewCurvePtr = Line;
    }
    else if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_Circle)))
    {
      gp_Trsf2d Trsf;
      NewCurvePtr                     = occ::down_cast<Geom2d_Curve>(CurvePtr->Copy());
      occ::handle<Geom2d_Circle> Circ = occ::down_cast<Geom2d_Circle>(NewCurvePtr);
      gp_Pnt2d                   P    = Circ->Location();
      double                     dU;
      if (Circ->Circ2d().IsDirect())
      {
        dU = FirstOnCurve - RequestedFirst;
      }
      else
      {
        dU = RequestedFirst - FirstOnCurve;
      }
      Trsf.SetRotation(P, dU);
      NewCurvePtr->Transform(Trsf);
    }
    else if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      occ::handle<Geom2d_TrimmedCurve> TC = occ::down_cast<Geom2d_TrimmedCurve>(CurvePtr);
      GeomLib::SameRange(Tolerance,
                         TC->BasisCurve(),
                         FirstOnCurve,
                         LastOnCurve,
                         RequestedFirst,
                         RequestedLast,
                         NewCurvePtr);
      NewCurvePtr = new Geom2d_TrimmedCurve(NewCurvePtr, RequestedFirst, RequestedLast);
    }
    //
    //  Beware of limitation issues: use the SAME test as in
    //  Geom2d_TrimmedCurve::SetTrim, otherwise since we may re-trim on
    //  RequestedFirst and RequestedLast we will have a problem
    //
    //
    else if (std::abs(LastOnCurve - FirstOnCurve) > Precision::PConfusion()
             || std::abs(RequestedLast + RequestedFirst) > Precision::PConfusion())
    {

      occ::handle<Geom2d_TrimmedCurve> TC =
        new Geom2d_TrimmedCurve(CurvePtr, FirstOnCurve, LastOnCurve);

      occ::handle<Geom2d_BSplineCurve> BS = Geom2dConvert::CurveToBSplineCurve(TC);
      NCollection_Array1<double>       Knots(BS->Knots());

      BSplCLib::Reparametrize(RequestedFirst, RequestedLast, Knots);

      BS->SetKnots(Knots);
      NewCurvePtr = BS;
    }
  }
  else
  { // We segment the result
    occ::handle<Geom2d_TrimmedCurve> TC;
    occ::handle<Geom2d_Curve>        aCCheck = CurvePtr;

    if (aCCheck->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      aCCheck = occ::down_cast<Geom2d_TrimmedCurve>(aCCheck)->BasisCurve();
    }

    if (aCCheck->IsPeriodic())
    {
      if (std::abs(LastOnCurve - FirstOnCurve) > Precision::PConfusion())
      {
        TC = new Geom2d_TrimmedCurve(CurvePtr, FirstOnCurve, LastOnCurve);
      }
      else
      {
        TC =
          new Geom2d_TrimmedCurve(CurvePtr, CurvePtr->FirstParameter(), CurvePtr->LastParameter());
      }
    }
    else
    {
      const double Udeb = std::max(CurvePtr->FirstParameter(), FirstOnCurve);
      const double Ufin = std::min(CurvePtr->LastParameter(), LastOnCurve);
      if (std::abs(Ufin - Udeb) > Precision::PConfusion())
      {
        TC = new Geom2d_TrimmedCurve(CurvePtr, Udeb, Ufin);
      }
      else
      {
        TC =
          new Geom2d_TrimmedCurve(CurvePtr, CurvePtr->FirstParameter(), CurvePtr->LastParameter());
      }
    }

    //
    occ::handle<Geom2d_BSplineCurve> BS = Geom2dConvert::CurveToBSplineCurve(TC);
    NCollection_Array1<double>       Knots(BS->Knots());

    BSplCLib::Reparametrize(RequestedFirst, RequestedLast, Knots);

    BS->SetKnots(Knots);
    NewCurvePtr = BS;
  }
}

//=================================================================================================

class GeomLib_CurveOnSurfaceEvaluator : public AdvApprox_EvaluatorFunction
{
public:
  GeomLib_CurveOnSurfaceEvaluator(Adaptor3d_CurveOnSurface& theCurveOnSurface,
                                  double                    theFirst,
                                  double                    theLast)
      : CurveOnSurface(theCurveOnSurface),
        FirstParam(theFirst),
        LastParam(theLast),
        TrimCurve(theCurveOnSurface.Trim(theFirst, theLast, Precision::PConfusion()))
  {
  }

  void Evaluate(int*    Dimension,
                double  StartEnd[2],
                double* Parameter,
                int*    DerivativeRequest,
                double* Result, // [Dimension]
                int*    ErrorCode) override;

private:
  Adaptor3d_CurveOnSurface& CurveOnSurface;
  double                    FirstParam;
  double                    LastParam;

  occ::handle<Adaptor3d_Curve> TrimCurve;
};

void GeomLib_CurveOnSurfaceEvaluator::Evaluate(int*, /*Dimension*/
                                               double  DebutFin[2],
                                               double* Parameter,
                                               int*    DerivativeRequest,
                                               double* Result, // [Dimension]
                                               int*    ReturnCode)
{
  // Handle left / right positioning
  if ((DebutFin[0] != FirstParam) || (DebutFin[1] != LastParam))
  {
    TrimCurve  = CurveOnSurface.Trim(DebutFin[0], DebutFin[1], Precision::PConfusion());
    FirstParam = DebutFin[0];
    LastParam  = DebutFin[1];
  }

  switch (*DerivativeRequest)
  {
    case 0: {
      const gp_Pnt aPoint = TrimCurve->EvalD0(*Parameter);
      Result[0]           = aPoint.X();
      Result[1]           = aPoint.Y();
      Result[2]           = aPoint.Z();
      break;
    }
    case 1: {
      const gp_Vec aDerivative = TrimCurve->EvalD1(*Parameter).D1;
      Result[0]                 = aDerivative.X();
      Result[1]                 = aDerivative.Y();
      Result[2]                 = aDerivative.Z();
      break;
    }
    case 2: {
      const gp_Vec aSecondDerivative = TrimCurve->EvalD2(*Parameter).D2;
      Result[0] = aSecondDerivative.X();
      Result[1] = aSecondDerivative.Y();
      Result[2] = aSecondDerivative.Z();
      break;
    }
    default:
      break;
  }
  ReturnCode[0] = 0;
}

//=================================================================================================

void GeomLib::BuildCurve3d(const double              Tolerance,
                           Adaptor3d_CurveOnSurface& Curve,
                           const double              FirstParameter,
                           const double              LastParameter,
                           occ::handle<Geom_Curve>&  NewCurvePtr,
                           double&                   MaxDeviation,
                           double&                   AverageDeviation,
                           const GeomAbs_Shape       Continuity,
                           const int                 MaxDegree,
                           const int                 MaxSegment)

{

  MaxDeviation     = 0.0e0;
  AverageDeviation = 0.0e0;
  occ::handle<GeomAdaptor_Surface> geom_adaptor_surface_ptr(
    occ::down_cast<GeomAdaptor_Surface>(Curve.GetSurface()));
  occ::handle<Geom2dAdaptor_Curve> geom_adaptor_curve_ptr(
    occ::down_cast<Geom2dAdaptor_Curve>(Curve.GetCurve()));

  if (!geom_adaptor_curve_ptr.IsNull() && !geom_adaptor_surface_ptr.IsNull())
  {
    occ::handle<Geom_Plane>    P;
    const GeomAdaptor_Surface& geom_surface = *geom_adaptor_surface_ptr;

    occ::handle<Geom_RectangularTrimmedSurface> RT =
      occ::down_cast<Geom_RectangularTrimmedSurface>(geom_surface.Surface());
    if (RT.IsNull())
    {
      P = occ::down_cast<Geom_Plane>(geom_surface.Surface());
    }
    else
    {
      P = occ::down_cast<Geom_Plane>(RT->BasisSurface());
    }

    if (!P.IsNull())
    {
      // compute the 3d curve
      gp_Ax2                     axes         = P->Position().Ax2();
      const Geom2dAdaptor_Curve& geom2d_curve = *geom_adaptor_curve_ptr;
      NewCurvePtr                             = GeomLib::To3d(axes, geom2d_curve.Curve());
      return;
    }

    occ::handle<Adaptor2d_Curve2d> TrimmedC2D =
      geom_adaptor_curve_ptr->Trim(FirstParameter, LastParameter, Precision::PConfusion());

    bool   isU, isForward;
    double aParam;
    if (isIsoLine(TrimmedC2D, isU, aParam, isForward))
    {
      NewCurvePtr = buildC3dOnIsoLine(TrimmedC2D,
                                      geom_adaptor_surface_ptr,
                                      FirstParameter,
                                      LastParameter,
                                      Tolerance,
                                      isU,
                                      aParam,
                                      isForward);
      if (!NewCurvePtr.IsNull())
      {
        return;
      }
    }
  }

  //
  // Entree
  //
  occ::handle<NCollection_HArray1<double>> Tolerance1DPtr, Tolerance2DPtr;
  occ::handle<NCollection_HArray1<double>> Tolerance3DPtr =
    new NCollection_HArray1<double>(1, 1, Tolerance);

  // Search for discontinuities
  const int                  NbIntervalC2 = Curve.NbIntervals(GeomAbs_C2);
  NCollection_Array1<double> Param_de_decoupeC2(1, NbIntervalC2 + 1);
  Curve.Intervals(Param_de_decoupeC2, GeomAbs_C2);

  const int                  NbIntervalC3 = Curve.NbIntervals(GeomAbs_C3);
  NCollection_Array1<double> Param_de_decoupeC3(1, NbIntervalC3 + 1);
  Curve.Intervals(Param_de_decoupeC3, GeomAbs_C3);

  GeomLib_CurveOnSurfaceEvaluator ev(Curve, FirstParameter, LastParameter);

  // Approximation with preferential cutting
  AdvApprox_PrefAndRec      Preferentiel(Param_de_decoupeC2, Param_de_decoupeC3);
  AdvApprox_ApproxAFunction anApproximator(0,
                                           0,
                                           1,
                                           Tolerance1DPtr,
                                           Tolerance2DPtr,
                                           Tolerance3DPtr,
                                           FirstParameter,
                                           LastParameter,
                                           Continuity,
                                           MaxDegree,
                                           MaxSegment,
                                           ev,
                                           //					      CurveOnSurfaceEvaluator,
                                           Preferentiel);

  if (anApproximator.HasResult())
  {
    GeomLib_MakeCurvefromApprox aCurveBuilder(anApproximator);

    occ::handle<Geom_BSplineCurve> aCurvePtr = aCurveBuilder.Curve(1);
    // Return the approximation results
    MaxDeviation     = anApproximator.MaxError(3, 1);
    AverageDeviation = anApproximator.AverageError(3, 1);
    NewCurvePtr      = aCurvePtr;
  }
}

//=================================================================================================

void GeomLib::AdjustExtremity(occ::handle<Geom_BoundedCurve>& Curve,
                              const gp_Pnt&                   P1,
                              const gp_Pnt&                   P2,
                              const gp_Vec&                   T1,
                              const gp_Vec&                   T2)
{
  // Convert the input (preserving the parameterization if possible)
  occ::handle<Geom_BSplineCurve> aIn, aDef;
  aIn = GeomConvert::CurveToBSplineCurve(Curve, Convert_QuasiAngular);

  int                               ii;
  NCollection_LocalArray<gp_Pnt, 4> aPolesDefStorage(4), aCoeffsStorage(4);
  NCollection_LocalArray<double, 4> aParameterStorage(4);
  NCollection_Array1<gp_Pnt>        PolesDef(*aPolesDefStorage.begin(), 1, 4);
  NCollection_Array1<gp_Pnt>        Coeffs(*aCoeffsStorage.begin(), 1, 4);
  NCollection_Array1<double>        Ti(*aParameterStorage.begin(), 1, 4);

  Ti(1) = Ti(2) = aIn->FirstParameter();
  Ti(3) = Ti(4) = aIn->LastParameter();

  // Calculation of deformation constraints
  const auto [aFirstPoint, aFirstDerivative] = aIn->EvalD1(Ti.First());
  PolesDef.ChangeFirst().ChangeCoord()       = P1.XYZ() - aFirstPoint.XYZ();
  gp_Vec aFirstUnitTangent                   = T1;
  aFirstUnitTangent.Normalize();
  const gp_Vec aFirstDerivativeCorrection =
    aFirstUnitTangent * (aFirstUnitTangent * aFirstDerivative) - aFirstDerivative;
  PolesDef(2).ChangeCoord() = (Ti.Last() - Ti.First()) * aFirstDerivativeCorrection.XYZ();

  const auto [aLastPoint, aLastDerivative] = aIn->EvalD1(Ti.Last());
  PolesDef(3).ChangeCoord()                = P2.XYZ() - aLastPoint.XYZ();
  gp_Vec aLastUnitTangent                  = T2;
  aLastUnitTangent.Normalize();
  const gp_Vec aLastDerivativeCorrection =
    aLastUnitTangent * (aLastUnitTangent * aLastDerivative) - aLastDerivative;
  PolesDef.ChangeLast().ChangeCoord() =
    (Ti.Last() - Ti.First()) * aLastDerivativeCorrection.XYZ();

  // Interpolation of constraints
  math_Matrix Mat(1, 4, 1, 4);
  if (!PLib::HermiteCoefficients(0., 1., 1, 1, Mat))
  {
    throw Standard_ConstructionError();
  }

  for (size_t aCoeffIndex = 0; aCoeffIndex < Coeffs.Size(); ++aCoeffIndex)
  {
    gp_XYZ aux(0., 0., 0.);
    for (size_t aConstraintIndex = 0; aConstraintIndex < PolesDef.Size(); ++aConstraintIndex)
    {
      aux.SetLinearForm(
        Mat(static_cast<int>(aConstraintIndex + 1), static_cast<int>(aCoeffIndex + 1)),
        PolesDef.At(aConstraintIndex).XYZ(),
        aux);
    }
    Coeffs.ChangeAt(aCoeffIndex).SetXYZ(aux);
  }

  PLib::CoefficientsPoles(Coeffs, PLib::NoWeights(), PolesDef, PLib::NoWeights());

  // Add the deformation
  NCollection_LocalArray<double, 2> aKnotStorage(2);
  NCollection_LocalArray<int, 2>    aMultStorage(2);
  NCollection_Array1<double>        K(*aKnotStorage.begin(), 1, 2);
  NCollection_Array1<int>           M(*aMultStorage.begin(), 1, 2);
  K(1) = Ti(1);
  K(2) = Ti(4);
  M.Init(4);

  aDef = new (Geom_BSplineCurve)(PolesDef, K, M, 3);
  if (aIn->Degree() < 3)
  {
    aIn->IncreaseDegree(3);
  }
  else
  {
    aDef->IncreaseDegree(aIn->Degree());
  }

  for (ii = 2; ii < aIn->NbKnots(); ii++)
  {
    aDef->InsertKnot(aIn->Knot(ii), aIn->Multiplicity(ii));
  }

  if (aDef->NbPoles() != aIn->NbPoles())
  {
    throw Standard_ConstructionError("Inconsistent poles's number");
  }

  for (ii = 1; ii <= aDef->NbPoles(); ii++)
  {
    gp_Pnt aPole = aIn->Pole(ii);
    aPole.ChangeCoord() += aDef->Pole(ii).XYZ();
    aIn->SetPole(ii, aPole);
  }
  Curve = aIn;
}

//=================================================================================================

void GeomLib::ExtendCurveToPoint(occ::handle<Geom_BoundedCurve>& Curve,
                                 const gp_Pnt&                   Point,
                                 const int                       Continuity,
                                 const bool                      After)
{
  if (Continuity < 1 || Continuity > THE_MAX_EXTENSION_CONTINUITY)
  {
    return;
  }
  const int   aConstraintCount = Continuity + 2;
  double      Ubord, Tol = THE_CURVE_EXTENSION_TOLERANCE;
  math_Matrix MatCoefs(1, aConstraintCount, 1, aConstraintCount);
  double      Lambda, L1;
  int         ii;
  // Convert the input (preserving the parameterization if possible)
  GeomConvert_CompCurveToBSplineCurve Concat(Curve, Convert_QuasiAngular);

  // Construction constraints
  NCollection_LocalArray<gp_XYZ, 5> aConstraintStorage(
    static_cast<size_t>(aConstraintCount));
  NCollection_Array1<gp_XYZ> Cont(*aConstraintStorage.begin(), 1, aConstraintCount);
  if (After)
  {
    Ubord = Curve->LastParameter();
  }
  else
  {
    Ubord = Curve->FirstParameter();
  }
  PLib::HermiteCoefficients(0,
                            1, // The Bounds
                            Continuity,
                            0, // The constraint orders
                            MatCoefs);

  const auto [aBoundaryPoint, aBoundaryD1, aBoundaryD2, aBoundaryD3] = Curve->EvalD3(Ubord);
  const double aDerivativeDirection = After ? 1.0 : -1.0;
  const gp_Vec aFirstDerivative     = aBoundaryD1.Multiplied(aDerivativeDirection);
  const gp_Vec aThirdDerivative     = aBoundaryD3.Multiplied(aDerivativeDirection);

  L1 = aBoundaryPoint.Distance(Point);
  if (L1 > Tol)
  {
    // Lambda is the ratio to apply to the derivative of the curve
    // to obtain the derivative of the extension (arbitrarily fixed to the
    // length of the segment end of curve - target point.
    // We try to have on the extension the average velocity that we
    // have on the curve.
    double f = Curve->FirstParameter(), t, dt, norm;
    dt       = (Curve->LastParameter() - f) / THE_CURVE_SPEED_SAMPLE_COUNT;
    norm     = aFirstDerivative.Magnitude();
    for (ii = 1, t = f + dt; ii < THE_CURVE_SPEED_SAMPLE_COUNT; ++ii, t += dt)
    {
      const gp_Vec aDerivative = Curve->EvalD1(t).D1;
      norm += aDerivative.Magnitude();
    }
    norm /= THE_CURVE_SPEED_SAMPLE_COUNT;
    dt = aFirstDerivative.Magnitude() / norm;
    if (dt < THE_MAX_CURVE_SPEED_RATIO && dt > THE_MIN_CURVE_SPEED_RATIO)
    { // The edge is within the average, keep it
      Lambda = 1.0 / std::max(aFirstDerivative.Magnitude() / L1, Tol);
    }
    else
    {
      Lambda = 1.0 / std::max(norm / L1, Tol);
    }
  }
  else
  {
    return; // No extension
  }

  // Optimization of Lambda
  math_Matrix Cons(1, 3, 1, aConstraintCount);
  Cons(1, 1)    = aBoundaryPoint.X();
  Cons(2, 1)    = aBoundaryPoint.Y();
  Cons(3, 1)    = aBoundaryPoint.Z();
  Cons(1, 2)    = aFirstDerivative.X();
  Cons(2, 2)    = aFirstDerivative.Y();
  Cons(3, 2)    = aFirstDerivative.Z();
  Cons(1, aConstraintCount) = Point.X();
  Cons(2, aConstraintCount) = Point.Y();
  Cons(3, aConstraintCount) = Point.Z();
  if (Continuity >= 2)
  {
    Cons(1, 3) = aBoundaryD2.X();
    Cons(2, 3) = aBoundaryD2.Y();
    Cons(3, 3) = aBoundaryD2.Z();
  }
  if (Continuity >= 3)
  {
    Cons(1, 4) = aThirdDerivative.X();
    Cons(2, 4) = aThirdDerivative.Y();
    Cons(3, 4) = aThirdDerivative.Z();
  }
  ComputeLambda(Cons, MatCoefs, L1, Lambda);

  // Construction in the Polynomial Basis
  Cont(1) = aBoundaryPoint.XYZ();
  Cont(2) = aFirstDerivative.XYZ() * Lambda;
  if (Continuity >= 2)
  {
    Cont(3) = aBoundaryD2.XYZ() * std::pow(Lambda, 2);
  }
  if (Continuity >= 3)
  {
    Cont(4) = aThirdDerivative.XYZ() * std::pow(Lambda, 3);
  }
  Cont.ChangeLast() = Point.XYZ();

  NCollection_LocalArray<gp_Pnt, 5> aPoleStorage(static_cast<size_t>(aConstraintCount));
  NCollection_LocalArray<gp_Pnt, 5> aCoeffStorage(static_cast<size_t>(aConstraintCount));
  NCollection_Array1<gp_Pnt>        ExtrapPoles(*aPoleStorage.begin(), 1, aConstraintCount);
  NCollection_Array1<gp_Pnt>        ExtraCoeffs(*aCoeffStorage.begin(), 1, aConstraintCount);

  gp_Pnt PNull(0., 0., 0.);
  ExtraCoeffs.Init(PNull);
  for (size_t aConstraintIndex = 0; aConstraintIndex < Cont.Size(); ++aConstraintIndex)
  {
    for (size_t aCoeffIndex = 0; aCoeffIndex < ExtraCoeffs.Size(); ++aCoeffIndex)
    {
      ExtraCoeffs.ChangeAt(aCoeffIndex).ChangeCoord() +=
        MatCoefs(static_cast<int>(aConstraintIndex + 1), static_cast<int>(aCoeffIndex + 1))
        * Cont.At(aConstraintIndex);
    }
  }

  // Conversion to the Bernstein Basis
  PLib::CoefficientsPoles(ExtraCoeffs, PLib::NoWeights(), ExtrapPoles, PLib::NoWeights());

  occ::handle<Geom_BezierCurve> Bezier = new (Geom_BezierCurve)(ExtrapPoles);

  double dist = ExtrapPoles(1).Distance(aBoundaryPoint);
  bool   Ok;
  Tol += dist;

  // Concatenation
  Ok = Concat.Add(Bezier, Tol, After);
  if (!Ok)
  {
    throw Standard_ConstructionError("ExtendCurveToPoint");
  }

  Curve = Concat.BSplineCurve();
}

//=================================================================================================

static bool ExtendKPart(occ::handle<Geom_RectangularTrimmedSurface>& Surface,
                        const double                                 Length,
                        const bool                                   InU,
                        const bool                                   After)
{

  if (Surface.IsNull())
  {
    return false;
  }

  bool                      Ok = true;
  double                    Uf, Ul, Vf, Vl;
  occ::handle<Geom_Surface> Support = Surface->BasisSurface();
  GeomAbs_SurfaceType       Type;

  Surface->Bounds(Uf, Ul, Vf, Vl);
  GeomAdaptor_Surface AS(Surface);
  Type = AS.GetType();

  if (InU)
  {
    switch (Type)
    {
      case GeomAbs_Plane: {
        if (After)
        {
          Ul += Length;
        }
        else
        {
          Uf -= Length;
        }
        Surface = new (Geom_RectangularTrimmedSurface)(Support, Uf, Ul, Vf, Vl);
        break;
      }

      default:
        Ok = false;
    }
  }
  else
  {
    switch (Type)
    {
      case GeomAbs_Plane:
      case GeomAbs_Cylinder:
      case GeomAbs_SurfaceOfExtrusion: {
        if (After)
        {
          Vl += Length;
        }
        else
        {
          Vf -= Length;
        }
        Surface = new (Geom_RectangularTrimmedSurface)(Support, Uf, Ul, Vf, Vl);
        break;
      }
      default:
        Ok = false;
    }
  }

  return Ok;
}

//=================================================================================================

void GeomLib::ExtendSurfByLength(occ::handle<Geom_BoundedSurface>& Surface,
                                 const double                      Length,
                                 const int                         Continuity,
                                 const bool                        InU,
                                 const bool                        After)
{
  if (Continuity < 0 || Continuity > THE_MAX_EXTENSION_CONTINUITY)
  {
    return;
  }
  int Cont = Continuity;

  // Kpart ?
  occ::handle<Geom_RectangularTrimmedSurface> TS =
    occ::down_cast<Geom_RectangularTrimmedSurface>(Surface);
  if (ExtendKPart(TS, Length, InU, After))
  {
    Surface = TS;
    return;
  }

  //  BSplineSurface format with a degree sufficient for the desired continuity
  occ::handle<Geom_BSplineSurface> BS = occ::down_cast<Geom_BSplineSurface>(Surface);
  if (BS.IsNull())
  {
    // BS = GeomConvert::SurfaceToBSplineSurface(Surface);
    const occ::handle<Geom_Surface>& aSurf = Surface; // to resolve ambiguity
    GeomConvert_ApproxSurface        theApprox(aSurf,
                                               THE_SURFACE_APPROX_TOLERANCE,
                                               GeomAbs_C1,
                                               GeomAbs_C1,
                                               THE_SURFACE_APPROX_MAX_DEGREE,
                                               THE_SURFACE_APPROX_MAX_DEGREE,
                                               THE_SURFACE_APPROX_MAX_SEGMENTS,
                                               THE_SURFACE_APPROX_PRECISION);
    if (theApprox.HasResult())
    {
      BS = theApprox.Surface();
    }
    else
    {
      BS = GeomConvert::SurfaceToBSplineSurface(Surface);
    }
  }
  if (InU && (BS->UDegree() < Continuity + 1))
  {
    BS->IncreaseDegree(Continuity + 1, BS->VDegree());
  }
  if (!InU && (BS->VDegree() < Continuity + 1))
  {
    BS->IncreaseDegree(BS->UDegree(), Continuity + 1);
  }

  // if BS was periodic in the extension direction, it will no longer be
  if ((InU && (BS->IsUPeriodic())) || (!InU && (BS->IsVPeriodic())))
  {
    double U0, U1, V0, V1;
    BS->Bounds(U0, U1, V0, V1);
    BS->Segment(U0, U1, V0, V1);
  }

  // IFV Fix OCC bug 0022694 - wrong result extrapolating rational surfaces
  //   bool rational = ( InU && BS->IsURational() )
  //                                   || ( !InU && BS->IsVRational() ) ;
  const bool   rational = BS->IsURational() || BS->IsVRational();
  const size_t gap      = rational ? 4 : 3;

  int                               Cdeg = 0, Cdim = 0, NbP = 0, aFlatKnotCount = 0;
  double                            Tbord, lambmin = Length;
  bool                              Ok         = false;
  int                               Kount      = 0;
  const NCollection_Array1<double>* aFlatKnots = nullptr;
  NCollection_LinearVector<double>  Point;
  NCollection_LinearVector<double>  Lambda;
  NCollection_LinearVector<double>  Tgte;
  NCollection_LinearVector<double>  Poles;
  NCollection_LinearVector<double>  Result;

  for (; Kount <= THE_MAX_SURFACE_DEGREE_RETRIES && !Ok; ++Kount)
  {
    //  Transform the surface into a non-rational BSpline in one variable
    //  of degree UDegree or VDegree and dimension 3 or 4 x NbVpoles or NbUpoles
    //  the number of poles equals NbUpoles or NbVpoles
    //  WARNING: in the rational case, a point with coordinates (x,y,z)
    //           and weight w becomes a point with coordinates (wx, wy, wz, w)

    if (InU)
    {
      Cdeg = BS->UDegree();
      NbP  = BS->NbUPoles();
      Cdim = BS->NbVPoles() * static_cast<int>(gap);
    }
    else
    {
      Cdeg = BS->VDegree();
      NbP  = BS->NbVPoles();
      Cdim = BS->NbUPoles() * static_cast<int>(gap);
    }

    //  the flat knots
    aFlatKnotCount = NbP + Cdeg + 1;
    if (InU)
    {
      aFlatKnots = &BS->UKnotSequence();
    }
    else
    {
      aFlatKnots = &BS->VKnotSequence();
    }

    //  the parameter of the connection knot
    if (After)
    {
      Tbord = aFlatKnots->Value(aFlatKnots->Upper() - Cdeg);
    }
    else
    {
      Tbord = aFlatKnots->Value(aFlatKnots->Lower() + Cdeg);
    }

    //  the poles
    const size_t aPolesSize = static_cast<size_t>(Cdim) * static_cast<size_t>(NbP);
    Poles.Resize(aPolesSize);

    const NCollection_Array2<gp_Pnt>& aSurfacePoles   = BS->Poles();
    const NCollection_Array2<double>* aSurfaceWeights = BS->Weights();
    size_t                            aPoleIndex      = 0;
    if (InU)
    {
      for (size_t aUIndex = 0; aUIndex < aSurfacePoles.RowSize(); ++aUIndex)
      {
        for (size_t aVIndex = 0; aVIndex < aSurfacePoles.ColSize(); ++aVIndex)
        {
          const gp_Pnt& aPole               = aSurfacePoles.At(aUIndex, aVIndex);
          Poles.ChangeValue(aPoleIndex)     = aPole.X();
          Poles.ChangeValue(aPoleIndex + 1) = aPole.Y();
          Poles.ChangeValue(aPoleIndex + 2) = aPole.Z();
          if (rational)
          {
            Poles.ChangeValue(aPoleIndex + 3) = aSurfaceWeights->At(aUIndex, aVIndex);
          }
          aPoleIndex += gap;
        }
      }
    }
    else
    {
      for (size_t aVIndex = 0; aVIndex < aSurfacePoles.ColSize(); ++aVIndex)
      {
        for (size_t aUIndex = 0; aUIndex < aSurfacePoles.RowSize(); ++aUIndex)
        {
          const gp_Pnt& aPole               = aSurfacePoles.At(aUIndex, aVIndex);
          Poles.ChangeValue(aPoleIndex)     = aPole.X();
          Poles.ChangeValue(aPoleIndex + 1) = aPole.Y();
          Poles.ChangeValue(aPoleIndex + 2) = aPole.Z();
          if (rational)
          {
            Poles.ChangeValue(aPoleIndex + 3) = aSurfaceWeights->At(aUIndex, aVIndex);
          }
          aPoleIndex += gap;
        }
      }
    }

    //  calculation of the connection point and tangent
    const size_t aDimension = static_cast<size_t>(Cdim);
    Point.Resize(aDimension);
    Tgte.Resize(aDimension);
    Lambda.Resize(aDimension);

    const bool isPeriodic         = false;
    int        anExtrapMode[2]    = {Cdeg, Cdeg};
    const int  aDerivativeRequest = std::max(Continuity, 1);
    Result.Resize(aDimension * static_cast<size_t>(aDerivativeRequest + 1));

    BSplCLib::Eval(Tbord,
                   isPeriodic,
                   aDerivativeRequest,
                   anExtrapMode[0],
                   Cdeg,
                   *aFlatKnots,
                   Cdim,
                   *Poles.Data(),
                   *Result.Data());
    Ok = true;
    for (size_t anIndex = 0; anIndex < aDimension; ++anIndex)
    {
      Point.ChangeValue(anIndex) = Result.Value(anIndex);
      Tgte.ChangeValue(anIndex)  = Result.Value(anIndex + aDimension);
    }

    //  calculation of the constraint to reach

    gp_Vec CurT, OldT;
    double OldN = 0.0;
    lambmin     = Length;
    for (size_t aCoord = 0; aCoord < aDimension; aCoord += gap)
    {
      CurT.SetCoord(Tgte.Value(aCoord), Tgte.Value(aCoord + 1), Tgte.Value(aCoord + 2));
      const double NTgte = CurT.Magnitude();
      double       val   = 0.0;
      if (NTgte > THE_SURFACE_TANGENT_TOLERANCE)
      {
        val = Length / NTgte;
        // Detect a reversal between adjacent boundary tangents. Increasing the
        // transverse degree moves surface poles closer to the boundary curve.
        if (OldN > THE_SURFACE_TANGENT_TOLERANCE && CurT.Angle(OldT) > THE_TANGENT_REVERSAL_ANGLE)
        {
          Ok = false;
        }
        lambmin = std::min(lambmin, val);
      }
      Lambda.ChangeValue(aCoord)     = val;
      Lambda.ChangeValue(aCoord + 1) = val;
      Lambda.ChangeValue(aCoord + 2) = val;
      if (rational)
      {
        Tgte.ChangeValue(aCoord + 3)   = 0.0;
        Lambda.ChangeValue(aCoord + 3) = 0.0;
      }
      OldT = CurT;
      OldN = NTgte;
    }
    if (!Ok && Kount < THE_MAX_SURFACE_DEGREE_RETRIES)
    {
      // We increase the degree of the border iso to bring the poles of the surface closer
      // And we retry
      if (InU)
      {
        BS->IncreaseDegree(BS->UDegree(), BS->VDegree() + 2);
      }
      else
      {
        BS->IncreaseDegree(BS->UDegree() + 2, BS->VDegree());
      }
    }
  }

  NCollection_Array1<double> ConstraintPoint(1, Cdim);
  const double               aDirection = After ? 1.0 : -1.0;
  for (size_t anIndex = 0; anIndex < ConstraintPoint.Size(); ++anIndex)
  {
    ConstraintPoint.ChangeAt(anIndex) =
      Point.Value(anIndex) + aDirection * Lambda.Value(anIndex) * Tgte.Value(anIndex);
  }

  //  special case of rational
  if (rational)
  {
    for (size_t aPoleIndex = 0; aPoleIndex < Poles.Size(); aPoleIndex += gap)
    {
      const double aWeight = Poles.Value(aPoleIndex + 3);
      Poles.ChangeValue(aPoleIndex) *= aWeight;
      Poles.ChangeValue(aPoleIndex + 1) *= aWeight;
      Poles.ChangeValue(aPoleIndex + 2) *= aWeight;
    }
    for (size_t anIndex = 0; anIndex < ConstraintPoint.Size(); anIndex += gap)
    {
      const double aWeight = ConstraintPoint.At(anIndex + 3);
      ConstraintPoint.ChangeAt(anIndex) *= aWeight;
      ConstraintPoint.ChangeAt(anIndex + 1) *= aWeight;
      ConstraintPoint.ChangeAt(anIndex + 2) *= aWeight;
    }
  }

  //  arrays needed for the extension
  const size_t anExtendedFlatKnotCapacity =
    static_cast<size_t>(aFlatKnotCount + Cdeg);
  NCollection_Array1<double> FK(anExtendedFlatKnotCapacity);

  const size_t anExtendedPoleCoordinateCapacity =
    Poles.Size() + static_cast<size_t>(Cdeg) * static_cast<size_t>(Cdim);
  NCollection_Array1<double> PRes(anExtendedPoleCoordinateCapacity);
  bool                       ExtOk   = false;
  int                        NbPoles = 0, NbKnots = 0;
  NCollection_Array2<gp_Pnt> NewPoles;
  NCollection_Array2<double> NewWeights;

  for (Kount = 1; Kount <= THE_MAX_SURFACE_WEIGHT_ATTEMPTS && !ExtOk; ++Kount)
  {
    //  extension
    BSplCLib::TangExtendToConstraint(*aFlatKnots,
                                     lambmin,
                                     NbP,
                                     *Poles.Data(),
                                     Cdim,
                                     Cdeg,
                                     ConstraintPoint,
                                     Cont,
                                     After,
                                     NbPoles,
                                     NbKnots,
                                     *FK.Data(),
                                     *PRes.Data());

    //  Copy the result poles as 3D points and weights
    int NU, NV;
    if (InU)
    {
      NU = NbPoles;
      NV = BS->NbVPoles();
    }
    else
    {
      NU = BS->NbUPoles();
      NV = NbPoles;
    }

    NewPoles.Resize(static_cast<size_t>(NU), static_cast<size_t>(NV), false);
    NewWeights.Resize(static_cast<size_t>(NU), static_cast<size_t>(NV), false);

    if (!rational)
    {
      NewWeights.Init(1.);
    }
    bool NullWeight = false;

    for (size_t aUIndex = 0; aUIndex < NewPoles.RowSize() && !NullWeight; ++aUIndex)
    {
      for (size_t aVIndex = 0; aVIndex < NewPoles.ColSize() && !NullWeight; ++aVIndex)
      {
        const size_t anIndex = InU ? aUIndex * static_cast<size_t>(Cdim) + aVIndex * gap
                                   : aUIndex * gap + aVIndex * static_cast<size_t>(Cdim);
        gp_Pnt&      aPole   = NewPoles.ChangeAt(aUIndex, aVIndex);
        aPole.SetCoord(PRes.At(anIndex), PRes.At(anIndex + 1), PRes.At(anIndex + 2));
        if (rational)
        {
          double aWeight = PRes.At(anIndex + 3);
          if (std::abs(aWeight - 1.0) < THE_SURFACE_WEIGHT_TOLERANCE)
          {
            aWeight = 1.0;
          }
          if (aWeight < THE_SURFACE_WEIGHT_TOLERANCE)
          {
            NullWeight = true;
          }
          else
          {
            NewWeights.ChangeAt(aUIndex, aVIndex) = aWeight;
            aPole.ChangeCoord() /= aWeight;
          }
        }
      }
    }

    if (NullWeight)
    {
#ifdef OCCT_DEBUG
      std::cout << "Echec de l'Extension rationnelle" << std::endl;
#endif
      lambmin /= 3.;
    }
    else
    {
      ExtOk = true;
    }
  }

  // Copy flat knots as knots with their multiplicities.
  // Compute the degrees of the result.
  int Usize = BS->NbUKnots(), Vsize = BS->NbVKnots(), UDeg, VDeg;
  if (InU)
  {
    Usize++;
  }
  else
  {
    Vsize++;
  }
  NCollection_Array1<double> UKnots(1, Usize);
  NCollection_Array1<double> VKnots(1, Vsize);
  NCollection_Array1<int>    UMults(1, Usize);
  NCollection_Array1<int>    VMults(1, Vsize);
  NCollection_Array1<double> FKRes(1, NbKnots);

  for (size_t anIndex = 0; anIndex < FKRes.Size(); ++anIndex)
  {
    FKRes.ChangeAt(anIndex) = FK.At(anIndex);
  }

  if (InU)
  {
    BSplCLib::Knots(FKRes, UKnots, UMults);
    UDeg          = Cdeg;
    UMults(Usize) = UDeg + 1; // Petite verrue utile quand la continuite
                              // n'est pas ok.
    VKnots = BS->VKnots();
    VMults = BS->VMultiplicities();
    VDeg   = BS->VDegree();
  }
  else
  {
    BSplCLib::Knots(FKRes, VKnots, VMults);
    VDeg          = Cdeg;
    VMults(Vsize) = VDeg + 1;
    UKnots        = BS->UKnots();
    UMults        = BS->UMultiplicities();
    UDeg          = BS->UDegree();
  }

  //  Construct the resulting BSpline surface
  occ::handle<Geom_BSplineSurface> Res = new (Geom_BSplineSurface)(NewPoles,
                                                                   NewWeights,
                                                                   UKnots,
                                                                   VKnots,
                                                                   UMults,
                                                                   VMults,
                                                                   UDeg,
                                                                   VDeg,
                                                                   BS->IsUPeriodic(),
                                                                   BS->IsVPeriodic());
  Surface                              = Res;
}

//=================================================================================================

void GeomLib::Inertia(const NCollection_Array1<gp_Pnt>& Points,
                      gp_Pnt&                           Bary,
                      gp_Dir&                           XDir,
                      gp_Dir&                           YDir,
                      double&                           Xgap,
                      double&                           Ygap,
                      double&                           Zgap)
{
  gp_XYZ    GB(0., 0., 0.), Diff;
  const int nb = Points.Length();
  for (const gp_Pnt& aPoint : Points)
  {
    GB += aPoint.XYZ();
  }

  GB /= nb;

  math_Matrix M(1, 3, 1, 3);
  M.Init(0.);
  for (const gp_Pnt& aPoint : Points)
  {
    Diff.SetLinearForm(-1, aPoint.XYZ(), GB);
    M(1, 1) += Diff.X() * Diff.X();
    M(2, 2) += Diff.Y() * Diff.Y();
    M(3, 3) += Diff.Z() * Diff.Z();
    M(1, 2) += Diff.X() * Diff.Y();
    M(1, 3) += Diff.X() * Diff.Z();
    M(2, 3) += Diff.Y() * Diff.Z();
  }

  M(2, 1) = M(1, 2);
  M(3, 1) = M(1, 3);
  M(3, 2) = M(2, 3);

  M /= nb;

  math_Jacobi J(M);
  if (!J.IsDone())
  {
#ifdef OCCT_DEBUG
    std::cout << "Erreur dans Jacobbi" << std::endl;
    M.Dump(std::cout);
#endif
  }

  double n1, n2, n3;

  n1 = J.Value(1);
  n2 = J.Value(2);
  n3 = J.Value(3);

  double r1 = std::min(std::min(n1, n2), n3), r2;
  int    m1, m2, m3;
  if (r1 == n1)
  {
    m1 = 1;
    r2 = std::min(n2, n3);
    if (r2 == n2)
    {
      m2 = 2;
      m3 = 3;
    }
    else
    {
      m2 = 3;
      m3 = 2;
    }
  }
  else
  {
    if (r1 == n2)
    {
      m1 = 2;
      r2 = std::min(n1, n3);
      if (r2 == n1)
      {
        m2 = 1;
        m3 = 3;
      }
      else
      {
        m2 = 3;
        m3 = 1;
      }
    }
    else
    {
      m1 = 3;
      r2 = std::min(n1, n2);
      if (r2 == n1)
      {
        m2 = 1;
        m3 = 2;
      }
      else
      {
        m2 = 2;
        m3 = 1;
      }
    }
  }

  math_Vector V2(1, 3), V3(1, 3);
  J.Vector(m2, V2);
  J.Vector(m3, V3);

  Bary.SetXYZ(GB);
  XDir.SetCoord(V3(1), V3(2), V3(3));
  YDir.SetCoord(V2(1), V2(2), V2(3));

  Zgap = sqrt(std::abs(J.Value(m1)));
  Ygap = sqrt(std::abs(J.Value(m2)));
  Xgap = sqrt(std::abs(J.Value(m3)));
}

//=================================================================================================

void GeomLib::AxeOfInertia(const NCollection_Array1<gp_Pnt>& Points,
                           gp_Ax2&                           Axe,
                           bool&                             IsSingular,
                           const double                      Tol)
{
  gp_Pnt Bary;
  gp_Dir OX, OY, OZ;
  double gx, gy, gz;

  GeomLib::Inertia(Points, Bary, OX, OY, gx, gy, gz);

  if (gy * Points.Length() <= Tol)
  {
    gp_Ax2 axe(Bary, OX);
    OY         = axe.XDirection();
    IsSingular = true;
  }
  else
  {
    IsSingular = false;
  }

  OZ = OX ^ OY;
  gp_Ax2 TheAxe(Bary, OZ, OX);
  Axe = TheAxe;
}

//=================================================================================================

static bool CanBeTreated(const occ::handle<Geom_BSplineSurface>& BSurf)
{
  if (!BSurf->IsURational() || BSurf->IsUPeriodic())
  {
    return false;
  }

  const NCollection_Array2<double>& aWeights    = BSurf->WeightsArray();
  const size_t                      aLastUIndex = aWeights.RowSize() - 1;
  const double                      aLambda     = aWeights.At(0, 0) / aWeights.At(aLastUIndex, 0);
  for (size_t aVIndex = 0; aVIndex < aWeights.ColSize(); ++aVIndex)
  {
    const double aBoundaryRatio =
      aWeights.At(0, aVIndex) / (aLambda * aWeights.At(aLastUIndex, aVIndex));
    if (std::abs(aBoundaryRatio - 1.0) > Precision::Confusion())
    {
      return false;
    }
  }

  // The first and last weight pairs differ only before this treatment is applied.
  for (size_t aVIndex = 0; aVIndex < aWeights.ColSize(); ++aVIndex)
  {
    const double aFirstRatio = aWeights.At(0, aVIndex) / aWeights.At(1, aVIndex);
    const double aLastRatio =
      aWeights.At(aLastUIndex - 1, aVIndex) / aWeights.At(aLastUIndex, aVIndex);
    if (std::abs(aFirstRatio - 1.0) > Precision::Confusion()
        || std::abs(aLastRatio - 1.0) > Precision::Confusion())
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

class law_evaluator : public BSplSLib_EvaluatorFunction
{

public:
  law_evaluator(GeomLib_DenominatorMultiplier* theDenominatorPtr)
      : myDenominator(theDenominatorPtr)
  {
  }

  void Evaluate(const int    theDerivativeRequest,
                const double theUParameter,
                const double theVParameter,
                double&      theResult,
                int&         theErrorCode) const override
  {
    if ((myDenominator != nullptr) && (theDerivativeRequest == 0))
    {
      theResult    = myDenominator->Value(theUParameter, theVParameter);
      theErrorCode = 0;
    }
    else
    {
      theErrorCode = 1;
    }
  }

private:
  GeomLib_DenominatorMultiplier* myDenominator;
};

//=================================================================================================

static bool CheckIfKnotExists(const NCollection_Array1<double>& theKnots, const double theKnot)
{
  for (const double aKnot : theKnots)
  {
    if (std::abs(aKnot - theKnot) <= Precision::Confusion())
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

static void AddAKnot(const NCollection_Array1<double>& theKnots,
                     const NCollection_Array1<int>&    theMults,
                     const double                      theKnotToInsert,
                     const int                         theDegreeDelta,
                     const int                         theFinalDegree,
                     NCollection_Array1<double>&       theNewKnots,
                     NCollection_Array1<int>&          theNewMults)
{
  const size_t aResultSize = theKnots.Size() + 1;
  theNewKnots.Resize(aResultSize, false);
  theNewMults.Resize(aResultSize, false);

  size_t anInputIndex = 0;
  while (anInputIndex < theKnots.Size() && theKnots.At(anInputIndex) < theKnotToInsert)
  {
    theNewKnots.ChangeAt(anInputIndex) = theKnots.At(anInputIndex);
    theNewMults.ChangeAt(anInputIndex) = theMults.At(anInputIndex) + theDegreeDelta;
    ++anInputIndex;
  }

  theNewKnots.ChangeAt(anInputIndex) = theKnotToInsert;
  theNewMults.ChangeAt(anInputIndex) = theFinalDegree - 2;
  for (size_t anOutputIndex = anInputIndex + 1; anOutputIndex < aResultSize; ++anOutputIndex)
  {
    theNewKnots.ChangeAt(anOutputIndex) = theKnots.At(anOutputIndex - 1);
    theNewMults.ChangeAt(anOutputIndex) = theMults.At(anOutputIndex - 1) + theDegreeDelta;
  }
}

//=================================================================================================

static void BuildFlatKnot(const NCollection_Array1<double>& theSurfaceKnots,
                          const NCollection_Array1<int>&    theSurfaceMults,
                          const int                         theDegreeDelta,
                          const int                         theFinalDegree,
                          const double                      theKnotMin,
                          const double                      theKnotMax,
                          NCollection_Array1<double>&       theResultKnots,
                          NCollection_Array1<int>&          theResultMults)
{
  const bool hasMinKnot = CheckIfKnotExists(theSurfaceKnots, theKnotMin);
  const bool hasMaxKnot = CheckIfKnotExists(theSurfaceKnots, theKnotMax);
  if (hasMinKnot && hasMaxKnot)
  {
    theResultKnots.Resize(theSurfaceKnots.Size(), false);
    theResultMults.Resize(theSurfaceMults.Size(), false);
    for (size_t anIndex = 0; anIndex < theSurfaceMults.Size(); ++anIndex)
    {
      theResultKnots.ChangeAt(anIndex) = theSurfaceKnots.At(anIndex);
      theResultMults.ChangeAt(anIndex) = theSurfaceMults.At(anIndex) + theDegreeDelta;
    }
    return;
  }

  if (hasMinKnot || hasMaxKnot || theKnotMin == theKnotMax)
  {
    const double aKnotToInsert = hasMinKnot ? theKnotMax : theKnotMin;
    AddAKnot(theSurfaceKnots,
             theSurfaceMults,
             aKnotToInsert,
             theDegreeDelta,
             theFinalDegree,
             theResultKnots,
             theResultMults);
    return;
  }

  NCollection_Array1<double> anIntermediateKnots;
  NCollection_Array1<int>    anIntermediateMults;
  AddAKnot(theSurfaceKnots,
           theSurfaceMults,
           theKnotMin,
           theDegreeDelta,
           theFinalDegree,
           anIntermediateKnots,
           anIntermediateMults);
  AddAKnot(anIntermediateKnots,
           anIntermediateMults,
           theKnotMax,
           0,
           theFinalDegree,
           theResultKnots,
           theResultMults);
}

//=================================================================================================

static void FunctionMultiply(occ::handle<Geom_BSplineSurface>& BSurf,
                             const double                      knotmin,
                             const double                      knotmax)

{
  const NCollection_Array1<double>& surface_u_knots = BSurf->UKnots();
  const NCollection_Array1<int>&    surface_u_mults = BSurf->UMultiplicities();
  const NCollection_Array1<double>& surface_v_knots = BSurf->VKnots();
  const NCollection_Array1<int>&    surface_v_mults = BSurf->VMultiplicities();
  const NCollection_Array2<gp_Pnt>& surface_poles   = BSurf->Poles();
  const NCollection_Array2<double>* surface_weights = BSurf->Weights();
  int                               status          = 0;
  NCollection_Array1<double>        newuknots;
  NCollection_Array1<double>        newvknots;
  NCollection_Array1<int>           newumults;
  NCollection_Array1<int>           newvmults;

  NCollection_LocalArray<double, 2> aKnotStorage(2);
  NCollection_LocalArray<int, 2>    aMultStorage(2);
  NCollection_Array1<double>        Knots(*aKnotStorage.begin(), 1, 2);
  NCollection_Array1<int>           Mults(*aMultStorage.begin(), 1, 2);
  NCollection_Array1<double>        NewKnots;
  NCollection_Array1<int>           NewMults;

  Knots(1) = 0;
  Knots(2) = 1;
  Mults(1) = 4;
  Mults(2) = 4;
  BuildFlatKnot(Knots, Mults, 0, 3, knotmin, knotmax, NewKnots, NewMults);

  int length = 0;
  for (const int aMultiplicity : NewMults)
  {
    length += aMultiplicity;
  }
  NCollection_Array1<double> FlatKnots(1, length);
  BSplCLib::KnotSequence(NewKnots, NewMults, FlatKnots);

  GeomLib_DenominatorMultiplier aDenominator(BSurf, FlatKnots);

  BuildFlatKnot(surface_u_knots,
                surface_u_mults,
                3,
                BSurf->UDegree() + 3,
                knotmin,
                knotmax,
                newuknots,
                newumults);
  BuildFlatKnot(surface_v_knots,
                surface_v_mults,
                BSurf->VDegree(),
                2 * (BSurf->VDegree()),
                1.0,
                0.0,
                newvknots,
                newvmults);
  length = 0;
  for (const int aMultiplicity : newumults)
  {
    length += aMultiplicity;
  }
  const int                  new_num_u_poles = length - BSurf->UDegree() - 4;
  NCollection_Array1<double> newuflatknots(1, length);
  length = 0;
  for (const int aMultiplicity : newvmults)
  {
    length += aMultiplicity;
  }
  const int                  new_num_v_poles = length - 2 * BSurf->VDegree() - 1;
  NCollection_Array1<double> newvflatknots(1, length);

  NCollection_Array2<gp_Pnt> NewNumerator(1, new_num_u_poles, 1, new_num_v_poles);
  NCollection_Array2<double> NewDenominator(1, new_num_u_poles, 1, new_num_v_poles);

  BSplCLib::KnotSequence(newuknots, newumults, newuflatknots);
  BSplCLib::KnotSequence(newvknots, newvmults, newvflatknots);
  // POP for WNT
  law_evaluator ev(&aDenominator);
  // BSplSLib::FunctionMultiply(law_evaluator,               //multiplication
  BSplSLib::FunctionMultiply(ev, // multiplication
                             BSurf->UDegree(),
                             BSurf->VDegree(),
                             surface_u_knots,
                             surface_v_knots,
                             &surface_u_mults,
                             &surface_v_mults,
                             surface_poles,
                             surface_weights,
                             newuflatknots,
                             newvflatknots,
                             BSurf->UDegree() + 3,
                             2 * (BSurf->VDegree()),
                             NewNumerator,
                             NewDenominator,
                             status);
  if (status != 0)
  {
    throw Standard_ConstructionError("GeomLib Multiplication Error");
  }
  for (size_t aUIndex = 0; aUIndex < NewNumerator.RowSize(); ++aUIndex)
  {
    for (size_t aVIndex = 0; aVIndex < NewNumerator.ColSize(); ++aVIndex)
    {
      NewNumerator.ChangeAt(aUIndex, aVIndex).ChangeCoord() /= NewDenominator.At(aUIndex, aVIndex);
    }
  }
  BSurf = new Geom_BSplineSurface(NewNumerator,
                                  NewDenominator,
                                  newuknots,
                                  newvknots,
                                  newumults,
                                  newvmults,
                                  BSurf->UDegree() + 3,
                                  2 * (BSurf->VDegree()));
}

//=================================================================================================

static void CancelDenominatorDerivative1D(occ::handle<Geom_BSplineSurface>& BSurf)

{
  if (!CanBeTreated(BSurf))
  {
    return;
  }

  double uknotmin = 1.0, uknotmax = 0.0, x, y;

  const double               startu_value = BSurf->UKnot(1);
  const double               endu_value   = BSurf->UKnot(BSurf->NbUKnots());
  NCollection_Array1<double> BSurf_u_knots(BSurf->UKnots());
  BSplCLib::Reparametrize(0.0, 1.0, BSurf_u_knots);
  BSurf->SetUKnots(BSurf_u_knots); // reparametrisation of the surface
  NCollection_Array1<double> BCurveWeights(1, BSurf->NbUPoles());
  NCollection_Array1<gp_Pnt> BCurvePoles(1, BSurf->NbUPoles());

  const NCollection_Array1<double>& BCurveKnots     = BSurf->UKnots();
  const NCollection_Array1<int>&    BCurveMults     = BSurf->UMultiplicities();
  const NCollection_Array2<double>& aSurfaceWeights = BSurf->WeightsArray();
  const NCollection_Array2<gp_Pnt>& aSurfacePoles   = BSurf->Poles();
  for (size_t aVIndex = 0; aVIndex < aSurfacePoles.ColSize(); ++aVIndex)
  {
    // Build each scalar weight function in the reusable pole and weight buffers.
    x = 1.0;
    y = 0.0;
    for (size_t aUIndex = 0; aUIndex < BCurveWeights.Size(); ++aUIndex)
    {
      BCurveWeights.ChangeAt(aUIndex) = aSurfaceWeights.At(aUIndex, aVIndex);
      BCurvePoles.ChangeAt(aUIndex)   = aSurfacePoles.At(aUIndex, aVIndex);
    }
    const occ::handle<Geom_BSplineCurve> BCurve =
      new Geom_BSplineCurve(BCurvePoles, BCurveWeights, BCurveKnots, BCurveMults, BSurf->UDegree());
    Hermit::Solutionbis(BCurve, x, y, Precision::Confusion(), Precision::Confusion());
    if (x < uknotmin)
    {
      uknotmin = x; // uknotmin,uknotmax:extremal knots
    }
    if ((x != 1.0) && (x > uknotmax))
    {
      uknotmax = x;
    }
    if ((y != 0.0) && (y < uknotmin))
    {
      uknotmin = y;
    }
    if (y > uknotmax)
    {
      uknotmax = y;
    }
  }

  FunctionMultiply(BSurf, uknotmin, uknotmax); // multiplication

  BSurf_u_knots = BSurf->UKnots();
  BSplCLib::Reparametrize(startu_value, endu_value, BSurf_u_knots);
  BSurf->SetUKnots(BSurf_u_knots);
}

//=================================================================================================

void GeomLib::CancelDenominatorDerivative(occ::handle<Geom_BSplineSurface>& BSurf,
                                          const bool                        udirection,
                                          const bool                        vdirection)

{
  if (udirection && !vdirection)
  {
    CancelDenominatorDerivative1D(BSurf);
  }
  else
  {
    if (!udirection && vdirection)
    {
      BSurf->ExchangeUV();
      CancelDenominatorDerivative1D(BSurf);
      BSurf->ExchangeUV();
    }
    else
    {
      if (udirection && vdirection)
      { // optimize the treatment
        if (BSurf->UDegree() <= BSurf->VDegree())
        {
          CancelDenominatorDerivative1D(BSurf);
          BSurf->ExchangeUV();
          CancelDenominatorDerivative1D(BSurf);
          BSurf->ExchangeUV();
        }
        else
        {
          BSurf->ExchangeUV();
          CancelDenominatorDerivative1D(BSurf);
          BSurf->ExchangeUV();
          CancelDenominatorDerivative1D(BSurf);
        }
      }
    }
  }
}

//=================================================================================================

int GeomLib::NormEstim(const occ::handle<Geom_Surface>& theSurf,
                       const gp_Pnt2d&                  theUV,
                       const double                     theTol,
                       gp_Dir&                          theNorm)
{
  const double aTol2 = Square(theTol);

  const Geom_Surface::ResD1 aFirstEvaluation = theSurf->EvalD1(theUV.X(), theUV.Y());
  const double MDU = aFirstEvaluation.D1U.SquareMagnitude();
  const double MDV = aFirstEvaluation.D1V.SquareMagnitude();
  if (MDU >= aTol2 && MDV >= aTol2)
  {
    const gp_Vec aNorm = aFirstEvaluation.D1U ^ aFirstEvaluation.D1V;
    const double aMagn = aNorm.SquareMagnitude();
    if (aMagn < aTol2)
    {
      return 3;
    }

    theNorm.SetXYZ(aNorm.XYZ());
    return 0;
  }

  bool               isDone = false;
  CSLib_NormalStatus aStatus;
  gp_Dir             aNormal;

  const Geom_Surface::ResD2 aSecondEvaluation = theSurf->EvalD2(theUV.X(), theUV.Y());
  CSLib::Normal(aSecondEvaluation.D1U,
                aSecondEvaluation.D1V,
                aSecondEvaluation.D2U,
                aSecondEvaluation.D2V,
                aSecondEvaluation.D2UV,
                theTol,
                isDone,
                aStatus,
                aNormal);
  if (!isDone)
  {
    // computation is impossible
    return aStatus == CSLib_D1NIsNull ? 2 : 3;
  }

  double Umin, Umax, Vmin, Vmax;
  double sign = -1.0;
  theSurf->Bounds(Umin, Umax, Vmin, Vmax);

  // check for cone apex singularity point
  if ((theUV.Y() > Vmin + THE_NORMAL_PARAMETER_STEP)
      && (theUV.Y() < Vmax - THE_NORMAL_PARAMETER_STEP))
  {
    const Geom_Surface::ResD1 anEvaluationBefore =
      theSurf->EvalD1(theUV.X(), theUV.Y() - sign * THE_NORMAL_PARAMETER_STEP);
    if (anEvaluationBefore.D1U.SquareMagnitude() > THE_NORMAL_SQUARED_MAGNITUDE_TOL
        && anEvaluationBefore.D1V.SquareMagnitude() > THE_NORMAL_SQUARED_MAGNITUDE_TOL)
    {
      const gp_Dir aNormal1(anEvaluationBefore.D1U ^ anEvaluationBefore.D1V);
      const Geom_Surface::ResD1 anEvaluationAfter =
        theSurf->EvalD1(theUV.X(), theUV.Y() + sign * THE_NORMAL_PARAMETER_STEP);
      if (anEvaluationAfter.D1U.SquareMagnitude() > THE_NORMAL_SQUARED_MAGNITUDE_TOL
          && anEvaluationAfter.D1V.SquareMagnitude() > THE_NORMAL_SQUARED_MAGNITUDE_TOL)
      {
        const gp_Dir aNormal2(anEvaluationAfter.D1U ^ anEvaluationAfter.D1V);
        if (aNormal1.IsOpposite(aNormal2, THE_CONE_NORMAL_ANGULAR_TOLERANCE))
        {
          return 2;
        }
      }
    }
  }

  // Along V
  if (MDU < aTol2 && MDV >= aTol2)
  {
    if ((Vmax - theUV.Y()) > (theUV.Y() - Vmin))
    {
      sign = 1.0;
    }

    const Geom_Surface::ResD1 aShiftedEvaluation =
      theSurf->EvalD1(theUV.X(), theUV.Y() + sign * THE_NORMAL_PARAMETER_STEP);
    gp_Vec Norm = aShiftedEvaluation.D1U ^ aShiftedEvaluation.D1V;
    if (Norm.SquareMagnitude() < THE_NORMAL_SQUARED_MAGNITUDE_TOL)
    {
      double sign1 = -1.0;
      if ((Umax - theUV.X()) > (theUV.X() - Umin))
      {
        sign1 = 1.0;
      }
      const Geom_Surface::ResD1 aDiagonalEvaluation =
        theSurf->EvalD1(theUV.X() + sign1 * THE_NORMAL_PARAMETER_STEP,
                        theUV.Y() + sign * THE_NORMAL_PARAMETER_STEP);
      Norm = aDiagonalEvaluation.D1U ^ aDiagonalEvaluation.D1V;
    }
    if (Norm.SquareMagnitude() >= THE_NORMAL_SQUARED_MAGNITUDE_TOL && Norm.Dot(aNormal) < 0.0)
    {
      aNormal.Reverse();
    }
  }

  // Along U
  if (MDV < aTol2 && MDU >= aTol2)
  {
    if ((Umax - theUV.X()) > (theUV.X() - Umin))
    {
      sign = 1.0;
    }

    const Geom_Surface::ResD1 aShiftedEvaluation =
      theSurf->EvalD1(theUV.X() + sign * THE_NORMAL_PARAMETER_STEP, theUV.Y());
    gp_Vec Norm = aShiftedEvaluation.D1U ^ aShiftedEvaluation.D1V;
    if (Norm.SquareMagnitude() < THE_NORMAL_SQUARED_MAGNITUDE_TOL)
    {
      double sign1 = -1.0;
      if ((Vmax - theUV.Y()) > (theUV.Y() - Vmin))
      {
        sign1 = 1.0;
      }

      const Geom_Surface::ResD1 aDiagonalEvaluation =
        theSurf->EvalD1(theUV.X() + sign * THE_NORMAL_PARAMETER_STEP,
                        theUV.Y() + sign1 * THE_NORMAL_PARAMETER_STEP);
      Norm = aDiagonalEvaluation.D1U ^ aDiagonalEvaluation.D1V;
    }
    if (Norm.SquareMagnitude() >= THE_NORMAL_SQUARED_MAGNITUDE_TOL && Norm.Dot(aNormal) < 0.0)
    {
      aNormal.Reverse();
    }
  }

  // quasysingular
  if (aStatus == CSLib_D1NuIsNull || aStatus == CSLib_D1NvIsNull
      || aStatus == CSLib_D1NuIsParallelD1Nv)
  {
    theNorm.SetXYZ(aNormal.XYZ());
    return 1;
  }

  return aStatus == CSLib_InfinityOfSolutions ? 2 : 3;
}

//=================================================================================================

void GeomLib::IsClosed(const occ::handle<Geom_Surface>& S,
                       const double                     Tol,
                       bool&                            isUClosed,
                       bool&                            isVClosed)
{
  isUClosed = false;
  isVClosed = false;
  //
  GeomAdaptor_Surface aGAS(S);
  GeomAbs_SurfaceType aSType = aGAS.GetType();
  //
  double u1, u2, v1, v2;
  u1 = aGAS.FirstUParameter();
  u2 = aGAS.LastUParameter();
  v1 = aGAS.FirstVParameter();
  v2 = aGAS.LastVParameter();
  //
  double Tol2 = Tol * Tol;
  switch (aSType)
  {
    case GeomAbs_Plane: {
      return;
    }
    case GeomAbs_SurfaceOfExtrusion: {
      if (Precision::IsInfinite(u1) || Precision::IsInfinite(u2))
      {
        // not closed
        return;
      }
    }
      [[fallthrough]];
    case GeomAbs_Cylinder: {
      if (Precision::IsInfinite(v1))
      {
        v1 = 0.;
      }
      const gp_Pnt p1 = aGAS.EvalD0(u1, v1);
      const gp_Pnt p2 = aGAS.EvalD0(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Cone: {
      // find v with maximal distance from axis
      if (!(Precision::IsInfinite(v1) || Precision::IsInfinite(v2)))
      {
        gp_Cone aCone  = aGAS.Cone();
        gp_Pnt  anApex = aCone.Apex();
        const gp_Pnt P1     = aGAS.EvalD0(u1, v1);
        const gp_Pnt P2     = aGAS.EvalD0(u1, v2);
        if (P2.SquareDistance(anApex) > P1.SquareDistance(anApex))
        {
          v1 = v2;
        }
      }
      else
      {
        v1 = 0.;
      }
      const gp_Pnt p1 = aGAS.EvalD0(u1, v1);
      const gp_Pnt p2 = aGAS.EvalD0(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Sphere: {
      // find v with maximal distance from axis
      if (v1 * v2 <= 0.)
      {
        v1 = 0.;
      }
      else
      {
        if (v1 < 0.)
        {
          v1 = v2;
        }
      }
      const gp_Pnt p1 = aGAS.EvalD0(u1, v1);
      const gp_Pnt p2 = aGAS.EvalD0(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Torus: {
      double ures = aGAS.UResolution(Tol);
      double vres = aGAS.VResolution(Tol);
      //
      isUClosed = (u2 - u1) >= aGAS.UPeriod() - ures;
      isVClosed = (v2 - v1) >= aGAS.VPeriod() - vres;
      return;
    }
    case GeomAbs_BSplineSurface: {
      occ::handle<Geom_BSplineSurface> aBSpl = aGAS.BSpline();
      isUClosed                              = GeomLib::IsBSplUClosed(aBSpl, u1, u2, Tol);
      isVClosed                              = GeomLib::IsBSplVClosed(aBSpl, v1, v2, Tol);
      return;
    }
    case GeomAbs_BezierSurface: {
      occ::handle<Geom_BezierSurface> aBz = aGAS.Bezier();
      isUClosed                           = GeomLib::IsBzUClosed(aBz, u1, u2, Tol);
      isVClosed                           = GeomLib::IsBzVClosed(aBz, v1, v2, Tol);
      return;
    }
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_OffsetSurface:
    case GeomAbs_OtherSurface: {
      int nbp = THE_CLOSEDNESS_SAMPLE_COUNT;
      if (Precision::IsInfinite(v1))
      {
        v1 = std::copysign(1., v1);
      }
      if (Precision::IsInfinite(v2))
      {
        v2 = std::copysign(1., v2);
      }
      //
      if (aSType == GeomAbs_OffsetSurface || aSType == GeomAbs_OtherSurface)
      {
        if (Precision::IsInfinite(u1))
        {
          u1 = std::copysign(1., u1);
        }
        if (Precision::IsInfinite(u2))
        {
          u2 = std::copysign(1., u2);
        }
      }
      isUClosed  = true;
      double dt  = (v2 - v1) / (nbp - 1);
      double res = std::max(aGAS.UResolution(Tol), Precision::PConfusion());
      if (dt <= res)
      {
        nbp = RealToInt((v2 - v1) / (2. * res)) + 1;
        nbp = std::max(nbp, 2);
        dt  = (v2 - v1) / (nbp - 1);
      }
      double t;
      int    i;
      for (i = 0; i < nbp; ++i)
      {
        t         = (i == nbp - 1 ? v2 : v1 + i * dt);
        const gp_Pnt p1 = aGAS.EvalD0(u1, t);
        const gp_Pnt p2 = aGAS.EvalD0(u2, t);
        if (p1.SquareDistance(p2) > Tol2)
        {
          isUClosed = false;
          break;
        }
      }
      //
      nbp       = THE_CLOSEDNESS_SAMPLE_COUNT;
      isVClosed = true;
      dt        = (u2 - u1) / (nbp - 1);
      res       = std::max(aGAS.VResolution(Tol), Precision::PConfusion());
      if (dt <= res)
      {
        nbp = RealToInt((u2 - u1) / (2. * res)) + 1;
        nbp = std::max(nbp, 2);
        dt  = (u2 - u1) / (nbp - 1);
      }
      for (i = 0; i < nbp; ++i)
      {
        t         = (i == nbp - 1 ? u2 : u1 + i * dt);
        const gp_Pnt p1 = aGAS.EvalD0(t, v1);
        const gp_Pnt p2 = aGAS.EvalD0(t, v2);
        if (p1.SquareDistance(p2) > Tol2)
        {
          isVClosed = false;
          break;
        }
      }
      return;
    }
    default: {
      return;
    }
  }
}

//=================================================================================================

bool GeomLib::IsBSplUClosed(const occ::handle<Geom_BSplineSurface>& S,
                            const double                            U1,
                            const double                            U2,
                            const double                            Tol)
{
  occ::handle<Geom_Curve> aCUF = S->UIso(U1);
  occ::handle<Geom_Curve> aCUL = S->UIso(U2);
  if (aCUF.IsNull() || aCUL.IsNull())
  {
    return false;
  }
  const double                      Tol2 = 2. * Tol;
  occ::handle<Geom_BSplineCurve>    aBsF = occ::down_cast<Geom_BSplineCurve>(aCUF);
  occ::handle<Geom_BSplineCurve>    aBsL = occ::down_cast<Geom_BSplineCurve>(aCUL);
  const NCollection_Array1<gp_Pnt>& aPF  = aBsF->Poles();
  const NCollection_Array1<gp_Pnt>& aPL  = aBsL->Poles();
  const NCollection_Array1<double>* WF   = aBsF->Weights();
  const NCollection_Array1<double>* WL   = aBsL->Weights();
  return CompareWeightPoles(aPF, WF, aPL, WL, Tol2);
}

//=================================================================================================

bool GeomLib::IsBSplVClosed(const occ::handle<Geom_BSplineSurface>& S,
                            const double                            V1,
                            const double                            V2,
                            const double                            Tol)
{
  occ::handle<Geom_Curve> aCVF = S->VIso(V1);
  occ::handle<Geom_Curve> aCVL = S->VIso(V2);
  if (aCVF.IsNull() || aCVL.IsNull())
  {
    return false;
  }
  const double                      Tol2 = 2. * Tol;
  occ::handle<Geom_BSplineCurve>    aBsF = occ::down_cast<Geom_BSplineCurve>(aCVF);
  occ::handle<Geom_BSplineCurve>    aBsL = occ::down_cast<Geom_BSplineCurve>(aCVL);
  const NCollection_Array1<gp_Pnt>& aPF  = aBsF->Poles();
  const NCollection_Array1<gp_Pnt>& aPL  = aBsL->Poles();
  const NCollection_Array1<double>* WF   = aBsF->Weights();
  const NCollection_Array1<double>* WL   = aBsL->Weights();
  return CompareWeightPoles(aPF, WF, aPL, WL, Tol2);
}

//=================================================================================================

bool GeomLib::IsBzUClosed(const occ::handle<Geom_BezierSurface>& S,
                          const double                           U1,
                          const double                           U2,
                          const double                           Tol)
{
  occ::handle<Geom_Curve> aCUF = S->UIso(U1);
  occ::handle<Geom_Curve> aCUL = S->UIso(U2);
  if (aCUF.IsNull() || aCUL.IsNull())
  {
    return false;
  }
  const double                      Tol2 = 2. * Tol;
  occ::handle<Geom_BezierCurve>     aBzF = occ::down_cast<Geom_BezierCurve>(aCUF);
  occ::handle<Geom_BezierCurve>     aBzL = occ::down_cast<Geom_BezierCurve>(aCUL);
  const NCollection_Array1<gp_Pnt>& aPF  = aBzF->Poles();
  const NCollection_Array1<gp_Pnt>& aPL  = aBzL->Poles();
  const NCollection_Array1<double>* WF   = aBzF->Weights();
  const NCollection_Array1<double>* WL   = aBzL->Weights();
  return CompareWeightPoles(aPF, WF, aPL, WL, Tol2);
}

//=================================================================================================

bool GeomLib::IsBzVClosed(const occ::handle<Geom_BezierSurface>& S,
                          const double                           V1,
                          const double                           V2,
                          const double                           Tol)
{
  occ::handle<Geom_Curve> aCVF = S->VIso(V1);
  occ::handle<Geom_Curve> aCVL = S->VIso(V2);
  if (aCVF.IsNull() || aCVL.IsNull())
  {
    return false;
  }
  const double                      Tol2 = 2. * Tol;
  occ::handle<Geom_BezierCurve>     aBzF = occ::down_cast<Geom_BezierCurve>(aCVF);
  occ::handle<Geom_BezierCurve>     aBzL = occ::down_cast<Geom_BezierCurve>(aCVL);
  const NCollection_Array1<gp_Pnt>& aPF  = aBzF->Poles();
  const NCollection_Array1<gp_Pnt>& aPL  = aBzL->Poles();
  const NCollection_Array1<double>* WF   = aBzF->Weights();
  const NCollection_Array1<double>* WL   = aBzL->Weights();
  return CompareWeightPoles(aPF, WF, aPL, WL, Tol2);
}

//=================================================================================================
static bool CompareWeightPoles(const NCollection_Array1<gp_Pnt>&       thePoles1,
                               const NCollection_Array1<double>* const theW1,
                               const NCollection_Array1<gp_Pnt>&       thePoles2,
                               const NCollection_Array1<double>* const theW2,
                               const double                            theTol)
{
  if (thePoles1.Length() != thePoles2.Length())
  {
    return false;
  }

  // Rational control points are projective: multiplying every homogeneous coordinate of one
  // boundary by the same non-zero factor does not change its geometry. Normalize both boundaries
  // by their first weights and compare all four homogeneous coordinates.
  const double aReferenceWeight1 = (theW1 == nullptr) ? 1.0 : theW1->First();
  const double aReferenceWeight2 = (theW2 == nullptr) ? 1.0 : theW2->First();
  for (size_t anIndex = 0; anIndex < thePoles1.Size(); ++anIndex)
  {
    const double aW1 = ((theW1 == nullptr) ? 1.0 : theW1->At(anIndex)) / aReferenceWeight1;
    const double aW2 = ((theW2 == nullptr) ? 1.0 : theW2->At(anIndex)) / aReferenceWeight2;

    const gp_XYZ aPole1 = thePoles1.At(anIndex).XYZ() * aW1;
    const gp_XYZ aPole2 = thePoles2.At(anIndex).XYZ() * aW2;
    if (std::abs(aW1 - aW2) > theTol || !aPole1.IsEqual(aPole2, theTol))
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool GeomLib::isIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                        bool&                                 theIsU,
                        double&                               theParam,
                        bool&                                 theIsForward)
{
  // These variables are used to check line state (vertical or horizontal).
  bool     isAppropriateType = false;
  gp_Pnt2d aLoc2d;
  gp_Dir2d aDir2d;

  // Test type.
  const GeomAbs_CurveType aType = theC2D->GetType();
  if (aType == GeomAbs_Line)
  {
    gp_Lin2d aLin2d   = theC2D->Line();
    aLoc2d            = aLin2d.Location();
    aDir2d            = aLin2d.Direction();
    isAppropriateType = true;
  }
  else if (aType == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom2d_BSplineCurve> aBSpline2d = theC2D->BSpline();
    if (aBSpline2d->Degree() != 1 || aBSpline2d->NbPoles() != 2)
    {
      return false; // Not a line or uneven parameterization.
    }

    aLoc2d = aBSpline2d->Pole(1);

    // Vector should be non-degenerated.
    gp_Vec2d aVec2d(aBSpline2d->Pole(1), aBSpline2d->Pole(2));
    if (aVec2d.SquareMagnitude() < Square(Precision::Confusion()))
    {
      return false; // Degenerated spline.
    }
    aDir2d = aVec2d;

    isAppropriateType = true;
  }
  else if (aType == GeomAbs_BezierCurve)
  {
    occ::handle<Geom2d_BezierCurve> aBezier2d = theC2D->Bezier();
    if (aBezier2d->Degree() != 1 || aBezier2d->NbPoles() != 2)
    {
      return false; // Not a line or uneven parameterization.
    }

    aLoc2d = aBezier2d->Pole(1);

    // Vector should be non-degenerated.
    gp_Vec2d aVec2d(aBezier2d->Pole(1), aBezier2d->Pole(2));
    if (aVec2d.SquareMagnitude() < Square(Precision::Confusion()))
    {
      return false; // Degenerated spline.
    }
    aDir2d = aVec2d;

    isAppropriateType = true;
  }

  if (!isAppropriateType)
  {
    return false;
  }

  // Check line to be vertical or horizontal.
  if (aDir2d.IsParallel(gp::DX2d(), Precision::Angular()))
  {
    // Horizontal line. V = const.
    theIsU       = false;
    theParam     = aLoc2d.Y();
    theIsForward = aDir2d.Dot(gp::DX2d()) > 0.0;
    return true;
  }
  else if (aDir2d.IsParallel(gp::DY2d(), Precision::Angular()))
  {
    // Vertical line. U = const.
    theIsU       = true;
    theParam     = aLoc2d.X();
    theIsForward = aDir2d.Dot(gp::DY2d()) > 0.0;
    return true;
  }

  return false;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomLib::buildC3dOnIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                                                   const occ::handle<Adaptor3d_Surface>& theSurf,
                                                   const double                          theFirst,
                                                   const double                          theLast,
                                                   const double theTolerance,
                                                   const bool   theIsU,
                                                   const double theParam,
                                                   const bool   theIsForward)
{
  // Convert adapter to the appropriate type.
  occ::handle<GeomAdaptor_Surface> aGeomAdapter = occ::down_cast<GeomAdaptor_Surface>(theSurf);
  if (aGeomAdapter.IsNull())
  {
    return occ::handle<Geom_Curve>();
  }

  if (theSurf->GetType() == GeomAbs_Sphere)
  {
    return occ::handle<Geom_Curve>();
  }

  // Extract isoline
  occ::handle<Geom_Surface> aSurf = aGeomAdapter->Surface();
  occ::handle<Geom_Curve>   aC3d;

  const gp_Pnt2d aF2d = theC2D->EvalD0(theC2D->FirstParameter());
  const gp_Pnt2d aL2d = theC2D->EvalD0(theC2D->LastParameter());

  bool   isToTrim = true;
  double U1, U2, V1, V2;
  aSurf->Bounds(U1, U2, V1, V2);

  if (theIsU)
  {
    double aV1Param = std::min(aF2d.Y(), aL2d.Y());
    double aV2Param = std::max(aF2d.Y(), aL2d.Y());
    if (aV2Param < V1 - theTolerance || aV1Param > V2 + theTolerance)
    {
      return occ::handle<Geom_Curve>();
    }
    else if (Precision::IsInfinite(V1) || Precision::IsInfinite(V2))
    {
      if (std::abs(aV2Param - aV1Param) < Precision::PConfusion())
      {
        return occ::handle<Geom_Curve>();
      }
      aSurf    = new Geom_RectangularTrimmedSurface(aSurf, U1, U2, aV1Param, aV2Param);
      isToTrim = false;
    }
    else
    {
      aV1Param = std::max(aV1Param, V1);
      aV2Param = std::min(aV2Param, V2);
      if (std::abs(aV2Param - aV1Param) < Precision::PConfusion())
      {
        return occ::handle<Geom_Curve>();
      }
    }
    aC3d = aSurf->UIso(theParam);
    if (isToTrim)
    {
      aC3d = new Geom_TrimmedCurve(aC3d, aV1Param, aV2Param);
    }
  }
  else
  {
    double aU1Param = std::min(aF2d.X(), aL2d.X());
    double aU2Param = std::max(aF2d.X(), aL2d.X());
    if (aU2Param < U1 - theTolerance || aU1Param > U2 + theTolerance)
    {
      return occ::handle<Geom_Curve>();
    }
    else if (Precision::IsInfinite(U1) || Precision::IsInfinite(U2))
    {
      if (std::abs(aU2Param - aU1Param) < Precision::PConfusion())
      {
        return occ::handle<Geom_Curve>();
      }
      aSurf    = new Geom_RectangularTrimmedSurface(aSurf, aU1Param, aU2Param, V1, V2);
      isToTrim = false;
    }
    else
    {
      aU1Param = std::max(aU1Param, U1);
      aU2Param = std::min(aU2Param, U2);
      if (std::abs(aU2Param - aU1Param) < Precision::PConfusion())
      {
        return occ::handle<Geom_Curve>();
      }
    }
    aC3d = aSurf->VIso(theParam);
    if (isToTrim)
    {
      aC3d = new Geom_TrimmedCurve(aC3d, aU1Param, aU2Param);
    }
  }

  // Convert arbitrary curve type to the b-spline.
  occ::handle<Geom_BSplineCurve> aCurve3d =
    GeomConvert::CurveToBSplineCurve(aC3d, Convert_QuasiAngular);
  if (!theIsForward)
  {
    aCurve3d->Reverse();
  }

  // Rebuild parameterization for the 3d curve to have the same parameterization with
  // a two-dimensional curve.
  NCollection_Array1<double> aKnots = aCurve3d->Knots();
  BSplCLib::Reparametrize(theC2D->FirstParameter(), theC2D->LastParameter(), aKnots);
  aCurve3d->SetKnots(aKnots);

  // Evaluate error.
  double anError3d = 0.0;

  const double aParF = theFirst;
  const double aParL = theLast;
  for (int anIdx = 0; anIdx <= THE_ISOLINE_CHECK_SEGMENTS; ++anIdx)
  {
    const double aPar = aParF + ((aParL - aParF) * anIdx) / THE_ISOLINE_CHECK_SEGMENTS;

    const gp_Pnt2d aPnt2d = theC2D->EvalD0(aPar);

    const gp_Pnt aPntC3D = aCurve3d->EvalD0(aPar);
    const gp_Pnt aPntC2D = theSurf->EvalD0(aPnt2d.X(), aPnt2d.Y());

    const double aSqDeviation = aPntC3D.SquareDistance(aPntC2D);
    anError3d                 = std::max(aSqDeviation, anError3d);
  }

  anError3d = std::sqrt(anError3d);

  // Target tolerance is not obtained. This situation happens for isolines on the sphere.
  // OCCT is unable to convert it keeping original parameterization, while the geometric
  // form of the result is entirely identical. In that case, it is better to utilize
  // a general-purpose approach.
  if (anError3d > theTolerance)
  {
    return occ::handle<Geom_Curve>();
  }

  return aCurve3d;
}
