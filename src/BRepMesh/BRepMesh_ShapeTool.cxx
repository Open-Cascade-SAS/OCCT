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

#include <BRepMesh_ShapeTool.ixx>
#include <Geom2d_Curve.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <BRepBndLib.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <TopExp.hxx>
#include <Precision.hxx>
#include <gp_Trsf.hxx>
#include <BRep_Builder.hxx>

Standard_Integer debug=0;

BRepMesh_ShapeTool::BRepMesh_ShapeTool() {}

Standard_Boolean BRepMesh_ShapeTool::MoreInternalVertex() 
{
  while (theVIterator.More()) {
    if (theVIterator.Current().Orientation() == TopAbs_INTERNAL) 
      return Standard_True;
    theVIterator.Next();
  }
  return Standard_False;
}


TopoDS_Vertex BRepMesh_ShapeTool::FirstVertex(const TopoDS_Edge& E)
{
  TopExp_Explorer Ex(E,TopAbs_VERTEX);
  while (Ex.More()) {
    if (Ex.Current().Orientation() == TopAbs_FORWARD) 
      return TopoDS::Vertex(Ex.Current());
    Ex.Next();
  }
  Standard_NoSuchObject::Raise("non existent first vertex");
  return TopoDS_Vertex();
}

TopoDS_Vertex BRepMesh_ShapeTool::LastVertex(const TopoDS_Edge& E)
{
  TopExp_Explorer Ex(E,TopAbs_VERTEX);
  while (Ex.More()) {
    if (Ex.Current().Orientation() == TopAbs_REVERSED) 
      return TopoDS::Vertex(Ex.Current());
    Ex.Next();
  }
  Standard_NoSuchObject::Raise("non existent last vertex");
  return TopoDS_Vertex();
}

void BRepMesh_ShapeTool::Vertices(const TopoDS_Edge& E,
				  TopoDS_Vertex& Vfirst,
				  TopoDS_Vertex& Vlast)
{
  TopExp::Vertices(E, Vfirst, Vlast);
}

Bnd_Box BRepMesh_ShapeTool::Bound(const TopoDS_Face& F)
{
  Bnd_Box Bf;
  BRepBndLib::Add(F, Bf);
  return Bf;
}

Bnd_Box BRepMesh_ShapeTool::Bound(const TopoDS_Edge& E)
{
  Bnd_Box Be;
  BRepBndLib::Add(E, Be);
  return Be;
}

void  BRepMesh_ShapeTool::Parameters(const TopoDS_Edge& E,
				     const TopoDS_Face& F,
				     const Standard_Real W,
				     gp_Pnt2d& UV)
{
  Standard_Real a,b;
  Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(E,F,a,b);
  C->D0(W,UV);
}

void  BRepMesh_ShapeTool::Locate(const BRepAdaptor_Curve& C,
				 const Standard_Real W,
				 Standard_Real& wFound,
				 const gp_Pnt& p3d,
				 gp_Pnt2d& UV)
{
  gp_Pnt plocal(p3d.Transformed(C.Trsf().Inverted()));
  Extrema_LocateExtPC 
    pcos(plocal, C.CurveOnSurface(), W, Precision::PConfusion());
  if (pcos.IsDone()) {
    wFound=pcos.Point().Parameter();
    C.CurveOnSurface().GetCurve()->D0(wFound, UV);
    if (debug!=0) {
      if (pcos.SquareDistance()>(4.* C.Tolerance()* C.Tolerance()))  {
	cout << " ShapeTool :LocateExtPCOnS Done but (Distance "<< 
	  sqrt(pcos.SquareDistance()) << ")(Tolerance "<<C.Tolerance()<<")" << endl;
	cout << "                W given : "<< W<< " W calculated : "<<  
	  wFound << endl;
      }
      else if (debug>1) {
	cout << " ShapeTool : LocateExtPCOnS OK ! "<<endl; 
	cout << "                W given : "<< W<< " W calculated : "<<  
	  wFound << endl;
      }
    }
  }
  else {
    wFound=W;
    if (debug!=0) 
      cout << " ShapeTool : LocateExtPCOnS Not Done ! " << endl;
    C.CurveOnSurface().GetCurve()->D0(W, UV);
  }
}


void BRepMesh_ShapeTool::AddInFace(const TopoDS_Face&          F,
				   Handle(Poly_Triangulation)& T)
{
  static BRep_Builder B1;
  TColgp_Array1OfPnt& Nodes = T->ChangeNodes();
  gp_Trsf tr = F.Location().Transformation();
  tr.Invert();
  for (Standard_Integer i = Nodes.Lower(); i <= Nodes.Upper(); i++) 
    Nodes(i).Transform(tr);
  B1.UpdateFace(F, T);
}
