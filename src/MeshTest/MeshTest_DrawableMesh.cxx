// Created on: 1994-08-03
// Created by: Modeling
// Copyright (c) 1994-1999 Matra Datavision
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

#include <MeshTest_DrawableMesh.hxx>
#include <TopoDS.hxx>
#include <Draw.hxx>
#include <Draw_ColorKind.hxx>
#include <Draw_Color.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_RangeError.hxx>
#include <BRepMesh_DegreeOfFreedom.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_Vertex.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <TopExp_Explorer.hxx>


IMPLEMENT_STANDARD_RTTIEXT(MeshTest_DrawableMesh,Draw_Drawable3D)

typedef NCollection_Map<BRepMesh_Edge> BRepMesh_MapOfLinks;

static inline void addLink(const Standard_Integer theIndex1,
                           const Standard_Integer theIndex2,
                           BRepMesh_MapOfLinks&   theMap)
{
  BRepMesh_Edge anEdge(theIndex1, theIndex2, BRepMesh_Free);
  theMap.Add(anEdge);
}

//=======================================================================
//function : MeshTest_DrawableMesh
//purpose  : 
//=======================================================================
MeshTest_DrawableMesh::MeshTest_DrawableMesh()
  : myDeflection(1.)
{
}

//=======================================================================
//function : MeshTest_DrawableMesh
//purpose  : 
//=======================================================================
MeshTest_DrawableMesh::MeshTest_DrawableMesh(const TopoDS_Shape& theShape,
                                             const Standard_Real theDeflection)
  : myDeflection(theDeflection)
{
  Add(theShape);
}

//=======================================================================
//function : MeshTest_DrawableMesh
//purpose  : 
//=======================================================================
MeshTest_DrawableMesh::MeshTest_DrawableMesh(
  const Handle(BRepMesh_IncrementalMesh)& theMesher)
  : myDeflection(1.)
{
  myMesher = theMesher;
  if (!myMesher.IsNull())
    myDeflection = myMesher->Parameters().Deflection;
}

//=======================================================================
//function : MeshTest_DrawableMesh
//purpose  : 
//=======================================================================
void MeshTest_DrawableMesh::Add(const TopoDS_Shape& theShape)
{
  if (myMesher.IsNull())
  {
    myMesher = new BRepMesh_IncrementalMesh;
    myMesher->ChangeParameters().Deflection = myDeflection;
    myMesher->ChangeParameters().Angle = 0.5;
  }
  
  myMesher->SetShape(theShape);
  myMesher->Perform();
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void MeshTest_DrawableMesh::DrawOn(Draw_Display& /*D*/) const 
{
  // should be reimplemented!!
  /*  Handle(BRepMesh_DataStructureOfDelaun) struc = myMesh->Result();
  Standard_Integer nbc;
  D.SetColor(Draw_vert);

  for (Standard_Integer iLi=1; iLi<=myMesh->NbEdges(); iLi++) {
  const BRepMesh_Edge& ed=myMesh->Edge(iLi);
  if (ed.Movability()!=BRepMesh_Deleted) {
  nbc=struc->ElemConnectedTo(iLi).Extent();
  if (nbc<=0) D.SetColor(Draw_bleu);
  else if (nbc==1) D.SetColor(Draw_jaune);
  else if (nbc==2) D.SetColor(Draw_vert);
  else D.SetColor(Draw_corail);
  D.MoveTo(myMesh->Pnt(ed.FirstNode()));
  D.DrawTo(myMesh->Pnt(ed.LastNode()));
  }
  }


  // highlighted triangles
  D.SetColor(Draw_blanc);
  Standard_Integer e1, e2, e3, i;
  Standard_Boolean o1, o2, o3;

  for (i = 1; i <= myTriangles.Length(); i++) {
  const BRepMesh_Triangle& tri=struc->GetElement(myTriangles(i));
  tri.Edges(e1, e2, e3, o1, o2, o3); 
  const BRepMesh_Edge& ed1=myMesh->Edge(e1);
  if (ed1.Movability()!=BRepMesh_Deleted) {
  D.MoveTo(myMesh->Pnt(ed1.FirstNode()));
  D.DrawTo(myMesh->Pnt(ed1.LastNode()));
  }
  const BRepMesh_Edge& ed2=myMesh->Edge(e2);
  if (ed2.Movability()!=BRepMesh_Deleted) {
  D.MoveTo(myMesh->Pnt(ed2.FirstNode()));
  D.DrawTo(myMesh->Pnt(ed2.LastNode()));
  }
  const BRepMesh_Edge& ed3=myMesh->Edge(e3);
  if (ed3.Movability()!=BRepMesh_Deleted) {
  D.MoveTo(myMesh->Pnt(ed3.FirstNode()));
  D.DrawTo(myMesh->Pnt(ed3.LastNode()));
  }
  }

  // highlighted edges
  D.SetColor(Draw_rouge);
  for (i = 1; i <= myEdges.Length(); i++) {
  const BRepMesh_Edge& ed=myMesh->Edge(myEdges(i));
  if (ed.Movability()!=BRepMesh_Deleted) {
  D.MoveTo(myMesh->Pnt(ed.FirstNode()));
  D.DrawTo(myMesh->Pnt(ed.LastNode()));
  }
  }

  // highlighted vertices
  for (i = 1; i <= myVertices.Length(); i++) {
  D.DrawMarker(myMesh->Pnt(myVertices(i)),Draw_Losange);
  }

  */

}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================
Handle(Draw_Drawable3D) MeshTest_DrawableMesh::Copy() const 
{
  return new MeshTest_DrawableMesh(myMesher);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void MeshTest_DrawableMesh::Dump(Standard_OStream&) const 
{
  // Should be reimplemented

  /*Handle(BRepMesh_DataStructureOfDelaun) struc=myMesh->Result();
  Standard_Integer e1, e2, e3;
  Standard_Boolean o1, o2, o3;
  Standard_Integer in, il, ie;
  Standard_Integer nbn=struc->NbNodes();
  Standard_Integer nbl=struc->NbLinks();
  Standard_Integer nbe=struc->NbElements();

  for (in=1; in<=nbn; in++) {
  BRepMesh_Vertex nod=struc->GetNode(in);
  S<<"(node "<<in<<" (uv "<<nod.Coord().X()<<" "
  <<nod.Coord().Y()<<") (3d "
  <<nod.Location3d()<<") ";
  printdegree(nod.Movability());
  S<<" (edgeconex";
  BRepMesh_ListOfInteger::Iterator tati(struc->LinkNeighboursOf(in));
  for (; tati.More(); tati.Next()) S<<" "<<tati.Value();
  S << "))\n";
  }
  S <<endl;
  for (il=1; il<=nbl; il++) {
  BRepMesh_Edge edg=struc->GetLink(il);
  S << "(edge "<<il<<" ("<<edg.FirstNode()<<" "<<edg.LastNode()
  <<" ";
  printdegree(edg.Movability());
  S<<") (triconex";
  const BRepMesh_PairOfIndex& pair = struc->ElemConnectedTo(il);
  for (Standard_Integer j = 1, jn = pair.Extent(); j <= jn; j++)
  S<<" "<<pair.Index(j);
  S << "))\n";
  }
  S <<endl;
  for (ie=1; ie<=nbe; ie++) {
  BRepMesh_Triangle tri=struc->GetElement(ie);
  tri.Edges(e1, e2, e3, o1, o2, o3); 
  if (!o1) e1=-e1;
  if (!o2) e2=-e2;
  if (!o3) e3=-e3;
  S<<" (maille "<<ie<<" (links "<<e1<<" "
  <<e2<<" "<<e3<<")";
  printdegree(tri.Movability());
  S<<")\n";
  }
  S << endl;
  */
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================
void MeshTest_DrawableMesh::Whatis(Draw_Interpretor& theStream) const 
{
  const TopoDS_Shape& aShape = myMesher->Shape();

  Standard_Integer aPointsNb    = 0;
  Standard_Integer aTrianglesNb = 0;
  Standard_Integer aEdgesNb     = 0;

  TopLoc_Location aLocation;
  Handle(Poly_Triangulation) aTriangulation;

  TopExp_Explorer aFaceIt(aShape, TopAbs_FACE);
  for (; aFaceIt.More(); aFaceIt.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceIt.Current());

    aTriangulation = BRep_Tool::Triangulation(aFace, aLocation);
    if (aTriangulation.IsNull())
      continue;

    // Count number of links
    BRepMesh_MapOfLinks aMap;
    const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();
    for (Standard_Integer i = 1, v[3]; i <= aTriangles.Length(); ++i)
    {
      aTriangles(i).Get(v[0], v[1], v[2]);

      addLink(v[0], v[1], aMap);
      addLink(v[1], v[2], aMap);
      addLink(v[2], v[0], aMap);
    }

    aPointsNb    += aTriangulation->NbNodes();
    aTrianglesNb += aTriangulation->NbTriangles();
    aEdgesNb     += aMap.Extent();
  }

  theStream << " 3d mesh\n";
  theStream << "    - Triangles : " << aTrianglesNb << "\n";
  theStream << "    - Edges     : " << aEdgesNb     << "\n";
  theStream << "    - Point3d   : " << aPointsNb    << "\n";
}

//=======================================================================
//function : Mesher
//purpose  : 
//=======================================================================
const Handle(BRepMesh_IncrementalMesh)& MeshTest_DrawableMesh::Mesher() const 
{
  return myMesher;
}


//=======================================================================
//function : Edges
//purpose  : 
//=======================================================================
TColStd_SequenceOfInteger& MeshTest_DrawableMesh::Edges()
{
  return myEdges;
}


//=======================================================================
//function : Vertices
//purpose  : 
//=======================================================================
TColStd_SequenceOfInteger& MeshTest_DrawableMesh::Vertices()
{
  return myVertices;
}

//=======================================================================
//function : Triangles
//purpose  : 
//=======================================================================
TColStd_SequenceOfInteger& MeshTest_DrawableMesh::Triangles()
{
  return myTriangles;
}

