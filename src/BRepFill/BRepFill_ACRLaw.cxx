// Created on: 1998-09-01
// Created by: Stephanie Humeau
// Copyright (c) 1998-1999 Matra Datavision
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

// calculate the nb of edge of the path
  BRepTools_WireExplorer wexp;
  Standard_Integer NbEdge = 0; 
  for (wexp.Init(myPath); wexp.More(); wexp.Next()) NbEdge++;

// tab to memorize ACR for each edge
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

// return ACR of edges of the trajectory
  OrigParam->SetValue(0,0); 
  for (ipath=1;ipath<=NbEdge;ipath++)
    OrigParam->SetValue(ipath, Orig(ipath));

// process each edge of the trajectory
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
	CBis->Reverse(); // To avoid damaging the topology
	C = CBis;
        First =  C->FirstParameter();
	Last  =  C->LastParameter();
      }
      AC = new  (GeomAdaptor_HCurve) (C, First, Last);

      // Set the parameters for the case multi-edges
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
