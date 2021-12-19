// Created on: 1994-09-01
// Created by: Christian CAILLET
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

#ifndef _IGESConvGeom_HeaderFile
#define _IGESConvGeom_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class IGESGeom_SplineCurve;
class Geom_BSplineCurve;
class Geom2d_BSplineCurve;
class IGESGeom_SplineSurface;
class Geom_BSplineSurface;


//! This package is intended to gather geometric conversion which
//! are not immediate but can be used for several purposes :
//! mainly, standard conversion to and from CasCade geometric and
//! topologic data, and adaptations of IGES files as required
//! (as replacing Spline entities to BSpline equivalents).
class IGESConvGeom 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! basic tool to build curves from IGESGeom (arrays of points,
  //! Transformations, evaluation of points in a datum)
  //! Converts a SplineCurve from IGES to a BSplineCurve from CasCade
  //! <epscoef> gives tolerance to consider coefficient to be nul
  //! <epsgeom> gives tolerance to consider poles to be equal
  //! The returned value is a status with these possible values :
  //! - 0  OK, done
  //! - 1  the result is not guaranteed to be C0 (with <epsgeom>)
  //! - 2  SplineType not processed (allowed : max 3)
  //! (no result produced)
  //! - 3  error during creation of control points
  //! (no result produced)
  //! - 4  polynomial equation is not correct (no result produced)
  //! - 5  less than one segment (no result produced)
  Standard_EXPORT static Standard_Integer SplineCurveFromIGES (const Handle(IGESGeom_SplineCurve)& igesent, const Standard_Real epscoef, const Standard_Real epsgeom, Handle(Geom_BSplineCurve)& result);
  
  //! Tries to increase curve continuity with tolerance <epsgeom>
  //! <continuity> is the new desired continuity, can be 1 or 2
  //! (more than 2 is considered as 2).
  //! Returns the new maximum continuity obtained on all knots.
  //! Remark that, for instance with <continuity> = 2, even if not
  //! all the knots can be passed to C2, all knots which can be are.
  Standard_EXPORT static Standard_Integer IncreaseCurveContinuity (const Handle(Geom_BSplineCurve)& curve, const Standard_Real epsgeom, const Standard_Integer continuity = 2);
  
  Standard_EXPORT static Standard_Integer IncreaseCurveContinuity (const Handle(Geom2d_BSplineCurve)& curve, const Standard_Real epsgeom, const Standard_Integer continuity = 2);
  
  //! Converts a SplineSurface from IGES to a BSplineSurface from CasCade
  //! <epscoef> gives tolerance to consider coefficient to be nul
  //! <epsgeom> gives tolerance to consider poles to be equal
  //! The returned value is a status with these possible values :
  //! - 0  OK, done
  //! - 1  the result is not guaranteed to be C0 (with <epsgeom>)
  //! - 2  degree is not compatible with code boundary type
  //! (warning) but C0 is OK
  //! - 3  idem but C0 is not guaranteed (warning)
  //! - 4  degree has been determined to be nul, either in U or V
  //! (no result produced)
  //! - 5  less than one segment in U or V (no result produced)
  Standard_EXPORT static Standard_Integer SplineSurfaceFromIGES (const Handle(IGESGeom_SplineSurface)& igesent, const Standard_Real epscoef, const Standard_Real epsgeom, Handle(Geom_BSplineSurface)& result);
  
  //! Tries to increase Surface continuity with tolerance <epsgeom>
  //! <continuity> is the new desired continuity, can be 1 or 2
  //! (more than 2 is considered as 2).
  //! Returns the new maximum continuity obtained on all knots.
  //! Remark that, for instance with <continuity> = 2, even if not
  //! all the knots can be passed to C2, all knots which can be are.
  Standard_EXPORT static Standard_Integer IncreaseSurfaceContinuity (const Handle(Geom_BSplineSurface)& surface, const Standard_Real epsgeom, const Standard_Integer continuity = 2);

};

#endif // _IGESConvGeom_HeaderFile
