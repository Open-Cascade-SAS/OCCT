// Created on: 1999-05-11
// Created by: Sergei ZERTCHANINOV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeFix_EdgeConnect_HeaderFile
#define _ShapeFix_EdgeConnect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
class TopoDS_Edge;
class TopoDS_Shape;


//! Rebuilds edges to connect with new vertices, was moved from ShapeBuild.
//! Makes vertices to be shared to connect edges,
//! updates positions and tolerances for shared vertices.
//! Accepts edges bounded by two vertices each.
class ShapeFix_EdgeConnect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ShapeFix_EdgeConnect();
  
  //! Adds information on connectivity between start vertex
  //! of second edge and end vertex of first edge,
  //! taking edges orientation into account
  Standard_EXPORT void Add (const TopoDS_Edge& aFirst, const TopoDS_Edge& aSecond);
  
  //! Adds connectivity information for the whole shape.
  //! Note: edges in wires must be well ordered
  //! Note: flag Closed should be set for closed wires
  Standard_EXPORT void Add (const TopoDS_Shape& aShape);
  
  //! Builds shared vertices, updates their positions and tolerances
  Standard_EXPORT void Build();
  
  //! Clears internal data structure
  Standard_EXPORT void Clear();




protected:





private:



  TopTools_DataMapOfShapeShape myVertices;
  TopTools_DataMapOfShapeListOfShape myLists;


};







#endif // _ShapeFix_EdgeConnect_HeaderFile
