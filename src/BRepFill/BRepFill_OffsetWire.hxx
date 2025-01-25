// Created on: 1995-04-19
// Created by: Yves FRICAUD
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

#ifndef _BRepFill_OffsetWire_HeaderFile
#define _BRepFill_OffsetWire_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepFill_IndexedDataMapOfOrientedShapeListOfShape.hxx>
#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_LinkTopoBilo.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepFill_DataMapOfOrientedShapeListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TColgp_SequenceOfPnt.hxx>
class Bisector_Bisec;
class BRepFill_TrimEdgeTool;

//! Constructs a Offset Wire to a spine (wire or face).
//! Offset direction will be to outer region in case of
//! positive offset value and to inner region in case of
//! negative offset value.
//! Inner/Outer region for open wire is defined by the
//! following rule: when we go along the wire (taking into
//! account of edges orientation) then outer region will be
//! on the right side, inner region will be on the left side.
//! In case of closed wire, inner region will always be
//! inside the wire (at that, edges orientation is not taken
//! into account).
//! The Wire or the Face must be planar and oriented correctly.
class BRepFill_OffsetWire
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFill_OffsetWire();

  Standard_EXPORT BRepFill_OffsetWire(const TopoDS_Face&     Spine,
                                      const GeomAbs_JoinType Join         = GeomAbs_Arc,
                                      const Standard_Boolean IsOpenResult = Standard_False);

  //! Initialize the evaluation of Offsetting.
  Standard_EXPORT void Init(const TopoDS_Face&     Spine,
                            const GeomAbs_JoinType Join         = GeomAbs_Arc,
                            const Standard_Boolean IsOpenResult = Standard_False);

  //! Performs  an OffsetWire at  an altitude <Alt> from
  //! the  face ( According  to  the orientation of  the
  //! face)
  Standard_EXPORT void Perform(const Standard_Real Offset, const Standard_Real Alt = 0.0);

  //! Performs an  OffsetWire
  Standard_EXPORT void PerformWithBiLo(const TopoDS_Face&              WSP,
                                       const Standard_Real             Offset,
                                       const BRepMAT2d_BisectingLocus& Locus,
                                       BRepMAT2d_LinkTopoBilo&         Link,
                                       const GeomAbs_JoinType          Join = GeomAbs_Arc,
                                       const Standard_Real             Alt  = 0.0);

  Standard_EXPORT Standard_Boolean IsDone() const;

  Standard_EXPORT const TopoDS_Face& Spine() const;

  //! returns the generated shape.
  Standard_EXPORT const TopoDS_Shape& Shape() const;

  //! Returns   the  shapes  created  from   a  subshape
  //! <SpineShape> of the spine.
  //! Returns the last computed Offset.
  Standard_EXPORT const TopTools_ListOfShape& GeneratedShapes(const TopoDS_Shape& SpineShape);

  Standard_EXPORT GeomAbs_JoinType JoinType() const;

protected:
private:
  Standard_EXPORT BRepFill_IndexedDataMapOfOrientedShapeListOfShape& Generated();

  //! Prepare the spine as follow
  //! - Cut the spine-Edges at the extrema of curvature and
  //! at the inflexion points.
  Standard_EXPORT void PrepareSpine();

  //! Add the OffsetWire <Other> to <me> and update <myMap>
  Standard_EXPORT void Add(const BRepFill_OffsetWire& Other);

  Standard_EXPORT void UpdateDetromp(BRepFill_DataMapOfOrientedShapeListOfShape& Detromp,
                                     const TopoDS_Shape&                         Shape1,
                                     const TopoDS_Shape&                         Shape2,
                                     const TopTools_SequenceOfShape&             Vertices,
                                     const TColgp_SequenceOfPnt&                 Params,
                                     const Bisector_Bisec&                       Bisec,
                                     const Standard_Boolean                      SOnE,
                                     const Standard_Boolean                      EOnE,
                                     const BRepFill_TrimEdgeTool&                Trim) const;

  //! Constructs the wires with the trimmed offset edges.
  Standard_EXPORT void MakeWires();

  //! Fix holes between open wires where it is possible
  Standard_EXPORT void FixHoles();

  TopoDS_Face                                       mySpine;
  TopoDS_Face                                       myWorkSpine;
  Standard_Real                                     myOffset;
  Standard_Boolean                                  myIsOpenResult;
  TopoDS_Shape                                      myShape;
  Standard_Boolean                                  myIsDone;
  GeomAbs_JoinType                                  myJoinType;
  BRepFill_IndexedDataMapOfOrientedShapeListOfShape myMap;
  BRepMAT2d_BisectingLocus                          myBilo;
  BRepMAT2d_LinkTopoBilo                            myLink;
  TopTools_DataMapOfShapeShape                      myMapSpine;
  Standard_Boolean                                  myCallGen;
};

#endif // _BRepFill_OffsetWire_HeaderFile
