
#include <BRepBndLib.ixx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Tool.hxx> 
#include <TopoDS.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <Geom_Surface.hxx>
#include <TopLoc_Location.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <BRep_Polygon3D.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <BndLib_Add3dCurve.hxx>


//=======================================================================
//function : Add
//purpose  : Add a shape bounding to a box
//=======================================================================

void BRepBndLib::Add(const TopoDS_Shape& S, Bnd_Box& B)
{
  TopExp_Explorer ex;

  // Add the faces

  BRepAdaptor_Surface BS;
  Handle(Geom_Surface) GS;
  Handle(Poly_Triangulation) T;
  TopLoc_Location l;
  Standard_Integer i, nbNodes;
  BRepAdaptor_Curve BC;

  for (ex.Init(S,TopAbs_FACE); ex.More(); ex.Next()) {
    const TopoDS_Face& F = TopoDS::Face(ex.Current());
    T = BRep_Tool::Triangulation(F, l);
    if (!T.IsNull()) {
      nbNodes = T->NbNodes();
      const TColgp_Array1OfPnt& Nodes = T->Nodes();
      for (i = 1; i <= nbNodes; i++) {
	if (l.IsIdentity()) B.Add(Nodes(i));
	else B.Add(Nodes(i).Transformed(l));
      }
      //       B.Enlarge(T->Deflection());
      B.Enlarge(T->Deflection() + BRep_Tool::Tolerance(F));
    }
    else {
      GS = BRep_Tool::Surface(F, l);
      if (!GS.IsNull()) {
	BS.Initialize(F, Standard_False);
	if (BS.GetType() != GeomAbs_Plane) {
	  BS.Initialize(F);
	  BndLib_AddSurface::Add(BS, BRep_Tool::Tolerance(F), B);
	}
	else {
	  // on travaille directement sur les courbes 3d.
	  TopExp_Explorer ex2(F, TopAbs_EDGE);
	  if (!ex2.More()) {
	    BS.Initialize(F);
	    BndLib_AddSurface::Add(BS, BRep_Tool::Tolerance(F), B);
	  }
	  else {
	    for (;ex2.More();ex2.Next()) {
	      BC.Initialize(TopoDS::Edge(ex2.Current()));
	      BndLib_Add3dCurve::Add(BC, BRep_Tool::Tolerance(F), B);
	    }
	    B.Enlarge(BRep_Tool::Tolerance(F));
	  }
	}
      }
    }
  }

  // Add the edges not in faces


  Handle(TColStd_HArray1OfInteger) HIndices;
  Handle(Poly_PolygonOnTriangulation) Poly;

  for (ex.Init(S,TopAbs_EDGE,TopAbs_FACE); ex.More(); ex.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    Handle(Poly_Polygon3D) P3d = BRep_Tool::Polygon3D(E, l);
    if (!P3d.IsNull()) {
      const TColgp_Array1OfPnt& Nodes = P3d->Nodes();
      nbNodes = P3d->NbNodes();
      for (i = 1; i <= nbNodes; i++) {
	if (l.IsIdentity()) B.Add(Nodes(i));
	else B.Add(Nodes(i).Transformed(l));
      }
      //       B.Enlarge(P3d->Deflection());
      B.Enlarge(P3d->Deflection() + BRep_Tool::Tolerance(E));
    }
    else {
      BRep_Tool::PolygonOnTriangulation(E, Poly, T, l);
      if (!Poly.IsNull()) {
	const TColStd_Array1OfInteger& Indices = Poly->Nodes();
	const TColgp_Array1OfPnt& Nodes = T->Nodes();
	nbNodes = Indices.Length();
	for (i = 1; i <= nbNodes; i++) {
	  if (l.IsIdentity()) B.Add(Nodes(Indices(i)));
	  else B.Add(Nodes(Indices(i)).Transformed(l));
	}
	// 	B.Enlarge(T->Deflection());
	B.Enlarge(Poly->Deflection() + BRep_Tool::Tolerance(E));
      }
      else {
	if (BRep_Tool::IsGeometric(E)) {
	  BC.Initialize(E);
	  BndLib_Add3dCurve::Add(BC, BRep_Tool::Tolerance(E), B);
	}
      }
    }
  }

  // Add the vertices not in edges

  for (ex.Init(S,TopAbs_VERTEX,TopAbs_EDGE); ex.More(); ex.Next()) {
    B.Add(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
    B.Enlarge(BRep_Tool::Tolerance(TopoDS::Vertex(ex.Current())));
  }
}



//=======================================================================
//function : AddClose
//purpose  : Add a precise shape bounding to a box
//=======================================================================

void BRepBndLib::AddClose(const TopoDS_Shape& S, Bnd_Box& B)
{
  TopExp_Explorer ex;

  // No faces

  // Add the edges

  BRepAdaptor_Curve BC;

  for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next()) {
    BC.Initialize(TopoDS::Edge(ex.Current()));
    BndLib_Add3dCurve::Add(BC,0.,B);
  }

  // Add the vertices not in edges

  for (ex.Init(S,TopAbs_VERTEX,TopAbs_EDGE); ex.More(); ex.Next()) {
    B.Add(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
  }
}

