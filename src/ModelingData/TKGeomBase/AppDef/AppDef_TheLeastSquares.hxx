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

#ifndef _AppDef_TheLeastSquares_HeaderFile
#define _AppDef_TheLeastSquares_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_Constraint.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <math_IntegerVector.hxx>
#include <Standard_Real.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_DimensionError;
class Standard_NoSuchObject;
class AppDef_MultiLine;
class AppDef_MyLineTool;
class AppParCurves_MultiCurve;
class AppParCurves_MultiBSpCurve;
class math_Matrix;

class AppDef_TheLeastSquares
{
public:
  DEFINE_STANDARD_ALLOC

  //! given a MultiLine, this algorithm computes the least
  //! square resolution using the Householder-QR method.
  //! If the first and/or the last point is a constraint
  //! point, the value of the tangency or curvature is
  //! computed in the resolution.
  //! NbPol is the number of control points wanted
  //! for the approximating curves.
  //! The system to solve is the following:
  //! A X = B.
  //! Where A is the Bernstein matrix computed with the
  //! parameters, B the points coordinates and X the poles
  //! solutions.
  //! The matrix A is the same for each coordinate x, y and z
  //! and is also the same for each MultiLine point because
  //! they are approximated in parallel(so with the same
  //! parameter, only the vector B changes).
  Standard_EXPORT AppDef_TheLeastSquares(const AppDef_MultiLine&       SSP,
                                         const int                     FirstPoint,
                                         const int                     LastPoint,
                                         const AppParCurves_Constraint FirstCons,
                                         const AppParCurves_Constraint LastCons,
                                         const math_Vector&            Parameters,
                                         const int                     NbPol);

  //! Initializes the fields of the object.
  Standard_EXPORT AppDef_TheLeastSquares(const AppDef_MultiLine&       SSP,
                                         const int                     FirstPoint,
                                         const int                     LastPoint,
                                         const AppParCurves_Constraint FirstCons,
                                         const AppParCurves_Constraint LastCons,
                                         const int                     NbPol);

  //! given a MultiLine, this algorithm computes the least
  //! square resolution using the Householder-QR method.
  //! If the first and/or the last point is a constraint
  //! point, the value of the tangency or curvature is
  //! computed in the resolution.
  //! Deg is the degree wanted for the approximating curves.
  //! The system to solve is the following:
  //! A X = B.
  //! Where A is the BSpline functions matrix computed with
  //! <parameters>, B the points coordinates and X the poles
  //! solutions.
  //! The matrix A is the same for each coordinate x, y and z
  //! and is also the same for each MultiLine point because
  //! they are approximated in parallel(so with the same
  //! parameter, only the vector B changes).
  Standard_EXPORT AppDef_TheLeastSquares(const AppDef_MultiLine&           SSP,
                                         const NCollection_Array1<double>& Knots,
                                         const NCollection_Array1<int>&    Mults,
                                         const int                         FirstPoint,
                                         const int                         LastPoint,
                                         const AppParCurves_Constraint     FirstCons,
                                         const AppParCurves_Constraint     LastCons,
                                         const math_Vector&                Parameters,
                                         const int                         NbPol);

  //! Initializes the fields of the object.
  Standard_EXPORT AppDef_TheLeastSquares(const AppDef_MultiLine&           SSP,
                                         const NCollection_Array1<double>& Knots,
                                         const NCollection_Array1<int>&    Mults,
                                         const int                         FirstPoint,
                                         const int                         LastPoint,
                                         const AppParCurves_Constraint     FirstCons,
                                         const AppParCurves_Constraint     LastCons,
                                         const int                         NbPol);

  //! Is used after having initialized the fields.
  //! The case "CurvaturePoint" is not treated in this method.
  Standard_EXPORT void Perform(const math_Vector& Parameters);

  //! Is used after having initialized the fields.
  Standard_EXPORT void Perform(const math_Vector& Parameters, const double l1, const double l2);

  //! Is used after having initialized the fields.
  //! <V1t> is the tangent vector at the first point.
  //! <V2t> is the tangent vector at the last point.
  Standard_EXPORT void Perform(const math_Vector& Parameters,
                               const math_Vector& V1t,
                               const math_Vector& V2t,
                               const double       l1,
                               const double       l2);

  //! Is used after having initialized the fields.
  //! <V1t> is the tangent vector at the first point.
  //! <V2t> is the tangent vector at the last point.
  //! <V1c> is the tangent vector at the first point.
  //! <V2c> is the tangent vector at the last point.
  Standard_EXPORT void Perform(const math_Vector& Parameters,
                               const math_Vector& V1t,
                               const math_Vector& V2t,
                               const math_Vector& V1c,
                               const math_Vector& V2c,
                               const double       l1,
                               const double       l2);

  //! returns True if all has been correctly done.
  Standard_EXPORT bool IsDone() const;

  //! returns the result of the approximation, i.e. all the
  //! Curves.
  //! An exception is raised if NotDone.
  Standard_EXPORT AppParCurves_MultiCurve BezierValue();

  //! returns the result of the approximation, i.e. all the
  //! Curves.
  //! An exception is raised if NotDone.
  Standard_EXPORT const AppParCurves_MultiBSpCurve& BSplineValue();

  //! returns the function matrix used to approximate the
  //! set.
  Standard_EXPORT const math_Matrix& FunctionMatrix() const;

  //! returns the derivative function matrix used
  //! to approximate the set.
  Standard_EXPORT const math_Matrix& DerivativeFunctionMatrix() const;

  //! returns the maximum errors between the MultiLine
  //! and the approximation curves. F is the sum of the square
  //! distances. Grad is the derivative vector of the
  //! function F.
  Standard_EXPORT void ErrorGradient(math_Vector& Grad, double& F, double& MaxE3d, double& MaxE2d);

  //! returns the distances between the points of the
  //! multiline and the approximation curves.
  Standard_EXPORT const math_Matrix& Distance();

  //! returns the maximum errors between the MultiLine
  //! and the approximation curves. F is the sum of the square
  //! distances.
  Standard_EXPORT void Error(double& F, double& MaxE3d, double& MaxE2d);

  //! returns the value (P2 - P1)/ V1 if the first point
  //! was a tangency point.
  Standard_EXPORT double FirstLambda() const;

  //! returns the value (PN - PN-1)/ VN if the last point
  //! was a tangency point.
  Standard_EXPORT double LastLambda() const;

  //! returns the matrix of points value.
  Standard_EXPORT const math_Matrix& Points() const;

  //! returns the matrix of resulting control points value.
  Standard_EXPORT const math_Matrix& Poles() const;

  //! Returns the indexes of the first non null values of
  //! A and DA.
  //! The values are non null from Index(ieme point) +1
  //! to Index(ieme point) + degree +1.
  Standard_EXPORT const math_IntegerVector& KIndex() const;

protected:
  //! is used by the constructors above.
  Standard_EXPORT void Init(const AppDef_MultiLine& SSP, const int FirstPoint, const int LastPoint);

  //! returns the number of second member columns.
  //! Is used internally to initialize the fields.
  Standard_EXPORT int NbBColumns(const AppDef_MultiLine& SSP) const;

  //! returns the first point being fitted.
  Standard_EXPORT int TheFirstPoint(const AppParCurves_Constraint FirstCons,
                                    const int                     FirstPoint) const;

  //! returns the last point being fitted.
  Standard_EXPORT int TheLastPoint(const AppParCurves_Constraint LastCons,
                                   const int                     LastPoint) const;

  //! Affects the fields in the case of a constraint point.
  Standard_EXPORT void Affect(const AppDef_MultiLine&  SSP,
                              const int                Index,
                              AppParCurves_Constraint& Cons,
                              math_Vector&             Vt,
                              math_Vector&             Vc);

  Standard_EXPORT void ComputeFunction(const math_Vector& Parameters);

  Standard_EXPORT void SearchIndex(math_IntegerVector& Index);

  //! computes internal matrixes for the resolution
  Standard_EXPORT void MakeTAA(math_Vector& TheA, math_Vector& TheB);

  //! computes internal matrixes for the resolution
  Standard_EXPORT void MakeTAA(math_Vector& TheA);

  //! computes internal matrixes for the resolution
  Standard_EXPORT void MakeTAA(math_Vector& TheA, math_Matrix& TheB);

private:
  AppParCurves_Constraint                  FirstConstraint;
  AppParCurves_Constraint                  LastConstraint;
  AppParCurves_MultiBSpCurve               SCU;
  occ::handle<NCollection_HArray1<double>> myknots;
  occ::handle<NCollection_HArray1<int>>    mymults;
  math_Matrix                              mypoles;
  math_Matrix                              A;
  math_Matrix                              DA;
  math_Matrix                              B2;
  math_Matrix                              mypoints;
  math_Vector                              Vflatknots;
  math_Vector                              Vec1t;
  math_Vector                              Vec1c;
  math_Vector                              Vec2t;
  math_Vector                              Vec2c;
  math_Matrix                              theError;
  math_IntegerVector                       myindex;
  double                                   lambda1;
  double                                   lambda2;
  int                                      FirstP;
  int                                      LastP;
  int                                      Nlignes;
  int                                      Ninc;
  int                                      NA;
  int                                      myfirstp;
  int                                      mylastp;
  int                                      resinit;
  int                                      resfin;
  int                                      nbP2d;
  int                                      nbP;
  int                                      nbpoles;
  int                                      deg;
  bool                                     done;
  bool                                     iscalculated;
  bool                                     isready;
};

#endif // _AppDef_TheLeastSquares_HeaderFile
