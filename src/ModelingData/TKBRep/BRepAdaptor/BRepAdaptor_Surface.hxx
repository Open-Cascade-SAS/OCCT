// Created on: 1993-02-22
// Created by: Remi LEQUETTE
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

#ifndef _BRepAdaptor_Surface_HeaderFile
#define _BRepAdaptor_Surface_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Trsf.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopoDS_Face.hxx>

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

DEFINE_STANDARD_HANDLE(BRepAdaptor_Surface, Adaptor3d_Surface)

//! The Surface from BRepAdaptor allows to  use a Face
//! of the BRep topology look like a 3D surface.
//!
//! It  has  the methods  of  the class   Surface from
//! Adaptor3d.
//!
//! It is created or initialized with a Face. It takes
//! into account the local coordinates system.
//!
//! The  u,v parameter range is   the minmax value for
//! the  restriction,  unless  the flag restriction is
//! set to false.
class BRepAdaptor_Surface : public Adaptor3d_Surface
{
  DEFINE_STANDARD_RTTIEXT(BRepAdaptor_Surface, Adaptor3d_Surface)
public:
  //! Creates an undefined surface with no face loaded.
  Standard_EXPORT BRepAdaptor_Surface();

  //! Creates a surface to  access the geometry  of <F>.
  //! If  <Restriction> is  true  the parameter range is
  //! the  parameter  range  in   the  UV space  of  the
  //! restriction.
  Standard_EXPORT BRepAdaptor_Surface(const TopoDS_Face&     F,
                                      const Standard_Boolean R = Standard_True);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual Handle(Adaptor3d_Surface) ShallowCopy() const Standard_OVERRIDE;

  //! Sets the surface to the geometry of <F>.
  Standard_EXPORT void Initialize(const TopoDS_Face&     F,
                                  const Standard_Boolean Restriction = Standard_True);

  //! Returns the surface.
  Standard_EXPORT const GeomAdaptor_Surface& Surface() const;

  //! Returns the surface.
  Standard_EXPORT GeomAdaptor_Surface& ChangeSurface();

  //! Returns the surface coordinate system.
  Standard_EXPORT const gp_Trsf& Trsf() const;

  //! Returns the face.
  Standard_EXPORT const TopoDS_Face& Face() const;

  //! Returns the face tolerance.
  Standard_EXPORT Standard_Real Tolerance() const;

  virtual Standard_Real FirstUParameter() const Standard_OVERRIDE
  {
    return mySurf.FirstUParameter();
  }

  virtual Standard_Real LastUParameter() const Standard_OVERRIDE { return mySurf.LastUParameter(); }

  virtual Standard_Real FirstVParameter() const Standard_OVERRIDE
  {
    return mySurf.FirstVParameter();
  }

  virtual Standard_Real LastVParameter() const Standard_OVERRIDE { return mySurf.LastVParameter(); }

  virtual GeomAbs_Shape UContinuity() const Standard_OVERRIDE { return mySurf.UContinuity(); }

  virtual GeomAbs_Shape VContinuity() const Standard_OVERRIDE { return mySurf.VContinuity(); }

  //! If necessary, breaks the surface in U intervals of
  //! continuity    <S>.  And   returns  the  number  of
  //! intervals.
  virtual Standard_Integer NbUIntervals(const GeomAbs_Shape theSh) const Standard_OVERRIDE
  {
    return mySurf.NbUIntervals(theSh);
  }

  //! If necessary, breaks the surface in V intervals of
  //! continuity    <S>.  And   returns  the  number  of
  //! intervals.
  virtual Standard_Integer NbVIntervals(const GeomAbs_Shape theSh) const Standard_OVERRIDE
  {
    return mySurf.NbVIntervals(theSh);
  }

  //! Returns the  intervals with the requested continuity
  //! in the U direction.
  Standard_EXPORT void UIntervals(TColStd_Array1OfReal& T,
                                  const GeomAbs_Shape   S) const Standard_OVERRIDE;

  //! Returns the  intervals with the requested continuity
  //! in the V direction.
  Standard_EXPORT void VIntervals(TColStd_Array1OfReal& T,
                                  const GeomAbs_Shape   S) const Standard_OVERRIDE;

  //! Returns    a  surface trimmed in the U direction
  //! equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_Surface) UTrim(const Standard_Real First,
                                                  const Standard_Real Last,
                                                  const Standard_Real Tol) const Standard_OVERRIDE;

  //! Returns    a  surface trimmed in the V direction  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_Surface) VTrim(const Standard_Real First,
                                                  const Standard_Real Last,
                                                  const Standard_Real Tol) const Standard_OVERRIDE;

  virtual Standard_Boolean IsUClosed() const Standard_OVERRIDE { return mySurf.IsUClosed(); }

  virtual Standard_Boolean IsVClosed() const Standard_OVERRIDE { return mySurf.IsVClosed(); }

  virtual Standard_Boolean IsUPeriodic() const Standard_OVERRIDE { return mySurf.IsUPeriodic(); }

  virtual Standard_Real UPeriod() const Standard_OVERRIDE { return mySurf.UPeriod(); }

  virtual Standard_Boolean IsVPeriodic() const Standard_OVERRIDE { return mySurf.IsVPeriodic(); }

  virtual Standard_Real VPeriod() const Standard_OVERRIDE { return mySurf.VPeriod(); }

  //! Computes the point of parameters U,V on the surface.
  //! Tip: use GeomLib::NormEstim() to calculate surface normal at specified (U, V) point.
  Standard_EXPORT gp_Pnt Value(const Standard_Real U,
                               const Standard_Real V) const Standard_OVERRIDE;

  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT void D0(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P) const Standard_OVERRIDE;

  //! Computes the point  and the first derivatives on the surface.
  //! Raised if the continuity of the current intervals is not C1.
  //!
  //! Tip: use GeomLib::NormEstim() to calculate surface normal at specified (U, V) point.
  Standard_EXPORT void D1(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P,
                          gp_Vec&             D1U,
                          gp_Vec&             D1V) const Standard_OVERRIDE;

  //! Computes   the point,  the  first  and  second
  //! derivatives on the surface.
  //! Raised  if   the   continuity   of the current
  //! intervals is not C2.
  Standard_EXPORT void D2(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P,
                          gp_Vec&             D1U,
                          gp_Vec&             D1V,
                          gp_Vec&             D2U,
                          gp_Vec&             D2V,
                          gp_Vec&             D2UV) const Standard_OVERRIDE;

  //! Computes the point,  the first, second and third
  //! derivatives on the surface.
  //! Raised  if   the   continuity   of the current
  //! intervals is not C3.
  Standard_EXPORT void D3(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P,
                          gp_Vec&             D1U,
                          gp_Vec&             D1V,
                          gp_Vec&             D2U,
                          gp_Vec&             D2V,
                          gp_Vec&             D2UV,
                          gp_Vec&             D3U,
                          gp_Vec&             D3V,
                          gp_Vec&             D3UUV,
                          gp_Vec&             D3UVV) const Standard_OVERRIDE;

  //! Computes the derivative of order Nu in the direction
  //! U and Nv in the direction V at the point P(U, V).
  //! Raised if the current U  interval is not not CNu
  //! and the current V interval is not CNv.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec DN(const Standard_Real    U,
                            const Standard_Real    V,
                            const Standard_Integer Nu,
                            const Standard_Integer Nv) const Standard_OVERRIDE;

  //! Returns the parametric U  resolution corresponding
  //! to the real space resolution <R3d>.
  virtual Standard_Real UResolution(const Standard_Real theR3d) const Standard_OVERRIDE
  {
    return mySurf.UResolution(theR3d);
  }

  //! Returns the parametric V  resolution corresponding
  //! to the real space resolution <R3d>.
  virtual Standard_Real VResolution(const Standard_Real theR3d) const Standard_OVERRIDE
  {
    return mySurf.VResolution(theR3d);
  }

  //! Returns the type of the surface : Plane, Cylinder,
  //! Cone,      Sphere,        Torus,    BezierSurface,
  //! BSplineSurface,               SurfaceOfRevolution,
  //! SurfaceOfExtrusion, OtherSurface
  virtual GeomAbs_SurfaceType GetType() const Standard_OVERRIDE { return mySurf.GetType(); }

  Standard_EXPORT gp_Pln Plane() const Standard_OVERRIDE;

  Standard_EXPORT gp_Cylinder Cylinder() const Standard_OVERRIDE;

  Standard_EXPORT gp_Cone Cone() const Standard_OVERRIDE;

  Standard_EXPORT gp_Sphere Sphere() const Standard_OVERRIDE;

  Standard_EXPORT gp_Torus Torus() const Standard_OVERRIDE;

  virtual Standard_Integer UDegree() const Standard_OVERRIDE { return mySurf.UDegree(); }

  virtual Standard_Integer NbUPoles() const Standard_OVERRIDE { return mySurf.NbUPoles(); }

  virtual Standard_Integer VDegree() const Standard_OVERRIDE { return mySurf.VDegree(); }

  virtual Standard_Integer NbVPoles() const Standard_OVERRIDE { return mySurf.NbVPoles(); }

  virtual Standard_Integer NbUKnots() const Standard_OVERRIDE { return mySurf.NbUKnots(); }

  virtual Standard_Integer NbVKnots() const Standard_OVERRIDE { return mySurf.NbVKnots(); }

  virtual Standard_Boolean IsURational() const Standard_OVERRIDE { return mySurf.IsURational(); }

  virtual Standard_Boolean IsVRational() const Standard_OVERRIDE { return mySurf.IsVRational(); }

  Standard_EXPORT Handle(Geom_BezierSurface) Bezier() const Standard_OVERRIDE;

  //! Warning : this will make a copy of the
  //! BSpline Surface since it applies
  //! to it the myTsrf transformation
  //! Be Careful when using this method
  Standard_EXPORT Handle(Geom_BSplineSurface) BSpline() const Standard_OVERRIDE;

  Standard_EXPORT gp_Ax1 AxeOfRevolution() const Standard_OVERRIDE;

  Standard_EXPORT gp_Dir Direction() const Standard_OVERRIDE;

  //! only for SurfaceOfExtrusion and SurfaceOfRevolution
  //! Warning: this will make a copy of the underlying curve
  //! since it applies to it the transformation
  //! myTrsf. Be careful when using this method.
  Standard_EXPORT Handle(Adaptor3d_Curve) BasisCurve() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Adaptor3d_Surface) BasisSurface() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Real OffsetValue() const Standard_OVERRIDE;

private:
  GeomAdaptor_Surface mySurf;
  gp_Trsf             myTrsf;
  TopoDS_Face         myFace;
};

#endif // _BRepAdaptor_Surface_HeaderFile
