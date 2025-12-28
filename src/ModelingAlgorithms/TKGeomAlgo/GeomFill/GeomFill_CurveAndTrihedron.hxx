// Created on: 1997-12-02
// Created by: Philippe MANGIN
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

#ifndef _GeomFill_CurveAndTrihedron_HeaderFile
#define _GeomFill_CurveAndTrihedron_HeaderFile

#include <Standard.hxx>

#include <gp_Mat.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <Standard_Real.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
class GeomFill_TrihedronLaw;

//! Define location law with an TrihedronLaw and an
//! curve
//! Definition Location is:
//! transformed section coordinates in (Curve(v)),
//! (Normal(v), BiNormal(v), Tangente(v))) systems are
//! the same like section shape coordinates in
//! (O,(OX, OY, OZ)) system.
class GeomFill_CurveAndTrihedron : public GeomFill_LocationLaw
{

public:
  Standard_EXPORT GeomFill_CurveAndTrihedron(const occ::handle<GeomFill_TrihedronLaw>& Trihedron);

  //! initialize curve of trihedron law
  //! @return true in case if execution end correctly
  Standard_EXPORT virtual bool SetCurve(const occ::handle<Adaptor3d_Curve>& C) override;

  Standard_EXPORT virtual const occ::handle<Adaptor3d_Curve>& GetCurve() const override;

  //! Set a transformation Matrix like the law M(t) become
  //! Mat * M(t)
  Standard_EXPORT virtual void SetTrsf(const gp_Mat& Transfo) override;

  Standard_EXPORT virtual occ::handle<GeomFill_LocationLaw> Copy() const override;

  //! compute Location and 2d points
  Standard_EXPORT virtual bool D0(const double Param, gp_Mat& M, gp_Vec& V) override;

  //! compute Location and 2d points
  Standard_EXPORT virtual bool D0(const double                  Param,
                                  gp_Mat&                       M,
                                  gp_Vec&                       V,
                                  NCollection_Array1<gp_Pnt2d>& Poles2d) override;

  //! compute location 2d points and associated
  //! first derivatives.
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual bool D1(const double                  Param,
                                  gp_Mat&                       M,
                                  gp_Vec&                       V,
                                  gp_Mat&                       DM,
                                  gp_Vec&                       DV,
                                  NCollection_Array1<gp_Pnt2d>& Poles2d,
                                  NCollection_Array1<gp_Vec2d>& DPoles2d) override;

  //! compute location 2d points and associated
  //! first and second derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual bool D2(const double                  Param,
                                  gp_Mat&                       M,
                                  gp_Vec&                       V,
                                  gp_Mat&                       DM,
                                  gp_Vec&                       DV,
                                  gp_Mat&                       D2M,
                                  gp_Vec&                       D2V,
                                  NCollection_Array1<gp_Pnt2d>& Poles2d,
                                  NCollection_Array1<gp_Vec2d>& DPoles2d,
                                  NCollection_Array1<gp_Vec2d>& D2Poles2d) override;

  //! Returns the number of intervals for continuity <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T,
                                         const GeomAbs_Shape         S) const override;

  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval(const double First, const double Last) override;

  //! Gets the bounds of the parametric interval on
  //! the function
  Standard_EXPORT virtual void GetInterval(double& First, double& Last) const override;

  //! Gets the bounds of the function parametric domain.
  //! Warning: This domain it is not modified by the
  //! SetValue method
  Standard_EXPORT virtual void GetDomain(double& First, double& Last) const override;

  //! Get the maximum Norm of the matrix-location part. It
  //! is usful to find a good Tolerance to approx M(t).
  Standard_EXPORT virtual double GetMaximalNorm() override;

  //! Get average value of M(t) and V(t) it is useful to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Mat& AM, gp_Vec& AV) override;

  //! Say if the Location Law, is an translation of Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual bool IsTranslation(double& Error) const override;

  //! Say if the Location Law, is a rotation of Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual bool IsRotation(double& Error) const override;

  Standard_EXPORT virtual void Rotation(gp_Pnt& Center) const override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_CurveAndTrihedron, GeomFill_LocationLaw)

private:
  bool                               WithTrans;
  occ::handle<GeomFill_TrihedronLaw> myLaw;
  occ::handle<Adaptor3d_Curve>       myCurve;
  occ::handle<Adaptor3d_Curve>       myTrimmed;
  gp_Pnt                             Point;
  gp_Vec                             V1;
  gp_Vec                             V2;
  gp_Vec                             V3;
  gp_Mat                             Trans;
};

#endif // _GeomFill_CurveAndTrihedron_HeaderFile
