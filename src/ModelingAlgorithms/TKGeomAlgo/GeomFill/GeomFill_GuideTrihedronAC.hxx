// Created on: 1998-06-23
// Created by: Stephanie HUMEAU
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _GeomFill_GuideTrihedronAC_HeaderFile
#define _GeomFill_GuideTrihedronAC_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomFill_TrihedronWithGuide.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
class Approx_CurvlinFunc;
class GeomFill_TrihedronLaw;
class gp_Vec;

//! Trihedron in the case of a sweeping along a guide curve.
//! defined by curviline absciss
class GeomFill_GuideTrihedronAC : public GeomFill_TrihedronWithGuide
{

public:
  Standard_EXPORT GeomFill_GuideTrihedronAC(const occ::handle<Adaptor3d_Curve>& guide);

  //! initialize curve of trihedron law
  //! @return true
  Standard_EXPORT bool SetCurve(const occ::handle<Adaptor3d_Curve>& C) override;

  Standard_EXPORT occ::handle<GeomFill_TrihedronLaw> Copy() const override;

  Standard_EXPORT occ::handle<Adaptor3d_Curve> Guide() const override;

  Standard_EXPORT bool D0(const double Param,
                          gp_Vec&      Tangent,
                          gp_Vec&      Normal,
                          gp_Vec&      BiNormal) override;

  Standard_EXPORT bool D1(const double Param,
                          gp_Vec&      Tangent,
                          gp_Vec&      DTangent,
                          gp_Vec&      Normal,
                          gp_Vec&      DNormal,
                          gp_Vec&      BiNormal,
                          gp_Vec&      DBiNormal) override;

  Standard_EXPORT bool D2(const double Param,
                          gp_Vec&      Tangent,
                          gp_Vec&      DTangent,
                          gp_Vec&      D2Tangent,
                          gp_Vec&      Normal,
                          gp_Vec&      DNormal,
                          gp_Vec&      D2Normal,
                          gp_Vec&      BiNormal,
                          gp_Vec&      DBiNormal,
                          gp_Vec&      D2BiNormal) override;

  //! Returns the number of intervals for continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT void SetInterval(const double First, const double Last) override;

  //! Get average value of M(t) and V(t) it is useful to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT void GetAverageLaw(gp_Vec& ATangent, gp_Vec& ANormal, gp_Vec& ABiNormal) override;

  //! Say if the law is Constant
  Standard_EXPORT bool IsConstant() const override;

  //! Say if the law is defined, only by the 3d Geometry of
  //! the set Curve
  //! Return False by Default.
  Standard_EXPORT bool IsOnlyBy3dCurve() const override;

  Standard_EXPORT void Origine(const double OrACR1, const double OrACR2) override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_GuideTrihedronAC, GeomFill_TrihedronWithGuide)

private:
  occ::handle<Approx_CurvlinFunc> myGuideAC;
  double                          Lguide;
  occ::handle<Approx_CurvlinFunc> myCurveAC;
  double                          L;
  occ::handle<Adaptor3d_Curve>    myCurve;
  double                          UTol;
  double                          STol;
  double                          Orig1;
  double                          Orig2;
};

#endif // _GeomFill_GuideTrihedronAC_HeaderFile
