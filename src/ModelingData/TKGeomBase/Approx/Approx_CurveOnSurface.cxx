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
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//=================================================================================================

class Approx_CurveOnSurface_Eval : public AdvApprox_EvaluatorFunction
{
public:
  Approx_CurveOnSurface_Eval(const Handle(Adaptor3d_Curve)&   theFunc,
                             const Handle(Adaptor2d_Curve2d)& theFunc2d,
                             Standard_Real                    First,
                             Standard_Real                    Last)
      : fonct(theFunc),
        fonct2d(theFunc2d)
  {
    StartEndSav[0] = First;
    StartEndSav[1] = Last;
  }

  virtual void Evaluate(Standard_Integer* Dimension,
                        Standard_Real     StartEnd[2],
                        Standard_Real*    Parameter,
                        Standard_Integer* DerivativeRequest,
                        Standard_Real*    Result, // [Dimension]
                        Standard_Integer* ErrorCode);

private:
  Handle(Adaptor3d_Curve)   fonct;
  Handle(Adaptor2d_Curve2d) fonct2d;
  Standard_Real             StartEndSav[2];
};

void Approx_CurveOnSurface_Eval::Evaluate(Standard_Integer* Dimension,
                                          Standard_Real     StartEnd[2],
                                          Standard_Real*    Param,  // Parameter at which evaluation
                                          Standard_Integer* Order,  // Derivative Request
                                          Standard_Real*    Result, // [Dimension]
                                          Standard_Integer* ErrorCode)
{
  *ErrorCode        = 0;
  Standard_Real par = *Param;

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
  Approx_CurveOnSurface_Eval3d(const Handle(Adaptor3d_Curve)& theFunc,
                               Standard_Real                  First,
                               Standard_Real                  Last)
      : fonct(theFunc)
  {
    StartEndSav[0] = First;
    StartEndSav[1] = Last;
  }

  virtual void Evaluate(Standard_Integer* Dimension,
                        Standard_Real     StartEnd[2],
                        Standard_Real*    Parameter,
                        Standard_Integer* DerivativeRequest,
                        Standard_Real*    Result, // [Dimension]
                        Standard_Integer* ErrorCode);

private:
  Handle(Adaptor3d_Curve) fonct;
  Standard_Real           StartEndSav[2];
};

void Approx_CurveOnSurface_Eval3d::Evaluate(Standard_Integer* Dimension,
                                            Standard_Real     StartEnd[2],
                                            Standard_Real* Param, // Parameter at which evaluation
                                            Standard_Integer* Order,  // Derivative Request
                                            Standard_Real*    Result, // [Dimension]
                                            Standard_Integer* ErrorCode)
{
  *ErrorCode        = 0;
  Standard_Real par = *Param;

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
  Approx_CurveOnSurface_Eval2d(const Handle(Adaptor2d_Curve2d)& theFunc2d,
                               Standard_Real                    First,
                               Standard_Real                    Last)
      : fonct2d(theFunc2d)
  {
    StartEndSav[0] = First;
    StartEndSav[1] = Last;
  }

  virtual void Evaluate(Standard_Integer* Dimension,
                        Standard_Real     StartEnd[2],
                        Standard_Real*    Parameter,
                        Standard_Integer* DerivativeRequest,
                        Standard_Real*    Result, // [Dimension]
                        Standard_Integer* ErrorCode);

private:
  Handle(Adaptor2d_Curve2d) fonct2d;
  Standard_Real             StartEndSav[2];
};

void Approx_CurveOnSurface_Eval2d::Evaluate(Standard_Integer* Dimension,
                                            Standard_Real     StartEnd[2],
                                            Standard_Real* Param, // Parameter at which evaluation
                                            Standard_Integer* Order,  // Derivative Request
                                            Standard_Real*    Result, // [Dimension]
                                            Standard_Integer* ErrorCode)
{
  *ErrorCode        = 0;
  Standard_Real par = *Param;

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

Approx_CurveOnSurface::Approx_CurveOnSurface(const Handle(Adaptor2d_Curve2d)& C2D,
                                             const Handle(Adaptor3d_Surface)& Surf,
                                             const Standard_Real              First,
                                             const Standard_Real              Last,
                                             const Standard_Real              Tol,
                                             const GeomAbs_Shape              S,
                                             const Standard_Integer           MaxDegree,
                                             const Standard_Integer           MaxSegments,
                                             const Standard_Boolean           only3d,
                                             const Standard_Boolean           only2d)
    : myC2D(C2D),
      mySurf(Surf),
      myFirst(First),
      myLast(Last),
      myTol(Tol),
      myIsDone(Standard_False),
      myHasResult(Standard_False),
      myError3d(0.0),
      myError2dU(0.0),
      myError2dV(0.0)
{
  Perform(MaxSegments, MaxDegree, S, only3d, only2d);
}

//=================================================================================================

Approx_CurveOnSurface::Approx_CurveOnSurface(const Handle(Adaptor2d_Curve2d)& theC2D,
                                             const Handle(Adaptor3d_Surface)& theSurf,
                                             const Standard_Real              theFirst,
                                             const Standard_Real              theLast,
                                             const Standard_Real              theTol)
    : myC2D(theC2D),
      mySurf(theSurf),
      myFirst(theFirst),
      myLast(theLast),
      myTol(theTol),
      myIsDone(Standard_False),
      myHasResult(Standard_False),
      myError3d(0.0),
      myError2dU(0.0),
      myError2dV(0.0)
{
}

//=================================================================================================

void Approx_CurveOnSurface::Perform(const Standard_Integer theMaxSegments,
                                    const Standard_Integer theMaxDegree,
                                    const GeomAbs_Shape    theContinuity,
                                    const Standard_Boolean theOnly3d,
                                    const Standard_Boolean theOnly2d)
{
  myIsDone    = Standard_False;
  myHasResult = Standard_False;
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

  Handle(Adaptor2d_Curve2d) TrimmedC2D = myC2D->Trim(myFirst, myLast, Precision::PConfusion());

  Standard_Boolean isU, isForward;
  Standard_Real    aParam;
  if (theOnly3d && isIsoLine(TrimmedC2D, isU, aParam, isForward))
  {
    if (buildC3dOnIsoLine(TrimmedC2D, isU, aParam, isForward))
    {
      myIsDone    = Standard_True;
      myHasResult = Standard_True;
      return;
    }
  }

  Handle(Adaptor3d_CurveOnSurface) HCOnS = new Adaptor3d_CurveOnSurface(TrimmedC2D, mySurf);

  Standard_Integer              Num1DSS = 0, Num2DSS = 0, Num3DSS = 0;
  Handle(TColStd_HArray1OfReal) OneDTol;
  Handle(TColStd_HArray1OfReal) TwoDTolNul;
  Handle(TColStd_HArray1OfReal) ThreeDTol;

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
    OneDTol = new TColStd_HArray1OfReal(1, Num1DSS);

    Standard_Real TolU, TolV;

    TolU = mySurf->UResolution(myTol) / 2.;
    TolV = mySurf->VResolution(myTol) / 2.;

    if (mySurf->UContinuity() == GeomAbs_C0)
    {
      if (!Adaptor3d_HSurfaceTool::IsSurfG1(mySurf, Standard_True, Precision::Angular()))
        TolU = Min(1.e-3, 1.e3 * TolU);
      if (!Adaptor3d_HSurfaceTool::IsSurfG1(mySurf, Standard_True, Precision::Confusion()))
        TolU = Min(1.e-3, 1.e2 * TolU);
    }

    if (mySurf->VContinuity() == GeomAbs_C0)
    {
      if (!Adaptor3d_HSurfaceTool::IsSurfG1(mySurf, Standard_False, Precision::Angular()))
        TolV = Min(1.e-3, 1.e3 * TolV);
      if (!Adaptor3d_HSurfaceTool::IsSurfG1(mySurf, Standard_False, Precision::Confusion()))
        TolV = Min(1.e-3, 1.e2 * TolV);
    }

    OneDTol->SetValue(1, TolU);
    OneDTol->SetValue(2, TolV);
  }

  if (!theOnly2d)
  {
    Num3DSS   = 1;
    ThreeDTol = new TColStd_HArray1OfReal(1, Num3DSS);
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
    Standard_Integer     NbInterv_C1 = HCOnS->NbIntervals(GeomAbs_C1);
    TColStd_Array1OfReal CutPnts_C1(1, NbInterv_C1 + 1);
    HCOnS->Intervals(CutPnts_C1, GeomAbs_C1);
    Standard_Integer     NbInterv_C2 = HCOnS->NbIntervals(GeomAbs_C2);
    TColStd_Array1OfReal CutPnts_C2(1, NbInterv_C2 + 1);
    HCOnS->Intervals(CutPnts_C2, GeomAbs_C2);

    CutTool = new AdvApprox_PrefAndRec(CutPnts_C1, CutPnts_C2);
  }
  else
  {
    Standard_Integer     NbInterv_C2 = HCOnS->NbIntervals(GeomAbs_C2);
    TColStd_Array1OfReal CutPnts_C2(1, NbInterv_C2 + 1);
    HCOnS->Intervals(CutPnts_C2, GeomAbs_C2);
    Standard_Integer     NbInterv_C3 = HCOnS->NbIntervals(GeomAbs_C3);
    TColStd_Array1OfReal CutPnts_C3(1, NbInterv_C3 + 1);
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
    Handle(TColStd_HArray1OfReal)    Knots  = aApprox.Knots();
    Handle(TColStd_HArray1OfInteger) Mults  = aApprox.Multiplicities();
    Standard_Integer                 Degree = aApprox.Degree();

    if (!theOnly2d)
    {
      TColgp_Array1OfPnt Poles(1, aApprox.NbPoles());
      aApprox.Poles(1, Poles);
      myCurve3d = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
      myError3d = aApprox.MaxError(3, 1);
    }
    if (!theOnly3d)
    {
      TColgp_Array1OfPnt2d Poles2d(1, aApprox.NbPoles());
      TColStd_Array1OfReal Poles1dU(1, aApprox.NbPoles());
      aApprox.Poles1d(1, Poles1dU);
      TColStd_Array1OfReal Poles1dV(1, aApprox.NbPoles());
      aApprox.Poles1d(2, Poles1dV);
      for (Standard_Integer i = 1; i <= aApprox.NbPoles(); i++)
        Poles2d.SetValue(i, gp_Pnt2d(Poles1dU.Value(i), Poles1dV.Value(i)));
      myCurve2d = new Geom2d_BSplineCurve(Poles2d, Knots->Array1(), Mults->Array1(), Degree);

      myError2dU = aApprox.MaxError(1, 1);
      myError2dV = aApprox.MaxError(1, 2);
    }
  }
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

//=================================================================================================

Standard_Boolean Approx_CurveOnSurface::isIsoLine(const Handle(Adaptor2d_Curve2d)& theC2D,
                                                  Standard_Boolean&                theIsU,
                                                  Standard_Real&                   theParam,
                                                  Standard_Boolean& theIsForward) const
{
  // These variables are used to check line state (vertical or horizontal).
  Standard_Boolean isAppropriateType = Standard_False;
  gp_Pnt2d         aLoc2d;
  gp_Dir2d         aDir2d;

  // Test type.
  const GeomAbs_CurveType aType = theC2D->GetType();
  if (aType == GeomAbs_Line)
  {
    gp_Lin2d aLin2d   = theC2D->Line();
    aLoc2d            = aLin2d.Location();
    aDir2d            = aLin2d.Direction();
    isAppropriateType = Standard_True;
  }
  else if (aType == GeomAbs_BSplineCurve)
  {
    Handle(Geom2d_BSplineCurve) aBSpline2d = theC2D->BSpline();
    if (aBSpline2d->Degree() != 1 || aBSpline2d->NbPoles() != 2)
      return Standard_False; // Not a line or uneven parameterization.

    aLoc2d = aBSpline2d->Pole(1);

    // Vector should be non-degenerated.
    gp_Vec2d aVec2d(aBSpline2d->Pole(1), aBSpline2d->Pole(2));
    if (aVec2d.SquareMagnitude() < Precision::Confusion())
      return Standard_False; // Degenerated spline.
    aDir2d = aVec2d;

    isAppropriateType = Standard_True;
  }
  else if (aType == GeomAbs_BezierCurve)
  {
    Handle(Geom2d_BezierCurve) aBezier2d = theC2D->Bezier();
    if (aBezier2d->Degree() != 1 || aBezier2d->NbPoles() != 2)
      return Standard_False; // Not a line or uneven parameterization.

    aLoc2d = aBezier2d->Pole(1);

    // Vector should be non-degenerated.
    gp_Vec2d aVec2d(aBezier2d->Pole(1), aBezier2d->Pole(2));
    if (aVec2d.SquareMagnitude() < Precision::Confusion())
      return Standard_False; // Degenerated spline.
    aDir2d = aVec2d;

    isAppropriateType = Standard_True;
  }

  if (!isAppropriateType)
    return Standard_False;

  // Check line to be vertical or horizontal.
  if (aDir2d.IsParallel(gp::DX2d(), Precision::Angular()))
  {
    // Horizontal line. V = const.
    theIsU       = Standard_False;
    theParam     = aLoc2d.Y();
    theIsForward = aDir2d.Dot(gp::DX2d()) > 0.0;
    return Standard_True;
  }
  else if (aDir2d.IsParallel(gp::DY2d(), Precision::Angular()))
  {
    // Vertical line. U = const.
    theIsU       = Standard_True;
    theParam     = aLoc2d.X();
    theIsForward = aDir2d.Dot(gp::DY2d()) > 0.0;
    return Standard_True;
  }

  return Standard_False;
}

#include <GeomLib.hxx>

//=================================================================================================

Standard_Boolean Approx_CurveOnSurface::buildC3dOnIsoLine(const Handle(Adaptor2d_Curve2d)& theC2D,
                                                          const Standard_Boolean           theIsU,
                                                          const Standard_Real              theParam,
                                                          const Standard_Boolean theIsForward)
{
  // Convert adapter to the appropriate type.
  Handle(GeomAdaptor_Surface) aGeomAdapter = Handle(GeomAdaptor_Surface)::DownCast(mySurf);
  if (aGeomAdapter.IsNull())
    return Standard_False;

  if (mySurf->GetType() == GeomAbs_Sphere)
    return Standard_False;

  // Extract isoline
  Handle(Geom_Surface) aSurf = aGeomAdapter->Surface();
  Handle(Geom_Curve)   aC3d;

  gp_Pnt2d aF2d = theC2D->Value(theC2D->FirstParameter());
  gp_Pnt2d aL2d = theC2D->Value(theC2D->LastParameter());

  Standard_Boolean isToTrim = Standard_True;
  Standard_Real    U1, U2, V1, V2;
  aSurf->Bounds(U1, U2, V1, V2);

  if (theIsU)
  {
    Standard_Real aV1Param = Min(aF2d.Y(), aL2d.Y());
    Standard_Real aV2Param = Max(aF2d.Y(), aL2d.Y());
    if (aV2Param < V1 - myTol || aV1Param > V2 + myTol)
    {
      return Standard_False;
    }
    else if (Precision::IsInfinite(V1) || Precision::IsInfinite(V2))
    {
      if (Abs(aV2Param - aV1Param) < Precision::PConfusion())
      {
        return Standard_False;
      }
      aSurf    = new Geom_RectangularTrimmedSurface(aSurf, U1, U2, aV1Param, aV2Param);
      isToTrim = Standard_False;
    }
    else
    {
      aV1Param = Max(aV1Param, V1);
      aV2Param = Min(aV2Param, V2);
      if (Abs(aV2Param - aV1Param) < Precision::PConfusion())
      {
        return Standard_False;
      }
    }
    aC3d = aSurf->UIso(theParam);
    if (isToTrim)
      aC3d = new Geom_TrimmedCurve(aC3d, aV1Param, aV2Param);
  }
  else
  {
    Standard_Real aU1Param = Min(aF2d.X(), aL2d.X());
    Standard_Real aU2Param = Max(aF2d.X(), aL2d.X());
    if (aU2Param < U1 - myTol || aU1Param > U2 + myTol)
    {
      return Standard_False;
    }
    else if (Precision::IsInfinite(U1) || Precision::IsInfinite(U2))
    {
      if (Abs(aU2Param - aU1Param) < Precision::PConfusion())
      {
        return Standard_False;
      }
      aSurf    = new Geom_RectangularTrimmedSurface(aSurf, aU1Param, aU2Param, V1, V2);
      isToTrim = Standard_False;
    }
    else
    {
      aU1Param = Max(aU1Param, U1);
      aU2Param = Min(aU2Param, U2);
      if (Abs(aU2Param - aU1Param) < Precision::PConfusion())
      {
        return Standard_False;
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
  TColStd_Array1OfReal aKnots = myCurve3d->Knots();
  BSplCLib::Reparametrize(theC2D->FirstParameter(), theC2D->LastParameter(), aKnots);
  myCurve3d->SetKnots(aKnots);

  // Evaluate error.
  myError3d = 0.0;

  const Standard_Real    aParF  = myFirst;
  const Standard_Real    aParL  = myLast;
  const Standard_Integer aNbPnt = 23;
  for (Standard_Integer anIdx = 0; anIdx <= aNbPnt; ++anIdx)
  {
    const Standard_Real aPar = aParF + ((aParL - aParF) * anIdx) / aNbPnt;

    const gp_Pnt2d aPnt2d = theC2D->Value(aPar);

    const gp_Pnt aPntC3D = myCurve3d->Value(aPar);
    const gp_Pnt aPntC2D = mySurf->Value(aPnt2d.X(), aPnt2d.Y());

    const Standard_Real aSqDeviation = aPntC3D.SquareDistance(aPntC2D);
    myError3d                        = Max(aSqDeviation, myError3d);
  }

  myError3d = Sqrt(myError3d);

  // Target tolerance is not obtained. This situation happens for isolines on the sphere.
  // OCCT is unable to convert it keeping original parameterization, while the geometric
  // form of the result is entirely identical. In that case, it is better to utilize
  // a general-purpose approach.
  if (myError3d > myTol)
    return Standard_False;

  return Standard_True;
}
