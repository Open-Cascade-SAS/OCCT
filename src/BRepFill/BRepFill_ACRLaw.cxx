// Created on: 1998-09-01
// Created by: Stephanie Humeau
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Approx_CurvlinFunc.hxx>
#include <BRep_Tool.hxx>
#include <BRepFill.hxx>
#include <BRepFill_ACRLaw.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomFill_HArray1OfLocationLaw.hxx>
#include <GeomFill_LocationGuide.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HArray1OfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepFill_ACRLaw,BRepFill_LocationLaw)

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
      Loc = theLaw;
      Loc->SetOrigine(t1,t2);

      myLaws->SetValue(ipath, Loc->Copy());
      myLaws->ChangeValue(ipath)->SetCurve(AC);
    }
  }
}
