// Created on: 1993-01-27
// Created by: Jean-Louis Frenkel
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _StdPrs_ShapeTool_HeaderFile
#define _StdPrs_ShapeTool_HeaderFile

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>

class Bnd_Box;
class Poly_Triangulation;
class Poly_PolygonOnTriangulation;
class Poly_Polygon3D;

//! Describes the behaviour requested for a wireframe shape presentation.
class StdPrs_ShapeTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs the tool and initializes it using theShape and theAllVertices
  //! (optional) arguments. By default, only isolated and internal vertices are considered,
  //! however if theAllVertices argument is equal to True, all shape's vertices are taken into
  //! account.
  Standard_EXPORT StdPrs_ShapeTool(const TopoDS_Shape&    theShape,
                                   const Standard_Boolean theAllVertices = Standard_False);

  void InitFace() { myFaceExplorer.Init(myShape, TopAbs_FACE); }

  Standard_Boolean MoreFace() const { return myFaceExplorer.More(); }

  void NextFace() { myFaceExplorer.Next(); }

  const TopoDS_Face& GetFace() const { return TopoDS::Face(myFaceExplorer.Current()); }

  Standard_EXPORT Bnd_Box FaceBound() const;

  Standard_Boolean IsPlanarFace() const
  {
    const TopoDS_Face& aFace = TopoDS::Face(myFaceExplorer.Current());
    return IsPlanarFace(aFace);
  }

  void InitCurve() { myEdge = 1; }

  Standard_Boolean MoreCurve() const { return myEdge <= myEdgeMap.Extent(); }

  void NextCurve() { ++myEdge; }

  const TopoDS_Edge& GetCurve() const { return TopoDS::Edge(myEdgeMap.FindKey(myEdge)); }

  Standard_EXPORT Bnd_Box CurveBound() const;

  Standard_EXPORT Standard_Integer Neighbours() const;

  Standard_EXPORT Handle(TopTools_HSequenceOfShape) FacesOfEdge() const;

  void InitVertex() { myVertex = 1; }

  Standard_Boolean MoreVertex() const { return myVertex <= myVertexMap.Extent(); }

  void NextVertex() { ++myVertex; }

  const TopoDS_Vertex& GetVertex() const { return TopoDS::Vertex(myVertexMap.FindKey(myVertex)); }

  Standard_EXPORT Standard_Boolean HasSurface() const;

  Standard_EXPORT Handle(Poly_Triangulation) CurrentTriangulation(TopLoc_Location& l) const;

  Standard_EXPORT Standard_Boolean HasCurve() const;

  Standard_EXPORT void PolygonOnTriangulation(Handle(Poly_PolygonOnTriangulation)& Indices,
                                              Handle(Poly_Triangulation)&          T,
                                              TopLoc_Location&                     l) const;

  Standard_EXPORT Handle(Poly_Polygon3D) Polygon3D(TopLoc_Location& l) const;

public:
  Standard_EXPORT static Standard_Boolean IsPlanarFace(const TopoDS_Face& theFace);

private:
  TopoDS_Shape                              myShape;
  TopExp_Explorer                           myFaceExplorer;
  TopTools_IndexedDataMapOfShapeListOfShape myEdgeMap;
  TopTools_IndexedMapOfShape                myVertexMap;
  Standard_Integer                          myEdge;
  Standard_Integer                          myVertex;
};

#endif // _StdPrs_ShapeTool_HeaderFile
