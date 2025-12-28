// Created on: 1993-12-20
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

#ifndef _BlendFunc_EvolRad_HeaderFile
#define _BlendFunc_EvolRad_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <BlendFunc_Tensor.hxx>
#include <BlendFunc_SectionShape.hxx>
#include <Convert_ParameterisationType.hxx>
#include <Blend_Function.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>

class Law_Function;
class gp_Circ;
class Blend_Point;

class BlendFunc_EvolRad : public Blend_Function
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_EvolRad(const occ::handle<Adaptor3d_Surface>& S1,
                                    const occ::handle<Adaptor3d_Surface>& S2,
                                    const occ::handle<Adaptor3d_Curve>&   C,
                                    const occ::handle<Law_Function>&      Law);

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

  Standard_EXPORT bool TwistOnS1() const override;

  Standard_EXPORT bool TwistOnS2() const override;

  Standard_EXPORT void Set(const int Choix);

  //! Sets the type of section generation for the
  //! approximations.
  Standard_EXPORT void Set(const BlendFunc_SectionShape TypeSection);

  //! Method for graphic traces
  Standard_EXPORT void Section(const double Param,
                               const double U1,
                               const double V1,
                               const double U2,
                               const double V2,
                               double&      Pdeb,
                               double&      Pfin,
                               gp_Circ&     C);

  //! Returns if the section is rational
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

  Standard_EXPORT void Resolution(const int    IC2d,
                                  const double Tol,
                                  double&      TolU,
                                  double&      TolV) const override;

private:
  Standard_EXPORT bool ComputeValues(const math_Vector& X,
                                     const int          Order,
                                     const bool         ByParam = false,
                                     const double       Param   = 0);

  occ::handle<Adaptor3d_Surface> surf1;
  occ::handle<Adaptor3d_Surface> surf2;
  occ::handle<Adaptor3d_Curve>   curv;
  occ::handle<Adaptor3d_Curve>   tcurv;
  occ::handle<Law_Function>      fevol;
  occ::handle<Law_Function>      tevol;
  gp_Pnt                         pts1;
  gp_Pnt                         pts2;
  bool                           istangent;
  gp_Vec                         tg1;
  gp_Vec2d                       tg12d;
  gp_Vec                         tg2;
  gp_Vec2d                       tg22d;
  double                         param;
  double                         sg1;
  double                         sg2;
  double                         ray;
  double                         dray;
  double                         d2ray;
  int                            choix;
  int                            myXOrder;
  int                            myTOrder;
  math_Vector                    xval;
  double                         tval;
  gp_Vec                         d1u1;
  gp_Vec                         d1u2;
  gp_Vec                         d1v1;
  gp_Vec                         d1v2;
  gp_Vec                         d2u1;
  gp_Vec                         d2v1;
  gp_Vec                         d2uv1;
  gp_Vec                         d2u2;
  gp_Vec                         d2v2;
  gp_Vec                         d2uv2;
  gp_Vec                         dn1w;
  gp_Vec                         dn2w;
  gp_Vec                         d2n1w;
  gp_Vec                         d2n2w;
  gp_Vec                         nplan;
  gp_Vec                         nsurf1;
  gp_Vec                         nsurf2;
  gp_Vec                         dns1u1;
  gp_Vec                         dns1u2;
  gp_Vec                         dns1v1;
  gp_Vec                         dns1v2;
  gp_Vec                         dnplan;
  gp_Vec                         d2nplan;
  gp_Vec                         dnsurf1;
  gp_Vec                         dnsurf2;
  gp_Vec                         dndu1;
  gp_Vec                         dndu2;
  gp_Vec                         dndv1;
  gp_Vec                         dndv2;
  gp_Vec                         d2ndu1;
  gp_Vec                         d2ndu2;
  gp_Vec                         d2ndv1;
  gp_Vec                         d2ndv2;
  gp_Vec                         d2nduv1;
  gp_Vec                         d2nduv2;
  gp_Vec                         d2ndtu1;
  gp_Vec                         d2ndtu2;
  gp_Vec                         d2ndtv1;
  gp_Vec                         d2ndtv2;
  math_Vector                    E;
  math_Matrix                    DEDX;
  math_Vector                    DEDT;
  BlendFunc_Tensor               D2EDX2;
  math_Matrix                    D2EDXDT;
  math_Vector                    D2EDT2;
  double                         minang;
  double                         maxang;
  double                         lengthmin;
  double                         lengthmax;
  double                         distmin;
  BlendFunc_SectionShape         mySShape;
  Convert_ParameterisationType   myTConv;
};

#endif // _BlendFunc_EvolRad_HeaderFile
