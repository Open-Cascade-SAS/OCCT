// Created on: 1993-12-02
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

#ifndef _BlendFunc_Ruled_HeaderFile
#define _BlendFunc_Ruled_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Blend_Function.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>

class math_Matrix;
class Blend_Point;
class gp_Ax1;

class BlendFunc_Ruled : public Blend_Function
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_Ruled(const occ::handle<Adaptor3d_Surface>& S1,
                                  const occ::handle<Adaptor3d_Surface>& S2,
                                  const occ::handle<Adaptor3d_Curve>&   C);

  //! returns the number of equations of the function.
  Standard_EXPORT int NbEquations() const override;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Derivatives(const math_Vector& X, math_Matrix& D) override;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override;

  Standard_EXPORT void Set(const double Param) override;

  Standard_EXPORT void Set(const double First, const double Last) override;

  Standard_EXPORT void GetTolerance(math_Vector& Tolerance, const double Tol) const override;

  Standard_EXPORT void GetBounds(math_Vector& InfBound, math_Vector& SupBound) const override;

  Standard_EXPORT bool IsSolution(const math_Vector& Sol, const double Tol) override;

  //! Returns the minimal Distance between two
  //! extremities of calculated sections.
  Standard_EXPORT double GetMinimalDistance() const override;

  Standard_EXPORT const gp_Pnt& PointOnS1() const override;

  Standard_EXPORT const gp_Pnt& PointOnS2() const override;

  Standard_EXPORT bool IsTangencyPoint() const override;

  Standard_EXPORT const gp_Vec& TangentOnS1() const override;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnS1() const override;

  Standard_EXPORT const gp_Vec& TangentOnS2() const override;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnS2() const override;

  //! Returns the tangent vector at the section,
  //! at the beginning and the end of the section, and
  //! returns the normal (of the surfaces) at
  //! these points.
  Standard_EXPORT void Tangent(const double U1,
                               const double V1,
                               const double U2,
                               const double V2,
                               gp_Vec&      TgFirst,
                               gp_Vec&      TgLast,
                               gp_Vec&      NormFirst,
                               gp_Vec&      NormLast) const override;

  Standard_EXPORT bool GetSection(const double                Param,
                                  const double                U1,
                                  const double                V1,
                                  const double                U2,
                                  const double                V2,
                                  NCollection_Array1<gp_Pnt>& tabP,
                                  NCollection_Array1<gp_Vec>& tabV);

  //! Returns False
  Standard_EXPORT bool IsRational() const override;

  //! Returns the length of the maximum section
  Standard_EXPORT double GetSectionSize() const override;

  //! Compute the minimal value of weight for each poles
  //! of all sections.
  Standard_EXPORT void GetMinimalWeight(NCollection_Array1<double>& Weigths) const override;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  //! raises OutOfRange from Standard
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  Standard_EXPORT void GetShape(int& NbPoles, int& NbKnots, int& Degree, int& NbPoles2d) override;

  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT void GetTolerance(const double BoundTol,
                                    const double SurfTol,
                                    const double AngleTol,
                                    math_Vector& Tol3d,
                                    math_Vector& Tol1D) const override;

  Standard_EXPORT void Knots(NCollection_Array1<double>& TKnots) override;

  Standard_EXPORT void Mults(NCollection_Array1<int>& TMults) override;

  //! Used for the first and last section
  Standard_EXPORT bool Section(const Blend_Point&            P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Vec>&   D2Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<gp_Vec2d>& D2Poles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths,
                               NCollection_Array1<double>&   D2Weigths) override;

  //! Used for the first and last section
  Standard_EXPORT bool Section(const Blend_Point&            P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths) override;

  Standard_EXPORT void Section(const Blend_Point&            P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<double>&   Weigths) override;

  Standard_EXPORT gp_Ax1 AxeRot(const double Prm);

  Standard_EXPORT void Resolution(const int    IC2d,
                                  const double Tol,
                                  double&      TolU,
                                  double&      TolV) const override;

private:
  occ::handle<Adaptor3d_Surface> surf1;
  occ::handle<Adaptor3d_Surface> surf2;
  occ::handle<Adaptor3d_Curve>   curv;
  gp_Pnt                         pts1;
  gp_Pnt                         pts2;
  bool                           istangent;
  gp_Vec                         tg1;
  gp_Vec2d                       tg12d;
  gp_Vec                         tg2;
  gp_Vec2d                       tg22d;
  gp_Pnt                         ptgui;
  gp_Vec                         d1gui;
  gp_Vec                         d2gui;
  gp_Vec                         nplan;
  double                         normtg;
  double                         theD;
  double                         distmin;
};

#endif // _BlendFunc_Ruled_HeaderFile
