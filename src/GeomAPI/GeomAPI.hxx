// Created on: 1994-03-17
// Created by: Bruno DUMORTIER
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

#ifndef _GeomAPI_HeaderFile
#define _GeomAPI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>

class gp_Pln;
class GeomAPI_ProjectPointOnCurve;
class GeomAPI_ProjectPointOnSurf;
class GeomAPI_ExtremaCurveCurve;
class GeomAPI_ExtremaCurveSurface;
class GeomAPI_ExtremaSurfaceSurface;
class GeomAPI_PointsToBSpline;
class GeomAPI_PointsToBSplineSurface;
class GeomAPI_Interpolate;
class GeomAPI_IntSS;
class GeomAPI_IntCS;


//! The   GeomAPI   package  provides  an  Application
//! Programming Interface for the Geometry.
//!
//! The API is a set of  classes and methods aiming to
//! provide :
//!
//! * High level and simple calls  for the most common
//! operations.
//!
//! *    Keeping   an   access  on    the    low-level
//! implementation of high-level calls.
//!
//! The API  provides classes to  call the algorithmes
//! of the Geometry
//!
//! * The  constructors  of the classes  provides  the
//! different constructions methods.
//!
//! * The  class keeps as fields the   different tools
//! used by the algorithmes
//!
//! *   The class  provides  a  casting  method to get
//! automatically the  result  with  a   function-like
//! call.
//!
//! For example to evaluate the distance <D> between a
//! point <P> and a curve <C>, one can writes :
//!
//! D = GeomAPI_ProjectPointOnCurve(P,C);
//!
//! or
//!
//! GeomAPI_ProjectPointOnCurve PonC(P,C);
//! D = PonC.LowerDistance();
class GeomAPI 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This function builds (in the
  //! parametric space of the plane P) a 2D curve equivalent to the 3D curve
  //! C. The 3D curve C is considered to be located in the plane P.
  //! Warning
  //! The 3D curve C must be of one of the following types:
  //! -      a line
  //! -      a circle
  //! -      an ellipse
  //! -      a hyperbola
  //! -      a parabola
  //! -      a Bezier curve
  //! -      a BSpline curve
  //! Exceptions Standard_NoSuchObject if C is not a defined type curve.
  Standard_EXPORT static Handle(Geom2d_Curve) To2d (const Handle(Geom_Curve)& C, const gp_Pln& P);
  
  //! Builds a 3D curve equivalent to the 2D curve C
  //! described in the parametric space defined by the local
  //! coordinate system of plane P.
  //! The resulting 3D curve is of the same nature as that of the curve C.
  Standard_EXPORT static Handle(Geom_Curve) To3d (const Handle(Geom2d_Curve)& C, const gp_Pln& P);




protected:





private:




friend class GeomAPI_ProjectPointOnCurve;
friend class GeomAPI_ProjectPointOnSurf;
friend class GeomAPI_ExtremaCurveCurve;
friend class GeomAPI_ExtremaCurveSurface;
friend class GeomAPI_ExtremaSurfaceSurface;
friend class GeomAPI_PointsToBSpline;
friend class GeomAPI_PointsToBSplineSurface;
friend class GeomAPI_Interpolate;
friend class GeomAPI_IntSS;
friend class GeomAPI_IntCS;

};







#endif // _GeomAPI_HeaderFile
