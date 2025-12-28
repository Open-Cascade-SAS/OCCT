// Created on: 1991-12-02
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _AppDef_Compute_HeaderFile
#define _AppDef_Compute_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_MultiCurve.hxx>
#include <NCollection_Sequence.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <Approx_ParametrizationType.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <AppParCurves_ConstraintCouple.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_Constraint.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array1.hxx>
class AppDef_MultiLine;
class AppDef_MyLineTool;
class AppDef_MyGradientOfCompute;
class AppDef_ParLeastSquareOfMyGradientOfCompute;
class AppDef_ResConstraintOfMyGradientOfCompute;
class AppDef_ParFunctionOfMyGradientOfCompute;
class AppDef_Gradient_BFGSOfMyGradientOfCompute;
class AppParCurves_MultiCurve;
class AppParCurves_MultiBSpCurve;

class AppDef_Compute
{
public:
  DEFINE_STANDARD_ALLOC

  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  //! If <Squares> is True, the computation will be done with
  //! no iteration at all.
  Standard_EXPORT AppDef_Compute(
    const AppDef_MultiLine&          Line,
    const int           degreemin       = 4,
    const int           degreemax       = 8,
    const double              Tolerance3d     = 1.0e-3,
    const double              Tolerance2d     = 1.0e-6,
    const int           NbIterations    = 5,
    const bool           cutting         = true,
    const Approx_ParametrizationType parametrization = Approx_ChordLength,
    const bool           Squares         = false);

  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  //! If <Squares> is True, the computation will be done with
  //! no iteration at all.
  Standard_EXPORT AppDef_Compute(const AppDef_MultiLine& Line,
                                 const math_Vector&      Parameters,
                                 const int  degreemin    = 4,
                                 const int  degreemax    = 8,
                                 const double     Tolerance3d  = 1.0e-03,
                                 const double     Tolerance2d  = 1.0e-06,
                                 const int  NbIterations = 5,
                                 const bool  cutting      = true,
                                 const bool  Squares      = false);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT AppDef_Compute(const math_Vector&     Parameters,
                                 const int degreemin    = 4,
                                 const int degreemax    = 8,
                                 const double    Tolerance3d  = 1.0e-03,
                                 const double    Tolerance2d  = 1.0e-06,
                                 const int NbIterations = 5,
                                 const bool cutting      = true,
                                 const bool Squares      = false);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT AppDef_Compute(
    const int           degreemin       = 4,
    const int           degreemax       = 8,
    const double              Tolerance3d     = 1.0e-03,
    const double              Tolerance2d     = 1.0e-06,
    const int           NbIterations    = 5,
    const bool           cutting         = true,
    const Approx_ParametrizationType parametrization = Approx_ChordLength,
    const bool           Squares         = false);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Init(const int           degreemin       = 4,
                            const int           degreemax       = 8,
                            const double              Tolerance3d     = 1.0e-03,
                            const double              Tolerance2d     = 1.0e-06,
                            const int           NbIterations    = 5,
                            const bool           cutting         = true,
                            const Approx_ParametrizationType parametrization = Approx_ChordLength,
                            const bool           Squares         = false);

  //! runs the algorithm after having initialized the fields.
  Standard_EXPORT void Perform(const AppDef_MultiLine& Line);

  //! changes the degrees of the approximation.
  Standard_EXPORT void SetDegrees(const int degreemin,
                                  const int degreemax);

  //! Changes the tolerances of the approximation.
  Standard_EXPORT void SetTolerances(const double Tolerance3d,
                                     const double Tolerance2d);

  //! changes the first and the last constraint points.
  Standard_EXPORT void SetConstraints(const AppParCurves_Constraint firstC,
                                      const AppParCurves_Constraint lastC);

  //! returns False if at a moment of the approximation,
  //! the status NoApproximation has been sent by the user
  //! when more points were needed.
  Standard_EXPORT bool IsAllApproximated() const;

  //! returns False if the status NoPointsAdded has been sent.
  Standard_EXPORT bool IsToleranceReached() const;

  //! returns the tolerances 2d and 3d of the <Index> MultiCurve.
  Standard_EXPORT void Error(const int Index,
                             double&         tol3d,
                             double&         tol2d) const;

  //! Returns the number of MultiCurve doing the approximation
  //! of the MultiLine.
  Standard_EXPORT int NbMultiCurves() const;

  //! returns the result of the approximation.
  Standard_EXPORT const AppParCurves_MultiCurve& Value(const int Index = 1) const;

  //! returns the result of the approximation.
  Standard_EXPORT AppParCurves_MultiCurve& ChangeValue(const int Index = 1);

  //! returns the result of the approximation.
  Standard_EXPORT const AppParCurves_MultiBSpCurve& SplineValue();

  //! returns the type of parametrization
  Standard_EXPORT Approx_ParametrizationType Parametrization() const;

  //! returns the new parameters of the approximation
  //! corresponding to the points of the multicurve <Index>.
  Standard_EXPORT const NCollection_Array1<double>& Parameters(const int Index = 1) const;

private:
  //! is internally used in the algorithm.
  Standard_EXPORT bool Compute(const AppDef_MultiLine& Line,
                                           const int  fpt,
                                           const int  lpt,
                                           math_Vector&            Para,
                                           double&          TheTol3d,
                                           double&          TheTol2d,
                                           int&       indbad);

  //! is internally used in the algorithm.
  Standard_EXPORT bool ComputeCurve(const AppDef_MultiLine& Line,
                                                const int  firspt,
                                                const int  lastpt);

  //! computes new parameters between firstP and lastP.
  Standard_EXPORT void Parameters(const AppDef_MultiLine& Line,
                                  const int  firstP,
                                  const int  LastP,
                                  math_Vector&            TheParameters) const;

  Standard_EXPORT double SearchFirstLambda(const AppDef_MultiLine& Line,
                                                  const math_Vector&      Para,
                                                  const math_Vector&      V,
                                                  const int  index) const;

  Standard_EXPORT double SearchLastLambda(const AppDef_MultiLine& Line,
                                                 const math_Vector&      Para,
                                                 const math_Vector&      V,
                                                 const int  index) const;

  Standard_EXPORT void FirstTangencyVector(const AppDef_MultiLine& Line,
                                           const int  index,
                                           math_Vector&            V) const;

  Standard_EXPORT void LastTangencyVector(const AppDef_MultiLine& Line,
                                          const int  index,
                                          math_Vector&            V) const;

  NCollection_Sequence<AppParCurves_MultiCurve>              myMultiCurves;
  AppParCurves_MultiCurve                        TheMultiCurve;
  AppParCurves_MultiBSpCurve                     myspline;
  bool                               alldone;
  bool                               tolreached;
  Approx_ParametrizationType                     Par;
  occ::handle<NCollection_HArray1<double>>                  myParameters;
  occ::handle<NCollection_HArray1<double>>                  myfirstParam;
  NCollection_Sequence<occ::handle<NCollection_HArray1<double>>>                 myPar;
  NCollection_Sequence<double>                         Tolers3d;
  NCollection_Sequence<double>                         Tolers2d;
  occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>> myConstraints;
  int                               mydegremin;
  int                               mydegremax;
  double                                  mytol3d;
  double                                  mytol2d;
  double                                  currenttol3d;
  double                                  currenttol2d;
  bool                               mycut;
  bool                               mysquares;
  int                               myitermax;
  AppParCurves_Constraint                        myfirstC;
  AppParCurves_Constraint                        mylastC;
  int                               myMultiLineNb;
  bool                               myIsClear;
};

#endif // _AppDef_Compute_HeaderFile
