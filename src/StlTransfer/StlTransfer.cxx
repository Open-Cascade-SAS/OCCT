// Created on: 2000-06-23
// Created by: Sergey MOZOKHIN
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

//=======================================================================

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <CSLib.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlTransfer.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

//function computes normals for surface
static void Normal(const TopoDS_Face&  aFace,
		   Poly_Connect&       pc,
		   TColgp_Array1OfDir& Nor)
{
  const Handle(Poly_Triangulation)& T = pc.Triangulation();
  BRepAdaptor_Surface S;
  Standard_Boolean hasUV = T->HasUVNodes();
  Standard_Integer i;
  TopLoc_Location l;
  Handle(Geom_Surface) GS = BRep_Tool::Surface(aFace, l);
  
  if (hasUV && !GS.IsNull()) {
    Standard_Boolean OK = Standard_True;
    gp_Vec D1U,D1V;
    gp_Vec D2U,D2V,D2UV;
    gp_Pnt P;
    Standard_Real U, V;
    CSLib_DerivativeStatus Status;
    CSLib_NormalStatus NStat;
    S.Initialize(aFace, Standard_False);
    const TColgp_Array1OfPnt2d& UVNodes = T->UVNodes();
    if (S.GetType() != GeomAbs_Plane) {
      for (i = UVNodes.Lower(); i <= UVNodes.Upper(); i++) {
	U = UVNodes(i).X();
	V = UVNodes(i).Y();
	S.D1(U,V,P,D1U,D1V);
	CSLib::Normal(D1U,D1V,Precision::Angular(),Status,Nor(i));
	if (Status != CSLib_Done) {
	  S.D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
	  CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,Precision::Angular(),OK,NStat,Nor(i));
	}
	if (aFace.Orientation() == TopAbs_REVERSED) (Nor(i)).Reverse();
      }
    }
    else {
      gp_Dir NPlane;
      U = UVNodes(UVNodes.Lower()).X();
      V = UVNodes(UVNodes.Lower()).Y();
      S.D1(U,V,P,D1U,D1V);
      CSLib::Normal(D1U,D1V,Precision::Angular(),Status,NPlane);
      if (Status != CSLib_Done) {
	S.D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
	CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,Precision::Angular(),OK,NStat,NPlane);
      }
      if (aFace.Orientation() == TopAbs_REVERSED) NPlane.Reverse();
      Nor.Init(NPlane);

    }
  }
  else {
    const TColgp_Array1OfPnt& Nodes = T->Nodes();
    Standard_Integer n[3];
    const Poly_Array1OfTriangle& triangles = T->Triangles();
    
    for (i = Nodes.Lower(); i <= Nodes.Upper(); i++) {
      gp_XYZ eqPlan(0, 0, 0);
      for (pc.Initialize(i);  pc.More(); pc.Next()) {
	triangles(pc.Value()).Get(n[0], n[1], n[2]);
	gp_XYZ v1(Nodes(n[1]).Coord()-Nodes(n[0]).Coord());
	gp_XYZ v2(Nodes(n[2]).Coord()-Nodes(n[1]).Coord());
	eqPlan += (v1^v2).Normalized();
      }
      Nor(i) = gp_Dir(eqPlan);
      if (aFace.Orientation() == TopAbs_REVERSED) (Nor(i)).Reverse();
    }
  }
  
}
void StlTransfer::RetrieveMesh (const TopoDS_Shape&  Shape,
										const Handle(StlMesh_Mesh)& Mesh)
{
  for (TopExp_Explorer itf(Shape,TopAbs_FACE); itf.More(); itf.Next()) {
    TopoDS_Face face = TopoDS::Face(itf.Current());
    TopLoc_Location Loc, loc;
    Handle(Poly_Triangulation) theTriangulation = BRep_Tool::Triangulation(face, Loc);
    if (theTriangulation.IsNull()) continue; //Meshing was not done for this face!
    Poly_Array1OfTriangle theTriangles(1,theTriangulation->NbTriangles());
    theTriangles.Assign(theTriangulation->Triangles());
    Mesh->AddDomain (theTriangulation->Deflection());
    
    TColgp_Array1OfPnt thePoints(1, theTriangulation->NbNodes());
    thePoints.Assign(theTriangulation->Nodes());
    //compute normal of face
    const TColgp_Array1OfPnt& Nodes = theTriangulation->Nodes();
    TColgp_Array1OfDir NORMAL(Nodes.Lower(), Nodes.Upper());
    Poly_Connect pc(theTriangulation);
    Normal(face, pc, NORMAL);
    Standard_Integer i;
    for(i=1;i<=thePoints.Length();i++) {
      Standard_Real X1, Y1, Z1;
      gp_Pnt p = thePoints.Value(i);
      p.Transform(Loc.Transformation());
      p.Coord (X1, Y1, Z1);
      Mesh->AddVertex (X1, Y1, Z1);
    }
    try {
      OCC_CATCH_SIGNALS
      for (i=1;i<=theTriangles.Length();i++) {
	Standard_Integer V1, V2, V3;
	Poly_Triangle triangle = theTriangles.Value(i);
	triangle.Get(V1, V2, V3);
	gp_Pnt P1, P2, P3;
	P1 = Mesh->Vertices(Mesh->NbDomains()).Value(V1);
	P2 = Mesh->Vertices(Mesh->NbDomains()).Value(V2);
	P3 = Mesh->Vertices(Mesh->NbDomains()).Value(V3);
	gp_Vec average = NORMAL(V1);;   
	
	//check angle between vectors
	gp_Vec V1V2(P1, P2), V2V3(P2, P3);
	Standard_Integer A,B,C;
	gp_Vec vec = V1V2^V2V3;
	Standard_Real modul1, modul2;
	modul1 = average.XYZ().Modulus();
	modul2 = vec.XYZ().Modulus();
	if (modul2>Precision::Confusion ()) vec.Divide(modul2);
	A=V1;B=V2;C=V3;
	//	vec.Transform(loc);
	if (modul1>Precision::Confusion () && modul2>Precision::Confusion ()) {
	  Standard_Real an = vec.Angle(average);
	  if ( an > M_PI/2)  {
	    A = V3;B=V2;C=V1;
	  }
	  else {
	    A=V1;B=V2;C=V3;
	  }
	}
	Mesh->AddTriangle (A, B, C, average.X(), average.Y(), average.Z());
      }
    }
    catch(Standard_Failure)
      {
#ifdef OCCT_DEBUG
	cout << "Fail in StlTransfer::BuildIncrementalMesh"   << endl;
#endif
      }
  }
}






