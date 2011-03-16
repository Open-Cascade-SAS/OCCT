// File:	BRepFill_ACRLaw.cxx
// Created:	Tue Sep 1 14:13:11 1998
// Author:	Stephanie Humeau
//		<shu@sun17>


#include <BRepFill_ACRLaw.ixx>

#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <BRepFill.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_LocationGuide.hxx>

#include <GeomFill_HArray1OfLocationLaw.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Approx_CurvlinFunc.hxx>

BRepFill_ACRLaw::BRepFill_ACRLaw(const TopoDS_Wire& Path,
				 const Handle(GeomFill_LocationGuide)& theLaw)
{
  Init(Path);

// calcul du nb d'edge du path
  BRepTools_WireExplorer wexp;
  Standard_Integer NbEdge = 0; 
  for (wexp.Init(myPath); wexp.More(); wexp.Next()) NbEdge++;

// tab pour memoriser les ACR pour chaque edge
  OrigParam = new (TColStd_HArray1OfReal)(0,NbEdge);
  TColStd_Array1OfReal Orig (0,NbEdge);
  BRepFill::ComputeACR(Path, Orig);

  Standard_Integer ipath;
  TopAbs_Orientation Or;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool B;
  TopoDS_Edge E;
  Handle(Geom_Curve) C;
  Handle(GeomAdaptor_HCurve) AC;
  Standard_Real First, Last;

// on recupere les ACR des edges de la trajectoire
  OrigParam->SetValue(0,0); 
  for (ipath=1;ipath<=NbEdge;ipath++)
    OrigParam->SetValue(ipath, Orig(ipath));

// on traite chaque edge de la trajectoire
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
      AC = new  (GeomAdaptor_HCurve) (C, First, Last);

      // on Set les parametres pour le cas multi-edges
      Standard_Real t1 = OrigParam->Value(ipath-1);
      Standard_Real t2 = OrigParam->Value(ipath);
      Handle(GeomFill_LocationGuide) Loc;
      Loc = Handle(GeomFill_LocationGuide)::DownCast(theLaw);
      Loc->SetOrigine(t1,t2);

      myLaws->SetValue(ipath, Loc->Copy());
      myLaws->ChangeValue(ipath)->SetCurve(AC);
    }
  }
}
