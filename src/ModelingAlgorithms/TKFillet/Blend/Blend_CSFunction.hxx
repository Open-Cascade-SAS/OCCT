// Created on: 1993-09-13
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Blend_CSFunction_HeaderFile
#define _Blend_CSFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Blend_AppFunction.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
class math_Matrix;
class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;
class Blend_Point;

//! Deferred class for a function used to compute a blending
//! surface between a surface and a curve, using a guide line.
//! The vector <X> used in Value, Values and Derivatives methods
//! may be the vector of the parametric coordinates U,V,
//! W of the extremities of a section on the surface and
//! the curve.
class Blend_CSFunction : public Blend_AppFunction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns 3 (default value). Can be redefined.
  Standard_EXPORT virtual int NbVariables() const override;

  //! returns the number of equations of the function.
  Standard_EXPORT virtual int NbEquations() const override = 0;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Value(const math_Vector& X,
                                                 math_Vector&       F) override = 0;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Derivatives(const math_Vector& X,
                                                       math_Matrix&       D) override = 0;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Values(const math_Vector& X,
                                                  math_Vector&       F,
                                                  math_Matrix&       D) override = 0;

  //! Sets the value of the parameter along the guide line.
  //! This determines the plane in which the solution has
  //! to be found.
  Standard_EXPORT virtual void Set(const double Param) override = 0;

  //! Sets the bounds of the parametric interval on
  //! the guide line.
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void Set(const double First,
                                   const double Last) override = 0;

  //! Returns in the vector Tolerance the parametric tolerance
  //! for each of the 3 variables;
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT virtual void GetTolerance(math_Vector&        Tolerance,
                                            const double Tol) const override = 0;

  //! Returns in the vector InfBound the lowest values allowed
  //! for each of the 3 variables.
  //! Returns in the vector SupBound the greatest values allowed
  //! for each of the 3 variables.
  Standard_EXPORT virtual void GetBounds(math_Vector& InfBound,
                                         math_Vector& SupBound) const override = 0;

  //! Returns true if Sol is a zero of the function.
  //! Tol is the tolerance used in 3d space.
  //! The computation is made at the current value of
  //! the parameter on the guide line.
  Standard_EXPORT virtual bool IsSolution(const math_Vector&  Sol,
                                                      const double Tol)
    override = 0;

  //! Returns the minimal Distance between two
  //! extremities of calculated sections.
  Standard_EXPORT virtual double GetMinimalDistance() const override;

  //! Returns the point on the first support.
  Standard_EXPORT const gp_Pnt& Pnt1() const override;

  //! Returns the point on the second support.
  Standard_EXPORT const gp_Pnt& Pnt2() const override;

  //! Returns the point on the surface.
  Standard_EXPORT virtual const gp_Pnt& PointOnS() const = 0;

  //! Returns the point on the curve.
  Standard_EXPORT virtual const gp_Pnt& PointOnC() const = 0;

  //! Returns U,V coordinates of the point on the surface.
  Standard_EXPORT virtual const gp_Pnt2d& Pnt2d() const = 0;

  //! Returns parameter of the point on the curve.
  Standard_EXPORT virtual double ParameterOnC() const = 0;

  //! Returns True when it is not possible to compute
  //! the tangent vectors at PointOnS and/or PointOnC.
  Standard_EXPORT virtual bool IsTangencyPoint() const = 0;

  //! Returns the tangent vector at PointOnS, in 3d space.
  Standard_EXPORT virtual const gp_Vec& TangentOnS() const = 0;

  //! Returns the tangent vector at PointOnS, in the
  //! parametric space of the first surface.
  Standard_EXPORT virtual const gp_Vec2d& Tangent2d() const = 0;

  //! Returns the tangent vector at PointOnC, in 3d space.
  Standard_EXPORT virtual const gp_Vec& TangentOnC() const = 0;

  //! Returns the tangent vector at the section,
  //! at the beginning and the end of the section, and
  //! returns the normal (of the surfaces) at
  //! these points.
  Standard_EXPORT virtual void Tangent(const double U,
                                       const double V,
                                       gp_Vec&             TgS,
                                       gp_Vec&             NormS) const = 0;

  Standard_EXPORT virtual void GetShape(int& NbPoles,
                                        int& NbKnots,
                                        int& Degree,
                                        int& NbPoles2d) override = 0;

  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT virtual void GetTolerance(const double BoundTol,
                                            const double SurfTol,
                                            const double AngleTol,
                                            math_Vector&        Tol3d,
                                            math_Vector&        Tol1D) const override = 0;

  Standard_EXPORT virtual void Knots(NCollection_Array1<double>& TKnots) override = 0;

  Standard_EXPORT virtual void Mults(NCollection_Array1<int>& TMults) override = 0;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false.
  Standard_EXPORT virtual bool Section(const Blend_Point&    P,
                                                   NCollection_Array1<gp_Pnt>&   Poles,
                                                   NCollection_Array1<gp_Vec>&   DPoles,
                                                   NCollection_Array1<gp_Pnt2d>& Poles2d,
                                                   NCollection_Array1<gp_Vec2d>& DPoles2d,
                                                   NCollection_Array1<double>& Weigths,
                                                   NCollection_Array1<double>& DWeigths)
    override = 0;

  Standard_EXPORT virtual void Section(const Blend_Point&    P,
                                       NCollection_Array1<gp_Pnt>&   Poles,
                                       NCollection_Array1<gp_Pnt2d>& Poles2d,
                                       NCollection_Array1<double>& Weigths) override = 0;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false.
  Standard_EXPORT virtual bool Section(const Blend_Point&    P,
                                                   NCollection_Array1<gp_Pnt>&   Poles,
                                                   NCollection_Array1<gp_Vec>&   DPoles,
                                                   NCollection_Array1<gp_Vec>&   D2Poles,
                                                   NCollection_Array1<gp_Pnt2d>& Poles2d,
                                                   NCollection_Array1<gp_Vec2d>& DPoles2d,
                                                   NCollection_Array1<gp_Vec2d>& D2Poles2d,
                                                   NCollection_Array1<double>& Weigths,
                                                   NCollection_Array1<double>& DWeigths,
                                                   NCollection_Array1<double>& D2Weigths)
    override;

};

#endif // _Blend_CSFunction_HeaderFile
