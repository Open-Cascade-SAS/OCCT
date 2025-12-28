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

#include <Approx_CurvilinearParameter.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Approx_CurvlinFunc.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Precision.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <math_Vector.hxx>

#ifdef OCCT_DEBUG_CHRONO
  #include <OSD_Timer.hxx>
static OSD_Chronometer chr_total, chr_init, chr_approx;

double t_total, t_init, t_approx;

void InitChron(OSD_Chronometer& ch)
{
  ch.Reset();
  ch.Start();
}

void ResultChron(OSD_Chronometer& ch, double& time)
{
  double tch;
  ch.Stop();
  ch.Show(tch);
  time = time + tch;
}

Standard_IMPORT int    uparam_count;
Standard_IMPORT double t_uparam;
#endif

//=================================================================================================

class Approx_CurvilinearParameter_EvalCurv : public AdvApprox_EvaluatorFunction
{
public:
  Approx_CurvilinearParameter_EvalCurv(const occ::handle<Approx_CurvlinFunc>& theFunc,
                                       double                                 First,
                                       double                                 Last)
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
  occ::handle<Approx_CurvlinFunc> fonct;
  double                          StartEndSav[2];
};

void Approx_CurvilinearParameter_EvalCurv::Evaluate(int*    Dimension,
                                                    double* StartEnd,
                                                    double* Param,
                                                    int*    Order,
                                                    double* Result,
                                                    int*    ErrorCode)
{
  *ErrorCode                   = 0;
  double                     S = *Param;
  NCollection_Array1<double> Res(0, 2);
  int                        i;

  // Dimension is incorrect
  if (*Dimension != 3)
  {
    *ErrorCode = 1;
  }
  // Parameter is incorrect
  if (S < StartEnd[0] || S > StartEnd[1])
  {
    *ErrorCode = 2;
  }

  if (StartEnd[0] != StartEndSav[0] || StartEnd[1] != StartEndSav[1])
  {
    fonct->Trim(StartEnd[0], StartEnd[1], Precision::Confusion());
    StartEndSav[0] = StartEnd[0];
    StartEndSav[1] = StartEnd[1];
  }

  if (!fonct->EvalCase1(S, *Order, Res))
  {
    *ErrorCode = 3;
  }

  for (i = 0; i <= 2; i++)
    Result[i] = Res(i);
}

Approx_CurvilinearParameter::Approx_CurvilinearParameter(const occ::handle<Adaptor3d_Curve>& C3D,
                                                         const double                        Tol,
                                                         const GeomAbs_Shape                 Order,
                                                         const int MaxDegree,
                                                         const int MaxSegments)
    : myMaxError2d1(0.0),
      myMaxError2d2(0.0)
{
#ifdef OCCT_DEBUG_CHRONO
  t_total = t_init = t_approx = t_uparam = 0;
  uparam_count                           = 0;
  InitChron(chr_total);
#endif
  myCase = 1;
  // Initialisation of input parameters of AdvApprox

  int                                      Num1DSS = 0, Num2DSS = 0, Num3DSS = 1;
  occ::handle<NCollection_HArray1<double>> OneDTolNul, TwoDTolNul;
  occ::handle<NCollection_HArray1<double>> ThreeDTol = new NCollection_HArray1<double>(1, Num3DSS);
  ThreeDTol->Init(Tol);

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_init);
#endif
  occ::handle<Approx_CurvlinFunc> fonct = new Approx_CurvlinFunc(C3D, Tol / 10);
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_init, t_init);
#endif

  double FirstS = fonct->FirstParameter();
  double LastS  = fonct->LastParameter();

  int                        NbInterv_C2 = fonct->NbIntervals(GeomAbs_C2);
  NCollection_Array1<double> CutPnts_C2(1, NbInterv_C2 + 1);
  fonct->Intervals(CutPnts_C2, GeomAbs_C2);
  int                        NbInterv_C3 = fonct->NbIntervals(GeomAbs_C3);
  NCollection_Array1<double> CutPnts_C3(1, NbInterv_C3 + 1);
  fonct->Intervals(CutPnts_C3, GeomAbs_C3);
  AdvApprox_PrefAndRec CutTool(CutPnts_C2, CutPnts_C3);

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_approx);
#endif

  Approx_CurvilinearParameter_EvalCurv evC(fonct, FirstS, LastS);
  AdvApprox_ApproxAFunction            aApprox(Num1DSS,
                                    Num2DSS,
                                    Num3DSS,
                                    OneDTolNul,
                                    TwoDTolNul,
                                    ThreeDTol,
                                    FirstS,
                                    LastS,
                                    Order,
                                    MaxDegree,
                                    MaxSegments,
                                    evC,
                                    CutTool);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_approx, t_approx);
#endif

  myDone      = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult)
  {
    NCollection_Array1<gp_Pnt> Poles(1, aApprox.NbPoles());
    aApprox.Poles(1, Poles);
    occ::handle<NCollection_HArray1<double>> Knots  = aApprox.Knots();
    occ::handle<NCollection_HArray1<int>>    Mults  = aApprox.Multiplicities();
    int                                      Degree = aApprox.Degree();
    myCurve3d = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
  }
  myMaxError3d = aApprox.MaxError(3, 1);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_total, t_total);

  std::cout << " total reparametrization time = " << t_total << std::endl;
  std::cout << "initialization time = " << t_init << std::endl;
  std::cout << "approximation time = " << t_approx << std::endl;
  std::cout << "total time for uparam computation = " << t_uparam << std::endl;
  std::cout << "number uparam calls = " << uparam_count << std::endl;
#endif
}

//=================================================================================================

class Approx_CurvilinearParameter_EvalCurvOnSurf : public AdvApprox_EvaluatorFunction
{
public:
  Approx_CurvilinearParameter_EvalCurvOnSurf(const occ::handle<Approx_CurvlinFunc>& theFunc,
                                             double                                 First,
                                             double                                 Last)
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
  occ::handle<Approx_CurvlinFunc> fonct;
  double                          StartEndSav[2];
};

void Approx_CurvilinearParameter_EvalCurvOnSurf::Evaluate(int*    Dimension,
                                                          double* StartEnd,
                                                          double* Param,
                                                          int*    Order,
                                                          double* Result,
                                                          int*    ErrorCode)
{
  *ErrorCode                   = 0;
  double                     S = *Param;
  NCollection_Array1<double> Res(0, 4);
  int                        i;

  // Dimension is incorrect
  if (*Dimension != 5)
  {
    *ErrorCode = 1;
  }
  // Parameter is incorrect
  if (S < StartEnd[0] || S > StartEnd[1])
  {
    *ErrorCode = 2;
  }

  if (StartEnd[0] != StartEndSav[0] || StartEnd[1] != StartEndSav[1])
  {
    fonct->Trim(StartEnd[0], StartEnd[1], Precision::Confusion());
    StartEndSav[0] = StartEnd[0];
    StartEndSav[1] = StartEnd[1];
  }

  if (!fonct->EvalCase2(S, *Order, Res))
  {
    *ErrorCode = 3;
  }

  for (i = 0; i <= 4; i++)
    Result[i] = Res(i);
}

Approx_CurvilinearParameter::Approx_CurvilinearParameter(const occ::handle<Adaptor2d_Curve2d>& C2D,
                                                         const occ::handle<Adaptor3d_Surface>& Surf,
                                                         const double                          Tol,
                                                         const GeomAbs_Shape Order,
                                                         const int           MaxDegree,
                                                         const int           MaxSegments)
{
#ifdef OCCT_DEBUG_CHRONO
  t_total = t_init = t_approx = t_uparam = 0;
  uparam_count                           = 0;
  InitChron(chr_total);
#endif
  myCase = 2;

  // Initialisation of input parameters of AdvApprox

  int Num1DSS = 2, Num2DSS = 0, Num3DSS = 1, i;

  occ::handle<NCollection_HArray1<double>> OneDTol = new NCollection_HArray1<double>(1, Num1DSS);
  double                                   TolV, TolW;

  ToleranceComputation(C2D, Surf, 10, Tol, TolV, TolW);
  OneDTol->SetValue(1, TolV);
  OneDTol->SetValue(2, TolW);

  OneDTol->SetValue(1, Tol);
  OneDTol->SetValue(2, Tol);

  occ::handle<NCollection_HArray1<double>> TwoDTolNul;
  occ::handle<NCollection_HArray1<double>> ThreeDTol = new NCollection_HArray1<double>(1, Num3DSS);
  ThreeDTol->Init(Tol / 2.);

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_init);
#endif
  occ::handle<Approx_CurvlinFunc> fonct = new Approx_CurvlinFunc(C2D, Surf, Tol / 20);
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_init, t_init);
#endif

  double FirstS = fonct->FirstParameter();
  double LastS  = fonct->LastParameter();

  int                        NbInterv_C2 = fonct->NbIntervals(GeomAbs_C2);
  NCollection_Array1<double> CutPnts_C2(1, NbInterv_C2 + 1);
  fonct->Intervals(CutPnts_C2, GeomAbs_C2);
  int                        NbInterv_C3 = fonct->NbIntervals(GeomAbs_C3);
  NCollection_Array1<double> CutPnts_C3(1, NbInterv_C3 + 1);
  fonct->Intervals(CutPnts_C3, GeomAbs_C3);
  AdvApprox_PrefAndRec CutTool(CutPnts_C2, CutPnts_C3);

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_approx);
#endif

  Approx_CurvilinearParameter_EvalCurvOnSurf evCOnS(fonct, FirstS, LastS);
  AdvApprox_ApproxAFunction                  aApprox(Num1DSS,
                                    Num2DSS,
                                    Num3DSS,
                                    OneDTol,
                                    TwoDTolNul,
                                    ThreeDTol,
                                    FirstS,
                                    LastS,
                                    Order,
                                    MaxDegree,
                                    MaxSegments,
                                    evCOnS,
                                    CutTool);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_approx, t_approx);
#endif

  myDone      = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult)
  {
    int                          NbPoles = aApprox.NbPoles();
    NCollection_Array1<gp_Pnt>   Poles(1, NbPoles);
    NCollection_Array1<gp_Pnt2d> Poles2d(1, NbPoles);
    NCollection_Array1<double>   Poles1d(1, NbPoles);
    aApprox.Poles(1, Poles);
    aApprox.Poles1d(1, Poles1d);
    for (i = 1; i <= NbPoles; i++)
      Poles2d(i).SetX(Poles1d(i));
    aApprox.Poles1d(2, Poles1d);
    for (i = 1; i <= NbPoles; i++)
      Poles2d(i).SetY(Poles1d(i));
    occ::handle<NCollection_HArray1<double>> Knots  = aApprox.Knots();
    occ::handle<NCollection_HArray1<int>>    Mults  = aApprox.Multiplicities();
    int                                      Degree = aApprox.Degree();
    myCurve3d  = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
    myCurve2d1 = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
  }
  myMaxError2d1 = std::max(aApprox.MaxError(1, 1), aApprox.MaxError(1, 2));
  myMaxError3d  = aApprox.MaxError(3, 1);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_total, t_total);

  std::cout << " total reparametrization time = " << t_total << std::endl;
  std::cout << "initialization time = " << t_init << std::endl;
  std::cout << "approximation time = " << t_approx << std::endl;
  std::cout << "total time for uparam computation = " << t_uparam << std::endl;
  std::cout << "number uparam calls = " << uparam_count << std::endl;
#endif
}

//=================================================================================================

class Approx_CurvilinearParameter_EvalCurvOn2Surf : public AdvApprox_EvaluatorFunction
{
public:
  Approx_CurvilinearParameter_EvalCurvOn2Surf(const occ::handle<Approx_CurvlinFunc>& theFunc,
                                              double                                 First,
                                              double                                 Last)
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
  occ::handle<Approx_CurvlinFunc> fonct;
  double                          StartEndSav[2];
};

void Approx_CurvilinearParameter_EvalCurvOn2Surf::Evaluate(int*    Dimension,
                                                           double* StartEnd,
                                                           double* Param,
                                                           int*    Order,
                                                           double* Result,
                                                           int*    ErrorCode)
{
  *ErrorCode                   = 0;
  double                     S = *Param;
  NCollection_Array1<double> Res(0, 6);
  int                        i;

  // Dimension is incorrect
  if (*Dimension != 7)
  {
    *ErrorCode = 1;
  }
  // Parameter is incorrect
  if (S < StartEnd[0] || S > StartEnd[1])
  {
    *ErrorCode = 2;
  }

  /*  if(StartEnd[0] != StartEndSav[0] || StartEnd[1]!= StartEndSav[1])
      {
        fonct->Trim(StartEnd[0],StartEnd[1], Precision::Confusion());
        StartEndSav[0]=StartEnd[0];
        StartEndSav[1]=StartEnd[1];
      }
  */
  if (!fonct->EvalCase3(S, *Order, Res))
  {
    *ErrorCode = 3;
  }

  for (i = 0; i <= 6; i++)
    Result[i] = Res(i);
}

Approx_CurvilinearParameter::Approx_CurvilinearParameter(
  const occ::handle<Adaptor2d_Curve2d>& C2D1,
  const occ::handle<Adaptor3d_Surface>& Surf1,
  const occ::handle<Adaptor2d_Curve2d>& C2D2,
  const occ::handle<Adaptor3d_Surface>& Surf2,
  const double                          Tol,
  const GeomAbs_Shape                   Order,
  const int                             MaxDegree,
  const int                             MaxSegments)
{
  int i;

#ifdef OCCT_DEBUG_CHRONO
  t_total = t_init = t_approx = t_uparam = 0;
  uparam_count                           = 0;
  InitChron(chr_total);
#endif
  myCase = 3;

  // Initialisation of input parameters of AdvApprox

  int                                      Num1DSS = 4, Num2DSS = 0, Num3DSS = 1;
  occ::handle<NCollection_HArray1<double>> OneDTol = new NCollection_HArray1<double>(1, Num1DSS);

  double TolV, TolW;
  ToleranceComputation(C2D1, Surf1, 10, Tol, TolV, TolW);
  OneDTol->SetValue(1, TolV);
  OneDTol->SetValue(2, TolW);

  ToleranceComputation(C2D2, Surf2, 10, Tol, TolV, TolW);
  OneDTol->SetValue(3, TolV);
  OneDTol->SetValue(4, TolW);

  occ::handle<NCollection_HArray1<double>> TwoDTolNul;
  occ::handle<NCollection_HArray1<double>> ThreeDTol = new NCollection_HArray1<double>(1, Num3DSS);
  ThreeDTol->Init(Tol / 2);

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_init);
#endif
  occ::handle<Approx_CurvlinFunc> fonct =
    new Approx_CurvlinFunc(C2D1, C2D2, Surf1, Surf2, Tol / 20);
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_init, t_init);
#endif

  double FirstS = fonct->FirstParameter();
  double LastS  = fonct->LastParameter();

  int                        NbInterv_C2 = fonct->NbIntervals(GeomAbs_C2);
  NCollection_Array1<double> CutPnts_C2(1, NbInterv_C2 + 1);
  fonct->Intervals(CutPnts_C2, GeomAbs_C2);
  int                        NbInterv_C3 = fonct->NbIntervals(GeomAbs_C3);
  NCollection_Array1<double> CutPnts_C3(1, NbInterv_C3 + 1);
  fonct->Intervals(CutPnts_C3, GeomAbs_C3);
  AdvApprox_PrefAndRec CutTool(CutPnts_C2, CutPnts_C3);

#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_approx);
#endif

  Approx_CurvilinearParameter_EvalCurvOn2Surf evCOn2S(fonct, FirstS, LastS);
  AdvApprox_ApproxAFunction                   aApprox(Num1DSS,
                                    Num2DSS,
                                    Num3DSS,
                                    OneDTol,
                                    TwoDTolNul,
                                    ThreeDTol,
                                    FirstS,
                                    LastS,
                                    Order,
                                    MaxDegree,
                                    MaxSegments,
                                    evCOn2S,
                                    CutTool);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_approx, t_approx);
#endif

  myDone      = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult)
  {
    int                          NbPoles = aApprox.NbPoles();
    NCollection_Array1<gp_Pnt>   Poles(1, NbPoles);
    NCollection_Array1<gp_Pnt2d> Poles2d(1, NbPoles);
    NCollection_Array1<double>   Poles1d(1, NbPoles);
    aApprox.Poles(1, Poles);
    aApprox.Poles1d(1, Poles1d);
    for (i = 1; i <= NbPoles; i++)
      Poles2d(i).SetX(Poles1d(i));
    aApprox.Poles1d(2, Poles1d);
    for (i = 1; i <= NbPoles; i++)
      Poles2d(i).SetY(Poles1d(i));
    occ::handle<NCollection_HArray1<double>> Knots  = aApprox.Knots();
    occ::handle<NCollection_HArray1<int>>    Mults  = aApprox.Multiplicities();
    int                                      Degree = aApprox.Degree();
    myCurve3d  = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
    myCurve2d1 = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
    aApprox.Poles1d(3, Poles1d);
    for (i = 1; i <= NbPoles; i++)
      Poles2d(i).SetX(Poles1d(i));
    aApprox.Poles1d(4, Poles1d);
    for (i = 1; i <= NbPoles; i++)
      Poles2d(i).SetY(Poles1d(i));
    myCurve2d2 = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);
  }
  myMaxError2d1 = std::max(aApprox.MaxError(1, 1), aApprox.MaxError(1, 2));
  myMaxError2d2 = std::max(aApprox.MaxError(1, 3), aApprox.MaxError(1, 4));
  myMaxError3d  = aApprox.MaxError(3, 1);

#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_total, t_total);

  std::cout << " total reparametrization time = " << t_total << std::endl;
  std::cout << "initialization time = " << t_init << std::endl;
  std::cout << "approximation time = " << t_approx << std::endl;
  std::cout << "total time for uparam computation = " << t_uparam << std::endl;
  std::cout << "number uparam calls = " << uparam_count << std::endl;
#endif
}

//=================================================================================================

bool Approx_CurvilinearParameter::IsDone() const
{
  return myDone;
}

//=================================================================================================

bool Approx_CurvilinearParameter::HasResult() const
{
  return myHasResult;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> Approx_CurvilinearParameter::Curve3d() const
{
  return myCurve3d;
}

//=================================================================================================

double Approx_CurvilinearParameter::MaxError3d() const
{
  return myMaxError3d;
}

//=================================================================================================
// function : Curve2d1
// purpose  : returns the BsplineCurve representing the reparametrized 2D curve on the
//           first surface (case of a curve on one or two surfaces)
//=================================================================================================

occ::handle<Geom2d_BSplineCurve> Approx_CurvilinearParameter::Curve2d1() const
{
  return myCurve2d1;
}

//=================================================================================================

double Approx_CurvilinearParameter::MaxError2d1() const
{
  return myMaxError2d1;
}

//=================================================================================================
// function : Curve2d2
// purpose  : returns the BsplineCurve representing the reparametrized 2D curve on the
//           second surface (case of a curve on two surfaces)
//=================================================================================================

occ::handle<Geom2d_BSplineCurve> Approx_CurvilinearParameter::Curve2d2() const
{
  return myCurve2d2;
}

//=================================================================================================

double Approx_CurvilinearParameter::MaxError2d2() const
{
  return myMaxError2d2;
}

//=================================================================================================
// function : Dump
// purpose  : print the maximum errors(s)
//=================================================================================================

void Approx_CurvilinearParameter::Dump(Standard_OStream& o) const
{
  o << "Dump of Approx_CurvilinearParameter" << std::endl;
  if (myCase == 2 || myCase == 3)
    o << "myMaxError2d1 = " << myMaxError2d1 << std::endl;
  if (myCase == 3)
    o << "myMaxError2d2 = " << myMaxError2d2 << std::endl;
  o << "myMaxError3d = " << myMaxError3d << std::endl;
}

//=================================================================================================

void Approx_CurvilinearParameter::ToleranceComputation(const occ::handle<Adaptor2d_Curve2d>& C2D,
                                                       const occ::handle<Adaptor3d_Surface>& S,
                                                       const int    MaxNumber,
                                                       const double Tol,
                                                       double&      TolV,
                                                       double&      TolW)
{
  double FirstU = C2D->FirstParameter(), LastU = C2D->LastParameter();
  //  double parU, Max_dS_dv=1.,Max_dS_dw=1.;
  double   Max_dS_dv = 1., Max_dS_dw = 1.;
  gp_Pnt   P;
  gp_Pnt2d pntVW;
  gp_Vec   dS_dv, dS_dw;

  for (int i = 1; i <= MaxNumber; i++)
  {
    pntVW = C2D->Value(FirstU + (i - 1) * (LastU - FirstU) / (MaxNumber - 1));
    S->D1(pntVW.X(), pntVW.Y(), P, dS_dv, dS_dw);
    Max_dS_dv = std::max(Max_dS_dv, dS_dv.Magnitude());
    Max_dS_dw = std::max(Max_dS_dw, dS_dw.Magnitude());
  }
  TolV = Tol / (4. * Max_dS_dv);
  TolW = Tol / (4. * Max_dS_dw);

#ifdef OCCT_DEBUG
  std::cout << "TolV = " << TolV << std::endl;
  std::cout << "TolW = " << TolW << std::endl;
#endif
}
