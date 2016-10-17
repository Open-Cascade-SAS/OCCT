// Created on: 1997-11-21
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepFill_Sweep_HeaderFile
#define _BRepFill_Sweep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <GeomFill_ApproxStyle.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_HArray2OfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepFill_DataMapOfShapeHArray2OfShape.hxx>
#include <BRepFill_TransitionStyle.hxx>
class BRepFill_LocationLaw;
class BRepFill_SectionLaw;
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_ConstructionError;
class TopoDS_Wire;
class TopoDS_Shape;
class TopoDS_Edge;


//! Topological Sweep Algorithm
//! Computes an  Sweep  shell using a  generating
//! wire, an SectionLaw and an LocationLaw.
class BRepFill_Sweep 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_Sweep(const Handle(BRepFill_SectionLaw)& Section, const Handle(BRepFill_LocationLaw)& Location, const Standard_Boolean WithKPart);
  
  Standard_EXPORT void SetBounds (const TopoDS_Wire& FirstShape, const TopoDS_Wire& LastShape);
  
  //! Set Approximation Tolerance
  //! Tol3d : Tolerance to surface approximation
  //! Tol2d : Tolerance used to perform curve approximation
  //! Normaly the 2d curve are approximated with a
  //! tolerance given by the resolution on support surfaces,
  //! but if this tolerance is too large Tol2d is used.
  //! TolAngular : Tolerance (in radian) to control the angle
  //! beetween tangents on the section law and
  //! tangent of iso-v on approximed surface
  Standard_EXPORT void SetTolerance (const Standard_Real Tol3d, const Standard_Real BoundTol = 1.0, const Standard_Real Tol2d = 1.0e-5, const Standard_Real TolAngular = 1.0e-2);
  
  //! Tolerance  To controle Corner management.
  //!
  //! If the discontinuity is lesser than <AngleMin> in radian The
  //! Transition Performed will be alway "Modified"
  Standard_EXPORT void SetAngularControl (const Standard_Real AngleMin = 0.01, const Standard_Real AngleMax = 6.0);
  
  //! Set the flag that indicates attempt to approximate
  //! a C1-continuous surface if a swept surface proved
  //! to be C0.
  Standard_EXPORT void SetForceApproxC1 (const Standard_Boolean ForceApproxC1);
  
  //! Build the Sweeep  Surface
  //! Transition define Transition strategy
  //! Approx define Approximation Strategy
  //! - GeomFill_Section : The composed Function Location X Section
  //! is directly approximed.
  //! - GeomFill_Location : The location law is approximed, and the
  //! SweepSurface is bulid algebric composition
  //! of approximed location law and section law
  //! This option is Ok, if Section.Surface() methode
  //! is effective.
  //! Continuity : The continuity in v waiting on the surface
  //! Degmax     : The maximum degree in v requiered on the surface
  //! Segmax     : The maximum number of span in v requiered on
  //! the surface.
  Standard_EXPORT void Build (TopTools_MapOfShape& ReversedEdges, BRepFill_DataMapOfShapeHArray2OfShape& Tapes, BRepFill_DataMapOfShapeHArray2OfShape& Rails, const BRepFill_TransitionStyle Transition = BRepFill_Modified, const GeomAbs_Shape Continuity = GeomAbs_C2, const GeomFill_ApproxStyle Approx = GeomFill_Location, const Standard_Integer Degmax = 11, const Standard_Integer Segmax = 30);
  
  //! Say if the Shape is Build.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns the Sweeping Shape
  Standard_EXPORT TopoDS_Shape Shape() const;
  
  //! Get the Approximation  error.
  Standard_EXPORT Standard_Real ErrorOnSurface() const;
  
  Standard_EXPORT Handle(TopTools_HArray2OfShape) SubShape() const;
  
  Standard_EXPORT Handle(TopTools_HArray2OfShape) InterFaces() const;
  
  Standard_EXPORT Handle(TopTools_HArray2OfShape) Sections() const;

  //! returns the Tape corresponding to Index-th edge of section
  Standard_EXPORT TopoDS_Shape Tape(const Standard_Integer Index) const;



protected:





private:

  
  Standard_EXPORT Standard_Boolean CorrectApproxParameters();
  
  Standard_EXPORT Standard_Boolean BuildWire (const BRepFill_TransitionStyle Transition);
  
  Standard_EXPORT Standard_Boolean BuildShell (const BRepFill_TransitionStyle Transition, const Standard_Integer Vf, const Standard_Integer Vl, TopTools_MapOfShape& ReversedEdges, BRepFill_DataMapOfShapeHArray2OfShape& Tapes, BRepFill_DataMapOfShapeHArray2OfShape& Rails, const Standard_Real ExtendFirst = 0.0, const Standard_Real ExtendLast = 0.0);
  
  Standard_EXPORT void PerformCorner (const Standard_Integer Index, const BRepFill_TransitionStyle Transition, const Handle(TopTools_HArray2OfShape)& Bounds);
  
  Standard_EXPORT Standard_Real EvalExtrapol (const Standard_Integer Index, const BRepFill_TransitionStyle Transition) const;
  
  Standard_EXPORT Standard_Boolean MergeVertex (const TopoDS_Shape& V1, TopoDS_Shape& V2) const;
  
  Standard_EXPORT void UpdateVertex (const Standard_Integer Ipath, const Standard_Integer Isec, const Standard_Real Error, const Standard_Real Param, TopoDS_Shape& V) const;
  
  Standard_EXPORT void RebuildTopOrBottomEdge (const TopoDS_Edge& aNewEdge, TopoDS_Edge& anEdge, TopTools_MapOfShape& ReversedEdges) const;


  Standard_Boolean isDone;
  Standard_Boolean KPart;
  Standard_Real myTol3d;
  Standard_Real myBoundTol;
  Standard_Real myTol2d;
  Standard_Real myTolAngular;
  Standard_Real myAngMin;
  Standard_Real myAngMax;
  GeomFill_ApproxStyle myApproxStyle;
  GeomAbs_Shape myContinuity;
  Standard_Integer myDegmax;
  Standard_Integer mySegmax;
  Standard_Boolean myForceApproxC1;
  TopoDS_Shape myShape;
  Handle(BRepFill_LocationLaw) myLoc;
  Handle(BRepFill_SectionLaw) mySec;
  Handle(TopTools_HArray2OfShape) myUEdges;
  Handle(TopTools_HArray2OfShape) myVEdges;
  TopTools_DataMapOfShapeShape myVEdgesModified;
  Handle(TopTools_HArray2OfShape) myFaces;
  TopTools_ListOfShape myAuxShape;
  Handle(TopTools_HArray1OfShape) myTapes;
  Standard_Real Error;
  TopoDS_Wire FirstShape;
  TopoDS_Wire LastShape;


};







#endif // _BRepFill_Sweep_HeaderFile
