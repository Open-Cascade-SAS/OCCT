// Created on: 1995-05-04
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiDS_ElSpine_HeaderFile
#define _ChFiDS_ElSpine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax1.hxx>
#include <NCollection_Sequence.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_CurveType.hxx>
class ChFiDS_SurfData;
class gp_Ax1;
class Geom_Curve;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class Geom_BezierCurve;
class Geom_BSplineCurve;

//! Elementary Spine for cheminements and approximations.
class ChFiDS_ElSpine : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(ChFiDS_ElSpine, Adaptor3d_Curve)
public:
  Standard_EXPORT ChFiDS_ElSpine();

  //! Shallow copy of adaptor
  Standard_EXPORT virtual occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  Standard_EXPORT virtual double FirstParameter() const override;

  Standard_EXPORT virtual double LastParameter() const override;

  Standard_EXPORT double GetSavedFirstParameter() const;

  Standard_EXPORT double GetSavedLastParameter() const;

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape   S) const override;

  //! Returns a curve equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  Standard_EXPORT virtual occ::handle<Adaptor3d_Curve> Trim(const double First,
                                                       const double Last,
                                                       const double Tol) const
    override;

  Standard_EXPORT virtual double Resolution(const double R3d) const override;

  Standard_EXPORT virtual GeomAbs_CurveType GetType() const override;

  Standard_EXPORT virtual bool IsPeriodic() const override;

  Standard_EXPORT void SetPeriodic(const bool I);

  Standard_EXPORT virtual double Period() const override;

  Standard_EXPORT virtual gp_Pnt Value(const double AbsC) const override;

  Standard_EXPORT virtual void D0(const double AbsC, gp_Pnt& P) const override;

  Standard_EXPORT virtual void D1(const double AbsC,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1) const override;

  Standard_EXPORT virtual void D2(const double AbsC,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1,
                                  gp_Vec&             V2) const override;

  Standard_EXPORT virtual void D3(const double AbsC,
                                  gp_Pnt&             P,
                                  gp_Vec&             V1,
                                  gp_Vec&             V2,
                                  gp_Vec&             V3) const override;

  Standard_EXPORT void FirstParameter(const double P);

  Standard_EXPORT void LastParameter(const double P);

  Standard_EXPORT void SaveFirstParameter();

  Standard_EXPORT void SaveLastParameter();

  Standard_EXPORT void SetOrigin(const double O);

  Standard_EXPORT void FirstPointAndTgt(gp_Pnt& P, gp_Vec& T) const;

  Standard_EXPORT void LastPointAndTgt(gp_Pnt& P, gp_Vec& T) const;

  Standard_EXPORT int NbVertices() const;

  Standard_EXPORT const gp_Ax1& VertexWithTangent(const int Index) const;

  Standard_EXPORT void SetFirstPointAndTgt(const gp_Pnt& P, const gp_Vec& T);

  Standard_EXPORT void SetLastPointAndTgt(const gp_Pnt& P, const gp_Vec& T);

  Standard_EXPORT void AddVertexWithTangent(const gp_Ax1& anAx1);

  Standard_EXPORT void SetCurve(const occ::handle<Geom_Curve>& C);

  Standard_EXPORT const occ::handle<ChFiDS_SurfData>& Previous() const;

  Standard_EXPORT occ::handle<ChFiDS_SurfData>& ChangePrevious();

  Standard_EXPORT const occ::handle<ChFiDS_SurfData>& Next() const;

  Standard_EXPORT occ::handle<ChFiDS_SurfData>& ChangeNext();

  Standard_EXPORT gp_Lin Line() const override;

  Standard_EXPORT gp_Circ Circle() const override;

  Standard_EXPORT gp_Elips Ellipse() const override;

  Standard_EXPORT gp_Hypr Hyperbola() const override;

  Standard_EXPORT gp_Parab Parabola() const override;

  Standard_EXPORT occ::handle<Geom_BezierCurve> Bezier() const override;

  Standard_EXPORT occ::handle<Geom_BSplineCurve> BSpline() const override;

private:
  GeomAdaptor_Curve       curve;
  gp_Pnt                  ptfirst;
  gp_Pnt                  ptlast;
  gp_Vec                  tgfirst;
  gp_Vec                  tglast;
  NCollection_Sequence<gp_Ax1>    VerticesWithTangents;
  occ::handle<ChFiDS_SurfData> previous;
  occ::handle<ChFiDS_SurfData> next;
  double           pfirst;
  double           plast;
  double           period;
  bool        periodic;
  double           pfirstsav;
  double           plastsav;
};

#endif // _ChFiDS_ElSpine_HeaderFile
