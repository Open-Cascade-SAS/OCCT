// Created by: Jacques  GOUSSARD Author:    Laurent    BOURESCHE --
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

#ifndef _Blend_SurfRstFunction_HeaderFile
#define _Blend_SurfRstFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Blend_AppFunction.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
class math_Matrix;
class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;
class Blend_Point;

//! Deferred class for a function used to compute a blending
//! surface between a surface and a pcurve on an other Surface,
//! using a guide line.
//! The vector <X> used in Value, Values and Derivatives methods
//! may be the vector of the parametric coordinates U,V,
//! W of the extremities of a section on the surface and
//! the curve.
class Blend_SurfRstFunction : public Blend_AppFunction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns 3 (default value). Can be redefined.
  Standard_EXPORT int NbVariables() const override = 0;

  //! returns the number of equations of the function.
  Standard_EXPORT int NbEquations() const override = 0;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override = 0;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Derivatives(const math_Vector& X, math_Matrix& D) override = 0;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override = 0;

  //! Sets the value of the parameter along the guide line.
  //! This determines the plane in which the solution has
  //! to be found.
  Standard_EXPORT void Set(const double Param) override = 0;

  //! Sets the bounds of the parametric interval on
  //! the guide line.
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT void Set(const double First, const double Last) override = 0;

  //! Returns in the vector Tolerance the parametric tolerance
  //! for each variable.
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT void GetTolerance(math_Vector& Tolerance, const double Tol) const override = 0;

  //! Returns in the vector InfBound the lowest values allowed
  //! for each variable.
  //! Returns in the vector SupBound the greatest values allowed
  //! for each of the 3 variables.
  Standard_EXPORT void GetBounds(math_Vector& InfBound, math_Vector& SupBound) const override = 0;

  //! Returns true if Sol is a zero of the function.
  //! Tol is the tolerance used in 3d space.
  //! The computation is made at the current value of
  //! the parameter on the guide line.
  Standard_EXPORT bool IsSolution(const math_Vector& Sol, const double Tol) override = 0;

  //! Returns the minimal Distance between two
  //! extremities of calculated sections.
  Standard_EXPORT double GetMinimalDistance() const override;

  //! Returns the point on the first support.
  Standard_EXPORT const gp_Pnt& Pnt1() const override;

  //! Returns the point on the second support.
  Standard_EXPORT const gp_Pnt& Pnt2() const override;

  //! Returns the point on the surface.
  Standard_EXPORT virtual const gp_Pnt& PointOnS() const = 0;

  //! Returns the point on the curve.
  Standard_EXPORT virtual const gp_Pnt& PointOnRst() const = 0;

  //! Returns U,V coordinates of the point on the surface.
  Standard_EXPORT virtual const gp_Pnt2d& Pnt2dOnS() const = 0;

  //! Returns U,V coordinates of the point on the curve on
  //! surface.
  Standard_EXPORT virtual const gp_Pnt2d& Pnt2dOnRst() const = 0;

  //! Returns parameter of the point on the curve.
  Standard_EXPORT virtual double ParameterOnRst() const = 0;

  //! Returns True when it is not possible to compute
  //! the tangent vectors at PointOnS and/or PointOnRst.
  Standard_EXPORT virtual bool IsTangencyPoint() const = 0;

  //! Returns the tangent vector at PointOnS, in 3d space.
  Standard_EXPORT virtual const gp_Vec& TangentOnS() const = 0;

  //! Returns the tangent vector at PointOnS, in the
  //! parametric space of the first surface.
  Standard_EXPORT virtual const gp_Vec2d& Tangent2dOnS() const = 0;

  //! Returns the tangent vector at PointOnC, in 3d space.
  Standard_EXPORT virtual const gp_Vec& TangentOnRst() const = 0;

  //! Returns the tangent vector at PointOnRst, in the
  //! parametric space of the second surface.
  Standard_EXPORT virtual const gp_Vec2d& Tangent2dOnRst() const = 0;

  //! Enables implementation of a criterion of decrochage
  //! specific to the function.
  Standard_EXPORT virtual bool Decroch(const math_Vector& Sol, gp_Vec& NS, gp_Vec& TgS) const = 0;

  //! Returns if the section is rational
  Standard_EXPORT bool IsRational() const override = 0;

  //! Returns the length of the maximum section
  Standard_EXPORT double GetSectionSize() const override = 0;

  //! Compute the minimal value of weight for each poles
  //! of all sections.
  Standard_EXPORT void GetMinimalWeight(NCollection_Array1<double>& Weigths) const override = 0;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override = 0;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override = 0;

  Standard_EXPORT void GetShape(int& NbPoles,
                                int& NbKnots,
                                int& Degree,
                                int& NbPoles2d) override = 0;

  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT void GetTolerance(const double BoundTol,
                                    const double SurfTol,
                                    const double AngleTol,
                                    math_Vector& Tol3d,
                                    math_Vector& Tol1D) const override = 0;

  Standard_EXPORT void Knots(NCollection_Array1<double>& TKnots) override = 0;

  Standard_EXPORT void Mults(NCollection_Array1<int>& TMults) override = 0;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false.
  Standard_EXPORT bool Section(const Blend_Point&            P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths) override = 0;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false.
  Standard_EXPORT bool Section(const Blend_Point&            P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Vec>&   D2Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<gp_Vec2d>& D2Poles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths,
                               NCollection_Array1<double>&   D2Weigths) override = 0;

  Standard_EXPORT void Section(const Blend_Point&            P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<double>&   Weigths) override = 0;
};

#endif // _Blend_SurfRstFunction_HeaderFile
