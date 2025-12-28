// Created on: 1997-09-11
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

#include <Adaptor3d_Curve.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=======================================================================
// class : GeomConvert_ApproxCurve_Eval
// purpose: evaluator class for approximation
//=======================================================================
class GeomConvert_ApproxCurve_Eval : public AdvApprox_EvaluatorFunction
{
public:
  GeomConvert_ApproxCurve_Eval(const occ::handle<Adaptor3d_Curve>& theFunc,
                               double                              First,
                               double                              Last)
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
  occ::handle<Adaptor3d_Curve> fonct;
  double                       StartEndSav[2];
};

void GeomConvert_ApproxCurve_Eval::Evaluate(int*    Dimension,
                                            double  StartEnd[2],
                                            double* Param,  // Parameter at which evaluation
                                            int*    Order,  // Derivative Request
                                            double* Result, // [Dimension]
                                            int*    ErrorCode)
{
  *ErrorCode = 0;
  double par = *Param;

  // Dimension is incorrect
  if (*Dimension != 3)
  {
    *ErrorCode = 1;
  }

  if (StartEnd[0] != StartEndSav[0] || StartEnd[1] != StartEndSav[1])
  {
    fonct          = fonct->Trim(StartEnd[0], StartEnd[1], Precision::PConfusion());
    StartEndSav[0] = StartEnd[0];
    StartEndSav[1] = StartEnd[1];
  }

  gp_Pnt pnt;
  gp_Vec v1, v2;

  switch (*Order)
  {
    case 0:
      pnt       = fonct->Value(par);
      Result[0] = pnt.X();
      Result[1] = pnt.Y();
      Result[2] = pnt.Z();
      break;
    case 1:
      fonct->D1(par, pnt, v1);
      Result[0] = v1.X();
      Result[1] = v1.Y();
      Result[2] = v1.Z();
      break;
    case 2:
      fonct->D2(par, pnt, v1, v2);
      Result[0] = v2.X();
      Result[1] = v2.Y();
      Result[2] = v2.Z();
      break;
    default:
      Result[0] = Result[1] = Result[2] = 0.;
      *ErrorCode                        = 3;
      break;
  }
}

GeomConvert_ApproxCurve::GeomConvert_ApproxCurve(const occ::handle<Geom_Curve>& Curve,
                                                 const double                   Tol3d,
                                                 const GeomAbs_Shape            Order,
                                                 const int                      MaxSegments,
                                                 const int                      MaxDegree)
{
  occ::handle<GeomAdaptor_Curve> HCurve = new GeomAdaptor_Curve(Curve);
  Approximate(HCurve, Tol3d, Order, MaxSegments, MaxDegree);
}

GeomConvert_ApproxCurve::GeomConvert_ApproxCurve(const occ::handle<Adaptor3d_Curve>& Curve,
                                                 const double                        Tol3d,
                                                 const GeomAbs_Shape                 Order,
                                                 const int                           MaxSegments,
                                                 const int                           MaxDegree)
{
  Approximate(Curve, Tol3d, Order, MaxSegments, MaxDegree);
}

void GeomConvert_ApproxCurve::Approximate(const occ::handle<Adaptor3d_Curve>& theCurve,
                                          const double                        theTol3d,
                                          const GeomAbs_Shape                 theOrder,
                                          const int                           theMaxSegments,
                                          const int                           theMaxDegree)
{
  // Initialisation of input parameters of AdvApprox

  int                                      Num1DSS = 0, Num2DSS = 0, Num3DSS = 1;
  occ::handle<NCollection_HArray1<double>> OneDTolNul, TwoDTolNul;
  occ::handle<NCollection_HArray1<double>> ThreeDTol = new NCollection_HArray1<double>(1, Num3DSS);
  ThreeDTol->Init(theTol3d);

  double First = theCurve->FirstParameter();
  double Last  = theCurve->LastParameter();

  int                        NbInterv_C2 = theCurve->NbIntervals(GeomAbs_C2);
  NCollection_Array1<double> CutPnts_C2(1, NbInterv_C2 + 1);
  theCurve->Intervals(CutPnts_C2, GeomAbs_C2);
  int                        NbInterv_C3 = theCurve->NbIntervals(GeomAbs_C3);
  NCollection_Array1<double> CutPnts_C3(1, NbInterv_C3 + 1);
  theCurve->Intervals(CutPnts_C3, GeomAbs_C3);

  AdvApprox_PrefAndRec CutTool(CutPnts_C2, CutPnts_C3);

  myMaxError = 0;

  GeomConvert_ApproxCurve_Eval ev(theCurve, First, Last);
  AdvApprox_ApproxAFunction    aApprox(Num1DSS,
                                    Num2DSS,
                                    Num3DSS,
                                    OneDTolNul,
                                    TwoDTolNul,
                                    ThreeDTol,
                                    First,
                                    Last,
                                    theOrder,
                                    theMaxDegree,
                                    theMaxSegments,
                                    ev,
                                    CutTool);

  myIsDone    = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult)
  {
    NCollection_Array1<gp_Pnt> Poles(1, aApprox.NbPoles());
    aApprox.Poles(1, Poles);
    occ::handle<NCollection_HArray1<double>> Knots  = aApprox.Knots();
    occ::handle<NCollection_HArray1<int>>    Mults  = aApprox.Multiplicities();
    int                                      Degree = aApprox.Degree();
    myBSplCurve = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
    myMaxError  = aApprox.MaxError(3, 1);
  }
}

occ::handle<Geom_BSplineCurve> GeomConvert_ApproxCurve::Curve() const
{
  return myBSplCurve;
}

bool GeomConvert_ApproxCurve::IsDone() const
{
  return myIsDone;
}

bool GeomConvert_ApproxCurve::HasResult() const
{
  return myHasResult;
}

double GeomConvert_ApproxCurve::MaxError() const
{
  return myMaxError;
}

void GeomConvert_ApproxCurve::Dump(Standard_OStream& o) const
{
  o << "******* Dump of ApproxCurve *******" << std::endl;
  o << "*******Error   " << MaxError() << std::endl;
}
