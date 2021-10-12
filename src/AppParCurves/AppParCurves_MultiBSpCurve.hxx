// Created on: 1993-09-20
// Created by: Modelistation
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

#ifndef _AppParCurves_MultiBSpCurve_HeaderFile
#define _AppParCurves_MultiBSpCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_Array1OfMultiPoint.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;


//! This class describes a MultiBSpCurve approximating a Multiline.
//! Just as a Multiline is a set of a given number of lines, a MultiBSpCurve is a set
//! of a specified number of bsplines defined by:
//! -   A specified number of MultiPoints - the poles of a specified number of curves
//! -   The degree of approximation identical for each of the specified number of curves.
//!
//! Example of a MultiBSpCurve composed of a specified number of MultiPoints:
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
//! MultiBSpCurves are created by the SplineValue method in the ComputeLine
//! class, and by the Value method in TheVariational class. MultiBSpCurve
//! provides the information required to create the BSpline defined by the approximation.
class AppParCurves_MultiBSpCurve  : public AppParCurves_MultiCurve
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns an indefinite MultiBSpCurve.
  Standard_EXPORT AppParCurves_MultiBSpCurve();
  
  //! creates a MultiBSpCurve, describing BSpline curves all
  //! containing the same number of MultiPoint.
  //! An exception is raised if Degree < 0.
  Standard_EXPORT AppParCurves_MultiBSpCurve(const Standard_Integer NbPol);
  
  //! creates a MultiBSpCurve, describing BSpline curves all
  //! containing the same number of MultiPoint.
  //! Each MultiPoint must have NbCurves Poles.
  Standard_EXPORT AppParCurves_MultiBSpCurve(const AppParCurves_Array1OfMultiPoint& tabMU, const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults);
  
  //! creates a MultiBSpCurve, describing BSpline
  //! curves, taking control points from <SC>.
  Standard_EXPORT AppParCurves_MultiBSpCurve(const AppParCurves_MultiCurve& SC, const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults);
  
  //! Knots of the multiBSpCurve are assigned to <theknots>.
  Standard_EXPORT void SetKnots (const TColStd_Array1OfReal& theKnots);
  
  //! Multiplicities of the multiBSpCurve are assigned
  //! to <theMults>.
  Standard_EXPORT void SetMultiplicities (const TColStd_Array1OfInteger& theMults);
  
  //! Returns an array of Reals containing
  //! the multiplicities of curves resulting from the approximation.
  Standard_EXPORT const TColStd_Array1OfReal& Knots() const;
  
  //! Returns an array of Reals containing the
  //! multiplicities of curves resulting from the approximation.
  Standard_EXPORT const TColStd_Array1OfInteger& Multiplicities() const;
  
  //! returns the degree of the curve(s).
  Standard_EXPORT virtual Standard_Integer Degree() const Standard_OVERRIDE;
  
  //! returns the value of the point with a parameter U
  //! on the BSpline curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 2d.
  Standard_EXPORT virtual void Value (const Standard_Integer CuIndex, const Standard_Real U, gp_Pnt& Pt) const Standard_OVERRIDE;
  
  //! returns the value of the point with a parameter U
  //! on the BSpline curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 3d.
  Standard_EXPORT virtual void Value (const Standard_Integer CuIndex, const Standard_Real U, gp_Pnt2d& Pt) const Standard_OVERRIDE;
  
  //! returns the value of the point with a parameter U
  //! on the BSpline curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 3d.
  Standard_EXPORT virtual void D1 (const Standard_Integer CuIndex, const Standard_Real U, gp_Pnt& Pt, gp_Vec& V1) const Standard_OVERRIDE;
  
  //! returns the value of the point with a parameter U
  //! on the BSpline curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 2d.
  Standard_EXPORT virtual void D1 (const Standard_Integer CuIndex, const Standard_Real U, gp_Pnt2d& Pt, gp_Vec2d& V1) const Standard_OVERRIDE;
  
  //! returns the value of the point with a parameter U
  //! on the BSpline curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 3d.
  Standard_EXPORT virtual void D2 (const Standard_Integer CuIndex, const Standard_Real U, gp_Pnt& Pt, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  
  //! returns the value of the point with a parameter U
  //! on the BSpline curve number CuIndex.
  //! An exception is raised if CuIndex <0 or > NbCurves.
  //! An exception is raised if the curve dimension is 2d.
  Standard_EXPORT virtual void D2 (const Standard_Integer CuIndex, const Standard_Real U, gp_Pnt2d& Pt, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  
  //! Prints on the stream o information on the current
  //! state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT virtual void Dump (Standard_OStream& o) const Standard_OVERRIDE;




protected:





private:



  Handle(TColStd_HArray1OfReal) myknots;
  Handle(TColStd_HArray1OfInteger) mymults;
  Standard_Integer myDegree;


};







#endif // _AppParCurves_MultiBSpCurve_HeaderFile
