// Copyright (c) 1995-1999 Matra Datavision
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

#include <Geom2dConvert.hxx>

#include <Convert_CircleToBSplineCurve.hxx>
#include <Convert_ConicToBSplineCurve.hxx>
#include <Convert_EllipseToBSplineCurve.hxx>
#include <Convert_HyperbolaToBSplineCurve.hxx>
#include <Convert_ParabolaToBSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Hermit.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>

typedef gp_Circ2d                    Circ2d;
typedef gp_Elips2d                   Elips2d;
typedef gp_Hypr2d                    Hypr2d;
typedef gp_Parab2d                   Parab2d;
typedef gp_Pnt2d                     Pnt2d;
typedef gp_Trsf2d                    Trsf2d;
typedef Geom2d_Curve                 Curve;
typedef Geom2d_BSplineCurve          BSplineCurve;
typedef NCollection_Array1<double>   Array1OfReal;
typedef NCollection_Array1<int>      Array1OfInteger;
typedef NCollection_Array1<gp_Pnt2d> Array1OfPnt2d;

//=================================================================================================

static occ::handle<Geom2d_BSplineCurve> BSplineCurveBuilder(

  const occ::handle<Geom2d_Conic>&   TheConic,
  const Convert_ConicToBSplineCurve& Convert)
{

  occ::handle<Geom2d_BSplineCurve> TheCurve;
  int                              NbPoles = Convert.NbPoles();
  int                              NbKnots = Convert.NbKnots();
  Array1OfPnt2d                    Poles(1, NbPoles);
  Array1OfReal                     Weights(1, NbPoles);
  Array1OfReal                     Knots(1, NbKnots);
  Array1OfInteger                  Mults(1, NbKnots);
  int                              i;
  for (i = 1; i <= NbPoles; i++)
  {
    Poles(i)   = Convert.Pole(i);
    Weights(i) = Convert.Weight(i);
  }
  for (i = 1; i <= NbKnots; i++)
  {
    Knots(i) = Convert.Knot(i);
    Mults(i) = Convert.Multiplicity(i);
  }
  TheCurve = new BSplineCurve(Poles, Weights, Knots, Mults, Convert.Degree(), Convert.IsPeriodic());

  gp_Ax22d Axis = TheConic->Position();
  if ((Axis.XDirection() ^ Axis.YDirection()) < 0.)
  {
    // Then the axis is left-handed, apply a symmetry to the curve.
    gp_Trsf2d Sym;
    Sym.SetMirror(gp::OX2d());
    TheCurve->Transform(Sym);
  }

  Trsf2d T;
  T.SetTransformation(TheConic->XAxis(), gp::OX2d());
  occ::handle<Geom2d_BSplineCurve> Cres =
    occ::down_cast<Geom2d_BSplineCurve>(TheCurve->Transformed(T));
  return Cres;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> Geom2dConvert::SplitBSplineCurve(

  const occ::handle<Geom2d_BSplineCurve>& C,
  const int                               FromK1,
  const int                               ToK2,
  const bool                              SameOrientation)
{

  int TheFirst = C->FirstUKnotIndex();
  int TheLast  = C->LastUKnotIndex();
  if (FromK1 == ToK2)
    throw Standard_DomainError();
  int FirstK = std::min(FromK1, ToK2);
  int LastK  = std::max(FromK1, ToK2);
  if (FirstK < TheFirst || LastK > TheLast)
    throw Standard_OutOfRange();

  occ::handle<Geom2d_BSplineCurve> NewCurve = occ::down_cast<Geom2d_BSplineCurve>(C->Copy());

  NewCurve->Segment(C->Knot(FirstK), C->Knot(LastK));

  if (C->IsPeriodic())
  {
    if (!SameOrientation)
      NewCurve->Reverse();
  }
  else
  {
    if (FromK1 > ToK2)
      NewCurve->Reverse();
  }
  return NewCurve;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> Geom2dConvert::SplitBSplineCurve(

  const occ::handle<Geom2d_BSplineCurve>& C,
  const double                            FromU1,
  const double                            ToU2,
  const double, // ParametricTolerance,
  const bool SameOrientation)
{
  double FirstU = std::min(FromU1, ToU2);
  double LastU  = std::max(FromU1, ToU2);

  occ::handle<Geom2d_BSplineCurve> C1 = occ::down_cast<Geom2d_BSplineCurve>(C->Copy());

  C1->Segment(FirstU, LastU);

  if (C->IsPeriodic())
  {
    if (!SameOrientation)
      C1->Reverse();
  }
  else
  {
    if (FromU1 > ToU2)
      C1->Reverse();
  }

  return C1;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> Geom2dConvert::CurveToBSplineCurve(

  const occ::handle<Geom2d_Curve>&   C,
  const Convert_ParameterisationType Parameterisation)
{

  occ::handle<BSplineCurve> TheCurve;

  if (C->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Curve>               Curv;
    occ::handle<Geom2d_TrimmedCurve> Ctrim = occ::down_cast<Geom2d_TrimmedCurve>(C);
    Curv                                   = Ctrim->BasisCurve();
    double U1                              = Ctrim->FirstParameter();
    double U2                              = Ctrim->LastParameter();

    // Si la courbe n'est pas vraiment restreinte, on ne risque pas
    // le Raise dans le BS->Segment.
    if (!Curv->IsPeriodic())
    {
      if (U1 < Curv->FirstParameter())
        U1 = Curv->FirstParameter();
      if (U2 > Curv->LastParameter())
        U2 = Curv->LastParameter();
    }

    if (Curv->IsKind(STANDARD_TYPE(Geom2d_Line)))
    {
      gp_Pnt2d      Pdeb = Ctrim->StartPoint();
      gp_Pnt2d      Pfin = Ctrim->EndPoint();
      Array1OfPnt2d Poles(1, 2);
      Poles(1) = Pdeb;
      Poles(2) = Pfin;
      Array1OfReal Knots(1, 2);
      Knots(1) = Ctrim->FirstParameter();
      Knots(2) = Ctrim->LastParameter();
      Array1OfInteger Mults(1, 2);
      Mults(1)   = 2;
      Mults(2)   = 2;
      int Degree = 1;
      TheCurve   = new Geom2d_BSplineCurve(Poles, Knots, Mults, Degree);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom2d_Circle)))
    {
      occ::handle<Geom2d_Circle> TheConic = occ::down_cast<Geom2d_Circle>(Curv);
      Circ2d                     C2d(gp::OX2d(), TheConic->Radius());
      if (Parameterisation != Convert_RationalC1)
      {
        Convert_CircleToBSplineCurve Convert(C2d, U1, U2, Parameterisation);
        TheCurve = BSplineCurveBuilder(TheConic, Convert);
      }
      else
      {
        if (U2 - U1 < 6.)
        {
          Convert_CircleToBSplineCurve Convert(C2d, U1, U2, Parameterisation);
          TheCurve = BSplineCurveBuilder(TheConic, Convert);
        }
        else
        { // split circle to avoide numerical
          // overflow when U2 - U1 =~ 2*PI

          double                       Umed = (U1 + U2) * .5;
          Convert_CircleToBSplineCurve Convert1(C2d, U1, Umed, Parameterisation);

          occ::handle<BSplineCurve> TheCurve1 = BSplineCurveBuilder(TheConic, Convert1);

          Convert_CircleToBSplineCurve Convert2(C2d, Umed, U2, Parameterisation);

          occ::handle<BSplineCurve> TheCurve2 = BSplineCurveBuilder(TheConic, Convert2);

          Geom2dConvert_CompCurveToBSplineCurve CCTBSpl(TheCurve1, Parameterisation);

          CCTBSpl.Add(TheCurve2, Precision::PConfusion(), true);

          TheCurve = CCTBSpl.BSplineCurve();
        }
      }
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom2d_Ellipse)))
    {
      occ::handle<Geom2d_Ellipse> TheConic = occ::down_cast<Geom2d_Ellipse>(Curv);

      Elips2d E2d(gp::OX2d(), TheConic->MajorRadius(), TheConic->MinorRadius());
      if (Parameterisation != Convert_RationalC1)
      {
        Convert_EllipseToBSplineCurve Convert(E2d, U1, U2, Parameterisation);
        TheCurve = BSplineCurveBuilder(TheConic, Convert);
      }
      else
      {
        if (U2 - U1 < 6.)
        {
          Convert_EllipseToBSplineCurve Convert(E2d, U1, U2, Parameterisation);
          TheCurve = BSplineCurveBuilder(TheConic, Convert);
        }
        else
        { // split ellipse to avoide numerical
          // overflow when U2 - U1 =~ 2*PI

          double                        Umed = (U1 + U2) * .5;
          Convert_EllipseToBSplineCurve Convert1(E2d, U1, Umed, Parameterisation);

          occ::handle<BSplineCurve> TheCurve1 = BSplineCurveBuilder(TheConic, Convert1);

          Convert_EllipseToBSplineCurve Convert2(E2d, Umed, U2, Parameterisation);

          occ::handle<BSplineCurve> TheCurve2 = BSplineCurveBuilder(TheConic, Convert2);

          Geom2dConvert_CompCurveToBSplineCurve CCTBSpl(TheCurve1, Parameterisation);

          CCTBSpl.Add(TheCurve2, Precision::PConfusion(), true);

          TheCurve = CCTBSpl.BSplineCurve();
        }
      }
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom2d_Hyperbola)))
    {
      occ::handle<Geom2d_Hyperbola> TheConic = occ::down_cast<Geom2d_Hyperbola>(Curv);

      Hypr2d H2d(gp::OX2d(), TheConic->MajorRadius(), TheConic->MinorRadius());
      Convert_HyperbolaToBSplineCurve Convert(H2d, U1, U2);
      TheCurve = BSplineCurveBuilder(TheConic, Convert);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom2d_Parabola)))
    {
      occ::handle<Geom2d_Parabola> TheConic = occ::down_cast<Geom2d_Parabola>(Curv);

      Parab2d                        Prb2d(gp::OX2d(), TheConic->Focal());
      Convert_ParabolaToBSplineCurve Convert(Prb2d, U1, U2);
      TheCurve = BSplineCurveBuilder(TheConic, Convert);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
    {

      occ::handle<Geom2d_BezierCurve> CBez = occ::down_cast<Geom2d_BezierCurve>(Curv->Copy());

      CBez->Segment(U1, U2);
      int             NbPoles = CBez->NbPoles();
      int             Degree  = CBez->Degree();
      Array1OfPnt2d   Poles(1, NbPoles);
      Array1OfReal    Knots(1, 2);
      Array1OfInteger Mults(1, 2);
      Knots(1) = 0.0;
      Knots(2) = 1.0;
      Mults(1) = Degree + 1;
      Mults(2) = Degree + 1;
      CBez->Poles(Poles);
      if (CBez->IsRational())
      {
        Array1OfReal Weights(1, NbPoles);
        CBez->Weights(Weights);
        TheCurve = new BSplineCurve(Poles, Weights, Knots, Mults, Degree);
      }
      else
      {
        TheCurve = new BSplineCurve(Poles, Knots, Mults, Degree);
      }
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
    {
      TheCurve = occ::down_cast<Geom2d_BSplineCurve>(Curv->Copy());
      TheCurve->Segment(U1, U2);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
    {

      double                    Tol2d       = 1.e-4;
      GeomAbs_Shape             Order       = GeomAbs_C2;
      int                       MaxSegments = 16, MaxDegree = 14;
      Geom2dConvert_ApproxCurve ApprCOffs(C, Tol2d, Order, MaxSegments, MaxDegree);
      if (ApprCOffs.HasResult())
        TheCurve = ApprCOffs.Curve();
      else
        throw Standard_ConstructionError();
    }

    else
    {
      throw Standard_DomainError("No such curve");
    }
  }

  else
  {

    if (C->IsKind(STANDARD_TYPE(Geom2d_Ellipse)))
    {
      occ::handle<Geom2d_Ellipse> TheConic = occ::down_cast<Geom2d_Ellipse>(C);

      Elips2d E2d(gp::OX2d(), TheConic->MajorRadius(), TheConic->MinorRadius());
      Convert_EllipseToBSplineCurve Convert(E2d, Parameterisation);
      TheCurve = BSplineCurveBuilder(TheConic, Convert);
      TheCurve->SetPeriodic();
    }

    else if (C->IsKind(STANDARD_TYPE(Geom2d_Circle)))
    {
      occ::handle<Geom2d_Circle> TheConic = occ::down_cast<Geom2d_Circle>(C);

      Circ2d                       C2d(gp::OX2d(), TheConic->Radius());
      Convert_CircleToBSplineCurve Convert(C2d, Parameterisation);
      TheCurve = BSplineCurveBuilder(TheConic, Convert);
      TheCurve->SetPeriodic();
    }

    else if (C->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
    {
      occ::handle<Geom2d_BezierCurve> CBez = occ::down_cast<Geom2d_BezierCurve>(C);

      int             NbPoles = CBez->NbPoles();
      int             Degree  = CBez->Degree();
      Array1OfPnt2d   Poles(1, NbPoles);
      Array1OfReal    Knots(1, 2);
      Array1OfInteger Mults(1, 2);
      Knots(1) = 0.0;
      Knots(2) = 1.0;
      Mults(1) = Degree + 1;
      Mults(2) = Degree + 1;
      CBez->Poles(Poles);
      if (CBez->IsRational())
      {
        Array1OfReal Weights(1, NbPoles);
        CBez->Weights(Weights);
        TheCurve = new BSplineCurve(Poles, Weights, Knots, Mults, Degree);
      }
      else
      {
        TheCurve = new BSplineCurve(Poles, Knots, Mults, Degree);
      }
    }
    else if (C->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
    {
      TheCurve = occ::down_cast<Geom2d_BSplineCurve>(C->Copy());
    }

    else if (C->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
    {

      double                    Tol2d       = 1.e-4;
      GeomAbs_Shape             Order       = GeomAbs_C2;
      int                       MaxSegments = 16, MaxDegree = 14;
      Geom2dConvert_ApproxCurve ApprCOffs(C, Tol2d, Order, MaxSegments, MaxDegree);
      if (ApprCOffs.HasResult())
        TheCurve = ApprCOffs.Curve();
      else
        throw Standard_ConstructionError();
    }

    else
    {
      throw Standard_DomainError();
    }
  }

  return TheCurve;
}

//=================================================================================================

class Geom2dConvert_law_evaluator : public BSplCLib_EvaluatorFunction
{

public:
  Geom2dConvert_law_evaluator(const occ::handle<Geom2d_BSplineCurve>& theAncore)
      : myAncore(theAncore)
  {
  }

  void Evaluate(const int     theDerivativeRequest,
                const double* theStartEnd,
                const double  theParameter,
                double&       theResult,
                int&          theErrorCode) const override
  {
    theErrorCode = 0;
    if (!myAncore.IsNull() && theParameter >= theStartEnd[0] && theParameter <= theStartEnd[1]
        && theDerivativeRequest == 0)
    {
      gp_Pnt2d aPoint;
      myAncore->D0(theParameter, aPoint);
      theResult = aPoint.Coord(2);
    }
    else
      theErrorCode = 1;
  }

private:
  occ::handle<Geom2d_BSplineCurve> myAncore;
};

//=======================================================================
// function : MultNumandDenom
// purpose  : Multiply two BSpline curves to make one
//=======================================================================

static occ::handle<Geom2d_BSplineCurve> MultNumandDenom(const occ::handle<Geom2d_BSplineCurve>& a,
                                                        const occ::handle<Geom2d_BSplineCurve>& BS)

{
  NCollection_Array1<double>               aKnots(1, a->NbKnots());
  NCollection_Array1<double>               BSKnots(1, BS->NbKnots());
  NCollection_Array1<double>               BSFlatKnots(1, BS->NbPoles() + BS->Degree() + 1);
  NCollection_Array1<double>               BSWeights(1, BS->NbPoles());
  NCollection_Array1<int>                  aMults(1, a->NbKnots());
  NCollection_Array1<int>                  BSMults(1, BS->NbKnots());
  NCollection_Array1<gp_Pnt2d>             aPoles(1, a->NbPoles());
  NCollection_Array1<gp_Pnt2d>             BSPoles(1, BS->NbPoles());
  occ::handle<Geom2d_BSplineCurve>         res;
  occ::handle<NCollection_HArray1<double>> resKnots;
  occ::handle<NCollection_HArray1<int>>    resMults;
  double                                   start_value, end_value;
  int                                      resNbPoles, degree, ii, jj, aStatus;

  BS->Knots(BSKnots);
  BS->Multiplicities(BSMults);
  BS->Poles(BSPoles);
  BS->Weights(BSWeights);
  BS->KnotSequence(BSFlatKnots);
  start_value      = BSKnots(1);
  end_value        = BSKnots(BS->NbKnots());
  double tolerance = 10. * Epsilon(std::abs(end_value));

  a->Knots(aKnots);
  a->Poles(aPoles);
  a->Multiplicities(aMults);
  BSplCLib::Reparametrize(BS->FirstParameter(), BS->LastParameter(), aKnots);
  occ::handle<Geom2d_BSplineCurve> anAncore =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMults, a->Degree());

  BSplCLib::MergeBSplineKnots(tolerance,
                              start_value,
                              end_value,
                              a->Degree(),
                              aKnots,
                              aMults,
                              BS->Degree(),
                              BSKnots,
                              BSMults,
                              resNbPoles,
                              resKnots,
                              resMults);
  degree = BS->Degree() + a->Degree();
  NCollection_Array1<gp_Pnt2d> resNumPoles(1, resNbPoles);
  NCollection_Array1<double>   resDenPoles(1, resNbPoles);
  NCollection_Array1<gp_Pnt2d> resPoles(1, resNbPoles);
  NCollection_Array1<double>   resFlatKnots(1, resNbPoles + degree + 1);
  BSplCLib::KnotSequence(resKnots->Array1(), resMults->Array1(), resFlatKnots);
  for (ii = 1; ii <= BS->NbPoles(); ii++)
    for (jj = 1; jj <= 2; jj++)
      BSPoles(ii).SetCoord(jj, BSPoles(ii).Coord(jj) * BSWeights(ii));
  // POP pour NT
  Geom2dConvert_law_evaluator ev(anAncore);
  BSplCLib::FunctionMultiply(ev,
                             BS->Degree(),
                             BSFlatKnots,
                             BSPoles,
                             resFlatKnots,
                             degree,
                             resNumPoles,
                             aStatus);
  BSplCLib::FunctionMultiply(ev,
                             BS->Degree(),
                             BSFlatKnots,
                             BSWeights,
                             resFlatKnots,
                             degree,
                             resDenPoles,
                             aStatus);
  for (ii = 1; ii <= resNbPoles; ii++)
    for (jj = 1; jj <= 2; jj++)
      resPoles(ii).SetCoord(jj, resNumPoles(ii).Coord(jj) / resDenPoles(ii));
  res =
    new Geom2d_BSplineCurve(resPoles, resDenPoles, resKnots->Array1(), resMults->Array1(), degree);
  return res;
}

//=======================================================================
// function : Pretreatment
// purpose  : Put the two first and two last weights at one if they are
//           equal
//=======================================================================

static void Pretreatment(NCollection_Array1<occ::handle<Geom2d_BSplineCurve>>& tab)

{
  int    i, j;
  double a;

  for (i = 0; i <= (tab.Length() - 1); i++)
  {
    if (tab(i)->IsRational())
    {
      a = tab(i)->Weight(1);
      if ((tab(i)->Weight(2) == a) && (tab(i)->Weight(tab(i)->NbPoles() - 1) == a)
          && (tab(i)->Weight(tab(i)->NbPoles()) == a))

        for (j = 1; j <= tab(i)->NbPoles(); j++)
          tab(i)->SetWeight(j, tab(i)->Weight(j) / a);
    }
  }
}

//=======================================================================
// function : NeedToBeTreated
// purpose  : Say if the BSpline is rational and if the two first and two
//           last weights are different
//=======================================================================

static bool NeedToBeTreated(const occ::handle<Geom2d_BSplineCurve>& BS)

{
  NCollection_Array1<double> tabWeights(1, BS->NbPoles());
  if (BS->IsRational())
  {
    BS->Weights(tabWeights);
    return (BSplCLib::IsRational(tabWeights, 1, BS->NbPoles()))
           && ((BS->Weight(1) < (1 - Precision::Confusion()))
               || (BS->Weight(1) > (1 + Precision::Confusion()))
               || (BS->Weight(2) < (1 - Precision::Confusion()))
               || (BS->Weight(2) > (1 + Precision::Confusion()))
               || (BS->Weight(BS->NbPoles() - 1) < (1 - Precision::Confusion()))
               || (BS->Weight(BS->NbPoles() - 1) > (1 + Precision::Confusion()))
               || (BS->Weight(BS->NbPoles()) < (1 - Precision::Confusion()))
               || (BS->Weight(BS->NbPoles()) > (1 + Precision::Confusion())));
  }
  else
    return false;
}

//=======================================================================
// function : Need2DegRepara
// purpose  : in the case of wire closed G1 it says if you will to use a
//           two degree reparametrisation to close it C1
//=======================================================================

static bool Need2DegRepara(const NCollection_Array1<occ::handle<Geom2d_BSplineCurve>>& tab)

{
  int      i;
  gp_Vec2d Vec1, Vec2;
  gp_Pnt2d Pint;
  double   Rapport = 1.0e0;

  for (i = 0; i <= tab.Length() - 2; i++)
  {
    tab(i + 1)->D1(tab(i + 1)->FirstParameter(), Pint, Vec1);
    tab(i)->D1(tab(i)->LastParameter(), Pint, Vec2);
    Rapport = Rapport * Vec2.Magnitude() / Vec1.Magnitude();
  }
  return (Rapport > (1.0e0 + Precision::Confusion()))
         || (Rapport < (1.0e0 - Precision::Confusion()));
}

//=======================================================================
// function : Indexmin
// purpose  : Give the index of the curve which has the lowest degree
//=======================================================================

static int Indexmin(const NCollection_Array1<occ::handle<Geom2d_BSplineCurve>>& tab)
{
  int i, index = 0, degree;

  degree = tab(0)->Degree();
  for (i = 0; i <= tab.Length() - 1; i++)
    if (tab(i)->Degree() <= degree)
    {
      degree = tab(i)->Degree();
      index  = i;
    }
  return index;
}

//=================================================================================================

static void ReorderArrayOfG1(NCollection_Array1<occ::handle<Geom2d_BSplineCurve>>& ArrayOfCurves,
                             NCollection_Array1<double>&                           ArrayOfToler,
                             NCollection_Array1<bool>&                             tabG1,
                             const int                                             StartIndex,
                             const double                                          ClosedTolerance)

{
  int                                                  i;
  NCollection_Array1<occ::handle<Geom2d_BSplineCurve>> ArraybisOfCurves(0,
                                                                        ArrayOfCurves.Length() - 1);
  NCollection_Array1<double> ArraybisOfToler(0, ArrayOfToler.Length() - 1);
  NCollection_Array1<bool>   tabbisG1(0, tabG1.Length() - 1);

  for (i = 0; i <= ArrayOfCurves.Length() - 1; i++)
  {
    if (i != ArrayOfCurves.Length() - 1)
    {
      ArraybisOfCurves(i) = ArrayOfCurves(i);
      ArraybisOfToler(i)  = ArrayOfToler(i);
      tabbisG1(i)         = tabG1(i);
    }
    else
      ArraybisOfCurves(i) = ArrayOfCurves(i);
  }

  for (i = 0; i <= (ArrayOfCurves.Length() - (StartIndex + 2)); i++)
  {
    ArrayOfCurves(i) = ArraybisOfCurves(i + StartIndex + 1);
    if (i != (ArrayOfCurves.Length() - (StartIndex + 2)))
    {
      ArrayOfToler(i) = ArraybisOfToler(i + StartIndex + 1);
      tabG1(i)        = tabbisG1(i + StartIndex + 1);
    }
  }

  ArrayOfToler(ArrayOfCurves.Length() - (StartIndex + 2)) = ClosedTolerance;
  tabG1(ArrayOfCurves.Length() - (StartIndex + 2))        = true;

  for (i = (ArrayOfCurves.Length() - (StartIndex + 1)); i <= (ArrayOfCurves.Length() - 1); i++)
  {
    if (i != ArrayOfCurves.Length() - 1)
    {
      ArrayOfCurves(i) = ArraybisOfCurves(i - (ArrayOfCurves.Length() - (StartIndex + 1)));
      ArrayOfToler(i)  = ArraybisOfToler(i - (ArrayOfCurves.Length() - (StartIndex + 1)));
      tabG1(i)         = tabbisG1(i - (ArrayOfCurves.Length() - (StartIndex + 1)));
    }
    else
      ArrayOfCurves(i) = ArraybisOfCurves(i - (ArrayOfCurves.Length() - (StartIndex + 1)));
  }
}

//=================================================================================================

static int GeomAbsToInteger(const GeomAbs_Shape gcont)
{
  int cont = 0;
  switch (gcont)
  {
    case GeomAbs_C0:
      cont = 0;
      break;
    case GeomAbs_G1:
      cont = 1;
      break;
    case GeomAbs_C1:
      cont = 2;
      break;
    case GeomAbs_G2:
      cont = 3;
      break;
    case GeomAbs_C2:
      cont = 4;
      break;
    case GeomAbs_C3:
      cont = 5;
      break;
    case GeomAbs_CN:
      cont = 6;
      break;
  }
  return cont;
}

//=================================================================================================

static GeomAbs_Shape Continuity(const occ::handle<Geom2d_Curve>& C1,
                                const occ::handle<Geom2d_Curve>& C2,
                                const double                     u1,
                                const double                     u2,
                                const bool                       r1,
                                const bool                       r2,
                                const double                     tl,
                                const double                     ta)
{
  GeomAbs_Shape cont = GeomAbs_C0;
  int           index1, index2;
  double        tolerance, value;
  //  bool fini = false;
  gp_Vec2d d1, d2;
  //  gp_Dir2d dir1,dir2;
  gp_Pnt2d      point1, point2;
  int           cont1, cont2;
  GeomAbs_Shape gcont1 = C1->Continuity(), gcont2 = C2->Continuity();
  cont1 = GeomAbsToInteger(gcont1);
  cont2 = GeomAbsToInteger(gcont2);

  occ::handle<Geom2d_Curve> aCurve1 = C1;
  occ::handle<Geom2d_Curve> aCurve2 = C2;
  if (C1->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Geom2d_TrimmedCurve> aTrimmed = occ::down_cast<Geom2d_TrimmedCurve>(aCurve1);
    aCurve1                                   = aTrimmed->BasisCurve();
  }
  if (C2->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Geom2d_TrimmedCurve> aTrimmed = occ::down_cast<Geom2d_TrimmedCurve>(aCurve2);
    aCurve2                                   = aTrimmed->BasisCurve();
  }
  if (aCurve1->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    occ::handle<Geom2d_BSplineCurve> BSplineCurve = occ::down_cast<Geom2d_BSplineCurve>(aCurve1);
    BSplineCurve->Resolution(tl, tolerance);
    BSplineCurve->LocateU(u1, tolerance, index1, index2);

    if (index1 > 1 && index2 < BSplineCurve->NbKnots() && index1 == index2)
    {
      cont1 = BSplineCurve->Degree() - BSplineCurve->Multiplicity(index1);
    }
    else
    {
      cont1 = 5;
    }
  }
  if (aCurve2->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    occ::handle<Geom2d_BSplineCurve> BSplineCurve = occ::down_cast<Geom2d_BSplineCurve>(aCurve2);
    BSplineCurve->Resolution(tl, tolerance);
    BSplineCurve->LocateU(u2, tolerance, index1, index2);

    if (index1 > 1 && index2 < BSplineCurve->NbKnots() && index1 == index2)
    {
      cont2 = BSplineCurve->Degree() - BSplineCurve->Multiplicity(index1);
    }
    else
    {
      cont2 = 5;
    }
  }
  aCurve1->D1(u1, point1, d1);
  aCurve2->D1(u2, point2, d2);
  if (point1.SquareDistance(point2) <= tl * tl)
  {
    if (cont1 != 0 && cont2 != 0)
    {

      if (d1.SquareMagnitude() >= tl * tl && d2.SquareMagnitude() >= tl * tl)
      {
        if (r1)
        {
          d1.SetCoord(-d1.X(), -d1.Y());
        }
        if (r2)
        {
          d2.SetCoord(-d2.X(), -d2.Y());
        }
        value = d1.Dot(d2);
        if ((d1.Magnitude() <= (d2.Magnitude() + tl)) && (d1.Magnitude() >= (d2.Magnitude() - tl))
            && (value / (d1.Magnitude() * d2.Magnitude()) >= 1.0e0 - ta * ta))
        {
          cont = GeomAbs_C1;
        }
        else
        {
          d1.Normalize();
          d2.Normalize();
          value = std::abs(d1.Dot(d2));
          if (value >= 1.0e0 - ta * ta)
          {
            cont = GeomAbs_G1;
          }
        }
      }
    }
  }
  else
    throw Standard_Failure("Courbes non jointives");
  return cont;
}

//=================================================================================================

static GeomAbs_Shape Continuity(const occ::handle<Geom2d_Curve>& C1,
                                const occ::handle<Geom2d_Curve>& C2,
                                const double                     u1,
                                const double                     u2,
                                const bool                       r1,
                                const bool                       r2)
{
  return Continuity(C1, C2, u1, u2, r1, r2, Precision::Confusion(), Precision::Angular());
}

//=================================================================================================

class Geom2dConvert_reparameterise_evaluator : public BSplCLib_EvaluatorFunction
{

public:
  Geom2dConvert_reparameterise_evaluator(const double thePolynomialCoefficient[3])
  {
    memcpy(myPolynomialCoefficient, thePolynomialCoefficient, sizeof(myPolynomialCoefficient));
  }

  void Evaluate(const int theDerivativeRequest,
                const double* /*theStartEnd*/,
                const double theParameter,
                double&      theResult,
                int&         theErrorCode) const override
  {
    theErrorCode = 0;
    PLib::EvalPolynomial(
      theParameter,
      theDerivativeRequest,
      2,
      1,
      *((double*)myPolynomialCoefficient), // function really only read values from this array
      theResult);
  }

private:
  double myPolynomialCoefficient[3];
};

//=================================================================================================

void Geom2dConvert::ConcatG1(
  NCollection_Array1<occ::handle<Geom2d_BSplineCurve>>&               ArrayOfCurves,
  const NCollection_Array1<double>&                                   ArrayOfToler,
  occ::handle<NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>>& ArrayOfConcatenated,
  bool&                                                               ClosedFlag,
  const double                                                        ClosedTolerance)

{
  int nb_curve = ArrayOfCurves.Length(), nb_vertexG1, nb_group = 0, index = 0, i, ii, j, jj,
      indexmin, nb_vertex_group0 = 0;
  double lambda, // coeff de raccord G1
    First, PreLast = 0;
  gp_Vec2d                         Vec1, Vec2; // vecteurs tangents consecutifs
  gp_Pnt2d                         Pint;
  occ::handle<Geom2d_BSplineCurve> Curve1, Curve2;
  NCollection_Array1<bool>         tabG1(0, nb_curve - 2); // tableau de continuite G1 aux raccords
  NCollection_Array1<double>       local_tolerance(0, ArrayOfToler.Length() - 1);

  for (i = 0; i < ArrayOfToler.Length(); i++)
  {
    local_tolerance(i) = ArrayOfToler(i);
  }
  for (i = 0; i < nb_curve; i++)
  {
    if (i >= 1)
    {
      First = ArrayOfCurves(i)->FirstParameter();
      if (Continuity(ArrayOfCurves(i - 1), ArrayOfCurves(i), PreLast, First, true, true)
          < GeomAbs_C0)
        // clang-format off
       throw Standard_ConstructionError("Geom2dConvert curves not C0") ;                //renvoi d'une erreur
      // clang-format on
      else
      {
        if (Continuity(ArrayOfCurves(i - 1), ArrayOfCurves(i), PreLast, First, true, true)
            >= GeomAbs_G1)
          tabG1(i - 1) = true; // True=Continuite G1
        else
          tabG1(i - 1) = false;
      }
    }
    PreLast = ArrayOfCurves(i)->LastParameter();
  }

  while (index <= nb_curve - 1)
  { // determination des caracteristiques du Wire
    nb_vertexG1 = 0;
    while (((index + nb_vertexG1) <= nb_curve - 2) && (tabG1(index + nb_vertexG1)))
      nb_vertexG1++;
    nb_group++;
    if (index == 0)
      nb_vertex_group0 = nb_vertexG1;
    index = index + 1 + nb_vertexG1;
  }

  if ((ClosedFlag) && (nb_group != 1))
  { // rearrangement du tableau
    nb_group--;
    ReorderArrayOfG1(ArrayOfCurves, local_tolerance, tabG1, nb_vertex_group0, ClosedTolerance);
  }

  ArrayOfConcatenated = new NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>(0, nb_group - 1);

  bool fusion;
  // int       k=0;
  index = 0;
  Pretreatment(ArrayOfCurves);

  double aPolynomialCoefficient[3];

  bool NeedDoubleDegRepara = Need2DegRepara(ArrayOfCurves);
  if (nb_group == 1 && ClosedFlag && NeedDoubleDegRepara)
  {
    Curve1 = ArrayOfCurves(nb_curve - 1);
    if (Curve1->Degree() > Geom2d_BSplineCurve::MaxDegree() / 2)
      ClosedFlag = false;
  }

  if ((nb_group == 1) && (ClosedFlag))
  { // traitement d'un cas particulier
    indexmin = Indexmin(ArrayOfCurves);
    if (indexmin != (ArrayOfCurves.Length() - 1))
      ReorderArrayOfG1(ArrayOfCurves, local_tolerance, tabG1, indexmin, ClosedTolerance);
    Curve2 = ArrayOfCurves(0);
    for (j = 1; j <= nb_curve - 1; j++)
    { // secondary loop inside each group
      Curve1 = ArrayOfCurves(j);
      if ((j == (nb_curve - 1)) && (NeedDoubleDegRepara))
      {
        const int aNewCurveDegree = 2 * Curve1->Degree();
        Curve2->D1(Curve2->LastParameter(), Pint, Vec1);
        Curve1->D1(Curve1->FirstParameter(), Pint, Vec2);
        lambda = Vec2.Magnitude() / Vec1.Magnitude();
        NCollection_Array1<double> KnotC1(1, Curve1->NbKnots());
        Curve1->Knots(KnotC1);
        Curve1->D1(Curve1->LastParameter(), Pint, Vec2);
        ArrayOfCurves(0)->D1(ArrayOfCurves(0)->FirstParameter(), Pint, Vec1);
        double lambda2 = Vec1.Magnitude() / Vec2.Magnitude();
        double tmax, a, b, c, umin = Curve1->FirstParameter(), umax = Curve1->LastParameter();
        tmax                      = 2 * lambda * (umax - umin) / (1 + lambda * lambda2);
        a                         = (lambda * lambda2 - 1) / (2 * lambda * tmax);
        aPolynomialCoefficient[2] = a;
        b                         = (1 / lambda);
        aPolynomialCoefficient[1] = b;
        c                         = umin;
        aPolynomialCoefficient[0] = c;
        NCollection_Array1<double> Curve1FlatKnots(1, Curve1->NbPoles() + Curve1->Degree() + 1);
        NCollection_Array1<int>    KnotC1Mults(1, Curve1->NbKnots());
        Curve1->Multiplicities(KnotC1Mults);
        BSplCLib::KnotSequence(KnotC1, KnotC1Mults, Curve1FlatKnots);
        KnotC1(1) = 0.0;
        for (ii = 2; ii <= KnotC1.Length(); ii++)
        {
          KnotC1(ii) =
            (-b + std::sqrt(b * b - 4 * a * (c - KnotC1(ii)))) / (2 * a); // ifv 17.05.00 buc60667
        }
        NCollection_Array1<gp_Pnt2d> Curve1Poles(1, Curve1->NbPoles());
        Curve1->Poles(Curve1Poles);

        for (ii = 1; ii <= Curve1->NbKnots(); ii++)
          KnotC1Mults(ii) = (Curve1->Degree() + KnotC1Mults(ii));

        NCollection_Array1<double> FlatKnots(1,
                                             Curve1FlatKnots.Length()
                                               + (Curve1->Degree() * Curve1->NbKnots()));

        BSplCLib::KnotSequence(KnotC1, KnotC1Mults, FlatKnots);
        NCollection_Array1<gp_Pnt2d> NewPoles(1, FlatKnots.Length() - (2 * Curve1->Degree() + 1));
        int                          aStatus;
        NCollection_Array1<double>   Curve1Weights(1, Curve1->NbPoles());
        Curve1->Weights(Curve1Weights);
        for (ii = 1; ii <= Curve1->NbPoles(); ii++)
          for (jj = 1; jj <= 2; jj++)
            Curve1Poles(ii).SetCoord(jj, Curve1Poles(ii).Coord(jj) * Curve1Weights(ii));
        // POP pour NT
        Geom2dConvert_reparameterise_evaluator ev(aPolynomialCoefficient);
        BSplCLib::FunctionReparameterise(ev,
                                         Curve1->Degree(),
                                         Curve1FlatKnots,
                                         Curve1Poles,
                                         FlatKnots,
                                         aNewCurveDegree,
                                         NewPoles,
                                         aStatus);
        NCollection_Array1<double> NewWeights(1, FlatKnots.Length() - (2 * Curve1->Degree() + 1));
        BSplCLib::FunctionReparameterise(ev,
                                         Curve1->Degree(),
                                         Curve1FlatKnots,
                                         Curve1Weights,
                                         FlatKnots,
                                         aNewCurveDegree,
                                         NewWeights,
                                         aStatus);
        //      BSplCLib::FunctionReparameterise(reparameterise_evaluator,
        //					Curve1->Degree(),
        //					Curve1FlatKnots,
        //					Curve1Poles,
        //					FlatKnots,
        //					2*Curve1->Degree(),
        //					NewPoles,
        //					aStatus
        //					);
        //       NCollection_Array1<double> NewWeights(1,FlatKnots.Length()-(2*Curve1->Degree()+1));
        //       BSplCLib::FunctionReparameterise(reparameterise_evaluator,
        //					Curve1->Degree(),
        //					Curve1FlatKnots,
        //					Curve1Weights,
        //					FlatKnots,
        //					2*Curve1->Degree(),
        //					NewWeights,
        //					aStatus
        //					);
        for (ii = 1; ii <= NewPoles.Length(); ii++)
          for (jj = 1; jj <= 2; jj++)
            NewPoles(ii).SetCoord(jj, NewPoles(ii).Coord(jj) / NewWeights(ii));
        Curve1 =
          new Geom2d_BSplineCurve(NewPoles, NewWeights, KnotC1, KnotC1Mults, aNewCurveDegree);
      }
      Geom2dConvert_CompCurveToBSplineCurve C(Curve2);
      fusion = C.Add(Curve1,
                     local_tolerance(j - 1)); // fusion de deux courbes adjacentes
      if (!fusion)
        throw Standard_ConstructionError("Geom2dConvert Concatenation Error");
      Curve2 = C.BSplineCurve();
    }
    Curve2->SetPeriodic(); // 1 seule courbe C1
    Curve2->RemoveKnot(Curve2->LastUKnotIndex(),
                       Curve2->Multiplicity(Curve2->LastUKnotIndex()) - 1,
                       Precision::Confusion());
    ArrayOfConcatenated->SetValue(0, Curve2);
  }

  else
    // clang-format off
   for (i=0;i<=nb_group-1;i++){                             //boucle principale sur chaque groupe de 
     nb_vertexG1=0;                                         //continuite interne G1
     
     while (((index+nb_vertexG1)<=nb_curve-2)&&(tabG1(index+nb_vertexG1)))
       nb_vertexG1++;
      
     for (j=index;j<=index+nb_vertexG1;j++){                //boucle secondaire a l'interieur de chaque groupe
       Curve1=ArrayOfCurves(j);
       
       if (index==j)                                      //initialisation en debut de groupe
	 ArrayOfConcatenated->SetValue(i,Curve1);
       else{
	 Geom2dConvert_CompCurveToBSplineCurve  C(ArrayOfConcatenated->Value(i));
	 fusion=C.Add(Curve1,ArrayOfToler(j-1));          //fusion de deux courbes adjacentes
          // clang-format on
          if (!fusion)
            throw Standard_ConstructionError("Geom2dConvert Concatenation Error");
          ArrayOfConcatenated->SetValue(i, C.BSplineCurve());
        }
      }
      index = index + 1 + nb_vertexG1;
    }
}

//=================================================================================================

void Geom2dConvert::ConcatC1(
  NCollection_Array1<occ::handle<Geom2d_BSplineCurve>>&               ArrayOfCurves,
  const NCollection_Array1<double>&                                   ArrayOfToler,
  occ::handle<NCollection_HArray1<int>>&                              ArrayOfIndices,
  occ::handle<NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>>& ArrayOfConcatenated,
  bool&                                                               ClosedFlag,
  const double                                                        ClosedTolerance)
{
  ConcatC1(ArrayOfCurves,
           ArrayOfToler,
           ArrayOfIndices,
           ArrayOfConcatenated,
           ClosedFlag,
           ClosedTolerance,
           Precision::Angular());
}

//=================================================================================================

void Geom2dConvert::ConcatC1(
  NCollection_Array1<occ::handle<Geom2d_BSplineCurve>>&               ArrayOfCurves,
  const NCollection_Array1<double>&                                   ArrayOfToler,
  occ::handle<NCollection_HArray1<int>>&                              ArrayOfIndices,
  occ::handle<NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>>& ArrayOfConcatenated,
  bool&                                                               ClosedFlag,
  const double                                                        ClosedTolerance,
  const double                                                        AngularTolerance)

{
  int nb_curve = ArrayOfCurves.Length(), nb_vertexG1, nb_group = 0, index = 0, i, ii, j, jj,
      indexmin, nb_vertex_group0 = 0;
  double lambda, // coeff de raccord G1
    First, PreLast = 0;
  gp_Vec2d                         Vec1, Vec2; // vecteurs tangents consecutifs
  gp_Pnt2d                         Pint;
  occ::handle<Geom2d_BSplineCurve> Curve1, Curve2;
  NCollection_Array1<bool>         tabG1(0, nb_curve - 2); // tableau de continuite G1 aux raccords
  NCollection_Array1<double>       local_tolerance(0, ArrayOfToler.Length() - 1);

  for (i = 0; i < ArrayOfToler.Length(); i++)
  {
    local_tolerance(i) = ArrayOfToler(i);
  }
  for (i = 0; i < nb_curve; i++)
  {
    if (i >= 1)
    {
      First = ArrayOfCurves(i)->FirstParameter();
      if (Continuity(ArrayOfCurves(i - 1),
                     ArrayOfCurves(i),
                     PreLast,
                     First,
                     true,
                     true,
                     ArrayOfToler(i - 1),
                     AngularTolerance)
          < GeomAbs_C0)
        // clang-format off
       throw Standard_ConstructionError("Geom2dConvert curves not C0") ;                //renvoi d'une erreur
      // clang-format on
      else
      {
        if (Continuity(ArrayOfCurves(i - 1),
                       ArrayOfCurves(i),
                       PreLast,
                       First,
                       true,
                       true,
                       ArrayOfToler(i - 1),
                       AngularTolerance)
            >= GeomAbs_G1)
          tabG1(i - 1) = true; // True=Continuite G1
        else
          tabG1(i - 1) = false;
      }
    }
    PreLast = ArrayOfCurves(i)->LastParameter();
  }

  while (index <= nb_curve - 1)
  { // determination des caracteristiques du Wire
    nb_vertexG1 = 0;
    while (((index + nb_vertexG1) <= nb_curve - 2) && (tabG1(index + nb_vertexG1)))
      nb_vertexG1++;
    nb_group++;
    if (index == 0)
      nb_vertex_group0 = nb_vertexG1;
    index = index + 1 + nb_vertexG1;
  }

  if ((ClosedFlag) && (nb_group != 1))
  { // rearrangement du tableau
    nb_group--;
    ReorderArrayOfG1(ArrayOfCurves, local_tolerance, tabG1, nb_vertex_group0, ClosedTolerance);
  }

  ArrayOfIndices      = new NCollection_HArray1<int>(0, nb_group);
  ArrayOfConcatenated = new NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>(0, nb_group - 1);

  bool fusion;
  int  k = 0;
  index  = 0;
  Pretreatment(ArrayOfCurves);
  double aPolynomialCoefficient[3];

  bool NeedDoubleDegRepara = Need2DegRepara(ArrayOfCurves);
  if (nb_group == 1 && ClosedFlag && NeedDoubleDegRepara)
  {
    Curve1 = ArrayOfCurves(nb_curve - 1);
    if (Curve1->Degree() > Geom2d_BSplineCurve::MaxDegree() / 2)
      ClosedFlag = false;
  }

  if ((nb_group == 1) && (ClosedFlag))
  { // traitement d'un cas particulier
    ArrayOfIndices->SetValue(0, 0);
    ArrayOfIndices->SetValue(1, 0);
    indexmin = Indexmin(ArrayOfCurves);
    if (indexmin != (ArrayOfCurves.Length() - 1))
      ReorderArrayOfG1(ArrayOfCurves, local_tolerance, tabG1, indexmin, ClosedTolerance);
    for (j = 0; j <= nb_curve - 1; j++)
    { // secondary loop inside each group
      if (NeedToBeTreated(ArrayOfCurves(j)))
      {
        Curve1 = MultNumandDenom(Hermit::Solution(ArrayOfCurves(j)), ArrayOfCurves(j));
      }
      else
        Curve1 = ArrayOfCurves(j);

      const int aNewCurveDegree = 2 * Curve1->Degree();

      if (j == 0) // initialisation en debut de groupe
        Curve2 = Curve1;
      else
      {
        if ((j == (nb_curve - 1)) && (NeedDoubleDegRepara))
        {
          Curve2->D1(Curve2->LastParameter(), Pint, Vec1);
          Curve1->D1(Curve1->FirstParameter(), Pint, Vec2);
          lambda = Vec2.Magnitude() / Vec1.Magnitude();
          NCollection_Array1<double> KnotC1(1, Curve1->NbKnots());
          Curve1->Knots(KnotC1);
          Curve1->D1(Curve1->LastParameter(), Pint, Vec2);
          ArrayOfCurves(0)->D1(ArrayOfCurves(0)->FirstParameter(), Pint, Vec1);
          double lambda2 = Vec1.Magnitude() / Vec2.Magnitude();
          double tmax, a, b, c, umin = Curve1->FirstParameter(), umax = Curve1->LastParameter();
          tmax                      = 2 * lambda * (umax - umin) / (1 + lambda * lambda2);
          a                         = (lambda * lambda2 - 1) / (2 * lambda * tmax);
          aPolynomialCoefficient[2] = a;
          b                         = (1 / lambda);
          aPolynomialCoefficient[1] = b;
          c                         = umin;
          aPolynomialCoefficient[0] = c;
          NCollection_Array1<double> Curve1FlatKnots(1, Curve1->NbPoles() + Curve1->Degree() + 1);
          NCollection_Array1<int>    KnotC1Mults(1, Curve1->NbKnots());
          Curve1->Multiplicities(KnotC1Mults);
          BSplCLib::KnotSequence(KnotC1, KnotC1Mults, Curve1FlatKnots);
          KnotC1(1) = 0.0;
          for (ii = 2; ii <= KnotC1.Length(); ii++)
          {
            KnotC1(ii) =
              (-b + std::sqrt(b * b - 4 * a * (c - KnotC1(ii)))) / (2 * a); // ifv 17.05.00 buc60667
          }
          NCollection_Array1<gp_Pnt2d> Curve1Poles(1, Curve1->NbPoles());
          Curve1->Poles(Curve1Poles);

          for (ii = 1; ii <= Curve1->NbKnots(); ii++)
            KnotC1Mults(ii) = (Curve1->Degree() + KnotC1Mults(ii));

          NCollection_Array1<double> FlatKnots(1,
                                               Curve1FlatKnots.Length()
                                                 + (Curve1->Degree() * Curve1->NbKnots()));

          BSplCLib::KnotSequence(KnotC1, KnotC1Mults, FlatKnots);
          NCollection_Array1<gp_Pnt2d> NewPoles(1, FlatKnots.Length() - (aNewCurveDegree + 1));
          int                          aStatus;
          NCollection_Array1<double>   Curve1Weights(1, Curve1->NbPoles());
          Curve1->Weights(Curve1Weights);
          for (ii = 1; ii <= Curve1->NbPoles(); ii++)
            for (jj = 1; jj <= 2; jj++)
              Curve1Poles(ii).SetCoord(jj, Curve1Poles(ii).Coord(jj) * Curve1Weights(ii));
          // POP pour NT
          Geom2dConvert_reparameterise_evaluator ev(aPolynomialCoefficient);
          BSplCLib::FunctionReparameterise(ev,
                                           Curve1->Degree(),
                                           Curve1FlatKnots,
                                           Curve1Poles,
                                           FlatKnots,
                                           aNewCurveDegree,
                                           NewPoles,
                                           aStatus);
          NCollection_Array1<double> NewWeights(1, FlatKnots.Length() - (aNewCurveDegree + 1));
          BSplCLib::FunctionReparameterise(ev,
                                           Curve1->Degree(),
                                           Curve1FlatKnots,
                                           Curve1Weights,
                                           FlatKnots,
                                           aNewCurveDegree,
                                           NewWeights,
                                           aStatus);
          for (ii = 1; ii <= NewPoles.Length(); ii++)
          {
            for (jj = 1; jj <= 2; jj++)
              NewPoles(ii).SetCoord(jj, NewPoles(ii).Coord(jj) / NewWeights(ii));
          }
          Curve1 =
            new Geom2d_BSplineCurve(NewPoles, NewWeights, KnotC1, KnotC1Mults, aNewCurveDegree);
        }
        Geom2dConvert_CompCurveToBSplineCurve C(Curve2);
        fusion = C.Add(Curve1,
                       local_tolerance(j - 1)); // fusion de deux courbes adjacentes
        if (!fusion)
          throw Standard_ConstructionError("Geom2dConvert Concatenation Error");
        Curve2 = C.BSplineCurve();
      }
    }
    Curve2->SetPeriodic(); // 1 seule courbe C1
    Curve2->RemoveKnot(Curve2->LastUKnotIndex(),
                       Curve2->Multiplicity(Curve2->LastUKnotIndex()) - 1,
                       Precision::Confusion());
    ArrayOfConcatenated->SetValue(0, Curve2);
  }

  else
    // clang-format off
   for (i=0;i<=nb_group-1;i++){                             //boucle principale sur chaque groupe de
      // clang-format on
      nb_vertexG1 = 0; // continuite interne G1

      while (((index + nb_vertexG1) <= nb_curve - 2) && (tabG1(index + nb_vertexG1)))
        nb_vertexG1++;

      if ((!ClosedFlag) || (nb_group == 1))
      { // remplissage du tableau des indices conserves
        k++;
        ArrayOfIndices->SetValue(k - 1, index);
        if (k == nb_group)
          ArrayOfIndices->SetValue(k, 0);
      }
      else
      {
        k++;
        ArrayOfIndices->SetValue(k - 1, index + nb_vertex_group0 + 1);
        if (k == nb_group)
          ArrayOfIndices->SetValue(k, nb_vertex_group0 + 1);
      }

      // clang-format off
     for (j=index;j<=index+nb_vertexG1;j++){                //boucle secondaire a l'interieur de chaque groupe
        // clang-format on
        if (NeedToBeTreated(ArrayOfCurves(j)))
          Curve1 = MultNumandDenom(Hermit::Solution(ArrayOfCurves(j)), ArrayOfCurves(j));
        else
          Curve1 = ArrayOfCurves(j);

        if (index == j) // initialisation en debut de groupe
          ArrayOfConcatenated->SetValue(i, Curve1);
        else
        {
          Geom2dConvert_CompCurveToBSplineCurve C(ArrayOfConcatenated->Value(i));
          // clang-format off
	 fusion=C.Add(Curve1,ArrayOfToler(j-1));          //fusion de deux courbes adjacentes
          // clang-format on
          if (!fusion)
            throw Standard_ConstructionError("Geom2dConvert Concatenation Error");
          ArrayOfConcatenated->SetValue(i, C.BSplineCurve());
        }
      }
      index = index + 1 + nb_vertexG1;
    }
}

//=================================================================================================

void Geom2dConvert::C0BSplineToC1BSplineCurve(occ::handle<Geom2d_BSplineCurve>& BS,
                                              const double                      tolerance)

{
  NCollection_Array1<int>    BSMults(1, BS->NbKnots());
  NCollection_Array1<double> BSKnots(1, BS->NbKnots());
  int                        i, j, nbcurveC1 = 1;
  double                     U1, U2;
  bool                       closed_flag = false;
  gp_Pnt2d                   point1, point2;
  gp_Vec2d                   V1, V2;
  bool                       fusion;

  BS->Knots(BSKnots);
  BS->Multiplicities(BSMults);
  for (i = BS->FirstUKnotIndex() + 1; i <= (BS->LastUKnotIndex() - 1); i++)
  {
    if (BSMults(i) == BS->Degree())
      nbcurveC1++;
  }

  nbcurveC1 = std::min(nbcurveC1, BS->NbKnots() - 1);

  if (nbcurveC1 > 1)
  {
    NCollection_Array1<occ::handle<Geom2d_BSplineCurve>> ArrayOfCurves(0, nbcurveC1 - 1);
    NCollection_Array1<double>                           ArrayOfToler(0, nbcurveC1 - 2);

    for (i = 0; i <= nbcurveC1 - 2; i++)
      ArrayOfToler(i) = tolerance;
    U2 = BS->FirstParameter();
    j  = BS->FirstUKnotIndex() + 1;
    for (i = 0; i < nbcurveC1; i++)
    {
      U1 = U2;

      while (j < BS->LastUKnotIndex() && BSMults(j) < BS->Degree())
        j++;

      U2 = BSKnots(j);
      j++;
      occ::handle<Geom2d_BSplineCurve> BSbis = occ::down_cast<Geom2d_BSplineCurve>(BS->Copy());
      BSbis->Segment(U1, U2);
      ArrayOfCurves(i) = BSbis;
    }

    const double                                                       anAngularToler = 1.0e-7;
    occ::handle<NCollection_HArray1<int>>                              ArrayOfIndices;
    occ::handle<NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>> ArrayOfConcatenated;

    BS->D1(BS->FirstParameter(), point1, V1); // a verifier
    BS->D1(BS->LastParameter(), point2, V2);

    if ((point1.SquareDistance(point2) < tolerance * tolerance)
        && (V1.IsParallel(V2, anAngularToler)))
    {
      closed_flag = true;
    }

    Geom2dConvert::ConcatC1(ArrayOfCurves,
                            ArrayOfToler,
                            ArrayOfIndices,
                            ArrayOfConcatenated,
                            closed_flag,
                            tolerance);

    Geom2dConvert_CompCurveToBSplineCurve C(ArrayOfConcatenated->Value(0));
    if (ArrayOfConcatenated->Length() >= 2)
    {
      for (i = 1; i < ArrayOfConcatenated->Length(); i++)
      {
        fusion = C.Add(ArrayOfConcatenated->Value(i), tolerance, true);
        if (!fusion)
          throw Standard_ConstructionError("Geom2dConvert Concatenation Error");
      }
    }
    BS = C.BSplineCurve();
  }
}

//=================================================================================================

void Geom2dConvert::C0BSplineToArrayOfC1BSplineCurve(
  const occ::handle<Geom2d_BSplineCurve>&                             BS,
  occ::handle<NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>>& tabBS,
  const double                                                        tolerance)
{
  C0BSplineToArrayOfC1BSplineCurve(BS, tabBS, tolerance, Precision::Angular());
}

//=================================================================================================

void Geom2dConvert::C0BSplineToArrayOfC1BSplineCurve(
  const occ::handle<Geom2d_BSplineCurve>&                             BS,
  occ::handle<NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>>& tabBS,
  const double                                                        AngularTolerance,
  const double                                                        Tolerance)

{
  NCollection_Array1<int>    BSMults(1, BS->NbKnots());
  NCollection_Array1<double> BSKnots(1, BS->NbKnots());
  int                        i, j, nbcurveC1 = 1;
  double                     U1, U2;
  bool                       closed_flag = false;
  gp_Pnt2d                   point1, point2;
  gp_Vec2d                   V1, V2;
  //  bool                 fusion;

  BS->Knots(BSKnots);
  BS->Multiplicities(BSMults);
  for (i = BS->FirstUKnotIndex(); i <= (BS->LastUKnotIndex() - 1); i++)
  {
    if (BSMults(i) == BS->Degree())
      nbcurveC1++;
  }

  nbcurveC1 = std::min(nbcurveC1, BS->NbKnots() - 1);

  if (nbcurveC1 > 1)
  {
    NCollection_Array1<occ::handle<Geom2d_BSplineCurve>> ArrayOfCurves(0, nbcurveC1 - 1);
    NCollection_Array1<double>                           ArrayOfToler(0, nbcurveC1 - 2);

    for (i = 0; i <= nbcurveC1 - 2; i++)
      ArrayOfToler(i) = Tolerance;
    U2 = BS->FirstParameter();
    j  = BS->FirstUKnotIndex() + 1;
    for (i = 0; i < nbcurveC1; i++)
    {
      U1 = U2;
      while (j < BS->LastUKnotIndex() && BSMults(j) < BS->Degree())
        j++;
      U2 = BSKnots(j);
      j++;
      occ::handle<Geom2d_BSplineCurve> BSbis = occ::down_cast<Geom2d_BSplineCurve>(BS->Copy());
      BSbis->Segment(U1, U2);
      ArrayOfCurves(i) = BSbis;
    }

    occ::handle<NCollection_HArray1<int>> ArrayOfIndices;

    BS->D1(BS->FirstParameter(), point1, V1);
    BS->D1(BS->LastParameter(), point2, V2);

    if (((point1.SquareDistance(point2) < Tolerance)) && (V1.IsParallel(V2, AngularTolerance)))
    {
      closed_flag = true;
    }

    Geom2dConvert::ConcatC1(ArrayOfCurves,
                            ArrayOfToler,
                            ArrayOfIndices,
                            tabBS,
                            closed_flag,
                            Tolerance,
                            AngularTolerance);
  }
  else
  {
    tabBS = new NCollection_HArray1<occ::handle<Geom2d_BSplineCurve>>(0, 0);
    tabBS->SetValue(0, BS);
  }
}
