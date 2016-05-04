// Created on: 2014-08-13
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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
#include <Geom2dAdaptor_HCurve.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepMesh_ShapeTool.hxx>


IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_EdgeTessellationExtractor,BRepMesh_IEdgeTool)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_EdgeTessellationExtractor::BRepMesh_EdgeTessellationExtractor(
  const TopoDS_Edge&                          theEdge,
  const Handle(Geom2dAdaptor_HCurve)&         thePCurve,
  const TopoDS_Face&                          theFace,
  const Handle(Poly_Triangulation)&           theTriangulation,
  const Handle(Poly_PolygonOnTriangulation)&  thePolygon,
  const TopLoc_Location&                      theLocation)
  : myProvider(theEdge, theFace, thePolygon->Parameters()),
    myPCurve(thePCurve),
    myNodes(theTriangulation->Nodes()),
    myIndices(thePolygon->Nodes()),
    myLoc(theLocation)
{
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_EdgeTessellationExtractor::Value(
  const Standard_Integer theIndex,
  Standard_Real&         theParameter,
  gp_Pnt&                thePoint,
  gp_Pnt2d&              theUV)
{
  const gp_Pnt& theRefPnt = myNodes(myIndices(theIndex));
  thePoint = BRepMesh_ShapeTool::UseLocation(theRefPnt, myLoc);

  theParameter = myProvider.Parameter(theIndex, thePoint);
  theUV        = myPCurve->Value(theParameter);

  return Standard_True;
}
