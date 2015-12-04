// Created on: 1998-07-27
// Created by: Philippe MANGIN
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


#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepFill_EdgeOnSurfLaw.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomFill_CurveAndTrihedron.hxx>
#include <GeomFill_Darboux.hxx>
#include <GeomFill_HArray1OfLocationLaw.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HArray1OfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepFill_EdgeOnSurfLaw,BRepFill_LocationLaw)

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
  Standard_Real First = 0., Last = 0.;
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
      if (!Trouve) { // Impossible to construct the law.
	hasresult = Standard_False;
	myLaws.Nullify();
	return;
      }
      
      Or = E.Orientation();
      if (Or == TopAbs_REVERSED) {
	Handle(Geom2d_TrimmedCurve) CBis = 
	  new (Geom2d_TrimmedCurve) (C, First, Last);
	CBis->Reverse(); // To avoid spoiling the topology
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

