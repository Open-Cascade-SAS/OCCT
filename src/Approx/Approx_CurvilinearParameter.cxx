// Created on: 1997-08-22
// Created by: Sergey SOKOLOV
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
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Approx_CurvilinearParameter.hxx>
#include <Approx_CurvlinFunc.hxx>
#include <CPnts_AbscissaPoint.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#ifdef OCCT_DEBUG_CHRONO
#include <OSD_Timer.hxx>
static OSD_Chronometer chr_total, chr_init, chr_approx;

Standard_Real t_total, t_init, t_approx;
void InitChron(OSD_Chronometer& ch)
{ 
    ch.Reset();
    ch.Start();
}

void ResultChron( OSD_Chronometer & ch, Standard_Real & time) 
{
    Standard_Real tch ;
    ch.Stop();
    ch.Show(tch);
    time=time +tch;
}

Standard_IMPORT Standard_Integer uparam_count;
Standard_IMPORT Standard_Real t_uparam;
#endif

//=======================================================================
//class : Approx_CurvilinearParameter_EvalCurv
//purpose  : case of a free 3D curve
//=======================================================================

class Approx_CurvilinearParameter_EvalCurv : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_CurvilinearParameter_EvalCurv (const Handle(Approx_CurvlinFunc)& theFunc, 
                                        Standard_Real First, Standard_Real Last)
    : fonct(theFunc) { StartEndSav[0] = First; StartEndSav[1] = Last; }
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Approx_CurvlinFunc) fonct;
  Standard_Real StartEndSav[2];
};

void Approx_CurvilinearParameter_EvalCurv::Evaluate (Standard_Integer * Dimension,
                                                     Standard_Real    * StartEnd,
                                                     Standard_Real    * Param,
                                                     Standard_Integer * Order,
                                                     Standard_Real    * Result,
                                                     Standard_Integer * ErrorCode)
{
  *ErrorCode = 0;
  Standard_Real S = *Param;
  TColStd_Array1OfReal Res(0, 2);
  Standard_Integer i;
  
// Dimension is incorrect
  if (*Dimension != 3) {
    *ErrorCode = 1;
  }
// Parameter is incorrect
  if ( S < StartEnd[0] || S > StartEnd[1] ) {
    *ErrorCode = 2;
  }

  if(StartEnd[0] != StartEndSav[0] || StartEnd[1]!= StartEndSav[1]) 
    {
      fonct->Trim(StartEnd[0],StartEnd[1], Precision::Confusion());
      StartEndSav[0]=StartEnd[0];
      StartEndSav[1]=StartEnd[1];
    }

  if(!fonct->EvalCase1(S, *Order, Res)) {
    *ErrorCode = 3;
  }

  for(i = 0; i <= 2; i++)
    Result[i] = Res(i);  
}

Approx_CurvilinearParameter::Approx_CurvilinearParameter(const Handle(Adaptor3d_HCurve)& C3D,
							 const Standard_Real Tol,
							 const GeomAbs_Shape Order,
							 const Standard_Integer MaxDegree,
							 const Standard_Integer MaxSegments)
{
#ifdef OCCT_DEBUG_CHRONO
  t_total = t_init = t_approx = t_uparam = 0;
  uparam_count = 0;
  InitChron(chr_total);
#endif
  myCase = 1;
// Initialisation of input parameters of AdvApprox

  Standard_Integer Num1DSS=0, Num2DSS=0, Num3DSS=1;
  Handle(TColStd_HArray1OfReal) OneDTolNul, TwoDTolNul; 
  Handle(TColStd_HArray1OfReal) ThreeDTol  = new TColStd_HArray1OfReal(1,Num3DSS);
  ThreeDTol->Init(Tol); 

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_init);
#endif
  Handle(Approx_CurvlinFunc) fonct = new Approx_CurvlinFunc(C3D, Tol/10);
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_init, t_init);
#endif

  Standard_Real FirstS = fonct->FirstParameter();
  Standard_Real  LastS = fonct->LastParameter();

  Standard_Integer NbInterv_C2 = fonct->NbIntervals(GeomAbs_C2);
  TColStd_Array1OfReal CutPnts_C2(1, NbInterv_C2+1);
  fonct->Intervals(CutPnts_C2,GeomAbs_C2);
  Standard_Integer NbInterv_C3 = fonct->NbIntervals(GeomAbs_C3);
  TColStd_Array1OfReal CutPnts_C3(1, NbInterv_C3+1);
  fonct->Intervals(CutPnts_C3,GeomAbs_C3);
  AdvApprox_PrefAndRec CutTool(CutPnts_C2,CutPnts_C3);

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_approx);
#endif

  Approx_CurvilinearParameter_EvalCurv evC (fonct, FirstS, LastS);
  AdvApprox_ApproxAFunction aApprox (Num1DSS, Num2DSS, Num3DSS, 
				     OneDTolNul, TwoDTolNul, ThreeDTol,
				     FirstS, LastS, Order,
				     MaxDegree, MaxSegments,
				     evC, CutTool);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_approx, t_approx);
#endif

  myDone      = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult) {
    TColgp_Array1OfPnt Poles(1,aApprox.NbPoles());
    aApprox.Poles(1,Poles);
    Handle(TColStd_HArray1OfReal)    Knots = aApprox.Knots();
    Handle(TColStd_HArray1OfInteger) Mults = aApprox.Multiplicities();
    Standard_Integer Degree = aApprox.Degree();
    myCurve3d = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
  }
  myMaxError3d = aApprox.MaxError(3,1);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_total, t_total);

  cout<<" total reparametrization time = "<<t_total<<endl;
  cout<<"initialization time = "<<t_init<<endl;
  cout<<"approximation time = "<<t_approx<<endl;
  cout<<"total time for uparam computation = "<<t_uparam<<endl;
  cout<<"number uparam calles = "<<uparam_count<<endl;
#endif
}

//=======================================================================
//class : Approx_CurvilinearParameter_EvalCurvOnSurf
//purpose  : case of a curve on one surface
//=======================================================================

class Approx_CurvilinearParameter_EvalCurvOnSurf : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_CurvilinearParameter_EvalCurvOnSurf (const Handle(Approx_CurvlinFunc)& theFunc, 
                                              Standard_Real First, Standard_Real Last)
    : fonct(theFunc) { StartEndSav[0] = First; StartEndSav[1] = Last; }
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Approx_CurvlinFunc) fonct;
  Standard_Real StartEndSav[2];
};

void Approx_CurvilinearParameter_EvalCurvOnSurf::Evaluate (Standard_Integer * Dimension,
                                                           Standard_Real    * StartEnd,
                                                           Standard_Real    * Param,
                                                           Standard_Integer * Order,
                                                           Standard_Real    * Result,
                                                           Standard_Integer * ErrorCode)
{
  *ErrorCode = 0;
  Standard_Real S = *Param;
  TColStd_Array1OfReal Res(0, 4);
  Standard_Integer i;

// Dimension is incorrect
  if (*Dimension != 5) {
    *ErrorCode = 1;
  }
// Parameter is incorrect
  if ( S < StartEnd[0] || S > StartEnd[1] ) {
    *ErrorCode = 2;
  }

  if(StartEnd[0] != StartEndSav[0] || StartEnd[1]!= StartEndSav[1]) 
    {
      fonct->Trim(StartEnd[0],StartEnd[1], Precision::Confusion());
      StartEndSav[0]=StartEnd[0];
      StartEndSav[1]=StartEnd[1];
    }

  if(!fonct->EvalCase2(S, *Order, Res)) {
    *ErrorCode = 3;
  }

  for(i = 0; i <= 4; i++)
    Result[i] = Res(i);  
}

Approx_CurvilinearParameter::Approx_CurvilinearParameter(const Handle(Adaptor2d_HCurve2d)& C2D,
							 const Handle(Adaptor3d_HSurface)& Surf,
							 const Standard_Real Tol,
							 const GeomAbs_Shape Order,
							 const Standard_Integer MaxDegree,
							 const Standard_Integer MaxSegments)
{
#ifdef OCCT_DEBUG_CHRONO
  t_total = t_init = t_approx = t_uparam = 0;
  uparam_count = 0;
  InitChron(chr_total);
#endif
  myCase = 2;

  // Initialisation of input parameters of AdvApprox

  Standard_Integer Num1DSS=2, Num2DSS=0, Num3DSS=1, i;

  Handle(TColStd_HArray1OfReal) OneDTol = new TColStd_HArray1OfReal(1,Num1DSS);
  Standard_Real TolV,TolW;

  ToleranceComputation(C2D,Surf,10,Tol,TolV,TolW);
  OneDTol->SetValue(1,TolV);
  OneDTol->SetValue(2,TolW);

  OneDTol->SetValue(1,Tol);
  OneDTol->SetValue(2,Tol);

  Handle(TColStd_HArray1OfReal) TwoDTolNul; 
  Handle(TColStd_HArray1OfReal) ThreeDTol = new TColStd_HArray1OfReal(1,Num3DSS);
  ThreeDTol->Init(Tol/2.); 

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_init);
#endif
  Handle(Approx_CurvlinFunc) fonct = new Approx_CurvlinFunc(C2D, Surf, Tol/20);
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_init, t_init);
#endif

  Standard_Real FirstS = fonct->FirstParameter();
  Standard_Real  LastS = fonct->LastParameter();

  Standard_Integer NbInterv_C2 = fonct->NbIntervals(GeomAbs_C2);
  TColStd_Array1OfReal CutPnts_C2(1, NbInterv_C2+1);
  fonct->Intervals(CutPnts_C2,GeomAbs_C2);
  Standard_Integer NbInterv_C3 = fonct->NbIntervals(GeomAbs_C3);
  TColStd_Array1OfReal CutPnts_C3(1, NbInterv_C3+1);
  fonct->Intervals(CutPnts_C3,GeomAbs_C3);
  AdvApprox_PrefAndRec CutTool(CutPnts_C2,CutPnts_C3);

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_approx);
#endif

  Approx_CurvilinearParameter_EvalCurvOnSurf evCOnS (fonct, FirstS, LastS);
  AdvApprox_ApproxAFunction aApprox (Num1DSS, Num2DSS, Num3DSS, 
				     OneDTol, TwoDTolNul, ThreeDTol,
				     FirstS, LastS, Order,
				     MaxDegree, MaxSegments,
				     evCOnS, CutTool);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_approx, t_approx);
#endif

  myDone      = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult) {
    Standard_Integer NbPoles = aApprox.NbPoles();
    TColgp_Array1OfPnt   Poles  (1,NbPoles);
    TColgp_Array1OfPnt2d Poles2d(1,NbPoles);
    TColStd_Array1OfReal Poles1d(1,NbPoles);
    aApprox.Poles(1,Poles);
    aApprox.Poles1d(1,Poles1d);
    for (i=1; i<=NbPoles; i++) 
      Poles2d(i).SetX(Poles1d(i));
    aApprox.Poles1d(2,Poles1d);
    for (i=1; i<=NbPoles; i++) 
      Poles2d(i).SetY(Poles1d(i));
    Handle(TColStd_HArray1OfReal)    Knots = aApprox.Knots();
    Handle(TColStd_HArray1OfInteger) Mults = aApprox.Multiplicities();
    Standard_Integer Degree = aApprox.Degree();
    myCurve3d = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
    myCurve2d1 = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
  }
  myMaxError2d1 = Max (aApprox.MaxError(1,1),aApprox.MaxError(1,2));
  myMaxError3d  = aApprox.MaxError(3,1);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_total, t_total);

  cout<<" total reparametrization time = "<<t_total<<endl;
  cout<<"initialization time = "<<t_init<<endl;
  cout<<"approximation time = "<<t_approx<<endl;
  cout<<"total time for uparam computation = "<<t_uparam<<endl;
  cout<<"number uparam calles = "<<uparam_count<<endl;
#endif
}

//=======================================================================
//function : Approx_CurvilinearParameter_EvalCurvOn2Surf
//purpose  : case of a curve on two surfaces
//=======================================================================

class Approx_CurvilinearParameter_EvalCurvOn2Surf : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_CurvilinearParameter_EvalCurvOn2Surf (const Handle(Approx_CurvlinFunc)& theFunc, 
                                               Standard_Real First, Standard_Real Last)
    : fonct(theFunc) { StartEndSav[0] = First; StartEndSav[1] = Last; }
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Approx_CurvlinFunc) fonct;
  Standard_Real StartEndSav[2];
};

void Approx_CurvilinearParameter_EvalCurvOn2Surf::Evaluate (Standard_Integer * Dimension,
                                                            Standard_Real    * StartEnd,
                                                            Standard_Real    * Param,
                                                            Standard_Integer * Order,
                                                            Standard_Real    * Result,
                                                            Standard_Integer * ErrorCode)
{
  *ErrorCode = 0;
  Standard_Real S = *Param;
  TColStd_Array1OfReal Res(0, 6);
  Standard_Integer i;

// Dimension is incorrect
  if (*Dimension != 7) {
    *ErrorCode = 1;
  }
// Parameter is incorrect
  if ( S < StartEnd[0] || S > StartEnd[1] ) {
    *ErrorCode = 2;
  }

/*  if(StartEnd[0] != StartEndSav[0] || StartEnd[1]!= StartEndSav[1]) 
    {
      fonct->Trim(StartEnd[0],StartEnd[1], Precision::Confusion());
      StartEndSav[0]=StartEnd[0];
      StartEndSav[1]=StartEnd[1];
    }
*/
  if(!fonct->EvalCase3(S, *Order, Res)) {
    *ErrorCode = 3;
  }

  for(i = 0; i <= 6; i++)
    Result[i] = Res(i);  
}

Approx_CurvilinearParameter::Approx_CurvilinearParameter(const Handle(Adaptor2d_HCurve2d)& C2D1,
							 const Handle(Adaptor3d_HSurface)& Surf1,
							 const Handle(Adaptor2d_HCurve2d)& C2D2,
							 const Handle(Adaptor3d_HSurface)& Surf2,
							 const Standard_Real Tol,
							 const GeomAbs_Shape Order,
							 const Standard_Integer MaxDegree,
							 const Standard_Integer MaxSegments)
{
  Standard_Integer i;

#ifdef OCCT_DEBUG_CHRONO
  t_total = t_init = t_approx = t_uparam = 0;
  uparam_count = 0;
  InitChron(chr_total);
#endif
  myCase = 3;

  // Initialisation of input parameters of AdvApprox

  Standard_Integer Num1DSS=4, Num2DSS=0, Num3DSS=1;
  Handle(TColStd_HArray1OfReal) OneDTol = new TColStd_HArray1OfReal(1,Num1DSS); 

  Standard_Real TolV,TolW;
  ToleranceComputation(C2D1,Surf1,10,Tol,TolV,TolW);
  OneDTol->SetValue(1,TolV); 
  OneDTol->SetValue(2,TolW); 
  
  ToleranceComputation(C2D2,Surf2,10,Tol,TolV,TolW);
  OneDTol->SetValue(3,TolV); 
  OneDTol->SetValue(4,TolW); 

  Handle(TColStd_HArray1OfReal) TwoDTolNul; 
  Handle(TColStd_HArray1OfReal) ThreeDTol = new TColStd_HArray1OfReal(1,Num3DSS);
  ThreeDTol->Init(Tol/2); 

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_init);
#endif
  Handle(Approx_CurvlinFunc) fonct = new Approx_CurvlinFunc(C2D1, C2D2, Surf1, Surf2, Tol/20);
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_init, t_init);
#endif

  Standard_Real FirstS = fonct->FirstParameter();
  Standard_Real  LastS = fonct->LastParameter();

  Standard_Integer NbInterv_C2 = fonct->NbIntervals(GeomAbs_C2);
  TColStd_Array1OfReal CutPnts_C2(1, NbInterv_C2+1);
  fonct->Intervals(CutPnts_C2,GeomAbs_C2);
  Standard_Integer NbInterv_C3 = fonct->NbIntervals(GeomAbs_C3);
  TColStd_Array1OfReal CutPnts_C3(1, NbInterv_C3+1);
  fonct->Intervals(CutPnts_C3,GeomAbs_C3);
  AdvApprox_PrefAndRec CutTool(CutPnts_C2,CutPnts_C3);  

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_approx);
#endif

  Approx_CurvilinearParameter_EvalCurvOn2Surf evCOn2S (fonct, FirstS, LastS);
  AdvApprox_ApproxAFunction aApprox (Num1DSS, Num2DSS, Num3DSS, 
				     OneDTol, TwoDTolNul, ThreeDTol,
				     FirstS, LastS, Order,
				     MaxDegree, MaxSegments,
				     evCOn2S, CutTool);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_approx, t_approx);
#endif

  myDone      = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult) {
    Standard_Integer NbPoles = aApprox.NbPoles();
    TColgp_Array1OfPnt   Poles  (1,NbPoles);
    TColgp_Array1OfPnt2d Poles2d(1,NbPoles);
    TColStd_Array1OfReal Poles1d(1,NbPoles);
    aApprox.Poles(1,Poles);
    aApprox.Poles1d(1,Poles1d);
    for (i=1; i<=NbPoles; i++) 
      Poles2d(i).SetX(Poles1d(i));
    aApprox.Poles1d(2,Poles1d);
    for (i=1; i<=NbPoles; i++) 
      Poles2d(i).SetY(Poles1d(i));
    Handle(TColStd_HArray1OfReal)    Knots = aApprox.Knots();
    Handle(TColStd_HArray1OfInteger) Mults = aApprox.Multiplicities();
    Standard_Integer Degree = aApprox.Degree();
    myCurve3d = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
    myCurve2d1 = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
    aApprox.Poles1d(3,Poles1d);
    for (i=1; i<=NbPoles; i++) 
      Poles2d(i).SetX(Poles1d(i));
    aApprox.Poles1d(4,Poles1d);
    for (i=1; i<=NbPoles; i++) 
      Poles2d(i).SetY(Poles1d(i));
    myCurve2d2 = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
  }
  myMaxError2d1 = Max (aApprox.MaxError(1,1),aApprox.MaxError(1,2));
  myMaxError2d2 = Max (aApprox.MaxError(1,3),aApprox.MaxError(1,4));
  myMaxError3d  = aApprox.MaxError(3,1);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_total, t_total);

  cout<<" total reparametrization time = "<<t_total<<endl;
  cout<<"initialization time = "<<t_init<<endl;
  cout<<"approximation time = "<<t_approx<<endl;
  cout<<"total time for uparam computation = "<<t_uparam<<endl;
  cout<<"number uparam calles = "<<uparam_count<<endl;
#endif
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

 Standard_Boolean Approx_CurvilinearParameter::IsDone() const
{
  return myDone;
}

//=======================================================================
//function : HasResult
//purpose  : 
//=======================================================================

 Standard_Boolean Approx_CurvilinearParameter::HasResult() const
{
  return myHasResult;
}

//=======================================================================
//function : Curve3d
//purpose  : returns the Bspline curve corresponding to the reparametrized 3D curve 
//=======================================================================

 Handle(Geom_BSplineCurve) Approx_CurvilinearParameter::Curve3d() const
{
  return myCurve3d;
}

//=======================================================================
//function : MaxError3d
//purpose  : returns the maximum error on the reparametrized 3D curve
//=======================================================================

 Standard_Real Approx_CurvilinearParameter::MaxError3d() const
{
  return myMaxError3d;
}

//=======================================================================
//function : Curve2d1
//purpose  : returns the BsplineCurve representing the reparametrized 2D curve on the
//           first surface (case of a curve on one or two surfaces)
//=======================================================================

 Handle(Geom2d_BSplineCurve) Approx_CurvilinearParameter::Curve2d1() const
{
  return myCurve2d1;
}

//=======================================================================
//function : MaxError2d1
//purpose  : returns the maximum error on the first reparametrized 2D curve
//=======================================================================

 Standard_Real Approx_CurvilinearParameter::MaxError2d1() const
{
  return myMaxError2d1;
}

//=======================================================================
//function : Curve2d2
//purpose  : returns the BsplineCurve representing the reparametrized 2D curve on the
//           second surface (case of a curve on two surfaces)
//=======================================================================

 Handle(Geom2d_BSplineCurve) Approx_CurvilinearParameter::Curve2d2() const
{
  return myCurve2d2;
}

//=======================================================================
//function : MaxError2d2
//purpose  : returns the maximum error on the second reparametrized 2D curve 
//=======================================================================

 Standard_Real Approx_CurvilinearParameter::MaxError2d2() const
{
  return myMaxError2d2;
}

//=======================================================================
//function : Dump
//purpose  : print the maximum errors(s)
//=======================================================================

void Approx_CurvilinearParameter::Dump(Standard_OStream& o) const
{
  o << "Dump of Approx_CurvilinearParameter" << endl;
  if (myCase==2 || myCase==3) 
    o << "myMaxError2d1 = " << myMaxError2d1 << endl;
  if (myCase==3) 
    o << "myMaxError2d2 = " << myMaxError2d2 << endl;
  o << "myMaxError3d = " << myMaxError3d << endl;
}

//=======================================================================
//function : ToleranceComputation
//purpose  : 
//=======================================================================

void Approx_CurvilinearParameter::ToleranceComputation(const Handle(Adaptor2d_HCurve2d) &C2D,
						       const Handle(Adaptor3d_HSurface) &S, 
						       const Standard_Integer MaxNumber, 
						       const Standard_Real Tol,
						       Standard_Real &TolV, Standard_Real &TolW)
{
  Standard_Real FirstU = C2D->FirstParameter(),
                LastU  = C2D->LastParameter();
//  Standard_Real parU, Max_dS_dv=1.,Max_dS_dw=1.;
  Standard_Real Max_dS_dv=1.,Max_dS_dw=1.;
  gp_Pnt P;
  gp_Pnt2d pntVW;
  gp_Vec dS_dv,dS_dw;

  for (Standard_Integer i=1; i<=MaxNumber; i++) {
    pntVW = C2D->Value(FirstU + (i-1)*(LastU-FirstU)/(MaxNumber-1));
    S->D1(pntVW.X(),pntVW.Y(),P,dS_dv,dS_dw);
    Max_dS_dv = Max (Max_dS_dv, dS_dv.Magnitude());
    Max_dS_dw = Max (Max_dS_dw, dS_dw.Magnitude());
  }
  TolV = Tol / (4.*Max_dS_dv);
  TolW = Tol / (4.*Max_dS_dw);

#ifdef OCCT_DEBUG
  cout << "TolV = " << TolV << endl;
  cout << "TolW = " << TolW << endl;
#endif
}
