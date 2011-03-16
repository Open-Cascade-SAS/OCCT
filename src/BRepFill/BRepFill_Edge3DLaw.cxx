// File:	BRepFill_Edge3DLaw.cxx
// Created:	Mon Jul 27 14:13:11 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <BRepFill_Edge3DLaw.ixx>

#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp.hxx>
#include <TopTools_HArray1OfShape.hxx>

#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_HArray1OfLocationLaw.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>

BRepFill_Edge3DLaw::BRepFill_Edge3DLaw(const TopoDS_Wire& Path,
				     const Handle(GeomFill_LocationLaw)& Law)
{
  Init(Path);

  Standard_Integer ipath;
  TopAbs_Orientation Or;
  BRepTools_WireExplorer wexp;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool B;
  TopoDS_Edge E;
  Handle(Geom_Curve) C;
  Handle(GeomAdaptor_HCurve) AC;
  Standard_Real First, Last;

  for (ipath=0, wexp.Init(myPath); 
       wexp.More(); wexp.Next()) {
    E = wexp.Current();
//    if (!B.Degenerated(E)) {
    if (!BRep_Tool::Degenerated(E)) {
      ipath++;
      myEdges->SetValue(ipath, E);
      C = BRep_Tool::Curve(E,First,Last);
      Or = E.Orientation();
      if (Or == TopAbs_REVERSED) {
	Handle(Geom_TrimmedCurve) CBis = 
	  new (Geom_TrimmedCurve) (C, First, Last);
	CBis->Reverse(); // Pour eviter de deteriorer la topologie
	C = CBis;
        First =  C->FirstParameter();
	Last  =  C->LastParameter();
      }

      AC = new  (GeomAdaptor_HCurve) (C,First, Last);
      myLaws->SetValue(ipath, Law->Copy());
      myLaws->ChangeValue(ipath)->SetCurve(AC);
    }  
  }
}




