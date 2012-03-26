// Created on: 2004-06-25
// Created by: QA Admin (qa)
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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



#include <QABugs.hxx>

#include <Draw_Interpretor.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Geom_Curve.hxx>
#include <DrawTrSurf.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_HCompCurve.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Standard_ErrorHandler.hxx>

#include <stdio.h>
#include <stdlib.h>

static Standard_Integer OCC6001 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc < 4)
  {
    di<<"missing parameters"<<"\n";
    return 1;
  }
  const char *name = argv[1];
  Handle(Adaptor3d_HCurve) hcurve;
  Handle(Geom_Curve) curve = DrawTrSurf::GetCurve(argv[2]);
  if (!curve.IsNull())
    hcurve = new GeomAdaptor_HCurve(curve);
  else
  {
    TopoDS_Shape wire = DBRep::Get(argv[2]);
    if (wire.IsNull() || wire.ShapeType() != TopAbs_WIRE)
    {
      di<<"incorrect 1st parameter, curve or wire expected"<<"\n";
      return 1;
    }
    BRepAdaptor_CompCurve comp_curve(TopoDS::Wire(wire));
    hcurve = new BRepAdaptor_HCompCurve(comp_curve);
  }
  Handle(Geom_Surface) surf = DrawTrSurf::GetSurface(argv[3]);
  Handle(GeomAdaptor_HSurface) hsurf = new GeomAdaptor_HSurface(surf);
  IntCurveSurface_HInter inter;
  inter.Perform(hcurve, hsurf);
  int nb = inter.NbPoints();
  if (!inter.IsDone() || nb == 0)
  {
    di<<"no intersections";
    return 0;
  }
  for (int i=1; i <= nb; i++)
  {
    const IntCurveSurface_IntersectionPoint &int_pnt = inter.Point(i);
    double par = int_pnt.W();
    gp_Pnt p = int_pnt.Pnt();
    di<<"inter "<<i<<": W = "<<par<<"\n"
      <<"\tpnt = "<<p.X()<<" "<<p.Y()<<" "<<p.Z()<<"\n";
    char n[20], *pname=n;
    sprintf(n,"%s_%d",name,i);
    DrawTrSurf::Set(pname,p);
  }

  return 0;

}

static Standard_Integer OCC5696 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,0),gp_Pnt(2,0,0));
  TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge);
  BRepAdaptor_CompCurve curve(wire);
  Standard_Real first = curve.FirstParameter();
  Standard_Real last = curve.LastParameter();
  Standard_Real par = (first + last)/2;
  Standard_Real par_edge;
  TopoDS_Edge edge_found;
  try {
    OCC_CATCH_SIGNALS
    curve.Edge(par,edge_found,par_edge);  // exception is here
    di << "par_edge = " << par_edge << "\n";
  }

  catch (Standard_Failure) {di << "OCC5696 Exception \n" ;return 0;}

  return 0;
}

void QABugs::Commands_5(Draw_Interpretor& theCommands) {
  char *group = "QABugs";

  theCommands.Add ("OCC6001", "OCC6001 name curve/wire surface\n\t\tintersect curve by surface", 
                   __FILE__, OCC6001, group);

  theCommands.Add ("OCC5696", "OCC5696", 
                   __FILE__, OCC5696, group);

  return;
}
