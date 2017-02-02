// Created on: 1997-10-06
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
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_DichoCutting.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//class : Approx_CurveOnSurface_Eval
//purpose: evaluator class for approximation of both 2d and 3d curves
//=======================================================================
class Approx_CurveOnSurface_Eval : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_CurveOnSurface_Eval (const Handle(Adaptor3d_HCurve)& theFunc, 
                              const Handle(Adaptor2d_HCurve2d)& theFunc2d, 
                              Standard_Real First, Standard_Real Last)
    : fonct(theFunc), fonct2d(theFunc2d) 
      { StartEndSav[0] = First; StartEndSav[1] = Last; }
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Adaptor3d_HCurve) fonct;
  Handle(Adaptor2d_HCurve2d) fonct2d;
  Standard_Real StartEndSav[2];
};

void Approx_CurveOnSurface_Eval::Evaluate (Standard_Integer *Dimension,
                                           Standard_Real     StartEnd[2],
                                           Standard_Real    *Param, // Parameter at which evaluation
                                           Standard_Integer *Order, // Derivative Request
                                           Standard_Real    *Result,// [Dimension]
                                           Standard_Integer *ErrorCode)
{
  *ErrorCode = 0;
  Standard_Real par = *Param;

// Dimension is incorrect
  if (*Dimension != 5) {
    *ErrorCode = 1;
  }

// Parameter is incorrect
  if(StartEnd[0] != StartEndSav[0] || StartEnd[1]!= StartEndSav[1]) 
    {
      fonct = fonct->Trim(StartEnd[0],StartEnd[1],Precision::PConfusion());
      fonct2d = fonct2d->Trim(StartEnd[0],StartEnd[1],
				Precision::PConfusion());
      StartEndSav[0]=StartEnd[0];
      StartEndSav[1]=StartEnd[1];
    }
  gp_Pnt pnt;


  gp_Pnt2d pnt2d;

  switch (*Order) {
  case 0: 
    {
      fonct2d->D0(par, pnt2d);
      fonct->D0(par, pnt);
      Result[0] = pnt2d.X();
      Result[1] = pnt2d.Y();
      Result[2] = pnt.X();
      Result[3] = pnt.Y();
      Result[4] = pnt.Z();
      break;
    }
  case 1:
    {
      gp_Vec v1;
      gp_Vec2d v21;
      fonct2d->D1(par, pnt2d, v21);
      fonct->D1(par,pnt, v1);
      Result[0] = v21.X();
      Result[1] = v21.Y();
      Result[2] = v1.X();
      Result[3] = v1.Y();
      Result[4] = v1.Z();
      break;
  }
  case 2:
    {
      gp_Vec v1, v2;
      gp_Vec2d v21, v22;    
      fonct2d->D2(par, pnt2d, v21, v22);
      fonct->D2(par, pnt, v1, v2);         
      Result[0] = v22.X();
      Result[1] = v22.Y();
      Result[2] = v2.X();
      Result[3] = v2.Y();
      Result[4] = v2.Z();
      break;
    }
  default:
    Result[0] = Result[1] = Result[2] = Result[3] = Result[4] = 0.;
    *ErrorCode = 3;
    break;
  }
}

//=======================================================================
//class : Approx_CurveOnSurface_Eval3d
//purpose: evaluator class for approximation of 3d curve
//=======================================================================

class Approx_CurveOnSurface_Eval3d : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_CurveOnSurface_Eval3d (const Handle(Adaptor3d_HCurve)& theFunc, 
                                Standard_Real First, Standard_Real Last)
    : fonct(theFunc) { StartEndSav[0] = First; StartEndSav[1] = Last; }
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Adaptor3d_HCurve) fonct;
  Standard_Real StartEndSav[2];
};

void Approx_CurveOnSurface_Eval3d::Evaluate (Standard_Integer *Dimension,
                                             Standard_Real     StartEnd[2],
                                             Standard_Real    *Param, // Parameter at which evaluation
                                             Standard_Integer *Order, // Derivative Request
                                             Standard_Real    *Result,// [Dimension]
                                             Standard_Integer *ErrorCode)
{
  *ErrorCode = 0;
  Standard_Real par = *Param;

// Dimension is incorrect
  if (*Dimension != 3) {
    *ErrorCode = 1;
  }

// Parameter is incorrect
  if(StartEnd[0] != StartEndSav[0] || StartEnd[1]!= StartEndSav[1]) 
    {
      fonct = fonct->Trim(StartEnd[0],StartEnd[1],Precision::PConfusion());
      StartEndSav[0]=StartEnd[0];
      StartEndSav[1]=StartEnd[1];
    }

  gp_Pnt pnt;

  switch (*Order) {
  case 0:
    pnt = fonct->Value(par);
    Result[0] = pnt.X();
    Result[1] = pnt.Y();
    Result[2] = pnt.Z();
    break;
  case 1:
    {
      gp_Vec v1;
      fonct->D1(par, pnt, v1);
      Result[0] = v1.X();
      Result[1] = v1.Y();
      Result[2] = v1.Z();
      break;
    }
  case 2:
    {
      gp_Vec v1, v2;
      fonct->D2(par, pnt, v1, v2);
      Result[0] = v2.X();
      Result[1] = v2.Y();
      Result[2] = v2.Z();
      break;
    }
  default:
    Result[0] = Result[1] = Result[2] = 0.;
    *ErrorCode = 3;
    break;
  }
}

//=======================================================================
//class : Approx_CurveOnSurface_Eval2d
//purpose: evaluator class for approximation of 2d curve
//=======================================================================

class Approx_CurveOnSurface_Eval2d : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_CurveOnSurface_Eval2d (const Handle(Adaptor2d_HCurve2d)& theFunc2d, 
                                Standard_Real First, Standard_Real Last)
    : fonct2d(theFunc2d) { StartEndSav[0] = First; StartEndSav[1] = Last; }
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Adaptor2d_HCurve2d) fonct2d;
  Standard_Real StartEndSav[2];
};

void Approx_CurveOnSurface_Eval2d::Evaluate (Standard_Integer *Dimension,
                                             Standard_Real     StartEnd[2],
                                             Standard_Real    *Param, // Parameter at which evaluation
                                             Standard_Integer *Order, // Derivative Request
                                             Standard_Real    *Result,// [Dimension]
                                             Standard_Integer *ErrorCode)
{
  *ErrorCode = 0;
  Standard_Real par = *Param;

// Dimension is incorrect
  if (*Dimension != 2) {
    *ErrorCode = 1;
  }

// Parameter is incorrect
  if(StartEnd[0] != StartEndSav[0] || StartEnd[1]!= StartEndSav[1]) 
    {
      fonct2d = fonct2d->Trim(StartEnd[0],StartEnd[1],Precision::PConfusion());
      StartEndSav[0]=StartEnd[0];
      StartEndSav[1]=StartEnd[1];
    }
 
  gp_Pnt2d pnt;

  switch (*Order) {
  case 0:
    {
      pnt = fonct2d->Value(par);
      Result[0] = pnt.X();
      Result[1] = pnt.Y();
      break;
    }
  case 1:
    {
      gp_Vec2d v1;
      fonct2d->D1(par, pnt, v1);
      Result[0] = v1.X();
      Result[1] = v1.Y();
      break;
    }
  case 2:
    {
      gp_Vec2d v1, v2;
      fonct2d->D2(par, pnt, v1, v2);
      Result[0] = v2.X();
      Result[1] = v2.Y();
      break;
    }
  default:
    Result[0] = Result[1] = 0.;
    *ErrorCode = 3;
    break;
  }
}

 Approx_CurveOnSurface::Approx_CurveOnSurface(const Handle(Adaptor2d_HCurve2d)& C2D,
					      const Handle(Adaptor3d_HSurface)& Surf,
					      const Standard_Real First,
					      const Standard_Real Last,
					      const Standard_Real Tol,
					      const GeomAbs_Shape S,
					      const Standard_Integer MaxDegree,
					      const Standard_Integer MaxSegments, 
					      const Standard_Boolean only3d, 
					      const Standard_Boolean only2d)
{
  myIsDone = Standard_False;
  if(only3d && only2d) throw Standard_ConstructionError();
  GeomAbs_Shape Order  = S;

  Handle( Adaptor2d_HCurve2d ) TrimmedC2D = C2D->Trim( First, Last, Precision::PConfusion() );

  Adaptor3d_CurveOnSurface COnS( TrimmedC2D, Surf );
  Handle(Adaptor3d_HCurveOnSurface) HCOnS = new Adaptor3d_HCurveOnSurface();
  HCOnS->Set(COnS);

  Standard_Integer Num1DSS = 0, Num2DSS=0, Num3DSS=0;
  Handle(TColStd_HArray1OfReal) OneDTol;
  Handle(TColStd_HArray1OfReal) TwoDTolNul;
  Handle(TColStd_HArray1OfReal) ThreeDTol;

  // create evaluators and choose appropriate one
  Approx_CurveOnSurface_Eval3d Eval3dCvOnSurf (HCOnS,             First, Last);
  Approx_CurveOnSurface_Eval2d Eval2dCvOnSurf (       TrimmedC2D, First, Last);
  Approx_CurveOnSurface_Eval   EvalCvOnSurf   (HCOnS, TrimmedC2D, First, Last);
  AdvApprox_EvaluatorFunction* EvalPtr;
  if ( only3d ) EvalPtr = &Eval3dCvOnSurf;
  else if ( only2d ) EvalPtr = &Eval2dCvOnSurf;
  else EvalPtr = &EvalCvOnSurf;

  // Initialization for 2d approximation
  if(!only3d) {
    Num1DSS = 2;
    OneDTol = new TColStd_HArray1OfReal(1,Num1DSS);

    Standard_Real TolU, TolV;

    TolU = Surf->UResolution(Tol)/2;
    TolV = Surf->VResolution(Tol)/2;

    OneDTol->SetValue(1,TolU);
    OneDTol->SetValue(2,TolV);
  }
  
  if(!only2d) {
    Num3DSS=1;
    ThreeDTol = new TColStd_HArray1OfReal(1,Num3DSS);
    ThreeDTol->Init(Tol/2);
  }

  myError2dU = 0;
  myError2dV = 0;
  myError3d = 0;

  Standard_Integer NbInterv_C2 = HCOnS->NbIntervals(GeomAbs_C2);
  TColStd_Array1OfReal CutPnts_C2(1, NbInterv_C2 + 1);
  HCOnS->Intervals(CutPnts_C2, GeomAbs_C2);
  Standard_Integer NbInterv_C3 = HCOnS->NbIntervals(GeomAbs_C3);
  TColStd_Array1OfReal CutPnts_C3(1, NbInterv_C3 + 1);
  HCOnS->Intervals(CutPnts_C3, GeomAbs_C3);
  
  AdvApprox_PrefAndRec CutTool(CutPnts_C2,CutPnts_C3);
  AdvApprox_ApproxAFunction aApprox (Num1DSS, Num2DSS, Num3DSS, 
	      			     OneDTol, TwoDTolNul, ThreeDTol,
				     First, Last, Order,
				     MaxDegree, MaxSegments,
				     *EvalPtr, CutTool);

  myIsDone = aApprox.IsDone();
  myHasResult = aApprox.HasResult();
  
  if (myHasResult) {
    Handle(TColStd_HArray1OfReal)    Knots = aApprox.Knots();
    Handle(TColStd_HArray1OfInteger) Mults = aApprox.Multiplicities();
    Standard_Integer Degree = aApprox.Degree();

    if(!only2d) 
      {
	TColgp_Array1OfPnt Poles(1,aApprox.NbPoles());
	aApprox.Poles(1,Poles);
	myCurve3d = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
	myError3d = aApprox.MaxError(3, 1);
      }
    if(!only3d) 
      {
	TColgp_Array1OfPnt2d Poles2d(1,aApprox.NbPoles());
	TColStd_Array1OfReal Poles1dU(1,aApprox.NbPoles());
	aApprox.Poles1d(1, Poles1dU);
	TColStd_Array1OfReal Poles1dV(1,aApprox.NbPoles());
	aApprox.Poles1d(2, Poles1dV);
	for(Standard_Integer i = 1; i <= aApprox.NbPoles(); i++)
	  Poles2d.SetValue(i, gp_Pnt2d(Poles1dU.Value(i), Poles1dV.Value(i)));
	myCurve2d = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
	
	myError2dU = aApprox.MaxError(1, 1);
	myError2dV = aApprox.MaxError(1, 2);
      }
  }
  
//    }

}

 Standard_Boolean Approx_CurveOnSurface::IsDone() const
{
  return myIsDone;
}

 Standard_Boolean Approx_CurveOnSurface::HasResult() const
{
  return myHasResult;
}

 Handle(Geom_BSplineCurve) Approx_CurveOnSurface::Curve3d() const
{
  return myCurve3d;
}

 Handle(Geom2d_BSplineCurve) Approx_CurveOnSurface::Curve2d() const
{
  return myCurve2d;
}

 Standard_Real Approx_CurveOnSurface::MaxError3d() const
{
  return myError3d;
}

 Standard_Real Approx_CurveOnSurface::MaxError2dU() const
{
  return myError2dU;
}

 Standard_Real Approx_CurveOnSurface::MaxError2dV() const
{
  return myError2dV;
}

