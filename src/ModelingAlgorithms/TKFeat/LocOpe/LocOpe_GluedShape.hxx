// Created on: 1996-01-30
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

#ifndef _LocOpe_GluedShape_HeaderFile
#define _LocOpe_GluedShape_HeaderFile

#include <Standard.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <LocOpe_GeneratedShape.hxx>
#include <NCollection_List.hxx>
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Vertex;

class LocOpe_GluedShape : public LocOpe_GeneratedShape
{

public:
  Standard_EXPORT LocOpe_GluedShape();

  Standard_EXPORT LocOpe_GluedShape(const TopoDS_Shape& S);

  Standard_EXPORT void Init(const TopoDS_Shape& S);

  Standard_EXPORT void GlueOnFace(const TopoDS_Face& F);

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& GeneratingEdges() override;

  //! Returns the edge created by the vertex <V>. If
  //! none, must return a null shape.
  Standard_EXPORT TopoDS_Edge Generated(const TopoDS_Vertex& V) override;

  //! Returns the face created by the edge <E>. If none,
  //! must return a null shape.
  Standard_EXPORT TopoDS_Face Generated(const TopoDS_Edge& E) override;

  //! Returns the list of correctly oriented generated
  //! faces.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& OrientedFaces() override;

  DEFINE_STANDARD_RTTIEXT(LocOpe_GluedShape, LocOpe_GeneratedShape)

private:
  Standard_EXPORT void MapEdgeAndVertices();

  TopoDS_Shape                                                             myShape;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                   myMap;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> myGShape;
};

#endif // _LocOpe_GluedShape_HeaderFile
