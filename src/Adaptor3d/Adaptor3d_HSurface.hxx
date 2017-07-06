// Created on: 1994-02-14
// Created by: model
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Adaptor3d_HSurface_HeaderFile
#define _Adaptor3d_HSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Standard_NotImplemented;
class Adaptor3d_Surface;
class gp_Pnt;
class gp_Vec;
class Geom_BezierSurface;
class Geom_BSplineSurface;
class Adaptor3d_HCurve;


class Adaptor3d_HSurface;
DEFINE_STANDARD_HANDLE(Adaptor3d_HSurface, Standard_Transient)

//! Root class for surfaces manipulated by handles, on
//! which geometric algorithms work.
//! An adapted surface is an interface between the
//! services provided by a surface and those required of
//! the surface by algorithms which use it.
//! A derived concrete class is provided:
//! GeomAdaptor_HSurface for a surface from the Geom package.
class Adaptor3d_HSurface : public Standard_Transient
{

public:

  
  //! Returns a reference to the Surface inside the HSurface.
  Standard_EXPORT virtual const Adaptor3d_Surface& Surface() const = 0;
  
    Standard_Real FirstUParameter() const;
  
    Standard_Real LastUParameter() const;
  
    Standard_Real FirstVParameter() const;
  
    Standard_Real LastVParameter() const;
  
    GeomAbs_Shape UContinuity() const;
  
    GeomAbs_Shape VContinuity() const;
  
    Standard_Integer NbUIntervals (const GeomAbs_Shape S) const;
  
    Standard_Integer NbVIntervals (const GeomAbs_Shape S) const;
  
    void UIntervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const;
  
    void VIntervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const;
  
    Handle(Adaptor3d_HSurface) UTrim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const;
  
    Handle(Adaptor3d_HSurface) VTrim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const;
  
    Standard_Boolean IsUClosed() const;
  
    Standard_Boolean IsVClosed() const;
  
    Standard_Boolean IsUPeriodic() const;
  
    Standard_Real UPeriod() const;
  
    Standard_Boolean IsVPeriodic() const;
  
    Standard_Real VPeriod() const;
  
    gp_Pnt Value (const Standard_Real U, const Standard_Real V) const;
  
    void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const;
  
    void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const;
  
    void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const;
  
    void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const;
  
    gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const;
  
    Standard_Real UResolution (const Standard_Real R3d) const;
  
    Standard_Real VResolution (const Standard_Real R3d) const;
  
    GeomAbs_SurfaceType GetType() const;
  
    gp_Pln Plane() const;
  
    gp_Cylinder Cylinder() const;
  
    gp_Cone Cone() const;
  
    gp_Sphere Sphere() const;
  
    gp_Torus Torus() const;
  
    Standard_Integer UDegree() const;
  
    Standard_Integer NbUPoles() const;
  
    Standard_Integer VDegree() const;
  
    Standard_Integer NbVPoles() const;
  
    Standard_Integer NbUKnots() const;
  
    Standard_Integer NbVKnots() const;
  
    Standard_Boolean IsURational() const;
  
    Standard_Boolean IsVRational() const;
  
    Handle(Geom_BezierSurface) Bezier() const;
  
    Handle(Geom_BSplineSurface) BSpline() const;
  
    gp_Ax1 AxeOfRevolution() const;
  
    gp_Dir Direction() const;
  
    Handle(Adaptor3d_HCurve) BasisCurve() const;
  
    Handle(Adaptor3d_HSurface) BasisSurface() const;
  
    Standard_Real OffsetValue() const;




  DEFINE_STANDARD_RTTIEXT(Adaptor3d_HSurface,Standard_Transient)

protected:




private:




};


#include <Adaptor3d_HSurface.lxx>





#endif // _Adaptor3d_HSurface_HeaderFile
