// File:	StdPrs_ToolShadedShape.cxx
// Created:	Wed Oct 27 09:29:14 1993
// Author:	Jean-LOuis FRENKEL
//		<jlf@stylox>


#include <StdPrs_ToolShadedShape.ixx>
#include <Poly_Triangulation.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Poly_Connect.hxx>
#include <TopAbs_Orientation.hxx>
#include <GeomAbs_SurfaceType.hxx>
//#include <CSLib.hxx>
#include <GeomLib.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <TopLoc_Location.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <TShort_Array1OfShortReal.hxx>

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean StdPrs_ToolShadedShape::IsClosed(const TopoDS_Shape& aShape) 
{
  return aShape.Closed();
}


//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) StdPrs_ToolShadedShape::Triangulation
   (const TopoDS_Face& aFace,
    TopLoc_Location&   loc)
{
  return BRep_Tool::Triangulation(aFace, loc);
}


//=======================================================================
//function : Normal
//purpose  : 
//=======================================================================

void StdPrs_ToolShadedShape::Normal(const TopoDS_Face&  aFace,
				    Poly_Connect&       pc,
				    TColgp_Array1OfDir& Nor)
{
  const Handle(Poly_Triangulation)& T = pc.Triangulation();
  BRepAdaptor_Surface S;
  Standard_Boolean hasUV = T->HasUVNodes();
  Standard_Integer i;
  TopLoc_Location l;
  // move to zero
  TopoDS_Face zeroFace = TopoDS::Face(aFace.Located(TopLoc_Location()));
  //take in face the surface location
  
  //Handle(Geom_Surface) GS = BRep_Tool::Surface(aFace, l);
  Handle(Geom_Surface) GS = BRep_Tool::Surface(zeroFace);

  if(T->HasNormals()) {
    const TColgp_Array1OfPnt& Nodes = T->Nodes();
    const TShort_Array1OfShortReal& Normals = T->Normals();
    const Standard_ShortReal * arrN = &(Normals.Value(Normals.Lower()));
    for( i = Nodes.Lower(); i <= Nodes.Upper(); i++) {
      Standard_Integer in = 3*(i-Nodes.Lower());
      gp_Dir N(arrN[in + 0], arrN[in + 1], arrN[in + 2]);
      Nor(i) = N;
    }

    if (aFace.Orientation() == TopAbs_REVERSED) {
      for( i = Nodes.Lower(); i <= Nodes.Upper(); i++) {
	Nor.ChangeValue(i).Reverse();
      }
    }
  
    
  }
  else if (hasUV && !GS.IsNull()) {
    Standard_Integer nbNormVal  = T->NbNodes() * 3; 
    Handle(TShort_HArray1OfShortReal) Normals = 
      new TShort_HArray1OfShortReal(1, nbNormVal);

    const TColgp_Array1OfPnt2d& UVNodes = T->UVNodes();
    Standard_Real Tol = Precision::Confusion();
    for (i = UVNodes.Lower(); i <= UVNodes.Upper(); i++) {

      if(GeomLib::NormEstim(GS, UVNodes(i), Tol, Nor(i)) > 1) {
	const TColgp_Array1OfPnt& Nodes = T->Nodes();
	Standard_Integer n[3];
	const Poly_Array1OfTriangle& triangles = T->Triangles();

	gp_XYZ eqPlan(0, 0, 0);
	
	Standard_Real modmax = 0.;
	for (pc.Initialize(i);  pc.More(); pc.Next()) {
	  triangles(pc.Value()).Get(n[0], n[1], n[2]);
	  gp_XYZ v1(Nodes(n[1]).Coord()-Nodes(n[0]).Coord());
	  gp_XYZ v2(Nodes(n[2]).Coord()-Nodes(n[1]).Coord());
	  gp_XYZ vv = v1^v2;
	  Standard_Real mod = vv.Modulus();

	  if(mod < Tol) continue;

	  eqPlan += vv/mod;
	}

	modmax = eqPlan.Modulus();
	if(modmax > Tol) Nor(i) = gp_Dir(eqPlan);
	else Nor(i) = gp_Dir(0., 0., 1.);

      }

      Standard_Integer j = (i - UVNodes.Lower()) * 3;
      Normals->SetValue(j + 1, Nor(i).X());
      Normals->SetValue(j + 2, Nor(i).Y());
      Normals->SetValue(j + 3, Nor(i).Z());

      if (aFace.Orientation() == TopAbs_REVERSED) (Nor(i)).Reverse();

    }

    T->SetNormals(Normals);    
  }
  else {
    Standard_Integer nbNormVal  = T->NbNodes() * 3; 
    Handle(TShort_HArray1OfShortReal) Normals = 
      new TShort_HArray1OfShortReal(1, nbNormVal);

    const TColgp_Array1OfPnt& Nodes = T->Nodes();
    Standard_Integer n[3];
    const Poly_Array1OfTriangle& triangles = T->Triangles();
    Standard_Real Tol = Precision::Confusion();

    for (i = Nodes.Lower(); i <= Nodes.Upper(); i++) {
      gp_XYZ eqPlan(0, 0, 0);
      for (pc.Initialize(i);  pc.More(); pc.Next()) {
	triangles(pc.Value()).Get(n[0], n[1], n[2]);
	gp_XYZ v1(Nodes(n[1]).Coord()-Nodes(n[0]).Coord());
	gp_XYZ v2(Nodes(n[2]).Coord()-Nodes(n[1]).Coord());
	gp_XYZ vv = v1^v2;
	Standard_Real mod = vv.Modulus();

	if(mod < Tol) continue;

	eqPlan += vv/mod;
      }

      Standard_Real modmax = eqPlan.Modulus();

      if(modmax > Tol) Nor(i) = gp_Dir(eqPlan);
      else Nor(i) = gp_Dir(0., 0., 1.);

      Nor(i) = gp_Dir(eqPlan);

      Standard_Integer j = (i - Nodes.Lower()) * 3;
      Normals->SetValue(j + 1, Nor(i).X());
      Normals->SetValue(j + 2, Nor(i).Y());
      Normals->SetValue(j + 3, Nor(i).Z());

      if (aFace.Orientation() == TopAbs_REVERSED) (Nor(i)).Reverse();

    }

    T->SetNormals(Normals);
  }

  
}

