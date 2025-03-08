// Created on: 1997-07-28
// Created by: Jerome LEMONIER
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

#ifndef _BRepBlend_SurfRstEvolRad_HeaderFile
#define _BRepBlend_SurfRstEvolRad_HeaderFile

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <BlendFunc_SectionShape.hxx>
#include <Convert_ParameterisationType.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <math_Vector.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>

class Law_Function;
class math_Matrix;
class gp_Circ;
class Blend_Point;

//! Function  to approximate by AppSurface  for
//! Edge/Face  and  evolutif  radius
class BRepBlend_SurfRstEvolRad : public Blend_SurfRstFunction
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepBlend_SurfRstEvolRad(const Handle(Adaptor3d_Surface)& Surf,
                                           const Handle(Adaptor3d_Surface)& SurfRst,
                                           const Handle(Adaptor2d_Curve2d)& Rst,
                                           const Handle(Adaptor3d_Curve)&   CGuide,
                                           const Handle(Law_Function)&      Evol);

  //! Returns 3.
  Standard_EXPORT Standard_Integer NbVariables() const Standard_OVERRIDE;

  //! Returns 3.
  Standard_EXPORT Standard_Integer NbEquations() const Standard_OVERRIDE;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, math_Vector& F) Standard_OVERRIDE;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives(const math_Vector& X,
                                               math_Matrix&       D) Standard_OVERRIDE;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values(const math_Vector& X,
                                          math_Vector&       F,
                                          math_Matrix&       D) Standard_OVERRIDE;

  Standard_EXPORT void Set(const Handle(Adaptor3d_Surface)& SurfRef,
                           const Handle(Adaptor2d_Curve2d)& RstRef);

  Standard_EXPORT void Set(const Standard_Real Param) Standard_OVERRIDE;

  //! Sets the bounds of the parametric interval on
  //! the guide line.
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT void Set(const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE;

  Standard_EXPORT void GetTolerance(math_Vector&        Tolerance,
                                    const Standard_Real Tol) const Standard_OVERRIDE;

  Standard_EXPORT void GetBounds(math_Vector& InfBound,
                                 math_Vector& SupBound) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsSolution(const math_Vector&  Sol,
                                              const Standard_Real Tol) Standard_OVERRIDE;

  //! Returns   the    minimal  Distance  between   two
  //! extremities of calculated sections.
  Standard_EXPORT virtual Standard_Real GetMinimalDistance() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Pnt& PointOnS() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Pnt& PointOnRst() const Standard_OVERRIDE;

  //! Returns U,V coordinates of the point on the surface.
  Standard_EXPORT const gp_Pnt2d& Pnt2dOnS() const Standard_OVERRIDE;

  //! Returns  U,V coordinates of the point  on the curve on
  //! surface.
  Standard_EXPORT const gp_Pnt2d& Pnt2dOnRst() const Standard_OVERRIDE;

  //! Returns parameter of the point on the curve.
  Standard_EXPORT Standard_Real ParameterOnRst() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsTangencyPoint() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Vec& TangentOnS() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnS() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Vec& TangentOnRst() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnRst() const Standard_OVERRIDE;

  //! Permet  d ' implementer   un   critere  de  decrochage
  //! specifique a la fonction.
  Standard_EXPORT Standard_Boolean Decroch(const math_Vector& Sol,
                                           gp_Vec&            NS,
                                           gp_Vec&            TgS) const Standard_OVERRIDE;

  Standard_EXPORT void Set(const Standard_Integer Choix);

  //! Sets  the  type  of   section generation   for the
  //! approximations.
  Standard_EXPORT void Set(const BlendFunc_SectionShape TypeSection);

  Standard_EXPORT void Section(const Standard_Real Param,
                               const Standard_Real U,
                               const Standard_Real V,
                               const Standard_Real W,
                               Standard_Real&      Pdeb,
                               Standard_Real&      Pfin,
                               gp_Circ&            C);

  //! Returns  if the section is rational
  Standard_EXPORT Standard_Boolean IsRational() const Standard_OVERRIDE;

  //! Returns the length of the maximum section
  Standard_EXPORT Standard_Real GetSectionSize() const Standard_OVERRIDE;

  //! Compute the minimal value of weight for each poles
  //! of all sections.
  Standard_EXPORT void GetMinimalWeight(TColStd_Array1OfReal& Weigths) const Standard_OVERRIDE;

  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbIntervals(const GeomAbs_Shape S) const Standard_OVERRIDE;

  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(TColStd_Array1OfReal& T,
                                 const GeomAbs_Shape   S) const Standard_OVERRIDE;

  Standard_EXPORT void GetShape(Standard_Integer& NbPoles,
                                Standard_Integer& NbKnots,
                                Standard_Integer& Degree,
                                Standard_Integer& NbPoles2d) Standard_OVERRIDE;

  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT void GetTolerance(const Standard_Real BoundTol,
                                    const Standard_Real SurfTol,
                                    const Standard_Real AngleTol,
                                    math_Vector&        Tol3d,
                                    math_Vector&        Tol1D) const Standard_OVERRIDE;

  Standard_EXPORT void Knots(TColStd_Array1OfReal& TKnots) Standard_OVERRIDE;

  Standard_EXPORT void Mults(TColStd_Array1OfInteger& TMults) Standard_OVERRIDE;

  //! Used for the first and last section
  Standard_EXPORT Standard_Boolean Section(const Blend_Point&    P,
                                           TColgp_Array1OfPnt&   Poles,
                                           TColgp_Array1OfVec&   DPoles,
                                           TColgp_Array1OfPnt2d& Poles2d,
                                           TColgp_Array1OfVec2d& DPoles2d,
                                           TColStd_Array1OfReal& Weigths,
                                           TColStd_Array1OfReal& DWeigths) Standard_OVERRIDE;

  //! Used for the first and last section
  //! The method returns Standard_True if the derivatives
  //! are computed, otherwise it returns Standard_False.
  Standard_EXPORT Standard_Boolean Section(const Blend_Point&    P,
                                           TColgp_Array1OfPnt&   Poles,
                                           TColgp_Array1OfVec&   DPoles,
                                           TColgp_Array1OfVec&   D2Poles,
                                           TColgp_Array1OfPnt2d& Poles2d,
                                           TColgp_Array1OfVec2d& DPoles2d,
                                           TColgp_Array1OfVec2d& D2Poles2d,
                                           TColStd_Array1OfReal& Weigths,
                                           TColStd_Array1OfReal& DWeigths,
                                           TColStd_Array1OfReal& D2Weigths) Standard_OVERRIDE;

  Standard_EXPORT void Section(const Blend_Point&    P,
                               TColgp_Array1OfPnt&   Poles,
                               TColgp_Array1OfPnt2d& Poles2d,
                               TColStd_Array1OfReal& Weigths) Standard_OVERRIDE;

  Standard_EXPORT void Resolution(const Standard_Integer IC2d,
                                  const Standard_Real    Tol,
                                  Standard_Real&         TolU,
                                  Standard_Real&         TolV) const Standard_OVERRIDE;

protected:
private:
  Handle(Adaptor3d_Surface)    surf;
  Handle(Adaptor3d_Surface)    surfrst;
  Handle(Adaptor2d_Curve2d)    rst;
  Adaptor3d_CurveOnSurface     cons;
  Handle(Adaptor3d_Curve)      guide;
  Handle(Adaptor3d_Curve)      tguide;
  gp_Pnt                       pts;
  gp_Pnt                       ptrst;
  gp_Pnt2d                     pt2ds;
  gp_Pnt2d                     pt2drst;
  Standard_Real                prmrst;
  Standard_Boolean             istangent;
  gp_Vec                       tgs;
  gp_Vec2d                     tg2ds;
  gp_Vec                       tgrst;
  gp_Vec2d                     tg2drst;
  Standard_Real                ray;
  Standard_Real                dray;
  Standard_Integer             choix;
  gp_Pnt                       ptgui;
  gp_Vec                       d1gui;
  gp_Vec                       d2gui;
  gp_Vec                       nplan;
  Standard_Real                normtg;
  Standard_Real                theD;
  Handle(Adaptor3d_Surface)    surfref;
  Handle(Adaptor2d_Curve2d)    rstref;
  Standard_Real                maxang;
  Standard_Real                minang;
  Standard_Real                distmin;
  BlendFunc_SectionShape       mySShape;
  Convert_ParameterisationType myTConv;
  Handle(Law_Function)         tevol;
  Handle(Law_Function)         fevol;
  Standard_Real                sg1;
};

#endif // _BRepBlend_SurfRstEvolRad_HeaderFile
