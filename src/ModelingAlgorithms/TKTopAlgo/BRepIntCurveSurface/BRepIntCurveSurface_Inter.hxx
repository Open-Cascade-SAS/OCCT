// Created on: 1994-02-07
// Created by: Modelistation
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

#ifndef _BRepIntCurveSurface_Inter_HeaderFile
#define _BRepIntCurveSurface_Inter_HeaderFile

#include <GeomAdaptor_Curve.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>

class BRepTopAdaptor_TopolTool;
class TopoDS_Shape;
class GeomAdaptor_Curve;
class gp_Lin;
class IntCurveSurface_IntersectionPoint;
class gp_Pnt;
class TopoDS_Face;

//! Computes the intersection between a face and a
//! curve. To intersect one curve with shape method
//! Init(Shape, curve, tTol) should be used. To
//! intersect a few curves with specified shape it is
//! necessary to load shape one time using method
//! Load(shape, tol) and find intersection points for
//! each curve using method Init(curve). For
//! iteration by intersection points method More() and
//! Next() should be used.
//!
//! Example:
//! Inter.Load(shape, tol);
//! for( i =1; i <= nbCurves;i++)
//! {
//! Inter.Init(curve);
//! for(  ;Inter.More(); Inter.Next())
//! {
//! .......
//! }
//! }
class BRepIntCurveSurface_Inter
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor;
  Standard_EXPORT BRepIntCurveSurface_Inter();

  //! Load the Shape, the curve and initialize the
  //! tolerance used for the classification.
  Standard_EXPORT void Init(const TopoDS_Shape&      theShape,
                            const GeomAdaptor_Curve& theCurve,
                            const Standard_Real      theTol);

  //! Load the Shape, the curve and initialize the
  //! tolerance used for the classification.
  Standard_EXPORT void Init(const TopoDS_Shape& theShape,
                            const gp_Lin&       theLine,
                            const Standard_Real theTol);

  //! Load the Shape, and initialize the
  //! tolerance used for the classification.
  Standard_EXPORT void Load(const TopoDS_Shape& theShape, const Standard_Real theTol);

  //! Method to find intersections of specified curve with loaded shape.
  Standard_EXPORT void Init(const GeomAdaptor_Curve& theCurve);

  //! returns True if there is a current face.
  Standard_EXPORT Standard_Boolean More() const;

  //! Sets the next intersection point to check.
  Standard_EXPORT void Next();

  //! returns the current Intersection point.
  Standard_EXPORT IntCurveSurface_IntersectionPoint Point() const;

  //! returns the current geometric Point
  Standard_EXPORT const gp_Pnt& Pnt() const;

  //! returns the U parameter of the current point
  //! on the current face.
  Standard_EXPORT Standard_Real U() const;

  //! returns the V parameter of the current point
  //! on the current face.
  Standard_EXPORT Standard_Real V() const;

  //! returns the parameter of the current point
  //! on the curve.
  Standard_EXPORT Standard_Real W() const;

  //! returns the current state (IN or ON)
  Standard_EXPORT TopAbs_State State() const;

  //! returns the transition of the line on the surface (IN or OUT or UNKNOWN)
  Standard_EXPORT IntCurveSurface_TransitionOnCurve Transition() const;

  //! returns the current face.
  Standard_EXPORT const TopoDS_Face& Face() const;

protected:
  //! Internal function
  Standard_EXPORT void Find();

  //! Method check found intersection point
  Standard_EXPORT Standard_Boolean FindPoint();

  //! Method to clear fields of class
  Standard_EXPORT void Clear();

private:
  Standard_Real                    myTolerance;
  Handle(GeomAdaptor_Curve)        myCurve;
  IntCurveSurface_HInter           myIntcs;
  Standard_Integer                 myCurrentindex;
  Standard_Integer                 myCurrentnbpoints;
  Handle(BRepTopAdaptor_TopolTool) myFastClass;
  TopAbs_State                     myCurrentstate;
  Standard_Real                    myCurrentU;
  Standard_Real                    myCurrentV;
  Bnd_Box                          myCurveBox;
  Standard_Integer                 myIndFace;
  TopTools_SequenceOfShape         myFaces;
  Handle(Bnd_HArray1OfBox)         myFaceBoxes;
};

#endif // _BRepIntCurveSurface_Inter_HeaderFile
