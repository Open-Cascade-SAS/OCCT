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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <MMgt_TShared.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
class ShapeBuild_ReShape;
class TopoDS_Shape;


class ShapeUpgrade_UnifySameDomain;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_UnifySameDomain, MMgt_TShared)

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
//! Method Generated() can be used to obtain the new (unified) shape from the old one 
class ShapeUpgrade_UnifySameDomain : public MMgt_TShared
{

public:

  
  //! empty constructor
  Standard_EXPORT ShapeUpgrade_UnifySameDomain();
  
  Standard_EXPORT ShapeUpgrade_UnifySameDomain(const TopoDS_Shape& aShape, const Standard_Boolean UnifyEdges = Standard_True, const Standard_Boolean UnifyFaces = Standard_True, const Standard_Boolean ConcatBSplines = Standard_False);
  
  //! Initializes with a shape
  Standard_EXPORT void Initialize (const TopoDS_Shape& aShape, const Standard_Boolean UnifyEdges = Standard_True, const Standard_Boolean UnifyFaces = Standard_True, const Standard_Boolean ConcatBSplines = Standard_False);
  
  //! Builds the resulting shape
  Standard_EXPORT void Build();
  
  //! Gives the resulting shape
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  //! Gets new common shape from the old one
  Standard_EXPORT TopoDS_Shape Generated (const TopoDS_Shape& aShape) const;
  
  //! this method makes if possible a common face from each
  //! group of faces lying on coincident surfaces
  Standard_EXPORT void UnifyFaces();
  
  //! this method makes if possible a common edge from each
  //! group of edges connecting common couple of faces
  Standard_EXPORT void UnifyEdges();
  
  //! this method unifies same domain faces and edges
  Standard_EXPORT void UnifyFacesAndEdges();




  DEFINE_STANDARD_RTTI(ShapeUpgrade_UnifySameDomain,MMgt_TShared)

protected:




private:


  TopoDS_Shape myInitShape;
  Standard_Boolean myUnifyFaces;
  Standard_Boolean myUnifyEdges;
  Standard_Boolean myConcatBSplines;
  TopoDS_Shape myShape;
  Handle(ShapeBuild_ReShape) myContext;
  TopTools_DataMapOfShapeShape myOldShapes; 


};







#endif // _ShapeUpgrade_UnifySameDomain_HeaderFile
