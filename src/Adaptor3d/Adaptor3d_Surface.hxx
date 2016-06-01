// Created on: 1993-03-31
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

#ifndef _Adaptor3d_Surface_HeaderFile
#define _Adaptor3d_Surface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_SurfaceType.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
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


//! Root class for surfaces on which geometric algorithms work.
//! An adapted surface is an interface between the
//! services provided by a surface and those required of
//! the surface by algorithms which use it.
//! A derived concrete class is provided:
//! GeomAdaptor_Surface for a surface from the Geom package.
//! The  Surface class describes  the standard behaviour
//! of a surface for generic algorithms.
//!
//! The Surface can  be decomposed in intervals of any
//! continuity    in  U    and    V using  the  method
//! NbIntervals.  A current interval can be set.  Most
//! of the methods apply to the current interval.
//! Warning: All the methods are virtual and implemented with a
//! raise to allow to redefined only the methods realy
//! used.
//!
//! Polynomial coefficients of BSpline surfaces used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class Adaptor3d_Surface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT virtual Standard_Real FirstUParameter() const;
  
  Standard_EXPORT virtual Standard_Real LastUParameter() const;
  
  Standard_EXPORT virtual Standard_Real FirstVParameter() const;
  
  Standard_EXPORT virtual Standard_Real LastVParameter() const;
  
  Standard_EXPORT virtual GeomAbs_Shape UContinuity() const;
  
  Standard_EXPORT virtual GeomAbs_Shape VContinuity() const;
  
  //! Returns the number of U intervals for  continuity
  //! <S>. May be one if UContinuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbUIntervals (const GeomAbs_Shape S) const;
  
  //! Returns the number of V intervals for  continuity
  //! <S>. May be one if VContinuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbVIntervals (const GeomAbs_Shape S) const;
  
  //! Returns the  intervals with the requested continuity
  //! in the U direction.
  Standard_EXPORT virtual void UIntervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const;
  
  //! Returns the  intervals with the requested continuity
  //! in the V direction.
  Standard_EXPORT virtual void VIntervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const;
  
  //! Returns    a  surface trimmed in the U direction
  //! equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT virtual Handle(Adaptor3d_HSurface) UTrim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const;
  
  //! Returns    a  surface trimmed in the V direction  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT virtual Handle(Adaptor3d_HSurface) VTrim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const;
  
  Standard_EXPORT virtual Standard_Boolean IsUClosed() const;
  
  Standard_EXPORT virtual Standard_Boolean IsVClosed() const;
  
  Standard_EXPORT virtual Standard_Boolean IsUPeriodic() const;
  
  Standard_EXPORT virtual Standard_Real UPeriod() const;
  
  Standard_EXPORT virtual Standard_Boolean IsVPeriodic() const;
  
  Standard_EXPORT virtual Standard_Real VPeriod() const;
  
  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT virtual gp_Pnt Value (const Standard_Real U, const Standard_Real V) const;
  
  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT virtual void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const;
  
  //! Computes the point  and the first derivatives on
  //! the surface.
  //! Raised   if  the continuity  of   the  current
  //! intervals is not C1.
  Standard_EXPORT virtual void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const;
  
  //! Computes   the point,  the  first  and  second
  //! derivatives on the surface.
  //! Raised  if   the   continuity   of the current
  //! intervals is not C2.
  Standard_EXPORT virtual void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const;
  
  //! Computes the point,  the first, second and third
  //! derivatives on the surface.
  //! Raised  if   the   continuity   of the current
  //! intervals is not C3.
  Standard_EXPORT virtual void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const;
  
  //! Computes the derivative of order Nu in the direction U and Nv
  //! in the direction V at the point P(U, V).
  //! Raised if the current U  interval is not not CNu
  //! and the current V interval is not CNv.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT virtual gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const;
  
  //! Returns the parametric U  resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT virtual Standard_Real UResolution (const Standard_Real R3d) const;
  
  //! Returns the parametric V  resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT virtual Standard_Real VResolution (const Standard_Real R3d) const;
  
  //! Returns the type of the surface : Plane, Cylinder,
  //! Cone,      Sphere,        Torus,    BezierSurface,
  //! BSplineSurface,               SurfaceOfRevolution,
  //! SurfaceOfExtrusion, OtherSurface
  Standard_EXPORT virtual GeomAbs_SurfaceType GetType() const;
  
  Standard_EXPORT virtual gp_Pln Plane() const;
  
  Standard_EXPORT virtual gp_Cylinder Cylinder() const;
  
  Standard_EXPORT virtual gp_Cone Cone() const;
  
  Standard_EXPORT virtual gp_Sphere Sphere() const;
  
  Standard_EXPORT virtual gp_Torus Torus() const;
  
  Standard_EXPORT virtual Standard_Integer UDegree() const;
  
  Standard_EXPORT virtual Standard_Integer NbUPoles() const;
  
  Standard_EXPORT virtual Standard_Integer VDegree() const;
  
  Standard_EXPORT virtual Standard_Integer NbVPoles() const;
  
  Standard_EXPORT virtual Standard_Integer NbUKnots() const;
  
  Standard_EXPORT virtual Standard_Integer NbVKnots() const;
  
  Standard_EXPORT virtual Standard_Boolean IsURational() const;
  
  Standard_EXPORT virtual Standard_Boolean IsVRational() const;
  
  Standard_EXPORT virtual Handle(Geom_BezierSurface) Bezier() const;
  
  Standard_EXPORT virtual Handle(Geom_BSplineSurface) BSpline() const;
  
  Standard_EXPORT virtual gp_Ax1 AxeOfRevolution() const;
  
  Standard_EXPORT virtual gp_Dir Direction() const;
  
  Standard_EXPORT virtual Handle(Adaptor3d_HCurve) BasisCurve() const;
  
  Standard_EXPORT virtual Handle(Adaptor3d_HSurface) BasisSurface() const;
  
  Standard_EXPORT virtual Standard_Real OffsetValue() const;
  Standard_EXPORT virtual ~Adaptor3d_Surface();




protected:





private:





};







#endif // _Adaptor3d_Surface_HeaderFile
