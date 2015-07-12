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


#include <Adaptor2d_HCurve2d.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Approx_Curve2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

//=======================================================================
//class : Approx_Curve2d_Eval
//purpose: evaluator class for approximation
//=======================================================================
class Approx_Curve2d_Eval : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_Curve2d_Eval (const Handle(Adaptor2d_HCurve2d)& theFunc, 
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

void Approx_Curve2d_Eval::Evaluate (Standard_Integer *Dimension,
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

 Approx_Curve2d::Approx_Curve2d(const Handle(Adaptor2d_HCurve2d)& C2D,const Standard_Real First,const Standard_Real Last,const Standard_Real TolU,const Standard_Real TolV,const GeomAbs_Shape Continuity,const Standard_Integer MaxDegree,const Standard_Integer MaxSegments)
{
  C2D->Trim(First,Last,Precision::PConfusion());

  Standard_Integer Num1DSS=2, Num2DSS=0, Num3DSS=0;
  Handle(TColStd_HArray1OfReal) TwoDTolNul, ThreeDTolNul; 
  Handle(TColStd_HArray1OfReal) OneDTol  = new TColStd_HArray1OfReal(1,Num1DSS);
  OneDTol->ChangeValue(1) = TolU;
  OneDTol->ChangeValue(2) = TolV;
  
  Standard_Integer NbInterv_C2 = C2D->NbIntervals(GeomAbs_C2);
  TColStd_Array1OfReal CutPnts_C2(1, NbInterv_C2+1);
  C2D->Intervals(CutPnts_C2, GeomAbs_C2);
  Standard_Integer NbInterv_C3 = C2D->NbIntervals(GeomAbs_C3);
  TColStd_Array1OfReal CutPnts_C3(1, NbInterv_C3+1);
  C2D->Intervals(CutPnts_C3, GeomAbs_C3);

  AdvApprox_PrefAndRec CutTool(CutPnts_C2,CutPnts_C3);

  myMaxError2dU = 0;
  myMaxError2dV = 0;

  Approx_Curve2d_Eval ev (C2D, First, Last);
  AdvApprox_ApproxAFunction aApprox (Num1DSS, Num2DSS, Num3DSS, 
				     OneDTol, TwoDTolNul, ThreeDTolNul,
				     First, Last, Continuity,
				     MaxDegree, MaxSegments,
				     ev, CutTool);

  myIsDone = aApprox.IsDone();
  myHasResult = aApprox.HasResult();
  
  if (myHasResult) {
    TColgp_Array1OfPnt2d Poles2d(1,aApprox.NbPoles());
    TColStd_Array1OfReal Poles1dU(1,aApprox.NbPoles());
    aApprox.Poles1d(1, Poles1dU);
    TColStd_Array1OfReal Poles1dV(1,aApprox.NbPoles());
    aApprox.Poles1d(2, Poles1dV);
    for(Standard_Integer i = 1; i <= aApprox.NbPoles(); i++)
      Poles2d.SetValue(i, gp_Pnt2d(Poles1dU.Value(i), Poles1dV.Value(i)));

    Handle(TColStd_HArray1OfReal)    Knots = aApprox.Knots();
    Handle(TColStd_HArray1OfInteger) Mults = aApprox.Multiplicities();
    Standard_Integer Degree = aApprox.Degree();
    myCurve = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
    myMaxError2dU = aApprox.MaxError(1, 1);
    myMaxError2dV = aApprox.MaxError(1, 2);
  } 
}

 Standard_Boolean Approx_Curve2d::IsDone() const
{
  return myIsDone;
}

 Standard_Boolean Approx_Curve2d::HasResult() const
{
  return myHasResult;
}

 Handle(Geom2d_BSplineCurve) Approx_Curve2d::Curve() const
{
  return myCurve;
}

 Standard_Real Approx_Curve2d::MaxError2dU() const
{
  return myMaxError2dU;
}

 Standard_Real Approx_Curve2d::MaxError2dV() const
{
  return myMaxError2dV;
}
