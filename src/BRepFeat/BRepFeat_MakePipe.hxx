// Created on: 1996-09-03
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

#ifndef _BRepFeat_MakePipe_HeaderFile
#define _BRepFeat_MakePipe_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepFeat_Form.hxx>
#include <Standard_Integer.hxx>
class Geom_Curve;
class TopoDS_Edge;


//! Constructs compound shapes with pipe
//! features. These can be depressions or protrusions.
//! The semantics of pipe feature creation is based on the construction of shapes:
//! -   along a length
//! -   up to a limiting face
//! -   from a limiting face to a height.
//! The shape defining construction of the pipe feature can be either the supporting edge or
//! the concerned area of a face.
//! In case of the supporting edge, this contour
//! can be attached to a face of the basis shape
//! by binding. When the contour is bound to this
//! face, the information that the contour will
//! slide on the face becomes available to the relevant class methods.
//! In case of the concerned area of a face, you
//! could, for example, cut it out and move it to a
//! different height which will define the limiting
//! face of a protrusion or depression.
class BRepFeat_MakePipe  : public BRepFeat_Form
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! initializes the pipe class.
    BRepFeat_MakePipe();
  
  //! A face Pbase is selected in the
  //! shape Sbase to serve as the basis for the
  //! pipe. It will be defined by the wire Spine.
  //! Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the setting 0
  //! -   adding matter with Boolean fusion using the setting 1.
  //! The sketch face Skface serves to determine
  //! the type of operation. If it is inside the basis
  //! shape, a local operation such as glueing can be performed.
    BRepFeat_MakePipe(const TopoDS_Shape& Sbase, const TopoDS_Shape& Pbase, const TopoDS_Face& Skface, const TopoDS_Wire& Spine, const Standard_Integer Fuse, const Standard_Boolean Modify);
  
  //! Initializes this algorithm for adding pipes to shapes.
  //! A face Pbase is selected in the shape Sbase to
  //! serve as the basis for the pipe. It will be defined by the wire Spine.
  //! Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the setting 0
  //! -   adding matter with Boolean fusion using the setting 1.
  //! The sketch face Skface serves to determine
  //! the type of operation. If it is inside the basis
  //! shape, a local operation such as glueing can be performed.
  Standard_EXPORT void Init (const TopoDS_Shape& Sbase, const TopoDS_Shape& Pbase, const TopoDS_Face& Skface, const TopoDS_Wire& Spine, const Standard_Integer Fuse, const Standard_Boolean Modify);
  
  //! Indicates that the edge <E> will slide on the face
  //! <OnFace>. Raises ConstructionError  if the  face does not belong to the
  //! basis shape, or the edge to the prismed shape.
  Standard_EXPORT void Add (const TopoDS_Edge& E, const TopoDS_Face& OnFace);
  
  Standard_EXPORT void Perform();
  
  Standard_EXPORT void Perform (const TopoDS_Shape& Until);
  
  //! Assigns one of the following semantics
  //! -   to a face Until
  //! -   from a face From to a height Until.
  //! Reconstructs the feature topologically according to the semantic option chosen.
  Standard_EXPORT void Perform (const TopoDS_Shape& From, const TopoDS_Shape& Until);
  
  Standard_EXPORT void Curves (TColGeom_SequenceOfCurve& S);
  
  Standard_EXPORT Handle(Geom_Curve) BarycCurve();




protected:





private:



  TopoDS_Shape myPbase;
  TopoDS_Face mySkface;
  TopTools_DataMapOfShapeListOfShape mySlface;
  TopoDS_Wire mySpine;
  TColGeom_SequenceOfCurve myCurves;
  Handle(Geom_Curve) myBCurve;


};


#include <BRepFeat_MakePipe.lxx>





#endif // _BRepFeat_MakePipe_HeaderFile
