// Created on: 1997-10-28
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Adaptor2d_Curve2d.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Approx_Curve2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

class Approx_Curve2d_Eval : public AdvApprox_EvaluatorFunction
{
public:
  Approx_Curve2d_Eval(const occ::handle<Adaptor2d_Curve2d>& theFunc, double First, double Last)
      : fonct(theFunc)
  {
    StartEndSav[0] = First;
    StartEndSav[1] = Last;
  }

  void Evaluate(int*    Dimension,
                double  StartEnd[2],
                double* Parameter,
                int*    DerivativeRequest,
                double* Result, // [Dimension]
                int*    ErrorCode) override;

private:
  occ::handle<Adaptor2d_Curve2d> fonct;
  double                         StartEndSav[2];
};

void Approx_Curve2d_Eval::Evaluate(int*    Dimension,
                                   double  StartEnd[2],
                                   double* Param,  // Parameter at which evaluation
                                   int*    Order,  // Derivative Request
                                   double* Result, // [Dimension]
                                   int*    ErrorCode)
{
  *ErrorCode = 0;
  double par = *Param;

  // Dimension is incorrect
  if (*Dimension != 2)
  {
    *ErrorCode = 1;
  }
  // Parameter is incorrect
  if (par < StartEnd[0] || par > StartEnd[1])
  {
    *ErrorCode = 2;
  }
  if (StartEnd[0] != StartEndSav[0] || StartEnd[1] != StartEndSav[1])
  {
    fonct          = fonct->Trim(StartEnd[0], StartEnd[1], Precision::PConfusion());
    StartEndSav[0] = StartEnd[0];
    StartEndSav[1] = StartEnd[1];
  }
  gp_Pnt2d pnt;
  gp_Vec2d v1, v2;

  switch (*Order)
  {
    case 0:
      pnt       = fonct->Value(par);
      Result[0] = pnt.X();
      Result[1] = pnt.Y();
      break;
    case 1:
      fonct->D1(par, pnt, v1);
      Result[0] = v1.X();
      Result[1] = v1.Y();
      break;
    case 2:
      fonct->D2(par, pnt, v1, v2);
      Result[0] = v2.X();
      Result[1] = v2.Y();
      break;
    default:
      Result[0] = Result[1] = 0.;
      *ErrorCode            = 3;
      break;
  }
}

Approx_Curve2d::Approx_Curve2d(const occ::handle<Adaptor2d_Curve2d>& C2D,
                               const double                          First,
                               const double                          Last,
                               const double                          TolU,
                               const double                          TolV,
                               const GeomAbs_Shape                   Continuity,
                               const int                             MaxDegree,
                               const int                             MaxSegments)
{
  C2D->Trim(First, Last, Precision::PConfusion());

  int                                      Num1DSS = 2, Num2DSS = 0, Num3DSS = 0;
  occ::handle<NCollection_HArray1<double>> TwoDTolNul, ThreeDTolNul;
  occ::handle<NCollection_HArray1<double>> OneDTol = new NCollection_HArray1<double>(1, Num1DSS);
  OneDTol->ChangeValue(1)                          = TolU;
  OneDTol->ChangeValue(2)                          = TolV;

  int                        NbInterv_C2 = C2D->NbIntervals(GeomAbs_C2);
  NCollection_Array1<double> CutPnts_C2(1, NbInterv_C2 + 1);
  C2D->Intervals(CutPnts_C2, GeomAbs_C2);
  int                        NbInterv_C3 = C2D->NbIntervals(GeomAbs_C3);
  NCollection_Array1<double> CutPnts_C3(1, NbInterv_C3 + 1);
  C2D->Intervals(CutPnts_C3, GeomAbs_C3);

  AdvApprox_PrefAndRec CutTool(CutPnts_C2, CutPnts_C3);

  myMaxError2dU = 0;
  myMaxError2dV = 0;

  Approx_Curve2d_Eval       ev(C2D, First, Last);
  AdvApprox_ApproxAFunction aApprox(Num1DSS,
                                    Num2DSS,
                                    Num3DSS,
                                    OneDTol,
                                    TwoDTolNul,
                                    ThreeDTolNul,
                                    First,
                                    Last,
                                    Continuity,
                                    MaxDegree,
                                    MaxSegments,
                                    ev,
                                    CutTool);

  myIsDone    = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult)
  {
    const int                    aNbPoles = aApprox.NbPoles();
    NCollection_Array1<gp_Pnt2d> Poles2d(1, aNbPoles);
    NCollection_Array1<double>   Poles1dU(1, aNbPoles);
    aApprox.Poles1d(1, Poles1dU);
    NCollection_Array1<double> Poles1dV(1, aNbPoles);
    aApprox.Poles1d(2, Poles1dV);
    for (int i = 1; i <= aNbPoles; i++)
      Poles2d.SetValue(i, gp_Pnt2d(Poles1dU.Value(i), Poles1dV.Value(i)));

    occ::handle<NCollection_HArray1<double>> Knots  = aApprox.Knots();
    occ::handle<NCollection_HArray1<int>>    Mults  = aApprox.Multiplicities();
    int                                      Degree = aApprox.Degree();
    myCurve       = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
    myMaxError2dU = aApprox.MaxError(1, 1);
    myMaxError2dV = aApprox.MaxError(1, 2);
  }
}

bool Approx_Curve2d::IsDone() const
{
  return myIsDone;
}

bool Approx_Curve2d::HasResult() const
{
  return myHasResult;
}

occ::handle<Geom2d_BSplineCurve> Approx_Curve2d::Curve() const
{
  return myCurve;
}

double Approx_Curve2d::MaxError2dU() const
{
  return myMaxError2dU;
}

double Approx_Curve2d::MaxError2dV() const
{
  return myMaxError2dV;
}
