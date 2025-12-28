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
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=======================================================================
// class : HelixGeom_Tools_Eval
// purpose: evaluator class for approximation
//=======================================================================
class HelixGeom_Tools_Eval : public AdvApprox_EvaluatorFunction
{
public:
  HelixGeom_Tools_Eval(const occ::handle<Adaptor3d_Curve>& theFunc)
      : fonct(theFunc)
  {
  }

  void Evaluate(int*    Dimension,
                        double  StartEnd[2],
                        double* Parameter,
                        int*    DerivativeRequest,
                        double* Result, // [Dimension]
                        int*    ErrorCode) override;

private:
  occ::handle<Adaptor3d_Curve> fonct;
};

void HelixGeom_Tools_Eval::Evaluate(int* Dimension,
                                    double /*StartEnd*/[2],
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

int HelixGeom_Tools::ApprCurve3D(const occ::handle<Adaptor3d_Curve>& theHC,
                                 const double                        theTol,
                                 const GeomAbs_Shape                 theCont,
                                 const int                           theMaxSeg,
                                 const int                           theMaxDeg,
                                 occ::handle<Geom_BSplineCurve>&     theBSpl,
                                 double&                             theMaxError)
{
  bool                                     anIsDone, aHasResult;
  int                                      Num1DSS, Num2DSS, Num3DSS;
  double                                   First, Last;
  occ::handle<NCollection_HArray1<double>> OneDTolNul, TwoDTolNul, ThreeDTol;
  AdvApprox_DichoCutting                   aCutTool;
  // Setup approximation dimensions and tolerances
  Num1DSS   = 0;
  Num2DSS   = 0;
  Num3DSS   = 1;
  ThreeDTol = new NCollection_HArray1<double>(1, Num3DSS);
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
  NCollection_Array1<gp_Pnt> Poles(1, aApprox.NbPoles());
  aApprox.Poles(1, Poles);
  occ::handle<NCollection_HArray1<double>> Knots  = aApprox.Knots();
  occ::handle<NCollection_HArray1<int>>    Mults  = aApprox.Multiplicities();
  int                                      Degree = aApprox.Degree();
  theBSpl     = new Geom_BSplineCurve(Poles, Knots->Array1(), Mults->Array1(), Degree);
  theMaxError = aApprox.MaxError(3, 1);
  // Return success
  return 0;
}

//=================================================================================================

int HelixGeom_Tools::ApprHelix(const double                    aT1,
                               const double                    aT2,
                               const double                    aPitch,
                               const double                    aRStart,
                               const double                    aTaperAngle,
                               const bool                      aIsCW,
                               const double                    theTol,
                               occ::handle<Geom_BSplineCurve>& theBSpl,
                               double&                         theMaxError)
{
  int                               iErr, aMaxDegree, aMaxSeg;
  GeomAbs_Shape                     aCont;
  HelixGeom_HelixCurve              aAdaptor;
  occ::handle<HelixGeom_HelixCurve> aHAdaptor;
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
