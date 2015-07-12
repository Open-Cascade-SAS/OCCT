// Created on: 1996-03-12
// Created by: Bruno DUMORTIER
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _GeomConvert_BSplineSurfaceToBezierSurface_HeaderFile
#define _GeomConvert_BSplineSurfaceToBezierSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColGeom_Array2OfBezierSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
class Geom_BSplineSurface;
class Standard_DimensionError;
class Standard_DomainError;
class Standard_OutOfRange;
class Geom_BezierSurface;



//! This algorithm converts a B-spline surface into several
//! Bezier surfaces. It uses an algorithm of knot insertion.
//! A BSplineSurfaceToBezierSurface object provides a framework for:
//! -   defining the BSpline surface to be converted,
//! -   implementing the construction algorithm, and
//! -   consulting the results.
//! References :
//! Generating the Bezier points of B-spline curves and surfaces
//! (Wolfgang Bohm) CAD volume 13 number 6 november 1981
class GeomConvert_BSplineSurfaceToBezierSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes all the data needed to convert
  //! -   the BSpline surface BasisSurface into a series of adjacent Bezier surfaces.
  //! The result consists of a grid of BasisSurface patches
  //! limited by isoparametric curves corresponding to knot
  //! values, both in the u and v parametric directions of
  //! the surface. A row in the grid corresponds to a series
  //! of adjacent patches, all limited by the same two
  //! u-isoparametric curves. A column in the grid
  //! corresponds to a series of adjacent patches, all
  //! limited by the same two v-isoparametric curves.
  //! Use the available interrogation functions to ascertain
  //! the number of computed Bezier patches, and then to
  //! construct each individual Bezier surface (or all Bezier surfaces).
  //! Note: ParametricTolerance is not used.
  Standard_EXPORT GeomConvert_BSplineSurfaceToBezierSurface(const Handle(Geom_BSplineSurface)& BasisSurface);
  
  //! Computes all the data needed to convert
  //! the patch of the BSpline surface BasisSurface
  //! limited by the two parameter values U1 and U2 in
  //! the u parametric direction, and by the two
  //! parameter values V1 and V2 in the v parametric
  //! direction, into a series of adjacent Bezier surfaces.
  //! The result consists of a grid of BasisSurface patches
  //! limited by isoparametric curves corresponding to knot
  //! values, both in the u and v parametric directions of
  //! the surface. A row in the grid corresponds to a series
  //! of adjacent patches, all limited by the same two
  //! u-isoparametric curves. A column in the grid
  //! corresponds to a series of adjacent patches, all
  //! limited by the same two v-isoparametric curves.
  //! Use the available interrogation functions to ascertain
  //! the number of computed Bezier patches, and then to
  //! construct each individual Bezier surface (or all Bezier surfaces).
  //! Note: ParametricTolerance is not used.  Raises DomainError
  //! if U1 or U2 or V1 or V2 are out of the parametric bounds
  //! of the basis surface [FirstUKnotIndex, LastUKnotIndex] ,
  //! [FirstVKnotIndex, LastVKnotIndex] The tolerance criterion is
  //! ParametricTolerance.
  //! Raised if U2 - U1 <= ParametricTolerance or
  //! V2 - V1 <= ParametricTolerance.
  Standard_EXPORT GeomConvert_BSplineSurfaceToBezierSurface(const Handle(Geom_BSplineSurface)& BasisSurface, const Standard_Real U1, const Standard_Real U2, const Standard_Real V1, const Standard_Real V2, const Standard_Real ParametricTolerance);
  
  //! Constructs and returns the Bezier surface of indices
  //! (UIndex, VIndex) to the patch grid computed on the
  //! BSpline surface analyzed by this algorithm.
  //! This Bezier surface has the same orientation as the
  //! BSpline surface analyzed in this framework.
  //! UIndex is an index common to a row in the patch
  //! grid. A row in the grid corresponds to a series of
  //! adjacent patches, all limited by the same two
  //! u-isoparametric curves of the surface. VIndex is an
  //! index common to a column in the patch grid. A column
  //! in the grid corresponds to a series of adjacent
  //! patches, all limited by the same two v-isoparametric
  //! curves of the surface.
  //! Exceptions
  //! Standard_OutOfRange if:
  //! -   UIndex is less than 1 or greater than the number
  //! of rows in the patch grid computed on the BSpline
  //! surface analyzed by this algorithm (as returned by
  //! the function NbUPatches); or if
  //! -   VIndex is less than 1 or greater than the number
  //! of columns in the patch grid computed on the
  //! BSpline surface analyzed by this algorithm (as
  //! returned by the function NbVPatches).
  Standard_EXPORT Handle(Geom_BezierSurface) Patch (const Standard_Integer UIndex, const Standard_Integer VIndex);
  
  //! Constructs all the Bezier surfaces whose data is
  //! computed by this algorithm, and loads them into the Surfaces table.
  //! These Bezier surfaces have the same orientation as
  //! the BSpline surface analyzed in this framework.
  //! The Surfaces array is organised in the same way as
  //! the patch grid computed on the BSpline surface
  //! analyzed by this algorithm. A row in the array
  //! corresponds to a series of adjacent patches, all
  //! limited by the same two u-isoparametric curves of
  //! the surface. A column in the array corresponds to a
  //! series of adjacent patches, all limited by the same two
  //! v-isoparametric curves of the surface.
  //! Exceptions
  //! Standard_DimensionError if the Surfaces array
  //! was not created with the following bounds:
  //! -   1, and the number of adjacent patch series in the
  //! u parametric direction of the patch grid computed
  //! on the BSpline surface, analyzed by this algorithm
  //! (as given by the function NbUPatches) as row bounds,
  //! -   1, and the number of adjacent patch series in the
  //! v parametric direction of the patch grid computed
  //! on the BSpline surface, analyzed by this algorithm
  //! (as given by the function NbVPatches) as column bounds.
  Standard_EXPORT void Patches (TColGeom_Array2OfBezierSurface& Surfaces);
  
  //! This methode returns the bspline's u-knots associated to
  //! the converted Patches
  //! Raised  if the length  of Curves is not equal to
  //! NbUPatches +  1.
  Standard_EXPORT void UKnots (TColStd_Array1OfReal& TKnots) const;
  
  //! This methode returns the bspline's v-knots associated to
  //! the converted Patches
  //! Raised  if the length  of Curves is not equal to
  //! NbVPatches +  1.
  Standard_EXPORT void VKnots (TColStd_Array1OfReal& TKnots) const;
  

  //! Returns the number of Bezier surfaces in the U direction.
  //! If at the creation time you have decomposed the basis Surface
  //! between the parametric values UFirst, ULast the number of
  //! Bezier surfaces in the U direction depends on the number of
  //! knots included inside the interval [UFirst, ULast].
  //! If you have decomposed the whole basis B-spline surface the
  //! number of Bezier surfaces NbUPatches is equal to the number of
  //! UKnots less one.
  Standard_EXPORT Standard_Integer NbUPatches() const;
  

  //! Returns the number of Bezier surfaces in the V direction.
  //! If at the creation time you have decomposed the basis surface
  //! between the parametric values VFirst, VLast the number of
  //! Bezier surfaces in the V direction depends on the number of
  //! knots included inside the interval [VFirst, VLast].
  //! If you have decomposed the whole basis B-spline surface the
  //! number of Bezier surfaces NbVPatches is equal to the number of
  //! VKnots less one.
  Standard_EXPORT Standard_Integer NbVPatches() const;




protected:





private:



  Handle(Geom_BSplineSurface) mySurface;


};







#endif // _GeomConvert_BSplineSurfaceToBezierSurface_HeaderFile
