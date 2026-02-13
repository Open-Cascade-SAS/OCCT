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

#include <Approx_CurveOnSurface.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Adaptor3d_Surface.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_DichoCutting.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Vec.hxx>

//=================================================================================================

class Approx_CurveOnSurface_Eval : public AdvApprox_EvaluatorFunction
{
public:
  Approx_CurveOnSurface_Eval(const occ::handle<Adaptor3d_Curve>&   theFunc,
                             const occ::handle<Adaptor2d_Curve2d>& theFunc2d,
                             double                                First,
                             double                                Last)
      : fonct(theFunc),
        fonct2d(theFunc2d)
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
  occ::handle<Adaptor3d_Curve>   fonct;
  occ::handle<Adaptor2d_Curve2d> fonct2d;
  double                         StartEndSav[2];
};

void Approx_CurveOnSurface_Eval::Evaluate(int*    Dimension,
                                          double  StartEnd[2],
                                          double* Param,  // Parameter at which evaluation
                                          int*    Order,  // Derivative Request
                                          double* Result, // [Dimension]
                                          int*    ErrorCode)
{
  *ErrorCode = 0;
  double par = *Param;

  // Dimension is incorrect
  if (*Dimension != 5)
  {
    *ErrorCode = 1;
  }

  // Parameter is incorrect
  if (StartEnd[0] != StartEndSav[0] || StartEnd[1] != StartEndSav[1])
  {
    fonct          = fonct->Trim(StartEnd[0], StartEnd[1], Precision::PConfusion());
    fonct2d        = fonct2d->Trim(StartEnd[0], StartEnd[1], Precision::PConfusion());
    StartEndSav[0] = StartEnd[0];
    StartEndSav[1] = StartEnd[1];
  }
  gp_Pnt pnt;

  gp_Pnt2d pnt2d;

  switch (*Order)
  {
    case 0: {
      fonct2d->D0(par, pnt2d);
      fonct->D0(par, pnt);
      Result[0] = pnt2d.X();
      Result[1] = pnt2d.Y();
      Result[2] = pnt.X();
      Result[3] = pnt.Y();
      Result[4] = pnt.Z();
      break;
    }
    case 1: {
      gp_Vec   v1;
      gp_Vec2d v21;
      fonct2d->D1(par, pnt2d, v21);
      fonct->D1(par, pnt, v1);
      Result[0] = v21.X();
      Result[1] = v21.Y();
      Result[2] = v1.X();
      Result[3] = v1.Y();
      Result[4] = v1.Z();
      break;
    }
    case 2: {
      gp_Vec   v1, v2;
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
      *ErrorCode                                                = 3;
      break;
  }
}

//=================================================================================================

class Approx_CurveOnSurface_Eval3d : public AdvApprox_EvaluatorFunction
{
public:
  Approx_CurveOnSurface_Eval3d(const occ::handle<Adaptor3d_Curve>& theFunc,
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

void Approx_CurveOnSurface_Eval3d::Evaluate(int*    Dimension,
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

  // Parameter is incorrect
  if (StartEnd[0] != StartEndSav[0] || StartEnd[1] != StartEndSav[1])
  {
    fonct          = fonct->Trim(StartEnd[0], StartEnd[1], Precision::PConfusion());
    StartEndSav[0] = StartEnd[0];
    StartEndSav[1] = StartEnd[1];
  }

  gp_Pnt pnt;

  switch (*Order)
  {
    case 0:
      pnt       = fonct->Value(par);
      Result[0] = pnt.X();
      Result[1] = pnt.Y();
      Result[2] = pnt.Z();
      break;
    case 1: {
      gp_Vec v1;
      fonct->D1(par, pnt, v1);
      Result[0] = v1.X();
      Result[1] = v1.Y();
      Result[2] = v1.Z();
      break;
    }
    case 2: {
      gp_Vec v1, v2;
      fonct->D2(par, pnt, v1, v2);
      Result[0] = v2.X();
      Result[1] = v2.Y();
      Result[2] = v2.Z();
      break;
    }
    default:
      Result[0] = Result[1] = Result[2] = 0.;
      *ErrorCode                        = 3;
      break;
  }
}

//=================================================================================================

class Approx_CurveOnSurface_Eval2d : public AdvApprox_EvaluatorFunction
{
public:
  Approx_CurveOnSurface_Eval2d(const occ::handle<Adaptor2d_Curve2d>& theFunc2d,
                               double                                First,
                               double                                Last)
      : fonct2d(theFunc2d)
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
  occ::handle<Adaptor2d_Curve2d> fonct2d;
  double                         StartEndSav[2];
};

void Approx_CurveOnSurface_Eval2d::Evaluate(int*    Dimension,
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
  if (StartEnd[0] != StartEndSav[0] || StartEnd[1] != StartEndSav[1])
  {
    fonct2d        = fonct2d->Trim(StartEnd[0], StartEnd[1], Precision::PConfusion());
    StartEndSav[0] = StartEnd[0];
    StartEndSav[1] = StartEnd[1];
  }

  gp_Pnt2d pnt;

  switch (*Order)
  {
    case 0: {
      pnt       = fonct2d->Value(par);
      Result[0] = pnt.X();
      Result[1] = pnt.Y();
      break;
    }
    case 1: {
      gp_Vec2d v1;
      fonct2d->D1(par, pnt, v1);
      Result[0] = v1.X();
      Result[1] = v1.Y();
      break;
    }
    case 2: {
      gp_Vec2d v1, v2;
      fonct2d->D2(par, pnt, v1, v2);
      Result[0] = v2.X();
      Result[1] = v2.Y();
      break;
    }
    default:
      Result[0] = Result[1] = 0.;
      *ErrorCode            = 3;
      break;
  }
}

//=================================================================================================

Approx_CurveOnSurface::Approx_CurveOnSurface(const occ::handle<Adaptor2d_Curve2d>& C2D,
                                             const occ::handle<Adaptor3d_Surface>& Surf,
                                             const double                          First,
                                             const double                          Last,
                                             const double                          Tol,
                                             const GeomAbs_Shape                   S,
                                             const int                             MaxDegree,
                                             const int                             MaxSegments,
                                             const bool                            only3d,
                                             const bool                            only2d)
    : myC2D(C2D),
      mySurf(Surf),
      myFirst(First),
      myLast(Last),
      myTol(Tol),
      myIsDone(false),
      myHasResult(false),
      myError3d(0.0),
      myError2dU(0.0),
      myError2dV(0.0)
{
  Perform(MaxSegments, MaxDegree, S, only3d, only2d);
}

//=================================================================================================

Approx_CurveOnSurface::Approx_CurveOnSurface(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                                             const occ::handle<Adaptor3d_Surface>& theSurf,
                                             const double                          theFirst,
                                             const double                          theLast,
                                             const double                          theTol)
    : myC2D(theC2D),
      mySurf(theSurf),
      myFirst(theFirst),
      myLast(theLast),
      myTol(theTol),
      myIsDone(false),
      myHasResult(false),
      myError3d(0.0),
      myError2dU(0.0),
      myError2dV(0.0)
{
}

//=================================================================================================

void Approx_CurveOnSurface::Perform(const int           theMaxSegments,
                                    const int           theMaxDegree,
                                    const GeomAbs_Shape theContinuity,
                                    const bool          theOnly3d,
                                    const bool          theOnly2d)
{
  myIsDone    = false;
  myHasResult = false;
  myError2dU  = 0.0;
  myError2dV  = 0.0;
  myError3d   = 0.0;

  if (theOnly3d && theOnly2d)
    throw Standard_ConstructionError();

  GeomAbs_Shape aContinuity = theContinuity;
  if (aContinuity == GeomAbs_G1)
    aContinuity = GeomAbs_C1;
  else if (aContinuity == GeomAbs_G2)
    aContinuity = GeomAbs_C2;
  else if (aContinuity > GeomAbs_C2)
    aContinuity = GeomAbs_C2; // Restriction of AdvApprox_ApproxAFunction

  occ::handle<Adaptor2d_Curve2d> TrimmedC2D = myC2D->Trim(myFirst, myLast, Precision::PConfusion());

  bool   isU, isForward;
  double aParam;
  if (theOnly3d && isIsoLine(TrimmedC2D, isU, aParam, isForward))
  {
    if (buildC3dOnIsoLine(TrimmedC2D, isU, aParam, isForward))
    {
      myIsDone    = true;
      myHasResult = true;
      return;
    }
  }

  occ::handle<Adaptor3d_CurveOnSurface> HCOnS = new Adaptor3d_CurveOnSurface(TrimmedC2D, mySurf);

  int                                      Num1DSS = 0, Num2DSS = 0, Num3DSS = 0;
  occ::handle<NCollection_HArray1<double>> OneDTol;
  occ::handle<NCollection_HArray1<double>> TwoDTolNul;
  occ::handle<NCollection_HArray1<double>> ThreeDTol;

  // create evaluators and choose appropriate one
  Approx_CurveOnSurface_Eval3d Eval3dCvOnSurf(HCOnS, myFirst, myLast);
  Approx_CurveOnSurface_Eval2d Eval2dCvOnSurf(TrimmedC2D, myFirst, myLast);
  Approx_CurveOnSurface_Eval   EvalCvOnSurf(HCOnS, TrimmedC2D, myFirst, myLast);
  AdvApprox_EvaluatorFunction* EvalPtr;
  if (theOnly3d)
    EvalPtr = &Eval3dCvOnSurf;
  else if (theOnly2d)
    EvalPtr = &Eval2dCvOnSurf;
  else
    EvalPtr = &EvalCvOnSurf;

  // Initialization for 2d approximation
  if (!theOnly3d)
  {
    Num1DSS = 2;
    OneDTol = new NCollection_HArray1<double>(1, Num1DSS);

    double TolU, TolV;

    TolU = mySurf->UResolution(myTol) / 2.;
    TolV = mySurf->VResolution(myTol) / 2.;

    if (mySurf->UContinuity() == GeomAbs_C0)
    {
      if (!Adaptor3d_HSurfaceTool::IsSurfG1(mySurf, true, Precision::Angular()))
        TolU = std::min(1.e-3, 1.e3 * TolU);
      if (!Adaptor3d_HSurfaceTool::IsSurfG1(mySurf, true, Precision::Confusion()))
        TolU = std::min(1.e-3, 1.e2 * TolU);
    }

    if (mySurf->VContinuity() == GeomAbs_C0)
    {
      if (!Adaptor3d_HSurfaceTool::IsSurfG1(mySurf, false, Precision::Angular()))
        TolV = std::min(1.e-3, 1.e3 * TolV);
      if (!Adaptor3d_HSurfaceTool::IsSurfG1(mySurf, false, Precision::Confusion()))
        TolV = std::min(1.e-3, 1.e2 * TolV);
    }

    OneDTol->SetValue(1, TolU);
    OneDTol->SetValue(2, TolV);
  }

  if (!theOnly2d)
  {
    Num3DSS   = 1;
    ThreeDTol = new NCollection_HArray1<double>(1, Num3DSS);
    ThreeDTol->Init(myTol / 2);
  }

  AdvApprox_Cutting* CutTool;

  if (aContinuity <= myC2D->Continuity() && aContinuity <= mySurf->UContinuity()
      && aContinuity <= mySurf->VContinuity())
  {
    CutTool = new AdvApprox_DichoCutting();
  }
  else if (aContinuity == GeomAbs_C1)
  {
    int                        NbInterv_C1 = HCOnS->NbIntervals(GeomAbs_C1);
    NCollection_Array1<double> CutPnts_C1(1, NbInterv_C1 + 1);
    HCOnS->Intervals(CutPnts_C1, GeomAbs_C1);
    int                        NbInterv_C2 = HCOnS->NbIntervals(GeomAbs_C2);
    NCollection_Array1<double> CutPnts_C2(1, NbInterv_C2 + 1);
    HCOnS->Intervals(CutPnts_C2, GeomAbs_C2);

    CutTool = new AdvApprox_PrefAndRec(CutPnts_C1, CutPnts_C2);
  }
  else
  {
    int                        NbInterv_C2 = HCOnS->NbIntervals(GeomAbs_C2);
    NCollection_Array1<double> CutPnts_C2(1, NbInterv_C2 + 1);
    HCOnS->Intervals(CutPnts_C2, GeomAbs_C2);
    int                        NbInterv_C3 = HCOnS->NbIntervals(GeomAbs_C3);
    NCollection_Array1<double> CutPnts_C3(1, NbInterv_C3 + 1);
    HCOnS->Intervals(CutPnts_C3, GeomAbs_C3);

    CutTool = new AdvApprox_PrefAndRec(CutPnts_C2, CutPnts_C3);
  }

  AdvApprox_ApproxAFunction aApprox(Num1DSS,
                                    Num2DSS,
                                    Num3DSS,
                                    OneDTol,
                                    TwoDTolNul,
                                    ThreeDTol,
                                    myFirst,
                                    myLast,
                                    aContinuity,
                                    theMaxDegree,
                                    theMaxSegments,
                                    *EvalPtr,
                                    *CutTool);

  delete CutTool;

  myIsDone    = aApprox.IsDone();
  myHasResult = aApprox.HasResult();

  if (myHasResult)
  {
    occ::handle<NCollection_HArray1<double>> Knots  = aApprox.Knots();
    occ::handle<NCollection_HArray1<int>>    Mults  = aApprox.Multiplicities();
    int                                      Degree = aApprox.Degree();

    const int aNbPoles = aApprox.NbPoles();
    if (!theOnly2d)
    {
      NCollection_Array1<gp_Pnt> Poles(1, aNbPoles);
      aApprox.Poles(1, Poles);
      myCurve3d = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
      myError3d = aApprox.MaxError(3, 1);
    }
    if (!theOnly3d)
    {
      NCollection_Array1<gp_Pnt2d> Poles2d(1, aNbPoles);
      NCollection_Array1<double>   Poles1dU(1, aNbPoles);
      aApprox.Poles1d(1, Poles1dU);
      NCollection_Array1<double> Poles1dV(1, aNbPoles);
      aApprox.Poles1d(2, Poles1dV);
      for (int i = 1; i <= aNbPoles; i++)
        Poles2d.SetValue(i, gp_Pnt2d(Poles1dU.Value(i), Poles1dV.Value(i)));
      myCurve2d = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);

      myError2dU = aApprox.MaxError(1, 1);
      myError2dV = aApprox.MaxError(1, 2);
    }
  }
}

bool Approx_CurveOnSurface::IsDone() const
{
  return myIsDone;
}

bool Approx_CurveOnSurface::HasResult() const
{
  return myHasResult;
}

occ::handle<Geom_BSplineCurve> Approx_CurveOnSurface::Curve3d() const
{
  return myCurve3d;
}

occ::handle<Geom2d_BSplineCurve> Approx_CurveOnSurface::Curve2d() const
{
  return myCurve2d;
}

double Approx_CurveOnSurface::MaxError3d() const
{
  return myError3d;
}

double Approx_CurveOnSurface::MaxError2dU() const
{
  return myError2dU;
}

double Approx_CurveOnSurface::MaxError2dV() const
{
  return myError2dV;
}

//=================================================================================================

bool Approx_CurveOnSurface::isIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                                      bool&                                 theIsU,
                                      double&                               theParam,
                                      bool&                                 theIsForward) const
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
      return false; // Not a line or uneven parameterization.

    aLoc2d = aBSpline2d->Pole(1);

    // Vector should be non-degenerated.
    gp_Vec2d aVec2d(aBSpline2d->Pole(1), aBSpline2d->Pole(2));
    if (aVec2d.SquareMagnitude() < Precision::Confusion())
      return false; // Degenerated spline.
    aDir2d = aVec2d;

    isAppropriateType = true;
  }
  else if (aType == GeomAbs_BezierCurve)
  {
    occ::handle<Geom2d_BezierCurve> aBezier2d = theC2D->Bezier();
    if (aBezier2d->Degree() != 1 || aBezier2d->NbPoles() != 2)
      return false; // Not a line or uneven parameterization.

    aLoc2d = aBezier2d->Pole(1);

    // Vector should be non-degenerated.
    gp_Vec2d aVec2d(aBezier2d->Pole(1), aBezier2d->Pole(2));
    if (aVec2d.SquareMagnitude() < Precision::Confusion())
      return false; // Degenerated spline.
    aDir2d = aVec2d;

    isAppropriateType = true;
  }

  if (!isAppropriateType)
    return false;

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

#include <GeomLib.hxx>

//=================================================================================================

bool Approx_CurveOnSurface::buildC3dOnIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                                              const bool                            theIsU,
                                              const double                          theParam,
                                              const bool                            theIsForward)
{
  // Convert adapter to the appropriate type.
  occ::handle<GeomAdaptor_Surface> aGeomAdapter = occ::down_cast<GeomAdaptor_Surface>(mySurf);
  if (aGeomAdapter.IsNull())
    return false;

  if (mySurf->GetType() == GeomAbs_Sphere)
    return false;

  // Extract isoline
  occ::handle<Geom_Surface> aSurf = aGeomAdapter->Surface();
  occ::handle<Geom_Curve>   aC3d;

  gp_Pnt2d aF2d = theC2D->Value(theC2D->FirstParameter());
  gp_Pnt2d aL2d = theC2D->Value(theC2D->LastParameter());

  bool   isToTrim = true;
  double U1, U2, V1, V2;
  aSurf->Bounds(U1, U2, V1, V2);

  if (theIsU)
  {
    double aV1Param = std::min(aF2d.Y(), aL2d.Y());
    double aV2Param = std::max(aF2d.Y(), aL2d.Y());
    if (aV2Param < V1 - myTol || aV1Param > V2 + myTol)
    {
      return false;
    }
    else if (Precision::IsInfinite(V1) || Precision::IsInfinite(V2))
    {
      if (std::abs(aV2Param - aV1Param) < Precision::PConfusion())
      {
        return false;
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
        return false;
      }
    }
    aC3d = aSurf->UIso(theParam);
    if (isToTrim)
      aC3d = new Geom_TrimmedCurve(aC3d, aV1Param, aV2Param);
  }
  else
  {
    double aU1Param = std::min(aF2d.X(), aL2d.X());
    double aU2Param = std::max(aF2d.X(), aL2d.X());
    if (aU2Param < U1 - myTol || aU1Param > U2 + myTol)
    {
      return false;
    }
    else if (Precision::IsInfinite(U1) || Precision::IsInfinite(U2))
    {
      if (std::abs(aU2Param - aU1Param) < Precision::PConfusion())
      {
        return false;
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
        return false;
      }
    }
    aC3d = aSurf->VIso(theParam);
    if (isToTrim)
      aC3d = new Geom_TrimmedCurve(aC3d, aU1Param, aU2Param);
  }

  // Convert arbitrary curve type to the b-spline.
  myCurve3d = GeomConvert::CurveToBSplineCurve(aC3d, Convert_QuasiAngular);
  if (!theIsForward)
    myCurve3d->Reverse();

  // Rebuild parameterization for the 3d curve to have the same parameterization with
  // a two-dimensional curve.
  NCollection_Array1<double> aKnots = myCurve3d->Knots();
  BSplCLib::Reparametrize(theC2D->FirstParameter(), theC2D->LastParameter(), aKnots);
  myCurve3d->SetKnots(aKnots);

  // Evaluate error.
  myError3d = 0.0;

  const double aParF  = myFirst;
  const double aParL  = myLast;
  const int    aNbPnt = 23;
  for (int anIdx = 0; anIdx <= aNbPnt; ++anIdx)
  {
    const double aPar = aParF + ((aParL - aParF) * anIdx) / aNbPnt;

    const gp_Pnt2d aPnt2d = theC2D->Value(aPar);

    const gp_Pnt aPntC3D = myCurve3d->Value(aPar);
    const gp_Pnt aPntC2D = mySurf->Value(aPnt2d.X(), aPnt2d.Y());

    const double aSqDeviation = aPntC3D.SquareDistance(aPntC2D);
    myError3d                 = std::max(aSqDeviation, myError3d);
  }

  myError3d = std::sqrt(myError3d);

  // Target tolerance is not obtained. This situation happens for isolines on the sphere.
  // OCCT is unable to convert it keeping original parameterization, while the geometric
  // form of the result is entirely identical. In that case, it is better to utilize
  // a general-purpose approach.
  return myError3d <= myTol;
}
