// Created on: 1995-01-16
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomAPI_PointsToBSplineSurface_HeaderFile
#define _GeomAPI_PointsToBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Real.hxx>
#include <Approx_ParametrizationType.hxx>
#include <TColStd_Array2OfReal.hxx>
class Geom_BSplineSurface;
class StdFail_NotDone;


//! This class is used to approximate or interpolate
//! a BSplineSurface passing through an  Array2 of
//! points, with a given continuity.
//! Describes functions for building a BSpline
//! surface which approximates or interpolates a set of points.
//! A PointsToBSplineSurface object provides a framework for:
//! -   defining the data of the BSpline surface to be built,
//! -   implementing the approximation algorithm
//! or the interpolation algorithm, and consulting the results.
class GeomAPI_PointsToBSplineSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty algorithm for
  //! approximation or interpolation of a surface.
  //! Use:
  //! -   an Init function to define and build the
  //! BSpline surface by approximation, or
  //! -   an Interpolate function to define and build
  //! the BSpline surface by interpolation.
  Standard_EXPORT GeomAPI_PointsToBSplineSurface();
  
  //! Approximates  a BSpline  Surface passing  through  an
  //! array of  Points.  The resulting BSpline will  have
  //! the following properties:
  //! 1- his degree will be in the range [Degmin,Degmax]
  //! 2- his  continuity will be  at  least <Continuity>
  //! 3- the distance from the point <Points> to the
  //! BSpline will be lower to Tol3D
  Standard_EXPORT GeomAPI_PointsToBSplineSurface(const TColgp_Array2OfPnt& Points, const Standard_Integer DegMin = 3, const Standard_Integer DegMax = 8, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Real Tol3D = 1.0e-3);
  
  //! Approximates  a BSpline  Surface passing  through  an
  //! array of  Points.  The resulting BSpline will  have
  //! the following properties:
  //! 1- his degree will be in the range [Degmin,Degmax]
  //! 2- his  continuity will be  at  least <Continuity>
  //! 3- the distance from the point <Points> to the
  //! BSpline will be lower to Tol3D
  Standard_EXPORT GeomAPI_PointsToBSplineSurface(const TColgp_Array2OfPnt& Points, const Approx_ParametrizationType ParType, const Standard_Integer DegMin = 3, const Standard_Integer DegMax = 8, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Real Tol3D = 1.0e-3);
  
  //! Approximates  a BSpline  Surface passing  through  an
  //! array of  points using variational smoothing algorithm,
  //! which tries to minimize additional criterium:
  //! Weight1*CurveLength + Weight2*Curvature + Weight3*Torsion
  Standard_EXPORT GeomAPI_PointsToBSplineSurface(const TColgp_Array2OfPnt& Points, const Standard_Real Weight1, const Standard_Real Weight2, const Standard_Real Weight3, const Standard_Integer DegMax = 8, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Real Tol3D = 1.0e-3);
  
  //! Approximates  a BSpline  Surface passing  through  an
  //! array of  Points.
  //!
  //! The points will be constructed as follow:
  //! P(i,j) = gp_Pnt( X0 + (i-1)*dX ,
  //! Y0 + (j-1)*dY ,
  //! ZPoints(i,j)   )
  //!
  //! The resulting BSpline will  have the following
  //! properties:
  //! 1- his degree will be in the range [Degmin,Degmax]
  //! 2- his  continuity will be  at  least <Continuity>
  //! 3- the distance from the point <Points> to the
  //! BSpline will be lower to Tol3D
  //! 4- the parametrization of the surface will verify:
  //! S->Value( U, V) = gp_Pnt( U, V, Z(U,V) );
  Standard_EXPORT GeomAPI_PointsToBSplineSurface(const TColStd_Array2OfReal& ZPoints, const Standard_Real X0, const Standard_Real dX, const Standard_Real Y0, const Standard_Real dY, const Standard_Integer DegMin = 3, const Standard_Integer DegMax = 8, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Real Tol3D = 1.0e-3);
  
  //! Approximates  a BSpline Surface passing  through  an
  //! array of  Point.  The resulting BSpline will  have
  //! the following properties:
  //! 1- his degree will be in the range [Degmin,Degmax]
  //! 2- his  continuity will be  at  least <Continuity>
  //! 3- the distance from the point <Points> to the
  //! BSpline will be lower to Tol3D
  Standard_EXPORT void Init (const TColgp_Array2OfPnt& Points, const Standard_Integer DegMin = 3, const Standard_Integer DegMax = 8, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Real Tol3D = 1.0e-3);
  
  //! Interpolates  a BSpline Surface passing  through  an
  //! array of  Point.  The resulting BSpline will  have
  //! the following properties:
  //! 1- his degree will be 3.
  //! 2- his  continuity will be  C2.
  Standard_EXPORT void Interpolate (const TColgp_Array2OfPnt& Points);
  
  //! Interpolates  a BSpline Surface passing  through  an
  //! array of  Point.  The resulting BSpline will  have
  //! the following properties:
  //! 1- his degree will be 3.
  //! 2- his  continuity will be  C2.
  Standard_EXPORT void Interpolate (const TColgp_Array2OfPnt& Points, const Approx_ParametrizationType ParType);
  
  //! Approximates  a BSpline  Surface passing  through  an
  //! array of  Points.
  //!
  //! The points will be constructed as follow:
  //! P(i,j) = gp_Pnt( X0 + (i-1)*dX ,
  //! Y0 + (j-1)*dY ,
  //! ZPoints(i,j)   )
  //!
  //! The resulting BSpline will  have the following
  //! properties:
  //! 1- his degree will be in the range [Degmin,Degmax]
  //! 2- his  continuity will be  at  least <Continuity>
  //! 3- the distance from the point <Points> to the
  //! BSpline will be lower to Tol3D
  //! 4- the parametrization of the surface will verify:
  //! S->Value( U, V) = gp_Pnt( U, V, Z(U,V) );
  Standard_EXPORT void Init (const TColStd_Array2OfReal& ZPoints, const Standard_Real X0, const Standard_Real dX, const Standard_Real Y0, const Standard_Real dY, const Standard_Integer DegMin = 3, const Standard_Integer DegMax = 8, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Real Tol3D = 1.0e-3);
  
  //! Interpolates  a BSpline  Surface passing  through  an
  //! array of  Points.
  //!
  //! The points will be constructed as follow:
  //! P(i,j) = gp_Pnt( X0 + (i-1)*dX ,
  //! Y0 + (j-1)*dY ,
  //! ZPoints(i,j)   )
  //!
  //! The resulting BSpline will  have the following
  //! properties:
  //! 1- his degree will be 3
  //! 2- his  continuity will be  C2.
  //! 4- the parametrization of the surface will verify:
  //! S->Value( U, V) = gp_Pnt( U, V, Z(U,V) );
  Standard_EXPORT void Interpolate (const TColStd_Array2OfReal& ZPoints, const Standard_Real X0, const Standard_Real dX, const Standard_Real Y0, const Standard_Real dY);
  
  //! Approximates  a BSpline Surface passing  through  an
  //! array of  Point.  The resulting BSpline will  have
  //! the following properties:
  //! 1- his degree will be in the range [Degmin,Degmax]
  //! 2- his  continuity will be  at  least <Continuity>
  //! 3- the distance from the point <Points> to the
  //! BSpline will be lower to Tol3D
  Standard_EXPORT void Init (const TColgp_Array2OfPnt& Points, const Approx_ParametrizationType ParType, const Standard_Integer DegMin = 3, const Standard_Integer DegMax = 8, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Real Tol3D = 1.0e-3);
  
  //! Approximates  a BSpline Surface passing  through  an
  //! array of  point using variational smoothing algorithm,
  //! which tries to minimize additional criterium:
  //! Weight1*CurveLength + Weight2*Curvature + Weight3*Torsion
  Standard_EXPORT void Init (const TColgp_Array2OfPnt& Points, const Standard_Real Weight1, const Standard_Real Weight2, const Standard_Real Weight3, const Standard_Integer DegMax = 8, const GeomAbs_Shape Continuity = GeomAbs_C2, const Standard_Real Tol3D = 1.0e-3);
  
  //! Returns the approximate BSpline Surface
  Standard_EXPORT const Handle(Geom_BSplineSurface)& Surface() const;
Standard_EXPORT operator Handle(Geom_BSplineSurface)() const;
  
  Standard_EXPORT Standard_Boolean IsDone() const;




protected:





private:



  Standard_Boolean myIsDone;
  Handle(Geom_BSplineSurface) mySurface;


};







#endif // _GeomAPI_PointsToBSplineSurface_HeaderFile
