// Created on: 1993-05-14
// Created by: Bruno DUMORTIER
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

#ifndef _GeomAdaptor_Surface_HeaderFile
#define _GeomAdaptor_Surface_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BSplSLib_Cache.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_XYZ.hxx>
#include <Standard_NullObject.hxx>
#include <NCollection_Array1.hxx>

#include <variant>

class GeomAdaptor_Curve;
class Geom_OffsetSurface;
class Geom_BezierSurface;
namespace Geom_EvalRepSurfaceDesc
{
class Base;
}

//! An interface between the services provided by any
//! surface from the package Geom and those required
//! of the surface by algorithms which use it.
//! Creation of the loaded surface the surface is C1 by piece
//!
//! Polynomial coefficients of BSpline surfaces used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class GeomAdaptor_Surface : public Adaptor3d_Surface
{
  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_Surface, Adaptor3d_Surface)
public:
  //! Internal structure for extrusion surface evaluation data.
  struct ExtrusionData
  {
    occ::handle<Adaptor3d_Curve> BasisCurve; //!< Adaptor for basis curve
    gp_XYZ                       Direction;  //!< Extrusion direction XYZ (normalized)
    occ::handle<Geom_EvalRepSurfaceDesc::Base> EvalRep; //!< Eval representation descriptor
  };

  //! Internal structure for revolution surface evaluation data.
  struct RevolutionData
  {
    occ::handle<Adaptor3d_Curve> BasisCurve; //!< Adaptor for basis curve
    gp_Ax1                       Axis;       //!< Revolution axis
    occ::handle<Geom_EvalRepSurfaceDesc::Base> EvalRep; //!< Eval representation descriptor
  };

  //! Internal structure for offset surface evaluation data.
  struct OffsetData
  {
    occ::handle<GeomAdaptor_Surface> BasisAdaptor; //!< Adaptor for basis surface
    occ::handle<GeomAdaptor_Surface>
      EquivalentAdaptor; //!< Adaptor for equivalent surface (if exists)
    occ::handle<Geom_OffsetSurface>
           OffsetSurface; //!< Original offset surface for osculating queries
    double Offset = 0.0;  //!< Offset distance
    occ::handle<Geom_EvalRepSurfaceDesc::Base> EvalRep; //!< Eval representation descriptor
  };

  //! Internal structure for Bezier surface cache data.
  struct BezierData
  {
    occ::handle<Geom_BezierSurface>    Surface; //!< Bezier surface to prevent downcasts
    mutable occ::handle<BSplSLib_Cache> Cache; //!< Cached data for evaluation
    occ::handle<Geom_EvalRepSurfaceDesc::Base> EvalRep; //!< Eval representation descriptor
  };

  //! Internal structure for BSpline surface cache data.
  struct BSplineData
  {
    occ::handle<Geom_BSplineSurface>    Surface; //!< BSpline surface to prevent downcasts
    mutable occ::handle<BSplSLib_Cache> Cache;   //!< Cached data for evaluation
    occ::handle<Geom_EvalRepSurfaceDesc::Base> EvalRep; //!< Eval representation descriptor
  };

  //! Variant type for surface-specific evaluation data.
  using SurfaceDataVariant = std::variant<std::monostate,
                                          gp_Pln,
                                          gp_Cylinder,
                                          gp_Cone,
                                          gp_Sphere,
                                          gp_Torus,
                                          ExtrusionData,
                                          RevolutionData,
                                          OffsetData,
                                          BezierData,
                                          BSplineData>;

public:
  GeomAdaptor_Surface()
      : myUFirst(0.),
        myULast(0.),
        myVFirst(0.),
        myVLast(0.),
        myTolU(0.),
        myTolV(0.),
        mySurfaceType(GeomAbs_OtherSurface)
  {
  }

  GeomAdaptor_Surface(const occ::handle<Geom_Surface>& theSurf)
      : myTolU(0.),
        myTolV(0.)
  {
    Load(theSurf);
  }

  //! Standard_ConstructionError is raised if UFirst>ULast or VFirst>VLast
  GeomAdaptor_Surface(const occ::handle<Geom_Surface>& theSurf,
                      const double                     theUFirst,
                      const double                     theULast,
                      const double                     theVFirst,
                      const double                     theVLast,
                      const double                     theTolU = 0.0,
                      const double                     theTolV = 0.0)
  {
    Load(theSurf, theUFirst, theULast, theVFirst, theVLast, theTolU, theTolV);
  }

  //! Shallow copy of adaptor
  Standard_EXPORT occ::handle<Adaptor3d_Surface> ShallowCopy() const override;

  void Load(const occ::handle<Geom_Surface>& theSurf)
  {
    if (theSurf.IsNull())
    {
      throw Standard_NullObject("GeomAdaptor_Surface::Load");
    }

    double aU1, aU2, aV1, aV2;
    theSurf->Bounds(aU1, aU2, aV1, aV2);
    load(theSurf, aU1, aU2, aV1, aV2);
  }

  //! Standard_ConstructionError is raised if theUFirst>theULast or theVFirst>theVLast
  void Load(const occ::handle<Geom_Surface>& theSurf,
            const double                     theUFirst,
            const double                     theULast,
            const double                     theVFirst,
            const double                     theVLast,
            const double                     theTolU = 0.0,
            const double                     theTolV = 0.0)
  {
    if (theSurf.IsNull())
    {
      throw Standard_NullObject("GeomAdaptor_Surface::Load");
    }
    if (theUFirst > theULast || theVFirst > theVLast)
    {
      throw Standard_ConstructionError("GeomAdaptor_Surface::Load");
    }

    load(theSurf, theUFirst, theULast, theVFirst, theVLast, theTolU, theTolV);
  }

  const occ::handle<Geom_Surface>& Surface() const { return mySurface; }

  double FirstUParameter() const override { return myUFirst; }

  double LastUParameter() const override { return myULast; }

  double FirstVParameter() const override { return myVFirst; }

  double LastVParameter() const override { return myVLast; }

  //! Returns the parametric bounds of the surface.
  //! @param[out] theU1 minimum U parameter
  //! @param[out] theU2 maximum U parameter
  //! @param[out] theV1 minimum V parameter
  //! @param[out] theV2 maximum V parameter
  void Bounds(double& theU1, double& theU2, double& theV1, double& theV2) const
  {
    theU1 = FirstUParameter();
    theU2 = LastUParameter();
    theV1 = FirstVParameter();
    theV2 = LastVParameter();
  }

  Standard_EXPORT GeomAbs_Shape UContinuity() const override;

  Standard_EXPORT GeomAbs_Shape VContinuity() const override;

  //! Returns the number of U intervals for continuity
  //! <S>. May be one if UContinuity(me) >= <S>
  Standard_EXPORT int NbUIntervals(const GeomAbs_Shape S) const override;

  //! Returns the number of V intervals for continuity
  //! <S>. May be one if VContinuity(me) >= <S>
  Standard_EXPORT int NbVIntervals(const GeomAbs_Shape S) const override;

  //! Returns the intervals with the requested continuity
  //! in the U direction.
  Standard_EXPORT void UIntervals(NCollection_Array1<double>& T,
                                  const GeomAbs_Shape         S) const override;

  //! Returns the intervals with the requested continuity
  //! in the V direction.
  Standard_EXPORT void VIntervals(NCollection_Array1<double>& T,
                                  const GeomAbs_Shape         S) const override;

  //! Returns a surface trimmed in the U direction
  //! equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor3d_Surface> UTrim(const double First,
                                                       const double Last,
                                                       const double Tol) const override;

  //! Returns a surface trimmed in the V direction between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor3d_Surface> VTrim(const double First,
                                                       const double Last,
                                                       const double Tol) const override;

  Standard_EXPORT bool IsUClosed() const override;

  Standard_EXPORT bool IsVClosed() const override;

  Standard_EXPORT bool IsUPeriodic() const override;

  Standard_EXPORT double UPeriod() const override;

  Standard_EXPORT bool IsVPeriodic() const override;

  Standard_EXPORT double VPeriod() const override;

  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT gp_Pnt Value(const double U, const double V) const final;

  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT void D0(const double U, const double V, gp_Pnt& P) const final;

  //! Computes the point and the first derivatives on
  //! the surface.
  //!
  //! Warning : On the specific case of BSplineSurface:
  //! if the surface is cut in interval of continuity at least C1,
  //! the derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis surface.
  Standard_EXPORT void D1(const double U,
                          const double V,
                          gp_Pnt&      P,
                          gp_Vec&      D1U,
                          gp_Vec&      D1V) const final;

  //! Computes the point, the first and second derivatives
  //! on the surface.
  //!
  //! Warning : On the specific case of BSplineSurface:
  //! if the surface is cut in interval of continuity at least C2,
  //! the derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis surface.
  Standard_EXPORT void D2(const double U,
                          const double V,
                          gp_Pnt&      P,
                          gp_Vec&      D1U,
                          gp_Vec&      D1V,
                          gp_Vec&      D2U,
                          gp_Vec&      D2V,
                          gp_Vec&      D2UV) const final;

  //! Computes the point, the first, second and third
  //! derivatives on the surface.
  //!
  //! Warning : On the specific case of BSplineSurface:
  //! if the surface is cut in interval of continuity at least C3,
  //! the derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis surface.
  Standard_EXPORT void D3(const double U,
                          const double V,
                          gp_Pnt&      P,
                          gp_Vec&      D1U,
                          gp_Vec&      D1V,
                          gp_Vec&      D2U,
                          gp_Vec&      D2V,
                          gp_Vec&      D2UV,
                          gp_Vec&      D3U,
                          gp_Vec&      D3V,
                          gp_Vec&      D3UUV,
                          gp_Vec&      D3UVV) const final;

  //! Computes the derivative of order Nu in the
  //! direction U and Nv in the direction V at the point P(U, V).
  //!
  //! Warning : On the specific case of BSplineSurface:
  //! if the surface is cut in interval of continuity CN,
  //! the derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis surface.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec DN(const double U, const double V, const int Nu, const int Nv) const final;

  //! Non-throwing point evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<gp_Pnt> EvalD0(double U, double V) const final;

  //! Non-throwing D1 evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<Geom_Surface::ResD1> EvalD1(double U,
                                                                          double V) const final;

  //! Non-throwing D2 evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<Geom_Surface::ResD2> EvalD2(double U,
                                                                          double V) const final;

  //! Non-throwing D3 evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<Geom_Surface::ResD3> EvalD3(double U,
                                                                          double V) const final;

  //! Non-throwing DN evaluation. Returns std::nullopt on failure.
  [[nodiscard]] Standard_EXPORT std::optional<gp_Vec> EvalDN(double U,
                                                             double V,
                                                             int    Nu,
                                                             int    Nv) const final;

  //! Returns the parametric U resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double UResolution(const double R3d) const override;

  //! Returns the parametric V resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double VResolution(const double R3d) const override;

  //! Returns the type of the surface: Plane, Cylinder,
  //! Cone, Sphere, Torus, BezierSurface,
  //! BSplineSurface, SurfaceOfRevolution,
  //! SurfaceOfExtrusion, OtherSurface
  GeomAbs_SurfaceType GetType() const override { return mySurfaceType; }

  Standard_EXPORT gp_Pln Plane() const override;

  Standard_EXPORT gp_Cylinder Cylinder() const override;

  Standard_EXPORT gp_Cone Cone() const override;

  Standard_EXPORT gp_Sphere Sphere() const override;

  Standard_EXPORT gp_Torus Torus() const override;

  Standard_EXPORT int UDegree() const override;

  Standard_EXPORT int NbUPoles() const override;

  Standard_EXPORT int VDegree() const override;

  Standard_EXPORT int NbVPoles() const override;

  Standard_EXPORT int NbUKnots() const override;

  Standard_EXPORT int NbVKnots() const override;

  Standard_EXPORT bool IsURational() const override;

  Standard_EXPORT bool IsVRational() const override;

  //! This will NOT make a copy of the
  //! Bezier Surface : If you want to modify
  //! the Surface please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myU/VFirst/Last.
  Standard_EXPORT occ::handle<Geom_BezierSurface> Bezier() const override;

  //! This will NOT make a copy of the
  //! BSpline Surface : If you want to modify
  //! the Surface please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myU/VFirst/Last.
  Standard_EXPORT occ::handle<Geom_BSplineSurface> BSpline() const override;

  Standard_EXPORT gp_Ax1 AxeOfRevolution() const override;

  Standard_EXPORT gp_Dir Direction() const override;

  Standard_EXPORT occ::handle<Adaptor3d_Curve> BasisCurve() const override;

  Standard_EXPORT occ::handle<Adaptor3d_Surface> BasisSurface() const override;

  Standard_EXPORT double OffsetValue() const override;

private:
  Standard_EXPORT void Span(const int Side,
                            const int Ideb,
                            const int Ifin,
                            int&      OutIdeb,
                            int&      OutIfin,
                            const int FKIndx,
                            const int LKIndx) const;

  Standard_EXPORT bool IfUVBound(const double U,
                                 const double V,
                                 int&         Ideb,
                                 int&         Ifin,
                                 int&         IVdeb,
                                 int&         IVfin,
                                 const int    USide,
                                 const int    VSide) const;

  Standard_EXPORT void load(const occ::handle<Geom_Surface>& S,
                            const double                     UFirst,
                            const double                     ULast,
                            const double                     VFirst,
                            const double                     VLast,
                            const double                     TolU = 0.0,
                            const double                     TolV = 0.0);

  //! Rebuilds B-spline cache
  //! \param theU first parameter to identify the span for caching
  //! \param theV second parameter to identify the span for caching
  Standard_EXPORT void RebuildCache(const double theU, const double theV) const;

protected:
  occ::handle<Geom_Surface> mySurface;
  double                    myUFirst;
  double                    myULast;
  double                    myVFirst;
  double                    myVLast;
  double                    myTolU;
  double                    myTolV;
  GeomAbs_SurfaceType       mySurfaceType;
  SurfaceDataVariant        mySurfaceData; ///< Surface-specific evaluation data
};

#endif // _GeomAdaptor_Surface_HeaderFile
