//-Copyright: Open CASCADE 2014
// Created on: 2012-06-09
// Created by: jgv@ROLEX
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _ShapeUpgrade_UnifySameDomain_HeaderFile
#define _ShapeUpgrade_UnifySameDomain_HeaderFile

#include <BRepTools_History.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
class ShapeBuild_ReShape;
class TopoDS_Shape;


class ShapeUpgrade_UnifySameDomain;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_UnifySameDomain, Standard_Transient)

//! This tool tries to unify faces and edges of the shape which lies on the same geometry.
//! Faces/edges considering as 'same-domain' if a group of neighbouring faces/edges lying on coincident surfaces/curves.
//! In this case these faces/edges can be unified into one face/edge.
//! ShapeUpgrade_UnifySameDomain initialized by the shape and the next optional parameters:
//! UnifyFaces - tries to unify all possible faces
//! UnifyEdges - tries to unify all possible edges
//! ConcatBSplines - if this flag set to true then all neighbouring edges which lays 
//! on the BSpline or Bezier curves with C1 continuity on their common vertices will be merged into one common edge 
//! The output result of tool is an unified shape
//! All the modifications of initial shape are recorded during unifying.
//! Methods History are intended to: <br>
//! - set a place holder for the history of the changing of the sub-shapes of
//!   the initial shape; <br>
//! - get the collected history. <br>
//! The algorithm provides a place holder for the history and collects the
//! history by default.
//! To avoid collecting of the history the place holder should be set to the null handle.
class ShapeUpgrade_UnifySameDomain : public Standard_Transient
{

public:
  //! empty constructor
  Standard_EXPORT ShapeUpgrade_UnifySameDomain();
  
  Standard_EXPORT ShapeUpgrade_UnifySameDomain(const TopoDS_Shape& aShape, const Standard_Boolean UnifyEdges = Standard_True, const Standard_Boolean UnifyFaces = Standard_True, const Standard_Boolean ConcatBSplines = Standard_False);
  
  //! Initializes with a shape
  Standard_EXPORT void Initialize (const TopoDS_Shape& aShape, const Standard_Boolean UnifyEdges = Standard_True, const Standard_Boolean UnifyFaces = Standard_True, const Standard_Boolean ConcatBSplines = Standard_False);
  
  //! Sets the flag defining whether it is allowed to create
  //! internal edges inside merged faces in the case of non-manifold
  //! topology. Without this flag merging through multi connected edge
  //! is forbidden. Default value is false.
  Standard_EXPORT void AllowInternalEdges (const Standard_Boolean theValue);

  //! Sets the shape for avoid merging of the faces in given places.
  //! This shape can be vertex or edge.
  //! If shape is vertex it forbids merging of connected edges.
  //! If shape is edge it forbids merging of connected faces.
  Standard_EXPORT void KeepShape(const TopoDS_Shape& theShape);

  //! Sets the map of shapes for avoid merging of the faces in given places
  //! These shapes can be vertexes or edges.
  //! If shape is vertex it forbids merging of connected edges.
  //! If shape is edge it forbids merging of connected faces.
  Standard_EXPORT void KeepShapes(const TopTools_MapOfShape& theShapes);

  //! Sets the flag defining the behavior of the algorithm regarding 
  //! modification of input shape.
  //! If this flag is equal to True then the input (original) shape can't be
  //! modified during modification process. Default value is true.
  Standard_EXPORT void SetSafeInputMode(Standard_Boolean theValue);

  //! Sets the linear tolerance. Default value is Precision::Confusion().
  void SetLinearTolerance(const Standard_Real theValue)
  {
    myLinTol = theValue;
  }

  //! Sets the angular tolerance. Default value is Precision::Angular().
  void SetAngularTolerance(const Standard_Real theValue)
  {
    myAngTol = theValue;
  }

  //! Builds the resulting shape
  Standard_EXPORT void Build();
  
  //! Gives the resulting shape
  const TopoDS_Shape& Shape() const
  {
    return myShape;
  }
  
  //! this method makes if possible a common face from each
  //! group of faces lying on coincident surfaces
  Standard_EXPORT void UnifyFaces();
  
  //! this method makes if possible a common edge from each
  //! group of edges connecting common couple of faces
  Standard_EXPORT void UnifyEdges();
  
  //! this method unifies same domain faces and edges
  Standard_EXPORT void UnifyFacesAndEdges();

  //! Returns the history of the processed shapes.
  const Handle(BRepTools_History)& History() const
  {
    return myHistory;
  }

  //! Returns the history of the processed shapes.
  Handle(BRepTools_History)& History()
  {
    return myHistory;
  }

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_UnifySameDomain,Standard_Transient)

protected:




private:

  void IntUnifyFaces(const TopoDS_Shape& theInpShape,
                     TopTools_IndexedDataMapOfShapeListOfShape& theGMapEdgeFaces,
                     Standard_Boolean IsCheckSharedEdgeOri);

  //! Fills the history of the modifications during the operation.
  Standard_EXPORT void FillHistory();


  TopoDS_Shape myInitShape;
  Standard_Real myLinTol;
  Standard_Real myAngTol;
  Standard_Boolean myUnifyFaces;
  Standard_Boolean myUnifyEdges;
  Standard_Boolean myConcatBSplines;
  Standard_Boolean myAllowInternal;
  Standard_Boolean mySafeInputMode;
  TopoDS_Shape myShape;
  Handle(ShapeBuild_ReShape) myContext;
  TopTools_MapOfShape myKeepShapes;

  Handle(BRepTools_History) myHistory; //!< The history.
};







#endif // _ShapeUpgrade_UnifySameDomain_HeaderFile
