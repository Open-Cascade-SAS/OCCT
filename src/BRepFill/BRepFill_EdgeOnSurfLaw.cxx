// File:	BRepFill_EdgeOnSurfLaw.cxx
// Created:	Mon Jul 27 14:30:36 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <BRepFill_EdgeOnSurfLaw.ixx>

#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HArray1OfShape.hxx>

#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_HArray1OfLocationLaw.hxx>
#include <GeomFill_Darboux.hxx>
#include <GeomFill_CurveAndTrihedron.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>


BRepFill_EdgeOnSurfLaw::BRepFill_EdgeOnSurfLaw(const TopoDS_Wire& Path,
					       const TopoDS_Shape& Surf)
{
  hasresult = Standard_True;
  Init(Path);

  Standard_Boolean Trouve;
  Standard_Integer  ipath;//  ,NbEdge;
  TopAbs_Orientation Or;
  BRepTools_WireExplorer wexp;
  TopExp_Explorer exp;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool B;
  TopoDS_Edge E;
  Handle(Geom2d_Curve) C;
  Handle(Geom2dAdaptor_HCurve) AC2d;
  Handle(Adaptor3d_HCurveOnSurface) AC;
  Handle(BRepAdaptor_HSurface) AS;
  Standard_Real First, Last;
  Handle(GeomFill_Darboux) TLaw = new (GeomFill_Darboux)() ;
  Handle(GeomFill_CurveAndTrihedron) Law = 
    new (GeomFill_CurveAndTrihedron) (TLaw);

  for (ipath=0, wexp.Init(myPath); 
       wexp.More(); wexp.Next()) {
    E = wexp.Current();
//    if (!B.Degenerated(E)) {
    if (!BRep_Tool::Degenerated(E)) {
      ipath++;
      myEdges->SetValue(ipath, E);
      for (Trouve=Standard_False, exp.Init(Surf, TopAbs_FACE);
	   exp.More() && !Trouve; exp.Next()) {
	const TopoDS_Face& F = TopoDS::Face(exp.Current());
	C = BRep_Tool::CurveOnSurface(E, F, First, Last);
	if (!C.IsNull()) {
	  Trouve=Standard_True;
	  AS =  new  (BRepAdaptor_HSurface) (F);
	}
      }
      if (!Trouve) { // Impossible de construire la loi.
	hasresult = Standard_False;
	myLaws.Nullify();
	return;
      }
      
      Or = E.Orientation();
      if (Or == TopAbs_REVERSED) {
	Handle(Geom2d_TrimmedCurve) CBis = 
	  new (Geom2d_TrimmedCurve) (C, First, Last);
	CBis->Reverse(); // Pour eviter de deteriorer la topologie
	C = CBis;
        First =  C->FirstParameter();
	Last  =  C->LastParameter();
      }

      AC2d = new  (Geom2dAdaptor_HCurve) (C,First, Last);
      AC   = new  (Adaptor3d_HCurveOnSurface) 
	(Adaptor3d_CurveOnSurface(AC2d, AS));
      myLaws->SetValue(ipath, Law->Copy());
      myLaws->ChangeValue(ipath)->SetCurve(AC);
    }  
  }
}

 Standard_Boolean BRepFill_EdgeOnSurfLaw::HasResult() const
{
  return hasresult;
}

