// Created on: 1993-03-11
// Created by: Isabelle GRIGNON
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

#ifndef _Adaptor3d_IsoCurve_HeaderFile
#define _Adaptor3d_IsoCurve_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_IsoType.hxx>

DEFINE_STANDARD_HANDLE(Adaptor3d_IsoCurve, Adaptor3d_Curve)

//! Defines an isoparametric curve on  a surface.  The
//! type  of isoparametric curve  (U  or V) is defined
//! with the   enumeration  IsoType from   GeomAbs  if
//! NoneIso is given an error is raised.
class Adaptor3d_IsoCurve : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(Adaptor3d_IsoCurve, Adaptor3d_Curve)
public:
  //! The iso is set to NoneIso.
  Standard_EXPORT Adaptor3d_IsoCurve();

  //! The surface is loaded. The iso is set to NoneIso.
  Standard_EXPORT Adaptor3d_IsoCurve(const Handle(Adaptor3d_Surface)& S);

  //! Creates  an  IsoCurve curve.   Iso  defines the
  //! type (isoU or  isoU) Param defines the value of
  //! the iso. The bounds  of  the iso are the bounds
  //! of the surface.
  Standard_EXPORT Adaptor3d_IsoCurve(const Handle(Adaptor3d_Surface)& S,
                                     const GeomAbs_IsoType            Iso,
                                     const Standard_Real              Param);

  //! Create an IsoCurve curve.  Iso defines the type
  //! (isoU or isov).  Param defines the value of the
  //! iso. WFirst,WLast define the bounds of the iso.
  Standard_EXPORT Adaptor3d_IsoCurve(const Handle(Adaptor3d_Surface)& S,
                                     const GeomAbs_IsoType            Iso,
                                     const Standard_Real              Param,
                                     const Standard_Real              WFirst,
                                     const Standard_Real              WLast);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual Handle(Adaptor3d_Curve) ShallowCopy() const Standard_OVERRIDE;

  //! Changes  the surface.  The  iso  is  reset  to
  //! NoneIso.
  Standard_EXPORT void Load(const Handle(Adaptor3d_Surface)& S);

  //! Changes the iso on the current surface.
  Standard_EXPORT void Load(const GeomAbs_IsoType Iso, const Standard_Real Param);

  //! Changes the iso on the current surface.
  Standard_EXPORT void Load(const GeomAbs_IsoType Iso,
                            const Standard_Real   Param,
                            const Standard_Real   WFirst,
                            const Standard_Real   WLast);

  const Handle(Adaptor3d_Surface)& Surface() const { return mySurface; }

  GeomAbs_IsoType Iso() const { return myIso; }

  Standard_Real Parameter() const { return myParameter; }

  virtual Standard_Real FirstParameter() const Standard_OVERRIDE { return myFirst; }

  virtual Standard_Real LastParameter() const Standard_OVERRIDE { return myLast; }

  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;

  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbIntervals(const GeomAbs_Shape S) const Standard_OVERRIDE;

  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(TColStd_Array1OfReal& T,
                                 const GeomAbs_Shape   S) const Standard_OVERRIDE;

  //! Returns    a  curve equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_Curve) Trim(const Standard_Real First,
                                               const Standard_Real Last,
                                               const Standard_Real Tol) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Real Period() const Standard_OVERRIDE;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT gp_Pnt Value(const Standard_Real U) const Standard_OVERRIDE;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT void D0(const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V) const Standard_OVERRIDE;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const Standard_Real U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2) const Standard_OVERRIDE;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3(const Standard_Real U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2,
                          gp_Vec&             V3) const Standard_OVERRIDE;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN(const Standard_Real    U,
                            const Standard_Integer N) const Standard_OVERRIDE;

  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT Standard_Real Resolution(const Standard_Real R3d) const Standard_OVERRIDE;

  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT GeomAbs_CurveType GetType() const Standard_OVERRIDE;

  Standard_EXPORT gp_Lin Line() const Standard_OVERRIDE;

  Standard_EXPORT gp_Circ Circle() const Standard_OVERRIDE;

  Standard_EXPORT gp_Elips Ellipse() const Standard_OVERRIDE;

  Standard_EXPORT gp_Hypr Hyperbola() const Standard_OVERRIDE;

  Standard_EXPORT gp_Parab Parabola() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer Degree() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsRational() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer NbPoles() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer NbKnots() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Geom_BezierCurve) Bezier() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Geom_BSplineCurve) BSpline() const Standard_OVERRIDE;

private:
  Handle(Adaptor3d_Surface) mySurface;
  GeomAbs_IsoType           myIso;
  Standard_Real             myFirst;
  Standard_Real             myLast;
  Standard_Real             myParameter;
};

#endif // _Adaptor3d_IsoCurve_HeaderFile
