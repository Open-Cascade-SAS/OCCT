// Created on: 1994-02-18
// Created by: Remi LEQUETTE
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

#ifndef _BRepAlgo_Section_HeaderFile
#define _BRepAlgo_Section_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <BRepAlgo_BooleanOperation.hxx>
class TopoDS_Shape;
class gp_Pln;
class Geom_Surface;


//! Construction of the section lines between two shapes.
//! For this Boolean operation, each face of the first
//! shape is intersected by each face of the second
//! shape. The resulting intersection edges are brought
//! together into a compound object, but not chained or
//! grouped into wires.
//! Computation of the intersection of two Shapes or Surfaces
//! The two parts involved in this Boolean operation may
//! be defined from geometric surfaces: the most common
//! use is the computation of the planar section of a shape.
//! A Section object provides the framework for:
//! - defining the shapes to be intersected, and the
//! computation options,
//! - implementing the construction algorithm, and
//! - consulting the result.
//! Example : giving two shapes S1,S2 accessing faces,
//! let compute the section edges R on S1,S2,
//! performing approximation on new curves,
//! performing PCurve on part 1 but not on part 2 :
//! Standard_Boolean PerformNow = Standard_False;
//! BRepBoolAPI_Section S(S1,S2,PerformNow);
//! S.ComputePCurveOn1(Standard_True);
//! S.Approximation(Standard_True);
//! S.Build();
//! TopoDS_Shape R = S.Shape();
//! On Null Shapes of geometries, NotDone() is called.
class BRepAlgo_Section  : public BRepAlgo_BooleanOperation
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepAlgo_Section(const TopoDS_Shape& Sh1, const TopoDS_Shape& Sh2, const Standard_Boolean PerformNow = Standard_True);
  
  Standard_EXPORT BRepAlgo_Section(const TopoDS_Shape& Sh, const gp_Pln& Pl, const Standard_Boolean PerformNow = Standard_True);
  
  Standard_EXPORT BRepAlgo_Section(const TopoDS_Shape& Sh, const Handle(Geom_Surface)& Sf, const Standard_Boolean PerformNow = Standard_True);
  
  Standard_EXPORT BRepAlgo_Section(const Handle(Geom_Surface)& Sf, const TopoDS_Shape& Sh, const Standard_Boolean PerformNow = Standard_True);
  
  //! This and the above algorithms construct a framework for computing the section lines of
  //! - the two shapes Sh1 and Sh2, or
  //! - the shape Sh and the plane Pl, or
  //! - the shape Sh and the surface Sf, or
  //! - the surface Sf and the shape Sh, or
  //! - the two surfaces Sf1 and Sf2,
  //! and builds the result if PerformNow equals true, its
  //! default value. If PerformNow equals false, the
  //! intersection will be computed later by the function Build.
  //! The constructed shape will be returned by the
  //! function Shape. This is a compound object
  //! composed of edges. These intersection edges may be built:
  //! - on new intersection lines, or
  //! - on coincident portions of edges in the two intersected shapes.
  //! These intersection edges are independent: they
  //! are not chained or grouped in wires.
  //! If no intersection edge exists, the result is an empty compound object.
  //! Note that other objects than TopoDS_Shape
  //! shapes involved in these syntaxes are converted
  //! into faces or shells before performing the
  //! computation of the intersection. A shape resulting
  //! from this conversion can be retrieved with the
  //! function Shape1 or Shape2.
  //! Parametric 2D curves on intersection edges
  //! No parametric 2D curve (pcurve) is defined for
  //! each elementary edge of the result. To attach such
  //! parametric curves to the constructed edges you
  //! may use a constructor with the PerformNow flag
  //! equal to false; then you use:
  //! - the function ComputePCurveOn1 to ask for the
  //! additional computation of a pcurve in the
  //! parametric space of the first shape,
  //! - the function ComputePCurveOn2 to ask for the
  //! additional computation of a pcurve in the
  //! parametric space of the second shape,
  //! - in the end, the function Build to construct the result.
  //! Note that as a result, pcurves will only be added on
  //! edges built on new intersection lines.
  //! Approximation of intersection edges
  //! The underlying 3D geometry attached to each
  //! elementary edge of the result is:
  //! - analytic where possible, provided the
  //! corresponding geometry corresponds to a type
  //! of analytic curve defined in the Geom package;
  //! for example, the intersection of a cylindrical
  //! shape with a plane gives an ellipse or a circle;
  //! - or elsewhere, given as a succession of points
  //! grouped together in a BSpline curve of degree 1.
  //! If you prefer to have an attached 3D geometry
  //! which is a BSpline approximation of the computed
  //! set of points on computed elementary intersection
  //! edges whose underlying geometry is not analytic,
  //! you may use a constructor with the PerformNow
  //! flag equal to false. Then you use:
  //! - the function Approximation to ask for this
  //! computation option, and
  //! - the function Build to construct the result.
  //! Note that as a result, approximations will only be
  //! computed on edges built on new intersection lines.
  //! Example
  //! You may also combine these computation options.
  //! In the following example:
  //! - each elementary edge of the computed
  //! intersection, built on a new intersection line,
  //! which does not correspond to an analytic Geom
  //! curve, will be approximated by a BSpline curve
  //! whose degree is not greater than 8.
  //! - each elementary edge built on a new intersection line, will have:
  //! - a pcurve in the parametric space of the shape S1,
  //! - no pcurve in the parametric space of the shape S2.
  //! // TopoDS_Shape S1 = ... , S2 = ... ;
  //! Standard_Boolean PerformNow = Standard_False;
  //! BRepAlgo_Section S ( S1, S2, PerformNow );
  //! S.ComputePCurveOn1 (Standard_True);
  //! S.Approximation (Standard_True);
  //! S.Build();
  //! TopoDS_Shape R = S.Shape();
  Standard_EXPORT BRepAlgo_Section(const Handle(Geom_Surface)& Sf1, const Handle(Geom_Surface)& Sf2, const Standard_Boolean PerformNow = Standard_True);
  
  //! Initializes the first part
  Standard_EXPORT void Init1 (const TopoDS_Shape& S1);
  
  //! Initializes the first part
  Standard_EXPORT void Init1 (const gp_Pln& Pl);
  
  //! Initializes the first part
  Standard_EXPORT void Init1 (const Handle(Geom_Surface)& Sf);
  
  //! initialize second part
  Standard_EXPORT void Init2 (const TopoDS_Shape& S2);
  
  //! Initializes the second part
  Standard_EXPORT void Init2 (const gp_Pln& Pl);
  
  //! This and the above algorithms
  //! reinitialize the first and the second parts on which
  //! this algorithm is going to perform the intersection
  //! computation. This is done with either: the surface
  //! Sf, the plane Pl or the shape Sh.
  //! You use the function Build to construct the result.
  Standard_EXPORT void Init2 (const Handle(Geom_Surface)& Sf);
  
  //! Defines an option for computation of further
  //! intersections. This computation will be performed by
  //! the function Build in this framework.
  //! By default, the underlying 3D geometry attached to
  //! each elementary edge of the result of a computed intersection is:
  //! - analytic where possible, provided the
  //! corresponding geometry corresponds to a type of
  //! analytic curve defined in the Geom package; for
  //! example the intersection of a cylindrical shape with
  //! a plane gives an ellipse or a circle;
  //! - or elsewhere, given as a succession of points
  //! grouped together in a BSpline curve of degree 1. If
  //! Approx equals true, when further computations are
  //! performed in this framework with the function
  //! Build, these edges will have an attached 3D
  //! geometry which is a BSpline approximation of the
  //! computed set of points.
  //! Note that as a result, approximations will be computed
  //! on edges built only on new intersection lines.
  Standard_EXPORT void Approximation (const Standard_Boolean B);
  
  //! Indicates if the Pcurve must be (or not) performed on first part.
  Standard_EXPORT void ComputePCurveOn1 (const Standard_Boolean B);
  
  //! Define options for the computation of further
  //! intersections which will be performed by the function
  //! Build in this framework.
  //! By default, no parametric 2D curve (pcurve) is defined
  //! for the elementary edges of the result.
  //! If ComputePCurve1 equals true, further computations
  //! performed in this framework with the function Build
  //! will attach an additional pcurve in the parametric
  //! space of the first shape to the constructed edges.
  //! If ComputePCurve2 equals true, the additional pcurve
  //! will be attached to the constructed edges in the
  //! parametric space of the second shape.
  //! These two functions may be used together.
  //! Note that as a result, pcurves will only be added onto
  //! edges built on new intersection lines.
  Standard_EXPORT void ComputePCurveOn2 (const Standard_Boolean B);
  
  //! Performs the computation of the section lines
  //! between the two parts defined at the time of
  //! construction of this framework or reinitialized with the
  //! Init1 and Init2 functions.
  //! The constructed shape will be returned by the function
  //! Shape. This is a compound object composed of
  //! edges. These intersection edges may be built:
  //! - on new intersection lines, or
  //! - on coincident portions of edges in the two intersected shapes.
  //! These intersection edges are independent: they are
  //! not chained or grouped into wires.
  //! If no intersection edge exists, the result is an empty compound object.
  //! The shapes involved in the construction of the section
  //! lines can be retrieved with the function Shape1 or
  //! Shape2. Note that other objects than
  //! TopoDS_Shape shapes given as arguments at the
  //! construction time of this framework, or to the Init1 or
  //! Init2 function, are converted into faces or shells
  //! before performing the computation of the intersection.
  //! Parametric 2D curves on intersection edges
  //! No parametric 2D curve (pcurve) is defined for the
  //! elementary edges of the result. To attach parametric
  //! curves like this to the constructed edges you have to use:
  //! - the function ComputePCurveOn1 to ask for the
  //! additional computation of a pcurve in the
  //! parametric space of the first shape,
  //! - the function ComputePCurveOn2 to ask for the
  //! additional computation of a pcurve in the
  //! parametric space of the second shape.
  //! This must be done before calling this function.
  //! Note that as a result, pcurves are added on edges
  //! built on new intersection lines only.
  //! Approximation of intersection edges
  //! The underlying 3D geometry attached to each
  //! elementary edge of the result is:
  //! - analytic where possible provided the corresponding
  //! geometry corresponds to a type of analytic curve
  //! defined in the Geom package; for example, the
  //! intersection of a cylindrical shape with a plane
  //! gives an ellipse or a circle; or
  //! - elsewhere, given as a succession of points grouped
  //! together in a BSpline curve of degree 1.
  //! If, on computed elementary intersection edges whose
  //! underlying geometry is not analytic, you prefer to
  //! have an attached 3D geometry which is a BSpline
  //! approximation of the computed set of points, you have
  //! to use the function Approximation to ask for this
  //! computation option before calling this function.
  //! You may also have combined these computation
  //! options: look at the example given above to illustrate
  //! the use of the constructors.
  Standard_EXPORT void Build() Standard_OVERRIDE;
  
  //! Identifies the ancestor faces of the new
  //! intersection edge E resulting from the last
  //! computation performed in this framework, that is,
  //! the faces of the two original shapes on which the edge E lies:
  //! - HasAncestorFaceOn1 gives the ancestor face
  //! in the first shape, and
  //! These functions return:
  //! - true if an ancestor face F is found, or
  //! - false if not.
  //! An ancestor face is identifiable for the edge E if the
  //! three following conditions are satisfied:
  //! - the first part on which this algorithm performed
  //! its last computation is a shape, that is, it was not
  //! given as a surface or a plane at the time of
  //! construction of this algorithm or at a later time by
  //! the Init1 function,
  //! - E is one of the elementary edges built by the last
  //! computation of this section algorithm,
  //! - the edge E is built on an intersection curve. In
  //! other words, E is a new edge built on the
  //! intersection curve, not on edges belonging to the
  //! intersecting shapes.
  //! To use these functions properly, you have to test
  //! the returned Boolean value before using the
  //! ancestor face: F is significant only if the returned
  //! Boolean value equals true.
  Standard_EXPORT Standard_Boolean HasAncestorFaceOn1 (const TopoDS_Shape& E, TopoDS_Shape& F) const;
  
  //! Identifies the ancestor faces of the new
  //! intersection edge E resulting from the last
  //! computation performed in this framework, that is,
  //! the faces of the two original shapes on which the edge E lies:
  //! - HasAncestorFaceOn2 gives the ancestor face in the second shape.
  //! These functions return:
  //! - true if an ancestor face F is found, or
  //! - false if not.
  //! An ancestor face is identifiable for the edge E if the
  //! three following conditions are satisfied:
  //! - the first part on which this algorithm performed
  //! its last computation is a shape, that is, it was not
  //! given as a surface or a plane at the time of
  //! construction of this algorithm or at a later time by
  //! the Init1 function,
  //! - E is one of the elementary edges built by the last
  //! computation of this section algorithm,
  //! - the edge E is built on an intersection curve. In
  //! other words, E is a new edge built on the
  //! intersection curve, not on edges belonging to the
  //! intersecting shapes.
  //! To use these functions properly, you have to test
  //! the returned Boolean value before using the
  //! ancestor face: F is significant only if the returned
  //! Boolean value equals true.
  Standard_EXPORT Standard_Boolean HasAncestorFaceOn2 (const TopoDS_Shape& E, TopoDS_Shape& F) const;




protected:





private:

  
  Standard_EXPORT virtual void InitParameters() Standard_OVERRIDE;


  Standard_Boolean myS1Changed;
  Standard_Boolean myS2Changed;
  Standard_Boolean myApproxChanged;
  Standard_Boolean myPCurve1Changed;
  Standard_Boolean myPCurve2Changed;
  Standard_Boolean myshapeisnull;


};







#endif // _BRepAlgo_Section_HeaderFile
