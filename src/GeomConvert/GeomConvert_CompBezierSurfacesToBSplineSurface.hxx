// Created on: 1996-06-06
// Created by: Philippe MANGIN
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

#ifndef _GeomConvert_CompBezierSurfacesToBSplineSurface_HeaderFile
#define _GeomConvert_CompBezierSurfacesToBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColGeom_Array2OfBezierSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>


//! An algorithm to convert a grid of adjacent
//! non-rational Bezier surfaces (with continuity CM) into a
//! BSpline surface (with continuity CM).
//! A CompBezierSurfacesToBSplineSurface object
//! provides a framework for:
//! -   defining the grid of adjacent Bezier surfaces
//! which is to be converted into a BSpline surface,
//! -   implementing the computation algorithm, and
//! -   consulting the results.
//! Warning
//! Do not attempt to convert rational Bezier surfaces using such an algorithm.
//! Input is array of Bezier patch
//! 1    2    3     4  -> VIndex [1, NbVPatches] -> VDirection
//! -----------------------
//! 1    |    |    |    |      |
//! -----------------------
//! 2    |    |    |    |      |
//! -----------------------
//! 3    |    |    |    |      |
//! -----------------------
//! UIndex [1, NbUPatches]  Udirection
//!
//! Warning! Patches must have compatible parametrization
class GeomConvert_CompBezierSurfacesToBSplineSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes all the data needed to build a "C0"
  //! continuous BSpline surface equivalent to the grid of
  //! adjacent non-rational Bezier surfaces Beziers.
  //! Each surface in the Beziers grid becomes a natural
  //! patch, limited by knots values, on the BSpline surface
  //! whose data is computed. Surfaces in the grid must
  //! satisfy the following conditions:
  //! -   Coincident bounding curves between two
  //! consecutive surfaces in a row of the Beziers grid
  //! must be u-isoparametric bounding curves of these two surfaces.
  //! -   Coincident bounding curves between two
  //! consecutive surfaces in a column of the Beziers
  //! grid must be v-isoparametric bounding curves of these two surfaces.
  //! The BSpline surface whose data is computed has the
  //! following characteristics:
  //! -   Its degree in the u (respectively v) parametric
  //! direction is equal to that of the Bezier surface
  //! which has the highest degree in the u
  //! (respectively v) parametric direction in the Beziers grid.
  //! -   It is a "Piecewise Bezier" in both u and v
  //! parametric directions, i.e.:
  //! -   the knots are regularly spaced in each
  //! parametric direction (i.e. the difference between
  //! two consecutive knots is a constant), and
  //! -   all the multiplicities of the surface knots in a
  //! given parametric direction are equal to
  //! Degree, which is the degree of the BSpline
  //! surface in this parametric direction, except for
  //! the first and last knots for which the multiplicity is
  //! equal to Degree + 1.
  //! -   Coincident bounding curves between two
  //! consecutive columns of Bezier surfaces in the
  //! Beziers grid become u-isoparametric curves,
  //! corresponding to knots values of the BSpline surface.
  //! -   Coincident bounding curves between two
  //! consecutive rows of Bezier surfaces in the Beziers
  //! grid become v-isoparametric curves
  //! corresponding to knots values of the BSpline surface.
  //! Use the available consultation functions to access the
  //! computed data. This data may be used to construct the BSpline surface.
  //! Warning
  //! The surfaces in the Beziers grid must be adjacent, i.e.
  //! two consecutive Bezier surfaces in the grid (in a row
  //! or column) must have a coincident bounding curve. In
  //! addition, the location of the parameterization on each
  //! of these surfaces (i.e. the relative location of u and v
  //! isoparametric curves on the surface) is of importance
  //! with regard to the positioning of the surfaces in the
  //! Beziers grid. Care must be taken with respect to the
  //! above, as these properties are not checked and an
  //! error may occur if they are not satisfied.
  //! Exceptions
  //! Standard_NotImplemented if one of the Bezier
  //! surfaces of the Beziers grid is rational.
  Standard_EXPORT GeomConvert_CompBezierSurfacesToBSplineSurface(const TColGeom_Array2OfBezierSurface& Beziers);
  
  //! Build an Ci uniform (Rational) BSpline surface
  //! The highest Continuity Ci is imposed, like the
  //! maximal deformation is lower than <Tolerance>.
  //! Warning:  The Continuity C0 is imposed without any check.
  Standard_EXPORT GeomConvert_CompBezierSurfacesToBSplineSurface(const TColGeom_Array2OfBezierSurface& Beziers, const Standard_Real Tolerance, const Standard_Boolean RemoveKnots = Standard_True);
  
  //! Computes all the data needed to construct a BSpline
  //! surface equivalent to the adjacent non-rational
  //! Bezier surfaces Beziers grid.
  //! Each surface in the Beziers grid becomes a natural
  //! patch, limited by knots values, on the BSpline surface
  //! whose data is computed. Surfaces in the grid must
  //! satisfy the following conditions:
  //! -   Coincident bounding curves between two
  //! consecutive surfaces in a row of the Beziers grid
  //! must be u-isoparametric bounding curves of these two surfaces.
  //! -   Coincident bounding curves between two
  //! consecutive surfaces in a column of the Beziers
  //! grid must be v-isoparametric bounding curves of these two surfaces.
  //! The BSpline surface whose data is computed has the
  //! following characteristics:
  //! -   Its degree in the u (respectively v) parametric
  //! direction is equal to that of the Bezier surface
  //! which has the highest degree in the u
  //! (respectively v) parametric direction in the Beziers grid.
  //! -   Coincident bounding curves between two
  //! consecutive columns of Bezier surfaces in the
  //! Beziers grid become u-isoparametric curves
  //! corresponding to knots values of the BSpline surface.
  //! -   Coincident bounding curves between two
  //! consecutive rows of Bezier surfaces in the Beziers
  //! grid become v-isoparametric curves
  //! corresponding to knots values of the BSpline surface.
  //! Knots values of the BSpline surface are given in the two tables:
  //! -   UKnots for the u parametric direction (which
  //! corresponds to the order of Bezier surface columns in the Beziers grid), and
  //! -   VKnots for the v parametric direction (which
  //! corresponds to the order of Bezier surface rows in the Beziers grid).
  //! The dimensions of UKnots (respectively VKnots)
  //! must be equal to the number of columns (respectively,
  //! rows) of the Beziers grid, plus 1 .
  //! UContinuity and VContinuity, which are both
  //! defaulted to GeomAbs_C0, specify the required
  //! continuity on the BSpline surface. If the required
  //! degree of continuity is greater than 0 in a given
  //! parametric direction, a deformation is applied locally
  //! on the initial surface (as defined by the Beziers grid)
  //! to satisfy this condition. This local deformation is not
  //! applied however, if it is greater than Tolerance
  //! (defaulted to 1.0 e-7). In such cases, the
  //! continuity condition is not satisfied, and the function
  //! IsDone will return false. A small tolerance value
  //! prevents any modification of the surface and a large
  //! tolerance value "smoothes" the surface.
  //! Use the available consultation functions to access the
  //! computed data. This data may be used to construct the BSpline surface.
  //! Warning
  //! The surfaces in the Beziers grid must be adjacent, i.e.
  //! two consecutive Bezier surfaces in the grid (in a row
  //! or column) must have a coincident bounding curve. In
  //! addition, the location of the parameterization on each
  //! of these surfaces (i.e. the relative location of u and v
  //! isoparametric curves on the surface) is of importance
  //! with regard to the positioning of the surfaces in the
  //! Beziers grid. Care must be taken with respect to the
  //! above, as these properties are not checked and an
  //! error may occur if they are not satisfied.
  //! Exceptions
  //! Standard_DimensionMismatch:
  //! -   if the number of knots in the UKnots table (i.e. the
  //! length of the UKnots array) is not equal to the
  //! number of columns of Bezier surfaces in the
  //! Beziers grid plus 1, or
  //! -   if the number of knots in the VKnots table (i.e. the
  //! length of the VKnots array) is not equal to the
  //! number of rows of Bezier surfaces in the Beziers grid, plus 1.
  //! Standard_ConstructionError:
  //! -   if UContinuity and VContinuity are not equal to
  //! one of the following values: GeomAbs_C0,
  //! GeomAbs_C1, GeomAbs_C2 and GeomAbs_C3; or
  //! -   if the number of columns in the Beziers grid is
  //! greater than 1, and the required degree of
  //! continuity in the u parametric direction is greater
  //! than that of the Bezier surface with the highest
  //! degree in the u parametric direction (in the Beziers grid), minus 1; or
  //! -   if the number of rows in the Beziers grid is
  //! greater than 1, and the required degree of
  //! continuity in the v parametric direction is greater
  //! than that of the Bezier surface with the highest
  //! degree in the v parametric direction (in the Beziers grid), minus 1 .
  //! Standard_NotImplemented if one of the Bezier
  //! surfaces in the Beziers grid is rational.
  Standard_EXPORT GeomConvert_CompBezierSurfacesToBSplineSurface(const TColGeom_Array2OfBezierSurface& Beziers, const TColStd_Array1OfReal& UKnots, const TColStd_Array1OfReal& VKnots, const GeomAbs_Shape UContinuity = GeomAbs_C0, const GeomAbs_Shape VContinuity = GeomAbs_C0, const Standard_Real Tolerance = 1.0e-4);
  
  //! Returns the number of knots in the U direction
  //! of the BSpline surface whose data is computed in this framework.
    Standard_Integer NbUKnots() const;
  
  //! Returns number of poles in the U direction
  //! of the BSpline surface whose data is computed in this framework.
    Standard_Integer NbUPoles() const;
  
  //! Returns the number of knots in the V direction
  //! of the BSpline surface whose data is computed in this framework.
    Standard_Integer NbVKnots() const;
  
  //! Returns the number of poles in the V direction
  //! of the BSpline surface whose data is computed in this framework.
    Standard_Integer NbVPoles() const;
  
  //! Returns the table of poles of the BSpline surface
  //! whose data is computed in this framework.
    const Handle(TColgp_HArray2OfPnt)& Poles() const;
  
  //! Returns the knots table for the u parametric
  //! direction of the BSpline surface whose data is computed in this framework.
    const Handle(TColStd_HArray1OfReal)& UKnots() const;
  
  //! Returns the degree for the u  parametric
  //! direction of the BSpline surface whose data is computed in this framework.
    Standard_Integer UDegree() const;
  
  //! Returns the knots table for the v parametric
  //! direction of the BSpline surface whose data is computed in this framework.
    const Handle(TColStd_HArray1OfReal)& VKnots() const;
  
  //! Returns the degree for the v  parametric
  //! direction of the BSpline surface whose data is computed in this framework.
    Standard_Integer VDegree() const;
  

  //! Returns the multiplicities table for the u
  //! parametric direction of the knots of the BSpline
  //! surface whose data is computed in this framework.
    const Handle(TColStd_HArray1OfInteger)& UMultiplicities() const;
  
  //! -- Returns the multiplicities table for the v
  //! parametric direction of the knots of the BSpline
  //! surface whose data is computed in this framework.
    const Handle(TColStd_HArray1OfInteger)& VMultiplicities() const;
  
  //! Returns true if the conversion was successful.
  //! Unless an exception was raised at the time of
  //! construction, the conversion of the Bezier surface
  //! grid assigned to this algorithm is always carried out.
  //! IsDone returns false if the constraints defined at the
  //! time of construction cannot be respected. This occurs
  //! when there is an incompatibility between a required
  //! degree of continuity on the BSpline surface, and the
  //! maximum tolerance accepted for local deformations
  //! of the surface. In such a case the computed data
  //! does not satisfy all the initial constraints.
  Standard_EXPORT Standard_Boolean IsDone() const;




protected:





private:

  
  //! It used internally by the constructors.
  Standard_EXPORT void Perform (const TColGeom_Array2OfBezierSurface& Beziers);


  Standard_Integer myUDegree;
  Standard_Integer myVDegree;
  Handle(TColStd_HArray1OfInteger) myVMults;
  Handle(TColStd_HArray1OfInteger) myUMults;
  Handle(TColStd_HArray1OfReal) myUKnots;
  Handle(TColStd_HArray1OfReal) myVKnots;
  Handle(TColgp_HArray2OfPnt) myPoles;
  Standard_Boolean isrational;
  Standard_Boolean myDone;


};


#include <GeomConvert_CompBezierSurfacesToBSplineSurface.lxx>





#endif // _GeomConvert_CompBezierSurfacesToBSplineSurface_HeaderFile
