// Created on: 1991-12-02
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _AppParCurves_MultiCurve_HeaderFile
#define _AppParCurves_MultiCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_HArray1OfMultiPoint.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_Array1OfMultiPoint.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class AppParCurves_MultiPoint;
class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;

//! This class describes a MultiCurve approximating a Multiline.
//! As a Multiline is a set of n lines, a MultiCurve is a set
//! of n curves. These curves are Bezier curves.
//! A MultiCurve is composed of m MultiPoint.
//! The approximating degree of these n curves is the same for
//! each one.
//!
//! Example of a MultiCurve composed of MultiPoints:
//!
//! P1______P2_____P3______P4________........_____PNbMPoints
//!
//! Q1______Q2_____Q3______Q4________........_____QNbMPoints
//! .                                               .
//! .                                               .
//! .                                               .
//! R1______R2_____R3______R4________........_____RNbMPoints
//!
//! Pi, Qi, ..., Ri are points of dimension 2 or 3.
//!
//! (Pi, Qi, ...Ri), i= 1,...NbPoles are MultiPoints.
//! each MultiPoint has got NbPol Poles.
class AppParCurves_MultiCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! returns an indefinite MultiCurve.
  Standard_EXPORT AppParCurves_MultiCurve();

  //! creates a MultiCurve, describing Bezier curves all
  //! containing the same number of MultiPoint.
  //! An exception is raised if Degree < 0.
  Standard_EXPORT AppParCurves_MultiCurve(const Standard_Integer NbPol);

  //! creates a MultiCurve, describing Bezier curves all
  //! containing the same number of MultiPoint.
  //! Each MultiPoint must have NbCurves Poles.
  Standard_EXPORT AppParCurves_MultiCurve(const AppParCurves_Array1OfMultiPoint& tabMU);
  Standard_EXPORT virtual ~AppParCurves_MultiCurve();

  //! The number of poles of the MultiCurve
  //! will be set to <nbPoles>.
  Standard_EXPORT void SetNbPoles(const Standard_Integer nbPoles);

  //! sets the MultiPoint of range Index to the value
  //! <MPoint>.
  //! An exception is raised if Index <0 or Index >NbMPoint.
  Standard_EXPORT void SetValue(const Standard_Integer         Index,
                                const AppParCurves_MultiPoint& MPoint);

  //! Returns the number of curves resulting from the
  //! approximation of a MultiLine.
  Standard_EXPORT Standard_Integer NbCurves() const;

  //! Returns the number of poles on curves resulting from the approximation of a MultiLine.
  Standard_EXPORT virtual Standard_Integer NbPoles() const;

  //! returns the degree of the curves.
  Standard_EXPORT virtual Standard_Integer Degree() const;

  //! returns the dimension of the CuIndex curve.
  //! An exception is raised if CuIndex<0 or CuIndex>NbCurves.
  Standard_EXPORT Standard_Integer Dimension(const Standard_Integer CuIndex) const;

  //! returns the Pole array of the curve of range CuIndex.
  //! An exception is raised if the dimension of the curve
  //! is 2d.
  Standard_EXPORT void Curve(const Standard_Integer CuIndex, TColgp_Array1OfPnt& TabPnt) const;

  //! returns the Pole array of the curve of range CuIndex.
  //! An exception is raised if the dimension of the curve
  //! is 3d.
  Standard_EXPORT void Curve(const Standard_Integer CuIndex, TColgp_Array1OfPnt2d& TabPnt) const;

  //! returns the Index MultiPoint.
  //! An exception is raised if Index <0 or Index >Degree+1.
  Standard_EXPORT const AppParCurves_MultiPoint& Value(const Standard_Integer Index) const;

  //! returns the Nieme pole of the CuIndex curve.
  //! the curve must be a 3D curve.
  Standard_EXPORT const gp_Pnt& Pole(const Standard_Integer CuIndex,
                                     const Standard_Integer Nieme) const;

  //! returns the Nieme pole of the CuIndex curve.
  //! the curve must be a 2D curve.
  Standard_EXPORT const gp_Pnt2d& Pole2d(const Standard_Integer CuIndex,
                                         const Standard_Integer Nieme) const;

  //! Applies a transformation to the curve of range
  //! <CuIndex>.
  //! newx = x + dx*oldx
  //! newy = y + dy*oldy    for all points of the curve.
  //! newz = z + dz*oldz
  Standard_EXPORT void Transform(const Standard_Integer CuIndex,
                                 const Standard_Real    x,
                                 const Standard_Real    dx,
                                 const Standard_Real    y,
                                 const Standard_Real    dy,
                                 const Standard_Real    z,
                                 const Standard_Real    dz);

  //! Applies a transformation to the Curve of range
  //! <CuIndex>.
  //! newx = x + dx*oldx
  //! newy = y + dy*oldy    for all points of the curve.
  Standard_EXPORT void Transform2d(const Standard_Integer CuIndex,
                                   const Standard_Real    x,
                                   const Standard_Real    dx,
                                   const Standard_Real    y,
                                   const Standard_Real    dy);

  //! returns the value of the point with a parameter U
  //! on the Bezier curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 2d.
  Standard_EXPORT virtual void Value(const Standard_Integer CuIndex,
                                     const Standard_Real    U,
                                     gp_Pnt&                Pt) const;

  //! returns the value of the point with a parameter U
  //! on the Bezier curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 3d.
  Standard_EXPORT virtual void Value(const Standard_Integer CuIndex,
                                     const Standard_Real    U,
                                     gp_Pnt2d&              Pt) const;

  //! returns the value of the point with a parameter U
  //! on the Bezier curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 3d.
  Standard_EXPORT virtual void D1(const Standard_Integer CuIndex,
                                  const Standard_Real    U,
                                  gp_Pnt&                Pt,
                                  gp_Vec&                V1) const;

  //! returns the value of the point with a parameter U
  //! on the Bezier curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 2d.
  Standard_EXPORT virtual void D1(const Standard_Integer CuIndex,
                                  const Standard_Real    U,
                                  gp_Pnt2d&              Pt,
                                  gp_Vec2d&              V1) const;

  //! returns the value of the point with a parameter U
  //! on the Bezier curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 3d.
  Standard_EXPORT virtual void D2(const Standard_Integer CuIndex,
                                  const Standard_Real    U,
                                  gp_Pnt&                Pt,
                                  gp_Vec&                V1,
                                  gp_Vec&                V2) const;

  //! returns the value of the point with a parameter U
  //! on the Bezier curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 2d.
  Standard_EXPORT virtual void D2(const Standard_Integer CuIndex,
                                  const Standard_Real    U,
                                  gp_Pnt2d&              Pt,
                                  gp_Vec2d&              V1,
                                  gp_Vec2d&              V2) const;

  //! Prints on the stream o information on the current
  //! state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT virtual void Dump(Standard_OStream& o) const;

protected:
  Handle(AppParCurves_HArray1OfMultiPoint) tabPoint;

private:
};

#endif // _AppParCurves_MultiCurve_HeaderFile
