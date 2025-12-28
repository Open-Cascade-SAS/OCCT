// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomInt_TheComputeLineOfWLApprox_HeaderFile
#define _GeomInt_TheComputeLineOfWLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_MultiBSpCurve.hxx>
#include <Approx_ParametrizationType.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_ConstraintCouple.hxx>
#include <AppParCurves_Constraint.hxx>
#include <math_Vector.hxx>
class GeomInt_TheMultiLineOfWLApprox;
class GeomInt_TheMultiLineToolOfWLApprox;
class GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox;
class GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox;
class GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox;
class GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox;
class GeomInt_MyGradientbisOfTheComputeLineOfWLApprox;
class GeomInt_ParLeastSquareOfMyGradientbisOfTheComputeLineOfWLApprox;
class GeomInt_ResConstraintOfMyGradientbisOfTheComputeLineOfWLApprox;
class GeomInt_ParFunctionOfMyGradientbisOfTheComputeLineOfWLApprox;
class GeomInt_Gradient_BFGSOfMyGradientbisOfTheComputeLineOfWLApprox;
class AppParCurves_MultiBSpCurve;
class AppParCurves_MultiCurve;

class GeomInt_TheComputeLineOfWLApprox
{
public:
  DEFINE_STANDARD_ALLOC

  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  //! If <Squares> is True, the computation will be done with
  //! no iteration at all.
  //!
  //! The multiplicities of the internal knots is set by
  //! default.
  Standard_EXPORT GeomInt_TheComputeLineOfWLApprox(
    const GeomInt_TheMultiLineOfWLApprox& Line,
    const int                             degreemin       = 4,
    const int                             degreemax       = 8,
    const double                          Tolerance3d     = 1.0e-3,
    const double                          Tolerance2d     = 1.0e-6,
    const int                             NbIterations    = 5,
    const bool                            cutting         = true,
    const Approx_ParametrizationType      parametrization = Approx_ChordLength,
    const bool                            Squares         = false);

  //! The MultiLine <Line> will be approximated until tolerances
  //! will be reached.
  //! The approximation will be done from degreemin to degreemax
  //! with a cutting if the corresponding boolean is True.
  //! If <Squares> is True, the computation will be done with
  //! no iteration at all.
  Standard_EXPORT GeomInt_TheComputeLineOfWLApprox(const GeomInt_TheMultiLineOfWLApprox& Line,
                                                   const math_Vector&                    Parameters,
                                                   const int    degreemin    = 4,
                                                   const int    degreemax    = 8,
                                                   const double Tolerance3d  = 1.0e-03,
                                                   const double Tolerance2d  = 1.0e-06,
                                                   const int    NbIterations = 5,
                                                   const bool   cutting      = true,
                                                   const bool   Squares      = false);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT GeomInt_TheComputeLineOfWLApprox(const math_Vector& Parameters,
                                                   const int          degreemin    = 4,
                                                   const int          degreemax    = 8,
                                                   const double       Tolerance3d  = 1.0e-03,
                                                   const double       Tolerance2d  = 1.0e-06,
                                                   const int          NbIterations = 5,
                                                   const bool         cutting      = true,
                                                   const bool         Squares      = false);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT GeomInt_TheComputeLineOfWLApprox(
    const int                        degreemin       = 4,
    const int                        degreemax       = 8,
    const double                     Tolerance3d     = 1.0e-03,
    const double                     Tolerance2d     = 1.0e-06,
    const int                        NbIterations    = 5,
    const bool                       cutting         = true,
    const Approx_ParametrizationType parametrization = Approx_ChordLength,
    const bool                       Squares         = false);

  //! Constructs an interpolation of the MultiLine <Line>
  //! The result will be a C2 curve of degree 3.
  Standard_EXPORT void Interpol(const GeomInt_TheMultiLineOfWLApprox& Line);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Init(const int                        degreemin       = 4,
                            const int                        degreemax       = 8,
                            const double                     Tolerance3d     = 1.0e-03,
                            const double                     Tolerance2d     = 1.0e-06,
                            const int                        NbIterations    = 5,
                            const bool                       cutting         = true,
                            const Approx_ParametrizationType parametrization = Approx_ChordLength,
                            const bool                       Squares         = false);

  //! runs the algorithm after having initialized the fields.
  Standard_EXPORT void Perform(const GeomInt_TheMultiLineOfWLApprox& Line);

  //! The approximation will begin with the
  //! set of parameters <ThePar>.
  Standard_EXPORT void SetParameters(const math_Vector& ThePar);

  //! The approximation will be done with the
  //! set of knots <Knots>. The multiplicities will be set
  //! with the degree and the desired continuity.
  Standard_EXPORT void SetKnots(const NCollection_Array1<double>& Knots);

  //! The approximation will be done with the
  //! set of knots <Knots> and the multiplicities <Mults>.
  Standard_EXPORT void SetKnotsAndMultiplicities(const NCollection_Array1<double>& Knots,
                                                 const NCollection_Array1<int>&    Mults);

  //! changes the degrees of the approximation.
  Standard_EXPORT void SetDegrees(const int degreemin, const int degreemax);

  //! Changes the tolerances of the approximation.
  Standard_EXPORT void SetTolerances(const double Tolerance3d, const double Tolerance2d);

  //! sets the continuity of the spline.
  //! if C = 2, the spline will be C2.
  Standard_EXPORT void SetContinuity(const int C);

  //! changes the first and the last constraint points.
  Standard_EXPORT void SetConstraints(const AppParCurves_Constraint firstC,
                                      const AppParCurves_Constraint lastC);

  //! Sets periodic flag.
  //! If thePeriodic = true, algorithm tries to build periodic
  //! multicurve using corresponding C1 boundary condition for first and last multipoints.
  //! Multiline must be closed.
  Standard_EXPORT void SetPeriodic(const bool thePeriodic);

  //! returns False if at a moment of the approximation,
  //! the status NoApproximation has been sent by the user
  //! when more points were needed.
  Standard_EXPORT bool IsAllApproximated() const;

  //! returns False if the status NoPointsAdded has been sent.
  Standard_EXPORT bool IsToleranceReached() const;

  //! returns the tolerances 2d and 3d of the MultiBSpCurve.
  Standard_EXPORT void Error(double& tol3d, double& tol2d) const;

  //! returns the result of the approximation.
  Standard_EXPORT const AppParCurves_MultiBSpCurve& Value() const;

  //! returns the result of the approximation.
  Standard_EXPORT AppParCurves_MultiBSpCurve& ChangeValue();

  //! returns the new parameters of the approximation
  //! corresponding to the points of the MultiBSpCurve.
  Standard_EXPORT const NCollection_Array1<double>& Parameters() const;

private:
  //! is internally used in the algorithm.
  Standard_EXPORT bool Compute(const GeomInt_TheMultiLineOfWLApprox& Line,
                               const int                             fpt,
                               const int                             lpt,
                               math_Vector&                          Para,
                               const NCollection_Array1<double>&     Knots,
                               NCollection_Array1<int>&              Mults);

  //! is internally used in the algorithm.
  Standard_EXPORT bool ComputeCurve(const GeomInt_TheMultiLineOfWLApprox& Line,
                                    const int                             firspt,
                                    const int                             lastpt);

  //! computes new parameters between firstP and lastP.
  Standard_EXPORT void Parameters(const GeomInt_TheMultiLineOfWLApprox& Line,
                                  const int                             firstP,
                                  const int                             LastP,
                                  math_Vector&                          TheParameters) const;

  Standard_EXPORT double SearchFirstLambda(const GeomInt_TheMultiLineOfWLApprox& Line,
                                           const math_Vector&                    Para,
                                           const NCollection_Array1<double>&     Knots,
                                           const math_Vector&                    V,
                                           const int                             index) const;

  Standard_EXPORT double SearchLastLambda(const GeomInt_TheMultiLineOfWLApprox& Line,
                                          const math_Vector&                    Para,
                                          const NCollection_Array1<double>&     Knots,
                                          const math_Vector&                    V,
                                          const int                             index) const;

  Standard_EXPORT void TangencyVector(const GeomInt_TheMultiLineOfWLApprox& Line,
                                      const AppParCurves_MultiCurve&        C,
                                      const double                          U,
                                      math_Vector&                          V) const;

  Standard_EXPORT void FirstTangencyVector(const GeomInt_TheMultiLineOfWLApprox& Line,
                                           const int                             index,
                                           math_Vector&                          V) const;

  Standard_EXPORT void LastTangencyVector(const GeomInt_TheMultiLineOfWLApprox& Line,
                                          const int                             index,
                                          math_Vector&                          V) const;

  Standard_EXPORT void FindRealConstraints(const GeomInt_TheMultiLineOfWLApprox& Line);

  AppParCurves_MultiBSpCurve                                      TheMultiBSpCurve;
  bool                                                            alldone;
  bool                                                            tolreached;
  Approx_ParametrizationType                                      Par;
  occ::handle<NCollection_HArray1<double>>                        myParameters;
  occ::handle<NCollection_HArray1<double>>                        myfirstParam;
  occ::handle<NCollection_HArray1<double>>                        myknots;
  occ::handle<NCollection_HArray1<int>>                           mymults;
  bool                                                            myhasknots;
  bool                                                            myhasmults;
  occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>> myConstraints;
  int                                                             mydegremin;
  int                                                             mydegremax;
  double                                                          mytol3d;
  double                                                          mytol2d;
  double                                                          currenttol3d;
  double                                                          currenttol2d;
  bool                                                            mycut;
  bool                                                            mysquares;
  int                                                             myitermax;
  AppParCurves_Constraint                                         myfirstC;
  AppParCurves_Constraint                                         mylastC;
  AppParCurves_Constraint                                         realfirstC;
  AppParCurves_Constraint                                         reallastC;
  int                                                             mycont;
  double                                                          mylambda1;
  double                                                          mylambda2;
  bool                                                            myPeriodic;
};

#endif // _GeomInt_TheComputeLineOfWLApprox_HeaderFile
