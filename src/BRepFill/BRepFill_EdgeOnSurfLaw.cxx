// Created on: 1998-07-27
// Created by: Philippe MANGIN
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

