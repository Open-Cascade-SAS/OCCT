// Created on: 1997-02-06
// Created by: Laurent BOURESCHE
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

#ifndef _BRepBlend_RstRstConstRad_HeaderFile
#define _BRepBlend_RstRstConstRad_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <BlendFunc_SectionShape.hxx>
#include <Convert_ParameterisationType.hxx>
#include <Blend_RstRstFunction.hxx>
#include <math_Vector.hxx>
#include <Blend_DecrochStatus.hxx>
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
class gp_Circ;
class Blend_Point;

//! Copy of CSConstRad with a pcurve on surface
//! as support.
class BRepBlend_RstRstConstRad : public Blend_RstRstFunction
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepBlend_RstRstConstRad(const occ::handle<Adaptor3d_Surface>& Surf1,
                                           const occ::handle<Adaptor2d_Curve2d>& Rst1,
                                           const occ::handle<Adaptor3d_Surface>& Surf2,
                                           const occ::handle<Adaptor2d_Curve2d>& Rst2,
                                           const occ::handle<Adaptor3d_Curve>&   CGuide);

  //! Returns 2.
  Standard_EXPORT int NbVariables() const override;

  //! Returns 2.
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
  Standard_EXPORT bool Derivatives(const math_Vector& X,
                                               math_Matrix&       D) override;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X,
                                          math_Vector&       F,
                                          math_Matrix&       D) override;

  Standard_EXPORT void Set(const occ::handle<Adaptor3d_Surface>& SurfRef1,
                           const occ::handle<Adaptor2d_Curve2d>& RstRef1,
                           const occ::handle<Adaptor3d_Surface>& SurfRef2,
                           const occ::handle<Adaptor2d_Curve2d>& RstRef2);

  Standard_EXPORT void Set(const double Param) override;

  //! Sets the bounds of the parametric interval on
  //! the guide line.
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT void Set(const double First, const double Last) override;

  Standard_EXPORT void GetTolerance(math_Vector&        Tolerance,
                                    const double Tol) const override;

  Standard_EXPORT void GetBounds(math_Vector& InfBound,
                                 math_Vector& SupBound) const override;

  Standard_EXPORT bool IsSolution(const math_Vector&  Sol,
                                              const double Tol) override;

  //! Returns the minimal Distance between two
  //! extremities of calculated sections.
  Standard_EXPORT virtual double GetMinimalDistance() const override;

  Standard_EXPORT const gp_Pnt& PointOnRst1() const override;

  Standard_EXPORT const gp_Pnt& PointOnRst2() const override;

  //! Returns U,V coordinates of the point on the surface.
  Standard_EXPORT const gp_Pnt2d& Pnt2dOnRst1() const override;

  //! Returns U,V coordinates of the point on the curve on
  //! surface.
  Standard_EXPORT const gp_Pnt2d& Pnt2dOnRst2() const override;

  //! Returns parameter of the point on the curve.
  Standard_EXPORT double ParameterOnRst1() const override;

  //! Returns parameter of the point on the curve.
  Standard_EXPORT double ParameterOnRst2() const override;

  Standard_EXPORT bool IsTangencyPoint() const override;

  Standard_EXPORT const gp_Vec& TangentOnRst1() const override;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnRst1() const override;

  Standard_EXPORT const gp_Vec& TangentOnRst2() const override;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnRst2() const override;

  //! Allows implementing a specific termination criterion
  //! for the function.
  Standard_EXPORT Blend_DecrochStatus Decroch(const math_Vector& Sol,
                                              gp_Vec&            NRst1,
                                              gp_Vec&            TgRst1,
                                              gp_Vec&            NRst2,
                                              gp_Vec&            TgRst2) const override;

  Standard_EXPORT void Set(const double Radius, const int Choix);

  //! Sets the type of section generation for the
  //! approximations.
  Standard_EXPORT void Set(const BlendFunc_SectionShape TypeSection);

  //! Give the center of circle define by PtRst1, PtRst2 and
  //! radius ray.
  Standard_EXPORT bool CenterCircleRst1Rst2(const gp_Pnt& PtRst1,
                                                        const gp_Pnt& PtRst2,
                                                        const gp_Vec& np,
                                                        gp_Pnt&       Center,
                                                        gp_Vec&       VdMed) const;

  Standard_EXPORT void Section(const double Param,
                               const double U,
                               const double V,
                               double&      Pdeb,
                               double&      Pfin,
                               gp_Circ&            C);

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
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape   S) const override;

  Standard_EXPORT void GetShape(int& NbPoles,
                                int& NbKnots,
                                int& Degree,
                                int& NbPoles2d) override;

  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT void GetTolerance(const double BoundTol,
                                    const double SurfTol,
                                    const double AngleTol,
                                    math_Vector&        Tol3d,
                                    math_Vector&        Tol1D) const override;

  Standard_EXPORT void Knots(NCollection_Array1<double>& TKnots) override;

  Standard_EXPORT void Mults(NCollection_Array1<int>& TMults) override;

  //! Used for the first and last section
  Standard_EXPORT bool Section(const Blend_Point&    P,
                                           NCollection_Array1<gp_Pnt>&   Poles,
                                           NCollection_Array1<gp_Vec>&   DPoles,
                                           NCollection_Array1<gp_Pnt2d>& Poles2d,
                                           NCollection_Array1<gp_Vec2d>& DPoles2d,
                                           NCollection_Array1<double>& Weigths,
                                           NCollection_Array1<double>& DWeigths) override;

  Standard_EXPORT void Section(const Blend_Point&    P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<double>& Weigths) override;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false.
  Standard_EXPORT bool Section(const Blend_Point&    P,
                                           NCollection_Array1<gp_Pnt>&   Poles,
                                           NCollection_Array1<gp_Vec>&   DPoles,
                                           NCollection_Array1<gp_Vec>&   D2Poles,
                                           NCollection_Array1<gp_Pnt2d>& Poles2d,
                                           NCollection_Array1<gp_Vec2d>& DPoles2d,
                                           NCollection_Array1<gp_Vec2d>& D2Poles2d,
                                           NCollection_Array1<double>& Weigths,
                                           NCollection_Array1<double>& DWeigths,
                                           NCollection_Array1<double>& D2Weigths) override;

  Standard_EXPORT void Resolution(const int IC2d,
                                  const double    Tol,
                                  double&         TolU,
                                  double&         TolV) const override;

private:
  occ::handle<Adaptor3d_Surface>    surf1;
  occ::handle<Adaptor3d_Surface>    surf2;
  occ::handle<Adaptor2d_Curve2d>    rst1;
  occ::handle<Adaptor2d_Curve2d>    rst2;
  Adaptor3d_CurveOnSurface     cons1;
  Adaptor3d_CurveOnSurface     cons2;
  occ::handle<Adaptor3d_Curve>      guide;
  occ::handle<Adaptor3d_Curve>      tguide;
  gp_Pnt                       ptrst1;
  gp_Pnt                       ptrst2;
  gp_Pnt2d                     pt2drst1;
  gp_Pnt2d                     pt2drst2;
  double                prmrst1;
  double                prmrst2;
  bool             istangent;
  gp_Vec                       tgrst1;
  gp_Vec2d                     tg2drst1;
  gp_Vec                       tgrst2;
  gp_Vec2d                     tg2drst2;
  double                ray;
  int             choix;
  gp_Pnt                       ptgui;
  gp_Vec                       d1gui;
  gp_Vec                       d2gui;
  gp_Vec                       nplan;
  double                normtg;
  double                theD;
  occ::handle<Adaptor3d_Surface>    surfref1;
  occ::handle<Adaptor2d_Curve2d>    rstref1;
  occ::handle<Adaptor3d_Surface>    surfref2;
  occ::handle<Adaptor2d_Curve2d>    rstref2;
  double                maxang;
  double                minang;
  double                distmin;
  BlendFunc_SectionShape       mySShape;
  Convert_ParameterisationType myTConv;
};

#endif // _BRepBlend_RstRstConstRad_HeaderFile
