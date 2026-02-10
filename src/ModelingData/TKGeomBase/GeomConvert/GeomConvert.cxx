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

#include <GeomConvert.hxx>

#include <Convert_CircleToBSplineCurve.hxx>
#include <Convert_ConicToBSplineCurve.hxx>
#include <Convert_EllipseToBSplineCurve.hxx>
#include <Convert_HyperbolaToBSplineCurve.hxx>
#include <Convert_ParabolaToBSplineCurve.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLProp.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Hermit.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

static occ::handle<Geom_BSplineCurve> BSplineCurveBuilder(
  const occ::handle<Geom_Conic>&     TheConic,
  const Convert_ConicToBSplineCurve& Convert)

{
  occ::handle<Geom_BSplineCurve>      TheCurve;
  const NCollection_Array1<gp_Pnt2d>& aPoles2d = Convert.Poles();
  const NCollection_Array1<double>&   aWeights = Convert.Weights();
  const NCollection_Array1<double>&   aKnots   = Convert.Knots();
  const NCollection_Array1<int>&      aMults   = Convert.Multiplicities();
  NCollection_Array1<gp_Pnt>          Poles(1, aPoles2d.Length());
  for (int i = aPoles2d.Lower(); i <= aPoles2d.Upper(); i++)
  {
    const gp_Pnt2d& aP2d = aPoles2d(i);
    Poles(i).SetCoord(aP2d.X(), aP2d.Y(), 0.0);
  }
  TheCurve =
    new Geom_BSplineCurve(Poles, aWeights, aKnots, aMults, Convert.Degree(), Convert.IsPeriodic());
  gp_Trsf T;
  T.SetTransformation(TheConic->Position(), gp::XOY());
  occ::handle<Geom_BSplineCurve> Cres;
  Cres = occ::down_cast<Geom_BSplineCurve>(TheCurve->Transformed(T));
  return Cres;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> GeomConvert::SplitBSplineCurve(
  const occ::handle<Geom_BSplineCurve>& C,
  const int                             FromK1,
  const int                             ToK2,
  const bool                            SameOrientation)
{
  int TheFirst = C->FirstUKnotIndex();
  int TheLast  = C->LastUKnotIndex();
  if (FromK1 == ToK2)
    throw Standard_DomainError();
  int FirstK = std::min(FromK1, ToK2);
  int LastK  = std::max(FromK1, ToK2);
  if (FirstK < TheFirst || LastK > TheLast)
    throw Standard_DomainError();

  occ::handle<Geom_BSplineCurve> C1 = occ::down_cast<Geom_BSplineCurve>(C->Copy());

  C1->Segment(C->Knot(FirstK), C->Knot(LastK));

  if (C->IsPeriodic())
  {
    if (!SameOrientation)
      C1->Reverse();
  }
  else
  {
    if (FromK1 > ToK2)
      C1->Reverse();
  }
  return C1;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> GeomConvert::SplitBSplineCurve(
  const occ::handle<Geom_BSplineCurve>& C,
  const double                          FromU1,
  const double                          ToU2,
  const double, // ParametricTolerance,
  const bool SameOrientation)
{
  double FirstU = std::min(FromU1, ToU2);
  double LastU  = std::max(FromU1, ToU2);

  occ::handle<Geom_BSplineCurve> C1 = occ::down_cast<Geom_BSplineCurve>(C->Copy());

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

occ::handle<Geom_BSplineCurve> GeomConvert::CurveToBSplineCurve(
  const occ::handle<Geom_Curve>&     C,
  const Convert_ParameterisationType Parameterisation)
{
  occ::handle<Geom_BSplineCurve> TheCurve;

  if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_Curve>        Curv;
    occ::handle<Geom_TrimmedCurve> Ctrim = occ::down_cast<Geom_TrimmedCurve>(C);
    Curv                                 = Ctrim->BasisCurve();
    double U1                            = Ctrim->FirstParameter();
    double U2                            = Ctrim->LastParameter();

    // Si la courbe n'est pas vraiment restreinte, on ne risque pas
    // le Raise dans le BS->Segment.
    if (!Curv->IsPeriodic())
    {
      if (U1 < Curv->FirstParameter())
        U1 = Curv->FirstParameter();
      if (U2 > Curv->LastParameter())
        U2 = Curv->LastParameter();
    }

    if (Curv->IsKind(STANDARD_TYPE(Geom_Line)))
    {
      gp_Pnt                     Pdeb = Ctrim->StartPoint();
      gp_Pnt                     Pfin = Ctrim->EndPoint();
      NCollection_Array1<gp_Pnt> Poles(1, 2);
      Poles(1) = Pdeb;
      Poles(2) = Pfin;
      NCollection_Array1<double> Knots(1, 2);
      Knots(1) = Ctrim->FirstParameter();
      Knots(2) = Ctrim->LastParameter();
      NCollection_Array1<int> Mults(1, 2);
      Mults(1)   = 2;
      Mults(2)   = 2;
      int Degree = 1;
      TheCurve   = new Geom_BSplineCurve(Poles, Knots, Mults, Degree);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      occ::handle<Geom_Circle> TheConic = occ::down_cast<Geom_Circle>(Curv);
      gp_Circ2d                C2d(gp::OX2d(), TheConic->Radius());
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

          occ::handle<Geom_BSplineCurve> TheCurve1 = BSplineCurveBuilder(TheConic, Convert1);

          Convert_CircleToBSplineCurve Convert2(C2d, Umed, U2, Parameterisation);

          occ::handle<Geom_BSplineCurve> TheCurve2 = BSplineCurveBuilder(TheConic, Convert2);

          GeomConvert_CompCurveToBSplineCurve CCTBSpl(TheCurve1, Parameterisation);

          CCTBSpl.Add(TheCurve2, Precision::PConfusion(), true);

          TheCurve = CCTBSpl.BSplineCurve();
        }
      }
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_Ellipse)))
    {
      occ::handle<Geom_Ellipse> TheConic = occ::down_cast<Geom_Ellipse>(Curv);
      gp_Elips2d                E2d(gp::OX2d(), TheConic->MajorRadius(), TheConic->MinorRadius());
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

          occ::handle<Geom_BSplineCurve> TheCurve1 = BSplineCurveBuilder(TheConic, Convert1);

          Convert_EllipseToBSplineCurve Convert2(E2d, Umed, U2, Parameterisation);

          occ::handle<Geom_BSplineCurve> TheCurve2 = BSplineCurveBuilder(TheConic, Convert2);

          GeomConvert_CompCurveToBSplineCurve CCTBSpl(TheCurve1, Parameterisation);

          CCTBSpl.Add(TheCurve2, Precision::PConfusion(), true);

          TheCurve = CCTBSpl.BSplineCurve();
        }
      }
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_Hyperbola)))
    {
      occ::handle<Geom_Hyperbola> TheConic = occ::down_cast<Geom_Hyperbola>(Curv);
      gp_Hypr2d                   H2d(gp::OX2d(), TheConic->MajorRadius(), TheConic->MinorRadius());
      Convert_HyperbolaToBSplineCurve Convert(H2d, U1, U2);
      TheCurve = BSplineCurveBuilder(TheConic, Convert);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_Parabola)))
    {
      occ::handle<Geom_Parabola>     TheConic = occ::down_cast<Geom_Parabola>(Curv);
      gp_Parab2d                     Prb2d(gp::OX2d(), TheConic->Focal());
      Convert_ParabolaToBSplineCurve Convert(Prb2d, U1, U2);
      TheCurve = BSplineCurveBuilder(TheConic, Convert);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
    {

      occ::handle<Geom_BezierCurve> CBez = occ::down_cast<Geom_BezierCurve>(Curv->Copy());
      CBez->Segment(U1, U2);
      int                               Degree = CBez->Degree();
      const NCollection_Array1<gp_Pnt>& Poles  = CBez->Poles();
      NCollection_Array1<double>        Knots(1, 2);
      NCollection_Array1<int>           Mults(1, 2);
      Knots(1)                                      = 0.0;
      Knots(2)                                      = 1.0;
      Mults(1)                                      = Degree + 1;
      Mults(2)                                      = Degree + 1;
      if (CBez->IsRational())
      {
        TheCurve = new Geom_BSplineCurve(Poles, CBez->WeightsArray(), Knots, Mults, Degree);
      }
      else
      {
        TheCurve = new Geom_BSplineCurve(Poles, Knots, Mults, Degree);
      }
    }
    else if (Curv->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
    {
      TheCurve = occ::down_cast<Geom_BSplineCurve>(Curv->Copy());
      //// modified by jgv, 14.01.05 for OCC7355 ////
      if (TheCurve->IsPeriodic())
      {
        double Uf = TheCurve->FirstParameter();
        double Ul = TheCurve->LastParameter();
        ElCLib::AdjustPeriodic(Uf, Ul, Precision::Confusion(), U1, U2);
        if (std::abs(U1 - Uf) <= Precision::Confusion()
            && std::abs(U2 - Ul) <= Precision::Confusion())
          TheCurve->SetNotPeriodic();
      }
      ///////////////////////////////////////////////
      TheCurve->Segment(U1, U2);
    }
    else if (Curv->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
    {
      double                  Tol3d       = 1.e-4;
      GeomAbs_Shape           Order       = GeomAbs_C2;
      int                     MaxSegments = 16, MaxDegree = 14;
      GeomConvert_ApproxCurve ApprCOffs(C, Tol3d, Order, MaxSegments, MaxDegree);
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
    if (C->IsKind(STANDARD_TYPE(Geom_Ellipse)))
    {
      occ::handle<Geom_Ellipse> TheConic = occ::down_cast<Geom_Ellipse>(C);
      gp_Elips2d                E2d(gp::OX2d(), TheConic->MajorRadius(), TheConic->MinorRadius());
      /*      if (Parameterisation == Convert_TgtThetaOver2_1 ||
            Parameterisation == Convert_TgtThetaOver2_2) {
          throw Standard_DomainError(); }

            else if ( Parameterisation == Convert_QuasiAngular) {
          Convert_EllipseToBSplineCurve Convert (E2d,
                                 0.0e0,
                                 2.0e0 * M_PI,
                                 Parameterisation);

          TheCurve = BSplineCurveBuilder (TheConic, Convert);
          TheCurve->SetPeriodic();
            }
            else {*/
      Convert_EllipseToBSplineCurve Convert(E2d, Parameterisation);
      TheCurve = BSplineCurveBuilder(TheConic, Convert);
      TheCurve->SetPeriodic(); // for polynomial and quasi angular
                               //      }
    }

    else if (C->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      occ::handle<Geom_Circle> TheConic = occ::down_cast<Geom_Circle>(C);
      gp_Circ2d                C2d(gp::OX2d(), TheConic->Radius());
      /*      if (Parameterisation == Convert_TgtThetaOver2_1 ||
            Parameterisation == Convert_TgtThetaOver2_2) {
          throw Standard_DomainError(); }

            else if ( Parameterisation == Convert_QuasiAngular) {
          Convert_CircleToBSplineCurve Convert (C2d,
                                0.0e0,
                                2.0e0 * M_PI,
                                Parameterisation);

          TheCurve = BSplineCurveBuilder (TheConic, Convert);
            }
            else {*/
      Convert_CircleToBSplineCurve Convert(C2d, Parameterisation);
      TheCurve = BSplineCurveBuilder(TheConic, Convert);
      TheCurve->SetPeriodic();
      //      }
    }

    else if (C->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
    {
      occ::handle<Geom_BezierCurve>     CBez   = occ::down_cast<Geom_BezierCurve>(C);
      int                               Degree = CBez->Degree();
      const NCollection_Array1<gp_Pnt>& Poles  = CBez->Poles();
      NCollection_Array1<double>        Knots(1, 2);
      NCollection_Array1<int>           Mults(1, 2);
      Knots(1)                                      = 0.0;
      Knots(2)                                      = 1.0;
      Mults(1)                                      = Degree + 1;
      Mults(2)                                      = Degree + 1;
      if (CBez->IsRational())
      {
        TheCurve = new Geom_BSplineCurve(Poles, CBez->WeightsArray(), Knots, Mults, Degree);
      }
      else
      {
        TheCurve = new Geom_BSplineCurve(Poles, Knots, Mults, Degree);
      }
    }

    else if (C->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
    {
      TheCurve = occ::down_cast<Geom_BSplineCurve>(C->Copy());
    }

    else if (C->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
    {
      double                  Tol3d       = 1.e-4;
      GeomAbs_Shape           Order       = GeomAbs_C2;
      int                     MaxSegments = 16, MaxDegree = 14;
      GeomConvert_ApproxCurve ApprCOffs(C, Tol3d, Order, MaxSegments, MaxDegree);
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

  return TheCurve;
}

//=======================================================================
// class : law_evaluator
// purpose  : useful to estimate the value of a function
//=======================================================================

class GeomConvert_law_evaluator : public BSplCLib_EvaluatorFunction
{

public:
  GeomConvert_law_evaluator(const occ::handle<Geom2d_BSplineCurve>& theAncore)
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

static occ::handle<Geom_BSplineCurve> MultNumandDenom(const occ::handle<Geom2d_BSplineCurve>& a,
                                                      const occ::handle<Geom_BSplineCurve>&   BS)

{
  const NCollection_Array1<double>&        BSKnots = BS->Knots();
  const NCollection_Array1<int>&           BSMults = BS->Multiplicities();
  NCollection_Array1<gp_Pnt>               BSPoles(BS->Poles());
  const NCollection_Array1<double>&        BSWeights   = BS->WeightsArray();
  const NCollection_Array1<double>&        BSFlatKnots = BS->KnotSequence();
  occ::handle<Geom_BSplineCurve>           res;
  occ::handle<NCollection_HArray1<double>> resKnots;
  occ::handle<NCollection_HArray1<int>>    resMults;
  double                                   start_value, end_value;
  double                                   tolerance = Precision::PConfusion();
  int                                      resNbPoles, degree, ii, jj, aStatus;

  start_value = BSKnots(1);
  end_value   = BSKnots(BS->NbKnots());
  if ((end_value - start_value) / 5 < tolerance)
    tolerance = (end_value - start_value) / 5;

  NCollection_Array1<double>          aKnots(a->Knots());
  const NCollection_Array1<gp_Pnt2d>& aPoles = a->Poles();
  const NCollection_Array1<int>&      aMults = a->Multiplicities();
  BSplCLib::Reparametrize(BS->FirstParameter(), BS->LastParameter(), aKnots);
  occ::handle<Geom2d_BSplineCurve> anAncore =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMults, a->Degree());

  BSplCLib::MergeBSplineKnots(tolerance,
                              start_value,
                              end_value, // merge of the knots
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
  NCollection_Array1<gp_Pnt> resNumPoles(1, resNbPoles);
  NCollection_Array1<double> resDenPoles(1, resNbPoles);
  NCollection_Array1<gp_Pnt> resPoles(1, resNbPoles);
  NCollection_Array1<double> resFlatKnots(1, resNbPoles + degree + 1);
  BSplCLib::KnotSequence(resKnots->Array1(), resMults->Array1(), resFlatKnots);
  for (ii = 1; ii <= BS->NbPoles(); ii++)
    for (jj = 1; jj <= 3; jj++)
      BSPoles(ii).SetCoord(jj, BSPoles(ii).Coord(jj) * BSWeights(ii));
  // POP pour WNT
  GeomConvert_law_evaluator ev(anAncore);

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
    for (jj = 1; jj <= 3; jj++)
      resPoles(ii).SetCoord(jj, resNumPoles(ii).Coord(jj) / resDenPoles(ii));
  res =
    new Geom_BSplineCurve(resPoles, resDenPoles, resKnots->Array1(), resMults->Array1(), degree);
  return res;
}

//=======================================================================
// function : Pretreatment
// purpose  : Put the two first and two last weights at one if they are
//           equal
//=======================================================================

static void Pretreatment(NCollection_Array1<occ::handle<Geom_BSplineCurve>>& tab)

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

static bool NeedToBeTreated(const occ::handle<Geom_BSplineCurve>& BS)

{
  if (BS->IsRational())
  {
    const NCollection_Array1<double>& tabWeights = BS->WeightsArray();
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

static bool Need2DegRepara(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& tab)

{
  int    i;
  gp_Vec Vec1, Vec2;
  gp_Pnt Pint;
  double Rapport = 1.0e0;

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

static int Indexmin(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& tab)
{
  int i = 0, index = 0, degree = 0;

  degree = tab(0)->Degree();
  for (i = 0; i <= tab.Length() - 1; i++)
    if (tab(i)->Degree() <= degree)
    {
      degree = tab(i)->Degree();
      index  = i;
    }
  return index;
}

//=======================================================================
// function : NewTabClosedG1
// purpose  : Sort the array of BSplines to start at the nb_vertex_group0 index
//=======================================================================

static void ReorderArrayOfG1Curves(
  NCollection_Array1<occ::handle<Geom_BSplineCurve>>& ArrayOfCurves,
  NCollection_Array1<double>&                         ArrayOfToler,
  NCollection_Array1<bool>&                           tabG1,
  const int                                           StartIndex,
  const double                                        ClosedTolerance)

{
  int                                                i;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> ArraybisOfCurves(0,
                                                                      ArrayOfCurves.Length()
                                                                        - 1); // temporary
  NCollection_Array1<double> ArraybisOfToler(0, ArrayOfToler.Length() - 1);   // arrays
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

class GeomConvert_reparameterise_evaluator : public BSplCLib_EvaluatorFunction
{

public:
  GeomConvert_reparameterise_evaluator(const double thePolynomialCoefficient[3])
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

void GeomConvert::ConcatG1(
  NCollection_Array1<occ::handle<Geom_BSplineCurve>>&               ArrayOfCurves,
  const NCollection_Array1<double>&                                 ArrayOfToler,
  occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& ArrayOfConcatenated,
  bool&                                                             ClosedG1Flag,
  const double                                                      ClosedTolerance)

{
  int nb_curve = ArrayOfCurves.Length(), nb_vertexG1 = 0, nb_group = 0, index = 0, i, ii, j, jj,
      indexmin, nb_vertex_group0 = 0;
  double lambda, // G1 coefficient
    First;
  double                         PreLast = 0.;
  GeomAbs_Shape                  Cont;
  gp_Vec                         Vec1, Vec2; // consecutive tangential vectors
  gp_Pnt                         Pint;
  occ::handle<Geom_BSplineCurve> Curve1, Curve2;
  // clang-format off
 NCollection_Array1<bool>      tabG1(0,nb_curve-2);         //array of the G1 continuity at the intersections
  // clang-format on
  NCollection_Array1<double> local_tolerance(0, ArrayOfToler.Length() - 1);

  for (i = 0; i < ArrayOfToler.Length(); i++)
  {
    local_tolerance(i) = ArrayOfToler(i);
  }
  for (i = 0; i < nb_curve; i++)
  {
    if (i >= 1)
    {
      First = ArrayOfCurves(i)->FirstParameter();
      Cont =
        GeomLProp::Continuity(ArrayOfCurves(i - 1), ArrayOfCurves(i), PreLast, First, true, true);
      if (Cont < GeomAbs_C0)
        throw Standard_ConstructionError("GeomConvert curves not C0");
      else
      {
        if (Cont >= GeomAbs_G1)
          tabG1(i - 1) = true; // True=G1 continuity
        else
          tabG1(i - 1) = false;
      }
    }
    PreLast = ArrayOfCurves(i)->LastParameter();
  }

  while (index <= nb_curve - 1)
  { // determination of the Wire features
    nb_vertexG1 = 0;
    while (((index + nb_vertexG1) <= nb_curve - 2) && (tabG1(index + nb_vertexG1)))
      nb_vertexG1++;
    nb_group++;
    if (index == 0)
      nb_vertex_group0 = nb_vertexG1;
    index = index + 1 + nb_vertexG1;
  }

  if ((ClosedG1Flag) && (nb_group != 1))
  { // sort of the array
    nb_group--;
    ReorderArrayOfG1Curves(ArrayOfCurves,
                           local_tolerance,
                           tabG1,
                           nb_vertex_group0,
                           ClosedTolerance);
  }

  ArrayOfConcatenated = new NCollection_HArray1<occ::handle<Geom_BSplineCurve>>(0, nb_group - 1);
  bool fusion;

  index = 0;
  Pretreatment(ArrayOfCurves);
  double aPolynomialCoefficient[3];

  bool NeedDoubleDegRepara = Need2DegRepara(ArrayOfCurves);
  if (nb_group == 1 && ClosedG1Flag && NeedDoubleDegRepara)
  {
    Curve1 = ArrayOfCurves(nb_curve - 1);
    if (Curve1->Degree() > Geom2d_BSplineCurve::MaxDegree() / 2)
      ClosedG1Flag = false;
  }

  if ((nb_group == 1) && (ClosedG1Flag))
  { // treatment of a particular case
    indexmin = Indexmin(ArrayOfCurves);
    if (indexmin != (ArrayOfCurves.Length() - 1))
      ReorderArrayOfG1Curves(ArrayOfCurves, local_tolerance, tabG1, indexmin, ClosedTolerance);
    Curve2 = ArrayOfCurves(0);
    for (j = 1; j <= nb_curve - 1; j++)
    { // secondary loop inside each group
      Curve1 = ArrayOfCurves(j);
      if ((j == (nb_curve - 1)) && (NeedDoubleDegRepara))
      {
        Curve2->D1(Curve2->LastParameter(), Pint, Vec1);
        Curve1->D1(Curve1->FirstParameter(), Pint, Vec2);
        lambda = Vec2.Magnitude() / Vec1.Magnitude();
        NCollection_Array1<double> KnotC1(Curve1->Knots());
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
        NCollection_Array1<int>    KnotC1Mults(Curve1->Multiplicities());
        BSplCLib::KnotSequence(KnotC1, KnotC1Mults, Curve1FlatKnots);
        KnotC1(1) = 0.0;
        for (ii = 2; ii <= KnotC1.Length(); ii++)
        {
          KnotC1(ii) =
            (-b + std::sqrt(b * b - 4 * a * (c - KnotC1(ii)))) / (2 * a); // ifv 17.05.00 buc60667
        }
        NCollection_Array1<gp_Pnt> Curve1Poles(Curve1->Poles());

        for (ii = 1; ii <= Curve1->NbKnots(); ii++)
          KnotC1Mults(ii) = (Curve1->Degree() + KnotC1Mults(ii));

        NCollection_Array1<double> FlatKnots(1,
                                             Curve1FlatKnots.Length()
                                               + (Curve1->Degree() * Curve1->NbKnots()));

        BSplCLib::KnotSequence(KnotC1, KnotC1Mults, FlatKnots);
        NCollection_Array1<gp_Pnt> NewPoles(1, FlatKnots.Length() - (2 * Curve1->Degree() + 1));
        int                        aStatus;
        const NCollection_Array1<double>& Curve1Weights = Curve1->WeightsArray();
        for (ii = 1; ii <= Curve1->NbPoles(); ii++)
          for (jj = 1; jj <= 3; jj++)
            Curve1Poles(ii).SetCoord(jj, Curve1Poles(ii).Coord(jj) * Curve1Weights(ii));
        // POP pour WNT
        GeomConvert_reparameterise_evaluator ev(aPolynomialCoefficient);
        //       BSplCLib::FunctionReparameterise(reparameterise_evaluator,
        BSplCLib::FunctionReparameterise(ev,
                                         Curve1->Degree(),
                                         Curve1FlatKnots,
                                         Curve1Poles,
                                         FlatKnots,
                                         2 * Curve1->Degree(),
                                         NewPoles,
                                         aStatus);
        NCollection_Array1<double> NewWeights(1, FlatKnots.Length() - (2 * Curve1->Degree() + 1));
        //       BSplCLib::FunctionReparameterise(reparameterise_evaluator,
        BSplCLib::FunctionReparameterise(ev,
                                         Curve1->Degree(),
                                         Curve1FlatKnots,
                                         Curve1Weights,
                                         FlatKnots,
                                         2 * Curve1->Degree(),
                                         NewWeights,
                                         aStatus);
        for (ii = 1; ii <= NewPoles.Length(); ii++)
          for (jj = 1; jj <= 3; jj++)
            NewPoles(ii).SetCoord(jj, NewPoles(ii).Coord(jj) / NewWeights(ii));
        Curve1 =
          new Geom_BSplineCurve(NewPoles, NewWeights, KnotC1, KnotC1Mults, 2 * Curve1->Degree());
      }
      GeomConvert_CompCurveToBSplineCurve C(Curve2);
      fusion = C.Add(Curve1,
                     local_tolerance(j - 1)); // merge of two consecutive curves
      if (!fusion)
        throw Standard_ConstructionError("GeomConvert Concatenation Error");
      Curve2 = C.BSplineCurve();
    }
    Curve2->SetPeriodic();
    Curve2->RemoveKnot(Curve2->LastUKnotIndex(),
                       Curve2->Multiplicity(Curve2->LastUKnotIndex()) - 1,
                       Precision::Confusion());
    ArrayOfConcatenated->SetValue(0, Curve2);
  }

  else
    // clang-format off
   for (i=0;i<=nb_group-1;i++){                             //principal loop on each G1 continuity
      // clang-format on
      nb_vertexG1 = 0; // group

      while (((index + nb_vertexG1) <= nb_curve - 2) && (tabG1(index + nb_vertexG1)))
        nb_vertexG1++;

      for (j = index; j <= index + nb_vertexG1; j++)
      { // secondary loop inside each group
        Curve1 = ArrayOfCurves(j);

        if (index == j) // initialisation at the beginning of the loop
          ArrayOfConcatenated->SetValue(i, Curve1);
        else
        {
          GeomConvert_CompCurveToBSplineCurve C(ArrayOfConcatenated->Value(i));
          // clang-format off
	 fusion=C.Add(Curve1,ArrayOfToler(j-1));            //merge of two consecutive curves
          // clang-format on
          if (!fusion)
            throw Standard_ConstructionError("GeomConvert Concatenation Error");
          ArrayOfConcatenated->SetValue(i, C.BSplineCurve());
        }
      }
      index = index + 1 + nb_vertexG1;
    }
}

//=================================================================================================

void GeomConvert::ConcatC1(
  NCollection_Array1<occ::handle<Geom_BSplineCurve>>&               ArrayOfCurves,
  const NCollection_Array1<double>&                                 ArrayOfToler,
  occ::handle<NCollection_HArray1<int>>&                            ArrayOfIndices,
  occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& ArrayOfConcatenated,
  bool&                                                             ClosedG1Flag,
  const double                                                      ClosedTolerance)
{
  ConcatC1(ArrayOfCurves,
           ArrayOfToler,
           ArrayOfIndices,
           ArrayOfConcatenated,
           ClosedG1Flag,
           ClosedTolerance,
           Precision::Angular());
}

//=================================================================================================

void GeomConvert::ConcatC1(
  NCollection_Array1<occ::handle<Geom_BSplineCurve>>&               ArrayOfCurves,
  const NCollection_Array1<double>&                                 ArrayOfToler,
  occ::handle<NCollection_HArray1<int>>&                            ArrayOfIndices,
  occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& ArrayOfConcatenated,
  bool&                                                             ClosedG1Flag,
  const double                                                      ClosedTolerance,
  const double                                                      AngularTolerance)

{
  int nb_curve = ArrayOfCurves.Length(), nb_vertexG1, nb_group = 0, index = 0, i, ii, j, jj,
      indexmin, nb_vertex_group0 = 0;
  double lambda, // G1 coefficient
    First;
  double PreLast = 0.;

  GeomAbs_Shape                  Cont;
  gp_Vec                         Vec1, Vec2; // consecutive tangential vectors
  gp_Pnt                         Pint;
  occ::handle<Geom_BSplineCurve> Curve1, Curve2;
  // clang-format off
 NCollection_Array1<bool>      tabG1(0,nb_curve-2);         //array of the G1 continuity at the intersections
  // clang-format on
  NCollection_Array1<double> local_tolerance(0, ArrayOfToler.Length() - 1);

  for (i = 0; i < ArrayOfToler.Length(); i++)
  {
    local_tolerance(i) = ArrayOfToler(i);
  }
  for (i = 0; i < nb_curve; i++)
  {
    if (i >= 1)
    {
      First = ArrayOfCurves(i)->FirstParameter();
      Cont  = GeomLProp::Continuity(ArrayOfCurves(i - 1),
                                   ArrayOfCurves(i),
                                   PreLast,
                                   First,
                                   true,
                                   true,
                                   local_tolerance(i - 1),
                                   AngularTolerance);
      if (Cont < GeomAbs_C0)
        throw Standard_ConstructionError("GeomConvert curves not C0");
      else
      {
        if (Cont >= GeomAbs_G1)
          tabG1(i - 1) = true; // True=G1 continuity
        else
          tabG1(i - 1) = false;
      }
    }
    PreLast = ArrayOfCurves(i)->LastParameter();
  }

  while (index <= nb_curve - 1)
  { // determination of the Wire features
    nb_vertexG1 = 0;
    while (((index + nb_vertexG1) <= nb_curve - 2) && (tabG1(index + nb_vertexG1)))
      nb_vertexG1++;
    nb_group++;
    if (index == 0)
      nb_vertex_group0 = nb_vertexG1;
    index = index + 1 + nb_vertexG1;
  }

  if ((ClosedG1Flag) && (nb_group != 1))
  { // sort of the array
    nb_group--;
    ReorderArrayOfG1Curves(ArrayOfCurves,
                           local_tolerance,
                           tabG1,
                           nb_vertex_group0,
                           ClosedTolerance);
  }

  ArrayOfIndices      = new NCollection_HArray1<int>(0, nb_group);
  ArrayOfConcatenated = new NCollection_HArray1<occ::handle<Geom_BSplineCurve>>(0, nb_group - 1);

  bool fusion;
  int  k = 0;
  index  = 0;
  Pretreatment(ArrayOfCurves);
  double aPolynomialCoefficient[3];

  bool NeedDoubleDegRepara = Need2DegRepara(ArrayOfCurves);
  if (nb_group == 1 && ClosedG1Flag && NeedDoubleDegRepara)
  {
    Curve1 = ArrayOfCurves(nb_curve - 1);
    if (Curve1->Degree() > Geom2d_BSplineCurve::MaxDegree() / 2)
      ClosedG1Flag = false;
  }

  if ((nb_group == 1) && (ClosedG1Flag))
  { // treatment of a particular case
    ArrayOfIndices->SetValue(0, 0);
    ArrayOfIndices->SetValue(1, 0);
    indexmin = Indexmin(ArrayOfCurves);
    if (indexmin != (ArrayOfCurves.Length() - 1))
      ReorderArrayOfG1Curves(ArrayOfCurves, local_tolerance, tabG1, indexmin, ClosedTolerance);
    for (j = 0; j <= nb_curve - 1; j++)
    { // secondary loop inside each group
      if (NeedToBeTreated(ArrayOfCurves(j)))
        Curve1 = MultNumandDenom(Hermit::Solution(ArrayOfCurves(j)), ArrayOfCurves(j));
      else
        Curve1 = ArrayOfCurves(j);

      if (j == 0) // initialisation at the beginning of the loop
        Curve2 = Curve1;
      else
      {
        if ((j == (nb_curve - 1)) && (NeedDoubleDegRepara))
        {
          Curve2->D1(Curve2->LastParameter(), Pint, Vec1);
          Curve1->D1(Curve1->FirstParameter(), Pint, Vec2);
          lambda = Vec2.Magnitude() / Vec1.Magnitude();
          NCollection_Array1<double> KnotC1(Curve1->Knots());
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
          NCollection_Array1<int>    KnotC1Mults(Curve1->Multiplicities());
          BSplCLib::KnotSequence(KnotC1, KnotC1Mults, Curve1FlatKnots);
          KnotC1(1) = 0.0;
          for (ii = 2; ii <= KnotC1.Length(); ii++)
          {
            KnotC1(ii) =
              (-b + std::sqrt(b * b - 4 * a * (c - KnotC1(ii)))) / (2 * a); // ifv 17.05.00 buc60667
          }
          NCollection_Array1<gp_Pnt> Curve1Poles(Curve1->Poles());

          for (ii = 1; ii <= Curve1->NbKnots(); ii++)
            KnotC1Mults(ii) = (Curve1->Degree() + KnotC1Mults(ii));

          NCollection_Array1<double> FlatKnots(1,
                                               Curve1FlatKnots.Length()
                                                 + (Curve1->Degree() * Curve1->NbKnots()));

          BSplCLib::KnotSequence(KnotC1, KnotC1Mults, FlatKnots);
          NCollection_Array1<gp_Pnt> NewPoles(1, FlatKnots.Length() - (2 * Curve1->Degree() + 1));
          int                        aStatus;
          const NCollection_Array1<double>& Curve1Weights = Curve1->WeightsArray();
          for (ii = 1; ii <= Curve1->NbPoles(); ii++)
            for (jj = 1; jj <= 3; jj++)
              Curve1Poles(ii).SetCoord(jj, Curve1Poles(ii).Coord(jj) * Curve1Weights(ii));
          // POP pour WNT
          GeomConvert_reparameterise_evaluator ev(aPolynomialCoefficient);

          BSplCLib::FunctionReparameterise(ev,
                                           Curve1->Degree(),
                                           Curve1FlatKnots,
                                           Curve1Poles,
                                           FlatKnots,
                                           2 * Curve1->Degree(),
                                           NewPoles,
                                           aStatus);
          NCollection_Array1<double> NewWeights(1, FlatKnots.Length() - (2 * Curve1->Degree() + 1));

          BSplCLib::FunctionReparameterise(ev,
                                           Curve1->Degree(),
                                           Curve1FlatKnots,
                                           Curve1Weights,
                                           FlatKnots,
                                           2 * Curve1->Degree(),
                                           NewWeights,
                                           aStatus);
          for (ii = 1; ii <= NewPoles.Length(); ii++)
            for (jj = 1; jj <= 3; jj++)
              NewPoles(ii).SetCoord(jj, NewPoles(ii).Coord(jj) / NewWeights(ii));
          Curve1 =
            new Geom_BSplineCurve(NewPoles, NewWeights, KnotC1, KnotC1Mults, 2 * Curve1->Degree());
        }
        GeomConvert_CompCurveToBSplineCurve C(Curve2);
        fusion = C.Add(Curve1,
                       local_tolerance(j - 1)); // merge of two consecutive curves
        if (!fusion)
          throw Standard_ConstructionError("GeomConvert Concatenation Error");
        Curve2 = C.BSplineCurve();
      }
    }
    Curve2->SetPeriodic(); // only one C1 curve
    Curve2->RemoveKnot(Curve2->LastUKnotIndex(),
                       Curve2->Multiplicity(Curve2->LastUKnotIndex()) - 1,
                       Precision::Confusion());
    ArrayOfConcatenated->SetValue(0, Curve2);
  }

  else
    // clang-format off
   for (i=0;i<=nb_group-1;i++){                             //principal loop on each G1 continuity
      // clang-format on
      nb_vertexG1 = 0; // group

      while (((index + nb_vertexG1) <= nb_curve - 2) && (tabG1(index + nb_vertexG1)))
        nb_vertexG1++;

      if ((!ClosedG1Flag) || (nb_group == 1))
      { // filling of the array of index which are kept
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

      for (j = index; j <= index + nb_vertexG1; j++)
      { // secondary loop inside each group
        if (NeedToBeTreated(ArrayOfCurves(j)))
          Curve1 = MultNumandDenom(Hermit::Solution(ArrayOfCurves(j)), ArrayOfCurves(j));
        else
          Curve1 = ArrayOfCurves(j);

        if (index == j) // initialisation at the beginning of the loop
          ArrayOfConcatenated->SetValue(i, Curve1);
        else
        {
          // Merge of two consecutive curves.
          GeomConvert_CompCurveToBSplineCurve C(ArrayOfConcatenated->Value(i));
          fusion = C.Add(Curve1, local_tolerance(j - 1), true);
          if (!fusion)
            throw Standard_ConstructionError("GeomConvert Concatenation Error");
          ArrayOfConcatenated->SetValue(i, C.BSplineCurve());
        }
      }
      index = index + 1 + nb_vertexG1;
    }
}

//=================================================================================================

void GeomConvert::C0BSplineToC1BSplineCurve(occ::handle<Geom_BSplineCurve>& BS,
                                            const double                    tolerance,
                                            const double                    AngularTol)

{
  bool                                                             fusion;
  occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>> ArrayOfConcatenated;
  // the array with the resulting curves

  GeomConvert::C0BSplineToArrayOfC1BSplineCurve(BS, ArrayOfConcatenated, AngularTol, tolerance);

  GeomConvert_CompCurveToBSplineCurve C(ArrayOfConcatenated->Value(0));
  if (ArrayOfConcatenated->Length() >= 2)
  {
    int i;
    for (i = 1; i < ArrayOfConcatenated->Length(); i++)
    {
      fusion = C.Add(ArrayOfConcatenated->Value(i), tolerance);
      if (!fusion)
        throw Standard_ConstructionError("GeomConvert Concatenation Error");
    }
  }
  BS = C.BSplineCurve();
}

//=================================================================================================

void GeomConvert::C0BSplineToArrayOfC1BSplineCurve(
  const occ::handle<Geom_BSplineCurve>&                             BS,
  occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& tabBS,
  const double                                                      tolerance)
{
  C0BSplineToArrayOfC1BSplineCurve(BS, tabBS, Precision::Angular(), tolerance);
}

//=================================================================================================

void GeomConvert::C0BSplineToArrayOfC1BSplineCurve(
  const occ::handle<Geom_BSplineCurve>&                             BS,
  occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>>& tabBS,
  const double                                                      AngularTolerance,
  const double                                                      tolerance)

{
  const NCollection_Array1<int>&    BSMults = BS->Multiplicities();
  const NCollection_Array1<double>& BSKnots = BS->Knots();
  int                               i, j, nbcurveC1 = 1;
  double                            U1, U2;
  bool                              closed_flag = false;
  gp_Pnt                            point;
  gp_Vec                            V1, V2;
  // clang-format off
 for (i=BS->FirstUKnotIndex() ;i<=(BS->LastUKnotIndex()-1);i++){                                 //give the number of C1 curves
    // clang-format on
    if (BSMults(i) == BS->Degree())
      nbcurveC1++;
  }

  if (nbcurveC1 > 1)
  {
    NCollection_Array1<occ::handle<Geom_BSplineCurve>> ArrayOfCurves(0, nbcurveC1 - 1);
    NCollection_Array1<double>                         ArrayOfToler(0, nbcurveC1 - 2);

    for (i = 0; i <= nbcurveC1 - 2; i++)
      // filling of the array of tolerances
      ArrayOfToler(i) = tolerance;
    // with the variable tolerance
    U2 = BS->FirstParameter();
    j  = BS->FirstUKnotIndex() + 1;
    for (i = 0; i < nbcurveC1; i++)
    {
      // filling of the array of curves
      U1 = U2;
      // with the curves C1 segmented
      while (BSMults(j) < BS->Degree() && j < BS->LastUKnotIndex())
        j++;
      U2 = BSKnots(j);
      j++;
      occ::handle<Geom_BSplineCurve> BSbis = occ::down_cast<Geom_BSplineCurve>(BS->Copy());
      BSbis->Segment(U1, U2);
      ArrayOfCurves(i) = BSbis;
    }

    occ::handle<NCollection_HArray1<int>> ArrayOfIndices;

    BS->D1(BS->FirstParameter(), point, V1);
    BS->D1(BS->LastParameter(), point, V2);

    if ((BS->IsClosed()) && (V1.IsParallel(V2, AngularTolerance)))
    {
      // check if the BSpline is closed G1
      closed_flag = true;
    }

    GeomConvert::ConcatC1(ArrayOfCurves,
                          ArrayOfToler,
                          ArrayOfIndices,
                          tabBS,
                          closed_flag,
                          tolerance,
                          AngularTolerance);
  }
  else
  {
    tabBS = new NCollection_HArray1<occ::handle<Geom_BSplineCurve>>(0, 0);
    tabBS->SetValue(0, BS);
  }
}
