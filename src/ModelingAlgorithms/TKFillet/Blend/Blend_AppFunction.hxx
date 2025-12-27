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

#ifndef _Blend_AppFunction_HeaderFile
#define _Blend_AppFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_Shape.hxx>
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
class Blend_Point;

//! Deferred class for a function used to compute a blending
//! surface between two surfaces, using a guide line.
//! The vector <X> used in Value, Values and Derivatives methods
//! has to be the vector of the parametric coordinates U1,V1,
//! U2,V2, of the extremities of a section on the first and
//! second surface.
class Blend_AppFunction : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  //! returns the number of variables of the function.
  Standard_EXPORT virtual int NbVariables() const = 0;

  //! returns the number of equations of the function.
  Standard_EXPORT virtual int NbEquations() const = 0;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Value(const math_Vector& X, math_Vector& F) = 0;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Derivatives(const math_Vector& X, math_Matrix& D) = 0;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual bool Values(const math_Vector& X,
                                                  math_Vector&       F,
                                                  math_Matrix&       D) = 0;

  //! Sets the value of the parameter along the guide line.
  //! This determines the plane in which the solution has
  //! to be found.
  Standard_EXPORT virtual void Set(const double Param) = 0;

  //! Sets the bounds of the parametric interval on
  //! the guide line.
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void Set(const double First, const double Last) = 0;

  //! Returns in the vector Tolerance the parametric tolerance
  //! for each of the 4 variables;
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT virtual void GetTolerance(math_Vector&        Tolerance,
                                            const double Tol) const = 0;

  //! Returns in the vector InfBound the lowest values allowed
  //! for each of the 4 variables.
  //! Returns in the vector SupBound the greatest values allowed
  //! for each of the 4 variables.
  Standard_EXPORT virtual void GetBounds(math_Vector& InfBound, math_Vector& SupBound) const = 0;

  //! Returns true if Sol is a zero of the function.
  //! Tol is the tolerance used in 3d space.
  //! The computation is made at the current value of
  //! the parameter on the guide line.
  Standard_EXPORT virtual bool IsSolution(const math_Vector&  Sol,
                                                      const double Tol) = 0;

  //! Returns the minimal Distance between two
  //! extremities of calculated sections.
  Standard_EXPORT virtual double GetMinimalDistance() const = 0;

  //! Returns the point on the first support.
  Standard_EXPORT virtual const gp_Pnt& Pnt1() const = 0;

  //! Returns the point on the first support.
  Standard_EXPORT virtual const gp_Pnt& Pnt2() const = 0;

  //! Returns if the section is rational
  Standard_EXPORT virtual bool IsRational() const = 0;

  //! Returns the length of the maximum section
  Standard_EXPORT virtual double GetSectionSize() const = 0;

  //! Compute the minimal value of weight for each poles
  //! of all sections.
  Standard_EXPORT virtual void GetMinimalWeight(NCollection_Array1<double>& Weigths) const = 0;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const = 0;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  //! raises
  //! OutOfRange from Standard
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const = 0;

  Standard_EXPORT virtual void GetShape(int& NbPoles,
                                        int& NbKnots,
                                        int& Degree,
                                        int& NbPoles2d) = 0;

  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT virtual void GetTolerance(const double BoundTol,
                                            const double SurfTol,
                                            const double AngleTol,
                                            math_Vector&        Tol3d,
                                            math_Vector&        Tol1D) const = 0;

  Standard_EXPORT virtual void Knots(NCollection_Array1<double>& TKnots) = 0;

  Standard_EXPORT virtual void Mults(NCollection_Array1<int>& TMults) = 0;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false.
  Standard_EXPORT virtual bool Section(const Blend_Point&    P,
                                                   NCollection_Array1<gp_Pnt>&   Poles,
                                                   NCollection_Array1<gp_Vec>&   DPoles,
                                                   NCollection_Array1<gp_Pnt2d>& Poles2d,
                                                   NCollection_Array1<gp_Vec2d>& DPoles2d,
                                                   NCollection_Array1<double>& Weigths,
                                                   NCollection_Array1<double>& DWeigths) = 0;

  Standard_EXPORT virtual void Section(const Blend_Point&    P,
                                       NCollection_Array1<gp_Pnt>&   Poles,
                                       NCollection_Array1<gp_Pnt2d>& Poles2d,
                                       NCollection_Array1<double>& Weigths) = 0;

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
                                                   NCollection_Array1<double>& D2Weigths) = 0;

  Standard_EXPORT virtual void Resolution(const int IC2d,
                                          const double    Tol,
                                          double&         TolU,
                                          double&         TolV) const = 0;

  //! Returns the parameter of the point P. Used to
  //! impose the parameters in the approximation.
  Standard_EXPORT double Parameter(const Blend_Point& P) const;

};

#endif // _Blend_AppFunction_HeaderFile
