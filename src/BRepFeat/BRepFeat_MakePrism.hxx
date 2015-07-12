// Created on: 1996-02-13
// Created by: Jacques GOUSSARD
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

#ifndef _BRepFeat_MakePrism_HeaderFile
#define _BRepFeat_MakePrism_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <gp_Dir.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <BRepFeat_StatusError.hxx>
#include <BRepFeat_Form.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom_Curve;
class Standard_ConstructionError;
class TopoDS_Shape;
class TopoDS_Face;
class gp_Dir;
class TopoDS_Edge;


//! Describes functions to build prism features.
//! These can be depressions or protrusions.
//! The semantics of prism feature creation is
//! based on the construction of shapes:
//! -   along a length
//! -   up to a limiting face
//! -   from a limiting face to a height.
//! The shape defining construction of the prism feature can be
//! either the supporting edge or the concerned area of a face.
//! In case of the supporting edge, this contour
//! can be attached to a face of the basis shape by
//! binding. When the contour is bound to this face,
//! the information that the contour will slide on the
//! face becomes available to the relevant class methods.
//! In case of the concerned area of a face, you
//! could, for example, cut it out and move it to a
//! different height which will define the limiting
//! face of a protrusion or depression.
class BRepFeat_MakePrism  : public BRepFeat_Form
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Builds a prism by projecting a
  //! wire along the face of a shape. Initializes the prism class.
    BRepFeat_MakePrism();
  
  //! Builds a prism by projecting a
  //! wire along the face of a shape. a face Pbase is selected in
  //! the shape Sbase to serve as the basis for
  //! the prism. The orientation of the prism will
  //! be defined by the vector Direction.
  //! Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the setting 0
  //! -   adding matter with Boolean fusion using the setting 1.
  //! The sketch face Skface serves to determine
  //! the type of operation. If it is inside the basis
  //! shape, a local operation such as glueing can be performed.
  //! Exceptions
  //! Standard_ConstructionError if the face
  //! does not belong to the basis or the prism shape.
    BRepFeat_MakePrism(const TopoDS_Shape& Sbase, const TopoDS_Shape& Pbase, const TopoDS_Face& Skface, const gp_Dir& Direction, const Standard_Integer Fuse, const Standard_Boolean Modify);
  
  //! Initializes this algorithm for building prisms along surfaces.
  //! A face Pbase is selected in the shape Sbase
  //! to serve as the basis for the prism. The
  //! orientation of the prism will be defined by the vector Direction.
  //! Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the setting 0
  //! -   adding matter with Boolean fusion using the setting 1.
  //! The sketch face Skface serves to determine
  //! the type of operation. If it is inside the basis
  //! shape, a local operation such as glueing can be performed.
  Standard_EXPORT void Init (const TopoDS_Shape& Sbase, const TopoDS_Shape& Pbase, const TopoDS_Face& Skface, const gp_Dir& Direction, const Standard_Integer Fuse, const Standard_Boolean Modify);
  
  //! Indicates that the edge <E> will slide on the face
  //! <OnFace>. Raises ConstructionError if the  face does not belong to the
  //! basis shape, or the edge to the prismed shape.
  Standard_EXPORT void Add (const TopoDS_Edge& E, const TopoDS_Face& OnFace);
  
  Standard_EXPORT void Perform (const Standard_Real Length);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& Until);
  
  //! Assigns one of the following semantics
  //! -   to a height Length
  //! -   to a face Until
  //! -   from a face From to a height Until.
  //! Reconstructs the feature topologically according to the semantic option chosen.
  Standard_EXPORT void Perform (const TopoDS_Shape& From, const TopoDS_Shape& Until);
  
  //! Realizes a semi-infinite prism, limited by the
  //! position of the prism base. All other faces extend infinitely.
  Standard_EXPORT void PerformUntilEnd();
  
  //! Realizes a semi-infinite prism, limited by the face Funtil.
  Standard_EXPORT void PerformFromEnd (const TopoDS_Shape& FUntil);
  
  //! Builds an infinite prism. The infinite descendants will not be kept in the result.
  Standard_EXPORT void PerformThruAll();
  
  //! Assigns both a limiting shape, Until from
  //! TopoDS_Shape, and a height, Length at which to stop generation of the prism feature.
  Standard_EXPORT void PerformUntilHeight (const TopoDS_Shape& Until, const Standard_Real Length);
  
  //! Returns the list of curves S parallel to the axis of the prism.
  Standard_EXPORT void Curves (TColGeom_SequenceOfCurve& S);
  
  //! Generates a curve along the center of mass of the primitive.
  Standard_EXPORT Handle(Geom_Curve) BarycCurve();




protected:





private:



  TopoDS_Shape myPbase;
  TopTools_DataMapOfShapeListOfShape mySlface;
  gp_Dir myDir;
  TColGeom_SequenceOfCurve myCurves;
  Handle(Geom_Curve) myBCurve;
  BRepFeat_StatusError myStatusError;


};


#include <BRepFeat_MakePrism.lxx>





#endif // _BRepFeat_MakePrism_HeaderFile
