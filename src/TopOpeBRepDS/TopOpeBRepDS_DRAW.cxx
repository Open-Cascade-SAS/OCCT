// Created on: 1997-11-26
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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


#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#include <TopOpeBRepDS_DRAW.hxx>

#include <DrawTrSurf.hxx>
#include <DBRep.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>

Standard_EXPORT void FUN_draw (const TopoDS_Shape& s)
{
  char* nnn = TCollection_AsciiString("name").ToCString();
  if (s.IsNull()) cout<<"nullshape"<<endl;
  DBRep::Set(nnn,s);
}
Standard_EXPORT void FUN_draw (const gp_Pnt& p)
{
  char* nnn = TCollection_AsciiString("name").ToCString();
  DrawTrSurf::Set(nnn,p);
}
Standard_EXPORT void FUN_draw (const gp_Pnt2d& p)
{
  char* nnn = TCollection_AsciiString("name").ToCString();
  DrawTrSurf::Set(nnn,p);
}
Standard_EXPORT void FUN_draw (const Handle(Geom2d_Curve) c, const Standard_Real dpar)
{
  char* nnn = TCollection_AsciiString("c2d").ToCString();
  if (dpar <= Precision::Confusion()) {
    DrawTrSurf::Set(nnn,c);
    return;
  }
  Handle(Geom2d_TrimmedCurve) tC = new Geom2d_TrimmedCurve(c,0.,dpar);
  DrawTrSurf::Set(nnn,tC);
}

Standard_EXPORT void FUN_draw (const gp_Pnt& p, const gp_Dir& d)
{
  TCollection_AsciiString aa("dir"); FUN_tool_draw(aa,p,d);
}

Standard_EXPORT void FUN_brep_draw (const TCollection_AsciiString& aa, const gp_Pnt& p)
{  
  FUN_tool_draw(aa,p);
}

Standard_EXPORT void FUN_brep_draw (const TCollection_AsciiString& aa, const gp_Pnt& p, const gp_Dir& d)
{
  FUN_tool_draw(aa,p,d);
}
Standard_EXPORT void FUN_brep_draw (const TCollection_AsciiString& aa, const TopoDS_Shape& s)
{
  FUN_tool_draw(aa,s);
}
Standard_EXPORT void FUN_brep_draw 
(const TCollection_AsciiString& aa, const Handle(Geom_Curve)& C, const Standard_Real& f, const Standard_Real& l)
{
  FUN_tool_draw(aa,C,f,l);
}
Standard_EXPORT void FUN_brep_draw 
(const TCollection_AsciiString& aa, const Handle(Geom_Curve)& C)
{
  FUN_tool_draw(aa,C);
}

Standard_EXPORT void FUN_DrawMap
(const TopTools_DataMapOfShapeListOfShape& DataforDegenEd)
{
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itemap(DataforDegenEd);
  for (; itemap.More(); itemap.Next()) {
    TopoDS_Shape v =itemap.Key();
    TopTools_ListIteratorOfListOfShape itoflos(itemap.Value());
    if(!itoflos.More()) continue;
    TopoDS_Shape Ec = itoflos.Value(); 
    if(!itoflos.More()) continue;
    itoflos.Next();
    TopoDS_Shape Ed =itoflos.Value();

    Standard_Boolean tr = Standard_False;
    if (tr) { FUN_draw(v); FUN_draw(Ec); FUN_draw(Ed);} 
  }
}

static Standard_Boolean FUN_hascurveonsurf(const TopoDS_Edge& edge,const TopoDS_Face& face)
{
  TopLoc_Location L;
  Handle(Geom_Surface) S = BRep_Tool::Surface(face,L);

  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&edge.TShape());
  const BRep_ListOfCurveRepresentation& lcr = TE->Curves();
  BRep_ListIteratorOfListOfCurveRepresentation itcr(lcr);
  Standard_Boolean iscurveonS = Standard_False;

  for (;itcr.More();itcr.Next()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    Standard_Boolean iscurveonsurface = cr->IsCurveOnSurface();
    if (!iscurveonsurface) continue;
    iscurveonS = cr->IsCurveOnSurface(S,L);
    if (iscurveonS) break;
  }
  return iscurveonS;
}

Standard_EXPORT void FUN_draw2de (const TopoDS_Shape& ed,const TopoDS_Shape& fa)
{
  char* nnn = TCollection_AsciiString("name").ToCString();
  Standard_Real f,l;
  if (ed.IsNull()) return;
  if (fa.IsNull()) return;
  TopoDS_Edge edge = TopoDS::Edge(ed);
  TopoDS_Face face = TopoDS::Face(fa);
  Standard_Boolean hascons = FUN_hascurveonsurf(edge,face);
  if (!hascons) return;

  TopAbs_Orientation ori = edge.Orientation();
  Standard_Boolean sense = (ori == TopAbs_FORWARD)? Standard_True :Standard_False;
  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(edge, face,f,l);

  Handle(Geom2d_TrimmedCurve) tC2d = new Geom2d_TrimmedCurve(C2d,f,l,sense);
  DrawTrSurf::Set(nnn,tC2d);
} // FUN_draw2de

Standard_EXPORT void FUN_draw2d(const Standard_Real& par,const TopoDS_Edge& E,const TopoDS_Edge& Eref,const TopoDS_Face& Fref)
{
  TopAbs_Orientation oriE = E.Orientation();
  TopAbs_Orientation oriEref = Eref.Orientation();
  Standard_Boolean ErefonFref = Standard_False;
  Standard_Boolean EonFref = Standard_False;
  TopExp_Explorer ex; 
  Standard_Integer ne = 0;
  for (ex.Init(Fref,TopAbs_EDGE); ex.More(); ex.Next()) ne++;
  if (ne < 1) return;
  for (ex.Init(Fref,TopAbs_EDGE); ex.More(); ex.Next()) {
    const TopoDS_Edge ed = TopoDS::Edge(ex.Current());
    if (ed.IsSame(Eref)) {ErefonFref = Standard_True; break;}
    if (ed.IsSame(E)) {EonFref = Standard_True; break;}    
  }
  gp_Pnt2d p2d;
  if (ErefonFref || EonFref) {
    Standard_Real f,l;
    Handle(Geom2d_Curve) C2d;
    if (ErefonFref) {
      C2d = BRep_Tool::CurveOnSurface(Eref,Fref,f,l);  
      FUN_draw2de(Eref,Fref); 
    }
    if (EonFref) {
      C2d = BRep_Tool::CurveOnSurface(E,Fref,f,l); 
      FUN_draw2de(E,Fref); 
    }
    C2d->D0(par,p2d); 
  }
  else {
    Standard_Real f,l;    
    Handle(Geom_Curve) C3d = BRep_Tool::Curve(Eref,f,l);
    gp_Pnt P; C3d->D0(par,P);
    Handle(Geom_Surface) S = BRep_Tool::Surface(Fref);
    GeomAPI_ProjectPointOnSurf PonS(P, S);
    if (!PonS.Extrema().IsDone()) return;
    if (PonS.NbPoints() == 0) return; 
    Standard_Real u,v; PonS.Parameters(1,u,v);
    p2d = gp_Pnt2d(u,v); }
  
  FUN_draw(p2d);
} // FUN_draw2d

#endif
