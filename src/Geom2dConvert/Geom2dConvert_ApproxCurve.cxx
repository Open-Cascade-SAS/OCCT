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


#include <Adaptor2d_HCurve2d.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//class : Geom2dConvert_ApproxCurve_Eval
//purpose: evaluator class for approximation
//=======================================================================
class Geom2dConvert_ApproxCurve_Eval : public AdvApprox_EvaluatorFunction
{
 public:
  Geom2dConvert_ApproxCurve_Eval (const Handle(Adaptor2d_HCurve2d)& theFunc, 
                                  Standard_Real First, Standard_Real Last)
    : fonct(theFunc) { StartEndSav[0] = First; StartEndSav[1] = Last; }
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Adaptor2d_HCurve2d) fonct;
  Standard_Real StartEndSav[2];
};

void Geom2dConvert_ApproxCurve_Eval::Evaluate (Standard_Integer *Dimension,
                                               Standard_Real     StartEnd[2],
                                               Standard_Real    *Param, // Parameter at which evaluation
                                               Standard_Integer *Order, // Derivative Request
                                               Standard_Real    *Result,// [Dimension]
                                               Standard_Integer *ErrorCode)
{
  *ErrorCode = 0;
  Standard_Real par = *Param;

// Dimension is incorrect
  if (*Dimension!=2) {
    *ErrorCode = 1;
  }
// Parameter is incorrect
  if ( par < StartEnd[0] || par > StartEnd[1] ) {
    *ErrorCode = 2;
  }
  if(StartEnd[0] != StartEndSav[0] || StartEnd[1]!= StartEndSav[1]) 
    {
      fonct = fonct->Trim(StartEnd[0],StartEnd[1],Precision::PConfusion());
      StartEndSav[0]=StartEnd[0];
      StartEndSav[1]=StartEnd[1];
    }

  gp_Pnt2d pnt;
  gp_Vec2d v1, v2;

  switch (*Order) {
  case 0:
    pnt = fonct->Value(par);
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
    *ErrorCode = 3;
    break;
  }
}

Geom2dConvert_ApproxCurve::Geom2dConvert_ApproxCurve(const Handle(Geom2d_Curve)& Curve,const Standard_Real Tol2d,const GeomAbs_Shape Order,const Standard_Integer MaxSegments,const Standard_Integer MaxDegree)
{
  Handle(Geom2dAdaptor_HCurve) HCurve = new Geom2dAdaptor_HCurve (Curve);
  Approximate(HCurve, Tol2d, Order, MaxSegments, MaxDegree);
}

Geom2dConvert_ApproxCurve::Geom2dConvert_ApproxCurve(const Handle(Adaptor2d_HCurve2d)& Curve,
                                                     const Standard_Real               Tol2d,
                                                     const GeomAbs_Shape               Order,
                                                     const Standard_Integer            MaxSegments,
                                                     const Standard_Integer            MaxDegree)
{
  Approximate(Curve, Tol2d, Order, MaxSegments, MaxDegree);
}

void Geom2dConvert_ApproxCurve::Approximate(const Handle(Adaptor2d_HCurve2d)& theCurve,
                                            const Standard_Real               theTol2d,
                                            const GeomAbs_Shape               theOrder,
                                            const Standard_Integer            theMaxSegments,
                                            const Standard_Integer            theMaxDegree)
{
  // Initialisation of input parameters of AdvApprox

  Standard_Integer Num1DSS=0, Num2DSS=1, Num3DSS=0;
  Handle(TColStd_HArray1OfReal) OneDTolNul, ThreeDTolNul;
  Handle(TColStd_HArray1OfReal) TwoDTol  = new TColStd_HArray1OfReal(1,Num2DSS);
  TwoDTol->Init(theTol2d);

  Standard_Real First = theCurve->FirstParameter();
  Standard_Real Last  = theCurve->LastParameter();

  Standard_Integer NbInterv_C2 = theCurve->NbIntervals(GeomAbs_C2);
  TColStd_Array1OfReal CutPnts_C2(1, NbInterv_C2+1);
  theCurve->Intervals(CutPnts_C2,GeomAbs_C2);
  Standard_Integer NbInterv_C3 = theCurve->NbIntervals(GeomAbs_C3);
  TColStd_Array1OfReal CutPnts_C3(1, NbInterv_C3+1);
  theCurve->Intervals(CutPnts_C3,GeomAbs_C3);
  AdvApprox_PrefAndRec CutTool(CutPnts_C2,CutPnts_C3);

  myMaxError = 0;

  Geom2dConvert_ApproxCurve_Eval ev (theCurve, First, Last);
  AdvApprox_ApproxAFunction aApprox (Num1DSS, Num2DSS, Num3DSS, 
                                     OneDTolNul, TwoDTol, ThreeDTolNul,
                                     First, Last, theOrder,
                                     theMaxDegree, theMaxSegments,
                                     ev, CutTool);

  myIsDone = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult) {
    TColgp_Array1OfPnt2d Poles(1,aApprox.NbPoles());
    aApprox.Poles2d(1,Poles);
    Handle(TColStd_HArray1OfReal)    Knots = aApprox.Knots();
    Handle(TColStd_HArray1OfInteger) Mults = aApprox.Multiplicities();
    Standard_Integer Degree = aApprox.Degree();
    myBSplCurve = new Geom2d_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
    myMaxError = aApprox.MaxError(2, 1);
  } 
}

 Handle(Geom2d_BSplineCurve) Geom2dConvert_ApproxCurve::Curve() const
{
  return myBSplCurve;
}

 Standard_Boolean Geom2dConvert_ApproxCurve::IsDone() const
{
  return myIsDone; 
}

 Standard_Boolean Geom2dConvert_ApproxCurve::HasResult() const
{
  return myHasResult; 
}

 Standard_Real Geom2dConvert_ApproxCurve::MaxError() const
{
  return myMaxError;
}

 void Geom2dConvert_ApproxCurve::Dump(Standard_OStream& o) const
{
  o << "******* Dump of ApproxCurve *******" << endl;
  o << "******* Error   " << MaxError() << endl;
}

