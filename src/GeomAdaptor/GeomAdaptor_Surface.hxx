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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <Standard_Real.hxx>
#include <BSplSLib_Cache.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <GeomEvaluator_Surface.hxx>
class Geom_Surface;
class Standard_NoSuchObject;
class Standard_OutOfRange;
class Standard_ConstructionError;
class Standard_DomainError;
class Adaptor3d_HSurface;
class gp_Pnt;
class gp_Vec;
class gp_Pln;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class Geom_BezierSurface;
class Geom_BSplineSurface;
class gp_Ax1;
class gp_Dir;
class Adaptor3d_HCurve;


//! An interface between the services provided by any
//! surface from the package Geom and those required
//! of the surface by algorithms which use it.
//! Creation of the loaded surface the surface is C1 by piece
//!
//! Polynomial coefficients of BSpline surfaces used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class GeomAdaptor_Surface  : public Adaptor3d_Surface
{
public:

  DEFINE_STANDARD_ALLOC

  
    GeomAdaptor_Surface();
  
    GeomAdaptor_Surface(const Handle(Geom_Surface)& S);
  
  //! ConstructionError is raised if UFirst>ULast or VFirst>VLast
    GeomAdaptor_Surface(const Handle(Geom_Surface)& S, const Standard_Real UFirst, const Standard_Real ULast, const Standard_Real VFirst, const Standard_Real VLast, const Standard_Real TolU = 0.0, const Standard_Real TolV = 0.0);
  
    void Load (const Handle(Geom_Surface)& S);
  
  //! ConstructionError is raised if UFirst>ULast or VFirst>VLast
    void Load (const Handle(Geom_Surface)& S, const Standard_Real UFirst, const Standard_Real ULast, const Standard_Real VFirst, const Standard_Real VLast, const Standard_Real TolU = 0.0, const Standard_Real TolV = 0.0);
  
    const Handle(Geom_Surface)& Surface() const;
  
    Standard_Real FirstUParameter() const Standard_OVERRIDE;
  
    Standard_Real LastUParameter() const Standard_OVERRIDE;
  
    Standard_Real FirstVParameter() const Standard_OVERRIDE;
  
    Standard_Real LastVParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape UContinuity() const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape VContinuity() const Standard_OVERRIDE;
  
  //! Returns the number of U intervals for  continuity
  //! <S>. May be one if UContinuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbUIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Returns the number of V intervals for  continuity
  //! <S>. May be one if VContinuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbVIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Returns the  intervals with the requested continuity
  //! in the U direction.
  Standard_EXPORT void UIntervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Returns the  intervals with the requested continuity
  //! in the V direction.
  Standard_EXPORT void VIntervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Returns    a  surface trimmed in the U direction
  //! equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_HSurface) UTrim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;
  
  //! Returns    a  surface trimmed in the V direction  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_HSurface) VTrim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real UPeriod() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real VPeriod() const Standard_OVERRIDE;
  
  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT gp_Pnt Value (const Standard_Real U, const Standard_Real V) const Standard_OVERRIDE;
  
  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const Standard_OVERRIDE;
  
  //! Computes the point  and the first derivatives on
  //! the surface.
  //!
  //! Warning : On the specific case of BSplineSurface:
  //! if the surface is cut in interval of continuity at least C1,
  //! the derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis surface.
  Standard_EXPORT void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const Standard_OVERRIDE;
  
  //! Computes   the point,  the  first  and  second
  //! derivatives on the surface.
  //!
  //! Warning : On the specific case of BSplineSurface:
  //! if the surface is cut in interval of continuity at least C2,
  //! the derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis surface.
  Standard_EXPORT void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const Standard_OVERRIDE;
  
  //! Computes the point,  the first, second and third
  //! derivatives on the surface.
  //!
  //! Warning : On the specific case of BSplineSurface:
  //! if the surface is cut in interval of continuity at least C3,
  //! the derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis surface.
  Standard_EXPORT void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const Standard_OVERRIDE;
  
  //! Computes the derivative of order Nu in the
  //! direction U and Nv in the direction V at the point P(U, V).
  //!
  //! Warning : On the specific case of BSplineSurface:
  //! if the surface is cut in interval of continuity CN,
  //! the derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis surface.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const Standard_OVERRIDE;
  
  //! Returns the parametric U  resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT Standard_Real UResolution (const Standard_Real R3d) const Standard_OVERRIDE;
  
  //! Returns the parametric V  resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT Standard_Real VResolution (const Standard_Real R3d) const Standard_OVERRIDE;
  
  //! Returns the type of the surface : Plane, Cylinder,
  //! Cone,      Sphere,        Torus,    BezierSurface,
  //! BSplineSurface,               SurfaceOfRevolution,
  //! SurfaceOfExtrusion, OtherSurface
    GeomAbs_SurfaceType GetType() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Pln Plane() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Cylinder Cylinder() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Cone Cone() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Sphere Sphere() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Torus Torus() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer UDegree() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbUPoles() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer VDegree() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbVPoles() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbUKnots() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbVKnots() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsURational() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsVRational() const Standard_OVERRIDE;
  
  //! This will NOT make a copy of the
  //! Bezier Surface : If you want to modify
  //! the Surface please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myU/VFirst/Last.
  Standard_EXPORT Handle(Geom_BezierSurface) Bezier() const Standard_OVERRIDE;
  
  //! This will NOT make a copy of the
  //! BSpline Surface : If you want to modify
  //! the Surface please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myU/VFirst/Last.
  Standard_EXPORT Handle(Geom_BSplineSurface) BSpline() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Ax1 AxeOfRevolution() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Dir Direction() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Adaptor3d_HCurve) BasisCurve() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Adaptor3d_HSurface) BasisSurface() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real OffsetValue() const Standard_OVERRIDE;




protected:





private:

  
  Standard_EXPORT void Span (const Standard_Integer Side, const Standard_Integer Ideb, const Standard_Integer Ifin, Standard_Integer& OutIdeb, Standard_Integer& OutIfin, const Standard_Integer FKIndx, const Standard_Integer LKIndx) const;
  
  Standard_EXPORT Standard_Boolean IfUVBound (const Standard_Real U, const Standard_Real V, Standard_Integer& Ideb, Standard_Integer& Ifin, Standard_Integer& IVdeb, Standard_Integer& IVfin, const Standard_Integer USide, const Standard_Integer VSide) const;
  
  Standard_EXPORT void load (const Handle(Geom_Surface)& S, const Standard_Real UFirst, const Standard_Real ULast, const Standard_Real VFirst, const Standard_Real VLast, const Standard_Real TolU = 0.0, const Standard_Real TolV = 0.0);
  
  //! Rebuilds B-spline cache
  //! \param theU first parameter to identify the span for caching
  //! \param theV second parameter to identify the span for caching
  Standard_EXPORT void RebuildCache (const Standard_Real theU, const Standard_Real theV) const;


  Handle(Geom_Surface) mySurface;
  Standard_Real myUFirst;
  Standard_Real myULast;
  Standard_Real myVFirst;
  Standard_Real myVLast;
  Standard_Real myTolU;
  Standard_Real myTolV;
  
  Handle(Geom_BSplineSurface) myBSplineSurface; ///< B-spline representation to prevent downcasts
  mutable Handle(BSplSLib_Cache) mySurfaceCache; ///< Cached data for B-spline or Bezier surface

protected:
  GeomAbs_SurfaceType mySurfaceType;
  Handle(GeomEvaluator_Surface) myNestedEvaluator; ///< Calculates values of nested complex surfaces (offset surface, surface of extrusion or revolution)
};


#include <GeomAdaptor_Surface.lxx>





#endif // _GeomAdaptor_Surface_HeaderFile
