// Created on: 1996-09-03
// Created by: Olga PILLOT
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

#ifndef _BRepFeat_MakeDPrism_HeaderFile
#define _BRepFeat_MakeDPrism_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Standard_Real.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepFeat_StatusError.hxx>
#include <BRepFeat_Form.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Geom_Curve;
class Standard_ConstructionError;
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;


//! Describes functions to build draft
//! prism topologies from basis shape surfaces. These can be depressions or protrusions.
//! The semantics of draft prism feature creation is based on the
//! construction of shapes:
//! -          along a length
//! -          up to a limiting face
//! -          from a limiting face to a height.
//! The shape defining construction of the draft prism feature can be
//! either the supporting edge or the concerned area of a face.
//! In case of the supporting edge, this contour can be attached to a
//! face of the basis shape by binding. When the contour is bound to this
//! face, the information that the contour will slide on the face
//! becomes available to the relevant class methods.
//! In case of the concerned area of a face, you could, for example, cut
//! it out and move it to a different height which will define the
//! limiting face of a protrusion or depression.
class BRepFeat_MakeDPrism  : public BRepFeat_Form
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! A face Pbase is selected in the shape
  //! Sbase to serve as the basis for the draft prism. The
  //! draft will be defined by the angle Angle and Fuse offers a choice between:
  //! - removing matter with a Boolean cut using the setting 0
  //! - adding matter with Boolean fusion using the setting 1.
  //! The sketch face Skface serves to determine the type of
  //! operation. If it is inside the basis shape, a local
  //! operation such as glueing can be performed.
  //! Initializes the draft prism class
    BRepFeat_MakeDPrism(const TopoDS_Shape& Sbase, const TopoDS_Face& Pbase, const TopoDS_Face& Skface, const Standard_Real Angle, const Standard_Integer Fuse, const Standard_Boolean Modify);
  
  Standard_EXPORT BRepFeat_MakeDPrism();
  
  //! Initializes this algorithm for building draft prisms along surfaces.
  //! A face Pbase is selected in the basis shape Sbase to
  //! serve as the basis from the draft prism. The draft will be
  //! defined by the angle Angle and Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the setting 0
  //! -   adding matter with Boolean fusion using the setting  1.
  //! The sketch face Skface serves to determine the type of
  //! operation. If it is inside the basis shape, a local
  //! operation such as glueing can be performed.
  Standard_EXPORT void Init (const TopoDS_Shape& Sbase, const TopoDS_Face& Pbase, const TopoDS_Face& Skface, const Standard_Real Angle, const Standard_Integer Fuse, const Standard_Boolean Modify);
  
  //! Indicates that the edge <E> will slide on the face
  //! <OnFace>.
  //! Raises ConstructionError if the  face does not belong to the
  //! basis shape, or the edge to the prismed shape.
  Standard_EXPORT void Add (const TopoDS_Edge& E, const TopoDS_Face& OnFace);
  
  Standard_EXPORT void Perform (const Standard_Real Height);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& Until);
  
  //! Assigns one of the following semantics
  //! -   to a height Height
  //! -   to a face Until
  //! -   from a face From to a height Until.
  //! Reconstructs the feature topologically according to the semantic option chosen.
  Standard_EXPORT void Perform (const TopoDS_Shape& From, const TopoDS_Shape& Until);
  
  //! Realizes a semi-infinite prism, limited by the position of the prism base.
  Standard_EXPORT void PerformUntilEnd();
  
  //! Realizes a semi-infinite prism, limited by the face Funtil.
  Standard_EXPORT void PerformFromEnd (const TopoDS_Shape& FUntil);
  
  //! Builds an infinite prism. The infinite descendants will not be kept in the result.
  Standard_EXPORT void PerformThruAll();
  
  //! Assigns both a limiting shape, Until from
  //! TopoDS_Shape, and a height, Height at which to stop
  //! generation of the prism feature.
  Standard_EXPORT void PerformUntilHeight (const TopoDS_Shape& Until, const Standard_Real Height);
  
  Standard_EXPORT void Curves (TColGeom_SequenceOfCurve& S);
  
  Standard_EXPORT Handle(Geom_Curve) BarycCurve();
  
  //! Determination of TopEdges and LatEdges.
  //! sig = 1 -> TopEdges = FirstShape of the DPrism
  //! sig = 2 -> TOpEdges = LastShape of the DPrism
  Standard_EXPORT void BossEdges (const Standard_Integer sig);
  
  //! Returns the list of TopoDS Edges of the top of the boss.
  Standard_EXPORT const TopTools_ListOfShape& TopEdges();
  
  //! Returns the list of TopoDS Edges of the bottom of the boss.
  Standard_EXPORT const TopTools_ListOfShape& LatEdges();




protected:





private:



  TopoDS_Face myPbase;
  TopTools_DataMapOfShapeListOfShape mySlface;
  Standard_Real myAngle;
  TColGeom_SequenceOfCurve myCurves;
  Handle(Geom_Curve) myBCurve;
  TopTools_ListOfShape myTopEdges;
  TopTools_ListOfShape myLatEdges;
  BRepFeat_StatusError myStatusError;


};


#include <BRepFeat_MakeDPrism.lxx>





#endif // _BRepFeat_MakeDPrism_HeaderFile
