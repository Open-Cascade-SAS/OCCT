// Created on: 1995-08-07
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#define OCC215           //SAV: 01/04/02 vertex exploring is done for all types of shape.
#define OCC598		 //SAV: 22/10/02 searching for internal vertices. 

#include <Prs3d_ShapeTool.ixx>

#include <BRepTools.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BRepBndLib.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Plane.hxx>


//=======================================================================
//function : Prs3d_ShapeTool
//purpose  : 
//=======================================================================

Prs3d_ShapeTool::Prs3d_ShapeTool(const TopoDS_Shape& TheShape):
                                 myShape (TheShape) 
{
  myEdgeMap.Clear();
  myVertexMap.Clear();
  TopExp::MapShapesAndAncestors(TheShape,TopAbs_EDGE,TopAbs_FACE,myEdgeMap);

#ifndef OCC215
  // find vertices not under ancestors.
  TopAbs_ShapeEnum E = TheShape.ShapeType();


  // this check were done to reduce the number of selectable objects
  // in a local context. By now, there's no noticeable performance improvement.
  if (E != TopAbs_SOLID && E != TopAbs_SHELL)
#endif
    {
      TopExp_Explorer ex(TheShape,TopAbs_VERTEX, TopAbs_EDGE);
      while (ex.More()) {
	const TopoDS_Shape& aV=ex.Current();
	myVertexMap.Add(aV);
	ex.Next();
      }
    }
#ifdef OCC598
  TopExp_Explorer edges( TheShape, TopAbs_EDGE );
  while( edges.More() ) {
    //xf
    const TopoDS_Shape& aE= edges.Current();
    TopoDS_Iterator aIt(aE, Standard_False, Standard_True);
    while( aIt.More() ) {
      const TopoDS_Shape& aV=aIt.Value();
      if (aV.Orientation()==TopAbs_INTERNAL) {
	myVertexMap.Add(aV);
      }
      aIt.Next();
    }
    /*
    TopExp_Explorer vertices( edges.Current(), TopAbs_VERTEX );
    while( vertices.More() ) {
      TopoDS_Vertex current = TopoDS::Vertex( vertices.Current() );
      if ( current.Orientation() == TopAbs_INTERNAL )
	myVertexMap.Add( current );
      vertices.Next();
    }
    */
    //xt
    edges.Next();
  }
#endif
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

Standard_Boolean Prs3d_ShapeTool::IsPlanarFace() const 
{
  TopLoc_Location l;
  const TopoDS_Face& F = TopoDS::Face(myFaceExplorer.Current());
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F, l);
  Handle(Standard_Type) TheType = S->DynamicType();

  if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) 
	RTS = *((Handle(Geom_RectangularTrimmedSurface)*)&S);
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
