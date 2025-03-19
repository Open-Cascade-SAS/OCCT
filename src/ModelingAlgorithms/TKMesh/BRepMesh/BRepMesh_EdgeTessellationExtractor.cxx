// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_EdgeTessellationExtractor.hxx>

#include <BRepMesh_ShapeTool.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Face.hxx>
#include <Poly_Triangulation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_EdgeTessellationExtractor, IMeshTools_CurveTessellator)

//=================================================================================================

BRepMesh_EdgeTessellationExtractor::BRepMesh_EdgeTessellationExtractor(
  const IMeshData::IEdgeHandle& theEdge,
  const IMeshData::IFaceHandle& theFace)
{
  Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(theFace->GetFace(), myLoc);

  Handle(Poly_PolygonOnTriangulation) aPolygon =
    BRep_Tool::PolygonOnTriangulation(theEdge->GetEdge(), aTriangulation, myLoc);

  myTriangulation = aTriangulation.get();
  myIndices       = &aPolygon->Nodes();
  myProvider.Init(theEdge, TopAbs_FORWARD, theFace, aPolygon->Parameters());
}

//=================================================================================================

BRepMesh_EdgeTessellationExtractor::~BRepMesh_EdgeTessellationExtractor() {}

//=================================================================================================

Standard_Integer BRepMesh_EdgeTessellationExtractor::PointsNb() const
{
  return myIndices->Size();
}

//=================================================================================================

Standard_Boolean BRepMesh_EdgeTessellationExtractor::Value(const Standard_Integer theIndex,
                                                           gp_Pnt&                thePoint,
                                                           Standard_Real& theParameter) const
{
  const gp_Pnt aRefPnt = myTriangulation->Node(myIndices->Value(theIndex));
  thePoint             = BRepMesh_ShapeTool::UseLocation(aRefPnt, myLoc);

  theParameter = myProvider.Parameter(theIndex, thePoint);
  return Standard_True;
}
