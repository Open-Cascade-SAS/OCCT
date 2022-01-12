// Created on: 1993-04-21
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

#ifndef _GeomAdaptor_SurfaceOfRevolution_HeaderFile
#define _GeomAdaptor_SurfaceOfRevolution_HeaderFile

#include <GeomAdaptor_Surface.hxx>

class gp_Pln;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class Geom_BezierSurface;
class Geom_BSplineSurface;

DEFINE_STANDARD_HANDLE(GeomAdaptor_SurfaceOfRevolution, GeomAdaptor_Surface)

//! This class defines a complete surface of revolution.
//! The surface is obtained by rotating a curve a complete revolution
//! about an axis. The curve and the axis must be in the same plane.
//! If the curve and the axis are not in the same plane it is always
//! possible to be in the previous case after a cylindrical projection
//! of the curve in a referenced plane.
//! For a complete surface of revolution the parametric range is
//! 0 <= U <= 2*PI.       --
//! The parametric range for V is defined with the revolved curve.
//! The origin of the U parametrization is given by the position
//! of the revolved curve (reference). The direction of the revolution
//! axis defines the positive sense of rotation (trigonometric sense)
//! corresponding to the increasing of the parametric value U.
//! The derivatives are always defined for the u direction.
//! For the v direction the definition of the derivatives depends on
//! the degree of continuity of the referenced curve.
class GeomAdaptor_SurfaceOfRevolution  : public GeomAdaptor_Surface
{
  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_SurfaceOfRevolution, GeomAdaptor_Surface)
public:

  Standard_EXPORT GeomAdaptor_SurfaceOfRevolution();
  
  //! The Curve is loaded.
  Standard_EXPORT GeomAdaptor_SurfaceOfRevolution(const Handle(Adaptor3d_Curve)& C);
  
  //! The Curve and the Direction are loaded.
  Standard_EXPORT GeomAdaptor_SurfaceOfRevolution(const Handle(Adaptor3d_Curve)& C, const gp_Ax1& V);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual Handle(Adaptor3d_Surface) ShallowCopy() const Standard_OVERRIDE;
  
  //! Changes the Curve
  Standard_EXPORT void Load (const Handle(Adaptor3d_Curve)& C);
  
  //! Changes the Direction
  Standard_EXPORT void Load (const gp_Ax1& V);
  
  Standard_EXPORT gp_Ax1 AxeOfRevolution() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real FirstUParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real LastUParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real FirstVParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real LastVParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape UContinuity() const Standard_OVERRIDE;
  
  //! Return CN.
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
  Standard_EXPORT Handle(Adaptor3d_Surface) UTrim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;
  
  //! Returns    a  surface trimmed in the V direction  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_Surface) VTrim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real UPeriod() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real VPeriod() const Standard_OVERRIDE;

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
  Standard_EXPORT GeomAbs_SurfaceType GetType() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Pln Plane() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Cylinder Cylinder() const Standard_OVERRIDE;
  
  //! Apex of the Cone = Cone.Position().Location()
  //! ==> ReferenceRadius = 0.
  Standard_EXPORT gp_Cone Cone() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Sphere Sphere() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Torus Torus() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer VDegree() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbVPoles() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer NbVKnots() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsURational() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsVRational() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom_BezierSurface) Bezier() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom_BSplineSurface) BSpline() const Standard_OVERRIDE;
  
  Standard_EXPORT const gp_Ax3& Axis() const;
  
  Standard_EXPORT Handle(Adaptor3d_Curve) BasisCurve() const Standard_OVERRIDE;

private:
  Handle(Adaptor3d_Curve) myBasisCurve; ///< revolved curve
  gp_Ax1                   myAxis;       ///< axis of revolution
  Standard_Boolean         myHaveAxis;   ///< whether axis of revolution is initialized
  gp_Ax3                   myAxeRev;     ///< auxiliary trihedron according to the curve position
};

#endif // _GeomAdaptor_SurfaceOfRevolution_HeaderFile
