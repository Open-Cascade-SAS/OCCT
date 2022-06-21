// Created on: 1995-08-07
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#include <StdPrs_ShapeTool.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : StdPrs_ShapeTool
//purpose  :
//=======================================================================
StdPrs_ShapeTool::StdPrs_ShapeTool (const TopoDS_Shape& theShape,
                                    const Standard_Boolean theAllVertices)
: myShape (theShape)
{
  myEdgeMap.Clear();
  myVertexMap.Clear();
  TopExp::MapShapesAndAncestors (theShape,TopAbs_EDGE,TopAbs_FACE, myEdgeMap);

  TopExp_Explorer anExpl;
  if (theAllVertices)
  {
    for (anExpl.Init (theShape, TopAbs_VERTEX); anExpl.More(); anExpl.Next())
    {
      myVertexMap.Add (anExpl.Current());
    }
  }
  else
  {
    // Extracting isolated vertices
    for (anExpl.Init (theShape, TopAbs_VERTEX, TopAbs_EDGE); anExpl.More(); anExpl.Next())
    {
      myVertexMap.Add (anExpl.Current());
    }

    // Extracting internal vertices
    for (anExpl.Init (theShape, TopAbs_EDGE); anExpl.More(); anExpl.Next())
    {
      TopoDS_Iterator aIt (anExpl.Current(), Standard_False, Standard_True);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape& aV = aIt.Value();
        if (aV.Orientation() == TopAbs_INTERNAL)
        {
          myVertexMap.Add (aV);
        }
      }
    }
  }
}

//=======================================================================
//function : FaceBound
//purpose  :
//=======================================================================
Bnd_Box StdPrs_ShapeTool::FaceBound() const 
{
  const TopoDS_Face& F = TopoDS::Face(myFaceExplorer.Current());
  Bnd_Box B;
  BRepBndLib::Add(F, B);
  return B;
}

//=======================================================================
//function : IsPlanarFace
//purpose  :
//=======================================================================
Standard_Boolean StdPrs_ShapeTool::IsPlanarFace (const TopoDS_Face& theFace)
{
  TopLoc_Location l;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(theFace, l);
  if (S.IsNull())
  {
    return Standard_False;
  }

  Handle(Standard_Type) TheType = S->DynamicType();

  if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) 
	RTS = Handle(Geom_RectangularTrimmedSurface)::DownCast (S);
    TheType = RTS->BasisSurface()->DynamicType();
  }
  return (TheType == STANDARD_TYPE(Geom_Plane));
}

//=======================================================================
//function : CurveBound
//purpose  :
//=======================================================================
Bnd_Box StdPrs_ShapeTool::CurveBound() const 
{
  const TopoDS_Edge& E = TopoDS::Edge(myEdgeMap.FindKey(myEdge));
  Bnd_Box B;
  BRepBndLib::Add(E, B);
  return B;
}

//=======================================================================
//function : Neighbours
//purpose  :
//=======================================================================
Standard_Integer StdPrs_ShapeTool::Neighbours() const 
{
  const TopTools_ListOfShape& L = myEdgeMap.FindFromIndex(myEdge);
  return L.Extent();
}

//=======================================================================
//function : FacesOfEdge
//purpose  :
//=======================================================================
Handle(TopTools_HSequenceOfShape) StdPrs_ShapeTool::FacesOfEdge() const 
{
  Handle(TopTools_HSequenceOfShape) H = new TopTools_HSequenceOfShape();
  const TopTools_ListOfShape& L = myEdgeMap.FindFromIndex(myEdge);
  for (TopTools_ListIteratorOfListOfShape LI (L); LI.More(); LI.Next())
  {
    H->Append(LI.Value());
  }
  return H;
}

//=======================================================================
//function : HasSurface
//purpose  :
//=======================================================================
Standard_Boolean StdPrs_ShapeTool::HasSurface() const
{
  TopLoc_Location l;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(GetFace(), l);
  return !S.IsNull();
}

//=======================================================================
//function : CurrentTriangulation
//purpose  :
//=======================================================================
Handle(Poly_Triangulation) StdPrs_ShapeTool::CurrentTriangulation(TopLoc_Location& l) const
{
  return BRep_Tool::Triangulation(GetFace(), l);
}

//=======================================================================
//function : HasCurve
//purpose  :
//=======================================================================
Standard_Boolean StdPrs_ShapeTool::HasCurve() const
{
  return BRep_Tool::IsGeometric(GetCurve());
}

//=======================================================================
//function : PolygonOnTriangulation
//purpose  :
//=======================================================================
void StdPrs_ShapeTool::PolygonOnTriangulation (Handle(Poly_PolygonOnTriangulation)& Indices,
                                               Handle(Poly_Triangulation)& T,
                                               TopLoc_Location& l) const
{
  BRep_Tool::PolygonOnTriangulation(GetCurve(), Indices, T, l);
}

//=======================================================================
//function : Polygon3D
//purpose  :
//=======================================================================
Handle(Poly_Polygon3D) StdPrs_ShapeTool::Polygon3D(TopLoc_Location& l) const
{
  return BRep_Tool::Polygon3D(GetCurve(), l);
}
