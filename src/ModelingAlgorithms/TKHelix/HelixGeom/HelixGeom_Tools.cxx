// Copyright (c) 2025 OPEN CASCADE SAS
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
#include <AdvApprox_DichoCutting.hxx>
#include <AdvApprox_EvaluatorFunction.hxx>
#include <Geom_BSplineCurve.hxx>
#include <HelixGeom_HelixCurve.hxx>
#include <HelixGeom_Tools.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
// class : HelixGeom_Tools_Eval
// purpose: evaluator class for approximation
//=======================================================================
class HelixGeom_Tools_Eval : public AdvApprox_EvaluatorFunction
{
public:
  HelixGeom_Tools_Eval(const Handle(Adaptor3d_Curve)& theFunc)
      : fonct(theFunc)
  {
  }

  virtual void Evaluate(Standard_Integer* Dimension,
                        Standard_Real     StartEnd[2],
                        Standard_Real*    Parameter,
                        Standard_Integer* DerivativeRequest,
                        Standard_Real*    Result, // [Dimension]
                        Standard_Integer* ErrorCode);

private:
  Handle(Adaptor3d_Curve) fonct;
};

void HelixGeom_Tools_Eval::Evaluate(Standard_Integer* Dimension,
                                    Standard_Real /*StartEnd*/[2],
                                    Standard_Real*    Param,  // Parameter at which evaluation
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

//=================================================================================================

Standard_Integer HelixGeom_Tools::ApprCurve3D(const Handle(Adaptor3d_Curve)& theHC,
                                              const Standard_Real            theTol,
                                              const GeomAbs_Shape            theCont,
                                              const Standard_Integer         theMaxSeg,
                                              const Standard_Integer         theMaxDeg,
                                              Handle(Geom_BSplineCurve)&     theBSpl,
                                              Standard_Real&                 theMaxError)
{
  Standard_Boolean              anIsDone, aHasResult;
  Standard_Integer              Num1DSS, Num2DSS, Num3DSS;
  Standard_Real                 First, Last;
  Handle(TColStd_HArray1OfReal) OneDTolNul, TwoDTolNul, ThreeDTol;
  AdvApprox_DichoCutting        aCutTool;
  // Setup approximation dimensions and tolerances
  Num1DSS   = 0;
  Num2DSS   = 0;
  Num3DSS   = 1;
  ThreeDTol = new TColStd_HArray1OfReal(1, Num3DSS);
  ThreeDTol->Init(theTol);
  // Get curve parameter range
  First = theHC->FirstParameter();
  Last  = theHC->LastParameter();
  // Setup approximation function and perform approximation
  HelixGeom_Tools_Eval      ev(theHC);
  AdvApprox_ApproxAFunction aApprox(Num1DSS,
                                    Num2DSS,
                                    Num3DSS,
                                    OneDTolNul,
                                    TwoDTolNul,
                                    ThreeDTol,
                                    First,
                                    Last,
                                    theCont,
                                    theMaxDeg,
                                    theMaxSeg,
                                    ev,
                                    aCutTool);
  // Check if approximation was successful
  anIsDone = aApprox.IsDone();
  if (!anIsDone)
  {
    return 1;
  }
  // Initialize error and check for results
  theMaxError = 0.;
  // Verify approximation has results
  aHasResult = aApprox.HasResult();
  if (!aHasResult)
  {
    return 2;
  }
  // Extract B-spline curve data from approximation
  TColgp_Array1OfPnt Poles(1, aApprox.NbPoles());
  aApprox.Poles(1, Poles);
  Handle(TColStd_HArray1OfReal)    Knots  = aApprox.Knots();
  Handle(TColStd_HArray1OfInteger) Mults  = aApprox.Multiplicities();
  Standard_Integer                 Degree = aApprox.Degree();
  theBSpl     = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
  theMaxError = aApprox.MaxError(3, 1);
  // Return success
  return 0;
}

//=================================================================================================

Standard_Integer HelixGeom_Tools::ApprHelix(const Standard_Real        aT1,
                                            const Standard_Real        aT2,
                                            const Standard_Real        aPitch,
                                            const Standard_Real        aRStart,
                                            const Standard_Real        aTaperAngle,
                                            const Standard_Boolean     aIsCW,
                                            const Standard_Real        theTol,
                                            Handle(Geom_BSplineCurve)& theBSpl,
                                            Standard_Real&             theMaxError)
{
  Standard_Integer             iErr, aMaxDegree, aMaxSeg;
  GeomAbs_Shape                aCont;
  HelixGeom_HelixCurve         aAdaptor;
  Handle(HelixGeom_HelixCurve) aHAdaptor;
  // Load helix parameters and create adaptor handle
  aAdaptor.Load(aT1, aT2, aPitch, aRStart, aTaperAngle, aIsCW);
  aHAdaptor = new HelixGeom_HelixCurve(aAdaptor);
  // Set default approximation parameters
  aCont      = GeomAbs_C2;
  aMaxDegree = 8;
  aMaxSeg    = 150;
  // Perform curve approximation
  iErr = HelixGeom_Tools::ApprCurve3D(aHAdaptor,
                                      theTol,
                                      aCont,
                                      aMaxSeg,
                                      aMaxDegree,
                                      theBSpl,
                                      theMaxError);
  return iErr;
}
