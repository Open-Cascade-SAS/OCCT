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


#include <Bnd_Box.hxx>
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
#include <Prs3d_ShapeTool.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : Prs3d_ShapeTool
//purpose  : 
//=======================================================================
Prs3d_ShapeTool::Prs3d_ShapeTool (const TopoDS_Shape& theShape,
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
//function : InitFace
//purpose  : 
//=======================================================================

void Prs3d_ShapeTool::InitFace() 
{
  myFaceExplorer.Init(myShape,TopAbs_FACE);
}

//=======================================================================
//function : MoreFace
//purpose  : 
//=======================================================================

Standard_Boolean Prs3d_ShapeTool::MoreFace() const 
{
  return myFaceExplorer.More();
}

//=======================================================================
//function : NextFace
//purpose  : 
//=======================================================================

void Prs3d_ShapeTool::NextFace() 
{
  myFaceExplorer.Next();
}

//=======================================================================
//function : GetFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Prs3d_ShapeTool::GetFace () const 
{
  return TopoDS::Face(myFaceExplorer.Current());
}


//=======================================================================
//function : FaceBound
//purpose  : 
//=======================================================================

Bnd_Box Prs3d_ShapeTool::FaceBound() const 
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

Standard_Boolean Prs3d_ShapeTool::IsPlanarFace (const TopoDS_Face& theFace)
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
//function : InitCurve
//purpose  : 
//=======================================================================

void Prs3d_ShapeTool::InitCurve() 
{
  myEdge = 1;
}

//=======================================================================
//function : MoreCurve
//purpose  : 
//=======================================================================

Standard_Boolean Prs3d_ShapeTool::MoreCurve() const 
{
  return myEdge <= myEdgeMap.Extent();
}

//=======================================================================
//function : NextCurve
//purpose  : 
//=======================================================================

void Prs3d_ShapeTool::NextCurve() 
{
  myEdge++;
}

//=======================================================================
//function : GetCurve
//purpose  : 
//=======================================================================

const TopoDS_Edge& Prs3d_ShapeTool::GetCurve () const 
{
  return  TopoDS::Edge(myEdgeMap.FindKey(myEdge));
}

//=======================================================================
//function : CurveBound
//purpose  : 
//=======================================================================

Bnd_Box Prs3d_ShapeTool::CurveBound () const 
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

Standard_Integer Prs3d_ShapeTool::Neighbours () const 
{
  const TopTools_ListOfShape& L = myEdgeMap.FindFromIndex(myEdge);
  return L.Extent();
}

//=======================================================================
//function : FacesOfEdge
//purpose  : 
//=======================================================================

Handle(TopTools_HSequenceOfShape) Prs3d_ShapeTool::FacesOfEdge () const 
{
  Handle(TopTools_HSequenceOfShape) H = new TopTools_HSequenceOfShape;

  const TopTools_ListOfShape& L = myEdgeMap.FindFromIndex(myEdge);
  TopTools_ListIteratorOfListOfShape LI;

  for (LI.Initialize(L); LI.More(); LI.Next()) H->Append(LI.Value());
  return H;
}


//=======================================================================
//function : InitVertex
//purpose  : 
//=======================================================================

void Prs3d_ShapeTool::InitVertex() 
{
  myVertex = 1;
}

//=======================================================================
//function : MoreVertex
//purpose  : 
//=======================================================================

Standard_Boolean Prs3d_ShapeTool::MoreVertex() const 
{
  return myVertex <= myVertexMap.Extent();
}

//=======================================================================
//function : NextVertex
//purpose  : 
//=======================================================================

void Prs3d_ShapeTool::NextVertex() 
{
  myVertex++;
}

//=======================================================================
//function : GetVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex& Prs3d_ShapeTool::GetVertex () const 
{
  return  TopoDS::Vertex(myVertexMap.FindKey(myVertex));
}


//=======================================================================
//function : HasSurface
//purpose  : 
//=======================================================================

Standard_Boolean Prs3d_ShapeTool::HasSurface() const
{
  TopLoc_Location l;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(GetFace(), l);
  return (!S.IsNull());
}



//=======================================================================
//function : CurrentTriangulation
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) Prs3d_ShapeTool::CurrentTriangulation(TopLoc_Location& l) const
{
  return BRep_Tool::Triangulation(GetFace(), l);
}


//=======================================================================
//function : HasCurve
//purpose  : 
//=======================================================================

Standard_Boolean Prs3d_ShapeTool::HasCurve() const
{
  return (BRep_Tool::IsGeometric(GetCurve()));
}




//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

void Prs3d_ShapeTool::PolygonOnTriangulation
(Handle(Poly_PolygonOnTriangulation)& Indices,
 Handle(Poly_Triangulation)&          T,
 TopLoc_Location&                     l) const
{
  BRep_Tool::PolygonOnTriangulation(GetCurve(), Indices, T, l);
}



//=======================================================================
//function : Polygon3D
//purpose  : 
//=======================================================================

Handle(Poly_Polygon3D) Prs3d_ShapeTool::Polygon3D(TopLoc_Location& l) const
{
  return BRep_Tool::Polygon3D(GetCurve(), l);
}
