// Created on: 1994-08-03
// Created by: Modeling
// Copyright (c) 1994-1999 Matra Datavision
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



#include <MeshTest_DrawableMesh.ixx>
#include <TopoDS.hxx>
#include <Draw.hxx>
#include <Draw_ColorKind.hxx>
#include <Draw_Color.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <Standard_RangeError.hxx>
#include <BRepMesh_DegreeOfFreedom.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_Vertex.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

//=======================================================================
//function : MeshTest_DrawableMesh
//purpose  : 
//=======================================================================

MeshTest_DrawableMesh::MeshTest_DrawableMesh() :
myDeflection(1.), myinshape(Standard_False)
{
}

//=======================================================================
//function : MeshTest_DrawableMesh
//purpose  : 
//=======================================================================

MeshTest_DrawableMesh::MeshTest_DrawableMesh(const TopoDS_Shape&    S,
                                             const Standard_Real    Deflect,
                                             const Standard_Boolean Partage,
                                             const Standard_Boolean inshape) :
myDeflection(Deflect), myinshape(inshape)
{
  Bnd_Box B;
  BRepBndLib::Add(S, B);

  myMesh = new BRepMesh_FastDiscret(Deflect, S, B, 0.5, Partage, inshape);
}


//=======================================================================
//function : MeshTest_DrawableMesh
//purpose  : 
//=======================================================================

MeshTest_DrawableMesh::MeshTest_DrawableMesh(const Handle(BRepMesh_FastDiscret)& Tr):
myDeflection(1.0)
{
  myMesh = Tr;
}


//=======================================================================
//function : MeshTest_DrawableMesh
//purpose  : 
//=======================================================================

void MeshTest_DrawableMesh::Add(const TopoDS_Shape& S)
{
  Bnd_Box B;
  BRepBndLib::Add(S, B);

  if (myMesh.IsNull())
    myMesh=new BRepMesh_FastDiscret(myDeflection, S, B, 0.5, myinshape);
  else
    myMesh->Perform(S);
}

//=======================================================================
//function : AddInShape
//purpose  : 
//=======================================================================

void MeshTest_DrawableMesh::AddInShape(const Standard_Boolean inshape) 
{
  myinshape = inshape;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void MeshTest_DrawableMesh::DrawOn(Draw_Display& D) const 
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
  Handle(MeshTest_DrawableMesh) D = new MeshTest_DrawableMesh();
  return  D;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

static void printdegree(BRepMesh_DegreeOfFreedom dof)
{
  switch (dof) {
  case BRepMesh_InVolume :
    cout << "InVolume";
    break;
  case BRepMesh_OnSurface :
    cout << "OnSurface";
    break;
  case BRepMesh_OnCurve :
    cout << "OnCurve";
    break;
  case BRepMesh_Fixed :
    cout << "Fixed";
    break;
  case BRepMesh_Frontier :
    cout << "Frontier";
    break;
  case BRepMesh_Deleted :
    cout << "Deleted";
    break;
  case BRepMesh_Free :
    cout << "Free";
    break;
  }
}

void MeshTest_DrawableMesh::Dump(Standard_OStream& S) const 
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

void MeshTest_DrawableMesh::Whatis(Draw_Interpretor& S) const 
{
  S << " 3d mesh\n";
  S << "    - Triangles : " << myMesh->NbTriangles() << "\n";
  S << "    - Edges     : " << myMesh->NbEdges()     << "\n";
  S << "    - Vertices  : " << myMesh->NbVertices()  << "\n";
  S << "    - Point3d   : " << myMesh->NbPoint3d()   << "\n";
}

//=======================================================================
//function : Mesh
//purpose  : 
//=======================================================================

Handle(BRepMesh_FastDiscret) MeshTest_DrawableMesh::Mesh() const 
{
  return myMesh;
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

