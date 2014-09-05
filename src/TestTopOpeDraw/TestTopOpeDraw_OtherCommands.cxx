// Created on: 1996-02-02
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#include <TestTopOpeDraw.hxx>
#include <TestTopOpeDraw_Displayer.hxx>
#include <TestTopOpeDraw_TTOT.hxx>
#include <DBRep.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <stdio.h>

//-----------------------------------------------------------------------
// 0 = testtopopedraw_cdinp
// 1,2,3,4 = display point p of coords x y z
//-----------------------------------------------------------------------
Standard_Integer testtopopedraw_cdinp(Draw_Interpretor&,Standard_Integer na,const char** a)
{
  if ( na < 5 ) return 0;
  TestTopOpeDraw_P3DDisplayer PD(a[1],Draw::Atof(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]));
  return 0;
}

//-----------------------------------------------------------------------
// DRAWsuppressarg : remove a[d], modify na--
//-----------------------------------------------------------------------
Standard_EXPORT void DRAWsuppressarg(Standard_Integer& na,const char** a,const Standard_Integer d) 
{
  for(Standard_Integer i=d;i<na;i++) {
    a[i]=a[i+1];
    a[i+1]=NULL;
  }
  na--;
}

//-----------------------------------------------------------------------
// DRAWsuppressargs : remove a[*] from <f> to <l>, modify na = na - (l - f)
//-----------------------------------------------------------------------
Standard_EXPORT void DRAWsuppressargs(Standard_Integer& na,const char** a,const Standard_Integer f,const Standard_Integer l) 
{
  if(l == f) 
    DRAWsuppressarg(na,a,l);
  if(l <= f) return;
  for(Standard_Integer i=0;i<na-l;i++) {
    a[i+f]=a[i+l+1];
    a[i+l+1]=NULL;
  }
  na -= l-f+1;
}

//-----------------------------------------------------------------------
void OthersCommands_flags(Standard_Integer& na,const char** a,TestTopOpeDraw_Displayer& TD)
//-----------------------------------------------------------------------
{
  Standard_Integer i,j,l,dloc = 0,iloc,decal = 0, NbArg = na;
  Standard_Boolean dori = Standard_False; // display orientation
  Standard_Boolean tgeo = Standard_False; // name with geometry
  Standard_Boolean tori = Standard_False; // name with orientation
  Standard_Real tpar = -1.0;
  Standard_Integer inbr = 2; Standard_Boolean inbrdef = Standard_False;
  Standard_Boolean col = Standard_False; // display specific color
  Standard_Boolean tolflag = Standard_False;
  Draw_Color Color;
  for (i=1; i<NbArg; i++) {
    iloc = i - decal;
    if(!a[iloc]) break;
    if (a[iloc][0] == '-') {
      l = (Standard_Integer)strlen(a[iloc]);
      for (j=1; j<l; j++) {
	if     (a[iloc][j] == 'g') tgeo=Standard_True;
	else if(!strcmp(a[iloc],"-tol")) tolflag = Standard_True;
	else if(a[iloc][j] == 'o') tori=Standard_True;
	else if(a[iloc][j] == 'O') dori=Standard_True;
	else if(a[iloc][j] == 'i') {
	  if (j<(l-1)) {
	    char sis[40]; sis[0] = '\0';
	    Standard_Integer kk = 0, k = j+1;
	    for (; k<l; k++,kk++,sis[kk] = '\0' ) sis[kk] = a[iloc][k];
	    inbr = Draw::Atoi(sis);
	    inbrdef = Standard_True;
	  }
	}
	else if(a[iloc][j] == 'p') { 
	  if (iloc < na - 1) {
	    tpar = Draw::Atof(a[iloc+1]);
	    dloc++; decal++;
	  }
	}
	else if(a[iloc][j] == 'c') {
	  col=Standard_True;
	  if(strcmp(a[iloc+1], "blanc") == 0)         Color = Draw_blanc;
	  else if (strcmp(a[iloc+1], "rouge") == 0)   Color = Draw_rouge;
	  else if (strcmp(a[iloc+1], "vert") == 0)    Color = Draw_vert;
	  else if (strcmp(a[iloc+1], "bleu") == 0)    Color = Draw_bleu;
	  else if (strcmp(a[iloc+1], "cyan") == 0)    Color = Draw_cyan;
	  else if (strcmp(a[iloc+1], "or") == 0)      Color = Draw_or;
	  else if (strcmp(a[iloc+1], "magenta") == 0) Color = Draw_magenta;
	  else if (strcmp(a[iloc+1], "marron") == 0)  Color = Draw_marron;
	  else if (strcmp(a[iloc+1], "orange") == 0)  Color = Draw_orange;
	  else if (strcmp(a[iloc+1], "rose") == 0)    Color = Draw_rose;
	  else if (strcmp(a[iloc+1], "saumon") == 0)  Color = Draw_saumon;
	  else if (strcmp(a[iloc+1], "violet") == 0) Color = Draw_violet;
	  else if (strcmp(a[iloc+1], "jaune") == 0)   Color = Draw_jaune;
	  else if (strcmp(a[iloc+1], "kaki") == 0)    Color = Draw_kaki;
	  else if (strcmp(a[iloc+1], "corail") == 0)  Color = Draw_corail;
	  decal++;dloc++;
	}
      }
      DRAWsuppressargs(na,a,iloc,iloc+dloc);
      decal++;
      dloc = 0;
    }
  }
  if (inbr < 0 ) inbr = 2;
  TD.DisplayNameWithGeometry(tgeo);
  TD.DisplayNameWithOrientation(tori);
  TD.DisplayGeometry(dori);
  TD.NbIsos(inbr); TD.NbIsosDef(inbrdef);
  TD.SetPar(tpar);
  TD.TolIs(tolflag);
  if(col) TD.SetColor(Color);
}

//-----------------------------------------------------------------------
// OthersCommands_help
//-----------------------------------------------------------------------

Standard_EXPORT void OthersCommands_help(const char* CommandName, const char* syntaxe = "")
{
  if(strlen(syntaxe))
    cout<<CommandName<<syntaxe<<endl;
  cout<<"      -p <parameter> to display the name of an edge round a point of <parameter>"<<endl;
  cout<<"               in [0,1] on edge curve range. Default value is 0.3"<<endl;
//  cout<<"      -p <par> : on edge , diplay name at t in [0..1]"<<endl;
  cout<<"      -o : display name = DBRep name + orientation"<<endl;
  cout<<"      -g : display name = DBRep name + geometry"<<endl;
  cout<<"      -O : visualize shape orientation"<<endl;
  cout<<"      -i<n> : visualize face with <n> isos"<<endl;
  cout<<"      -c <col> : display name with color col (Draw_blanc, Draw_rouge, ...)"<<endl;
  cout<<"      -tol to display vertices with a circle of radius equal to its tolerance."<<endl;
  cout<<"example : '"<<CommandName<<" -p 0.8 e 8 9 23'"<<endl;
  cout<<""<<endl;
}
//-----------------------------------------------------------------------
// 0 = testtopopedraw_cdins
// 1,2,3,... = display shapes and visualise their names
//-----------------------------------------------------------------------

Standard_Integer testtopopedraw_cdins(Draw_Interpretor&,Standard_Integer na,const char** a)
{
  if (na == 1) {
    const char* syntaxe = " [arg] S : display shape S with its DBRep name";
    OthersCommands_help(a[0], syntaxe);
    return 0;
  }
  
  TestTopOpeDraw_Displayer TD;
  OthersCommands_flags(na, a, TD);

  Standard_Integer i ;
  for ( i=1;i<na;i++) {
    const TopoDS_Shape& S = DBRep::Get(a[i]);
    if (S.IsNull()) continue;
    const TopAbs_ShapeEnum t = S.ShapeType();
    if (t == TopAbs_FACE) continue;
    if (t == TopAbs_EDGE) continue;
    if (t == TopAbs_VERTEX) continue;
    TD.DisplayShape(a[i],S);
  }
  
  for (i=1;i<na;i++) {
    const TopoDS_Shape& S = DBRep::Get(a[i]); if (S.IsNull()) continue;
    const TopAbs_ShapeEnum t = S.ShapeType();    
    if (t == TopAbs_FACE) TD.DisplayShape(a[i],S);
  }
  
  for (i=1;i<na;i++) {
    const TopoDS_Shape& S = DBRep::Get(a[i]); if (S.IsNull()) continue;
    const TopAbs_ShapeEnum t = S.ShapeType();
    if (t == TopAbs_EDGE) TD.DisplayShape(a[i],S);
  }
  
  for (i=1;i<na;i++) {
    const TopoDS_Shape& S = DBRep::Get(a[i]); if (S.IsNull()) continue;
    const TopAbs_ShapeEnum t = S.ShapeType();
    if (t != TopAbs_VERTEX) continue;
    if(TD.TolIs()) {
      const TopoDS_Vertex& V = TopoDS::Vertex(S);
      Standard_Real Tol = BRep_Tool::Tolerance(V);
      TD.SetTol(Tol);
    }
    TD.DisplayShape(a[i],S);
  }

  return 0;
}

static void BoopReadInitFile(Draw_Interpretor& di, const char* filename)
{
  if (filename == NULL) return;
  di.EvalFile(filename);
}

static Standard_Integer ttab(Draw_Interpretor& di, Standard_Integer /*narg*/, const char** /*a*/) {
  char s[2000]; strcpy(s,"");
  Sprintf(s,"%s%s",s,"proc addt {at args} {upvar $at x;set L \"\";addl L [join $args];");
  Sprintf(s,"%s%s",s,"foreach l $L {set x([array size x]) $l}};");
  di.Eval(s);
  return 0;
}

Standard_IMPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color col);
Standard_IMPORT void DBRep_WriteColorOrientation ();
Standard_IMPORT Draw_Color DBRep_ColorOrientation (const TopAbs_Orientation Or);

#include <Geom2d_TrimmedCurve.hxx>
#include <DrawTrSurf.hxx>
#include <TopExp.hxx>

#include <TopTools_DataMapIteratorOfDataMapOfOrientedShapeInteger.hxx>
#include <TopTools_DataMapOfOrientedShapeInteger.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfIntegerShape.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TColStd_ListIteratorOfListOfAsciiString.hxx>
#include <TColStd_ListOfAsciiString.hxx>
#include <TestTopOpeDraw_ListOfPnt2d.hxx>

class cvx2d {
  
public: 
  cvx2d(Draw_Interpretor& di) { 
    clearall();
    mypdi = &di;
  }
  
  const TopoDS_Face& face() const { return myface; }
  const TCollection_AsciiString& facename() const { return myfacename; }

  void erasevisible() {
    TColStd_ListIteratorOfListOfAsciiString i(myvis);
    Standard_Boolean ya = i.More();
    TCollection_AsciiString str = "erase";
    for (; i.More(); i.Next()) {
      const TCollection_AsciiString& s = i.Value();
      str = str + " " + s;
    }
    if (ya) {
      TCollection_AsciiString str1 = "erase -mute;"; mypdi->Eval(str1.ToCString());
      str = str + ";"; mypdi->Eval(str.ToCString());
    }
    clearvisible();
  }
  
  void addvisible(const TCollection_AsciiString& N) { myvis.Append(N); }
  void clearvisible() { myvis.Clear(); }
  
  const TopoDS_Edge& edge(const Standard_Integer ie) const {
    if ( ie >= 1 && ie <= mymapis.Extent()) {
      const TopoDS_Edge& E = TopoDS::Edge(mymapis.Find(ie));
      return E;
    }
    return myemptyedge;
  }

  Standard_Integer nedge() const { return mymapis.Extent(); }

  void clearall() {
    clearvisible();
    mymapve.Clear();
    mymapsi.Clear(); mymapis.Clear(); mynemap = 0; myiemap = 1; myedgedisplayed = 0;
    myface.Nullify();
    myfacename = "";
  }
  
  void addedge(const TopoDS_Shape& E) {
    mynemap++;
    mymapsi.Bind(E,mynemap);
    mymapis.Bind(mynemap,E);
  }
  
  void initface(const TopoDS_Face& F,const TCollection_AsciiString& N) {
    clearall();
    myface = F; myfacename = N;
    TopExp::MapShapesAndAncestors(myface,TopAbs_VERTEX,TopAbs_EDGE,mymapve);
    TopExp_Explorer e;
    for (e.Init(F,TopAbs_EDGE);e.More();e.Next()) addedge(e.Current());
//    for (TopExp_Explorer e(F,TopAbs_EDGE);e.More();e.Next()) addedge(e.Current());
    initedgeiter();
    setedgedisplayed(0);
  }
  
  void displayface() const {
    char s[1000];
    mypdi->Eval("info proc vx2d_displayface");
    if (mypdi->Result()) {
      Sprintf(s,"vx2d_displayface %s",myfacename.ToCString());
      mypdi->Eval(s);
    }
    else cout<<"procedure vx2d_displayface non trouvee"<<endl;
  }
  
  Standard_Integer eindex(const TopoDS_Shape& E) const {
    const Standard_Integer ie = mymapsi.Find(E);
    return ie;
  }

  void ename(const TopoDS_Shape& S,TCollection_AsciiString& ste) const {
    ste = "";
    if (S.ShapeType() != TopAbs_EDGE) return;
    const TopoDS_Edge& E = TopoDS::Edge(S);
    const Standard_Integer ie = eindex(E);
    TopAbs_Orientation oe = E.Orientation();
    TCollection_AsciiString stoe;TestTopOpeDraw_TTOT::OrientationToString(oe,stoe);
    stoe = stoe.SubString(1,1);stoe.LowerCase();
    Standard_Boolean deg = BRep_Tool::Degenerated(E);
    if (deg) ste="D";
    ste=ste+"e"+ie+stoe;
  }

  void vname(const TopoDS_Shape& V, const TopoDS_Shape& E,TCollection_AsciiString& stv) const {
    stv = "";
    if (V.ShapeType() != TopAbs_VERTEX) return;
    TopAbs_Orientation ov = V.Orientation();
    TCollection_AsciiString sto;TestTopOpeDraw_TTOT::OrientationToString(ov,sto);
    TCollection_AsciiString sto11 = sto.SubString(1,1); sto11.LowerCase();
    const TopTools_ListOfShape& l = mymapve.FindFromKey(V);
    Standard_Integer ne=l.Extent(); TCollection_AsciiString ste; ename(E,ste);
    stv.Copy("v"); stv=stv+sto11+ste+"."+ne;
  }

  void fenames(TCollection_AsciiString& stef) const {
    stef="";
    Standard_Integer i=1,n=nedge();
    for (;i<=n;i++) {
      TCollection_AsciiString ste;ename(edge(i),ste);
      stef=stef+ste+" ";
    }
  }

  Draw_Color color(const TopoDS_Shape& S) const {
    Draw_Color col = Draw_blanc;
    TopAbs_Orientation o = S.Orientation();
    TopAbs_ShapeEnum t = S.ShapeType();
    if ( t == TopAbs_VERTEX ) { 
      if      ( o == TopAbs_FORWARD )  col = Draw_magenta;
      else if ( o == TopAbs_REVERSED ) col = Draw_cyan;
      else col = DBRep_ColorOrientation(o);
    }
    else if ( t == TopAbs_EDGE ) {
      if      (o == TopAbs_FORWARD)  col = Draw_magenta;
      else if (o == TopAbs_REVERSED) col = Draw_cyan;
      else col = DBRep_ColorOrientation(o);
    }
    return col;
  }

  const TopTools_ListOfShape& incidentedges(const TopoDS_Shape& V) const {
    if ( ! mymapve.Contains(V) ) return myemptylos;
    return mymapve.FindFromKey(V);
  }
  
  void lenames(const TopTools_ListOfShape& l,TCollection_AsciiString& stel) const {
    stel = "";Standard_Integer j = 0;
    for (TopTools_ListIteratorOfListOfShape it(l);it.More();it.Next(),j++) {
      if (j) stel = stel + " ";
      TCollection_AsciiString ste; ename(it.Value(),ste);
      stel = stel + ste;
    }
  }
  
  void subshapename(const TCollection_AsciiString& sts,const TopAbs_ShapeEnum tss,const Standard_Integer i,TCollection_AsciiString& stss) const {
    TCollection_AsciiString s; TestTopOpeDraw_TTOT::ShapeEnumToString(tss,s);
    stss=sts+"_"+s+i;
  }

  Handle(Geom2d_Curve) curve2d(const TopoDS_Edge& E) const {
    Standard_Real f,l; const Handle(Geom2d_Curve) c = BRep_Tool::CurveOnSurface(E,myface,f,l);
    if (c.IsNull()) return c;
    Handle(Geom2d_Curve) nc = new Geom2d_TrimmedCurve(c,f,l);
    return nc;
  }

  void printedges() const {
    Standard_Integer n=nedge();if (!n) return;
//JR/Hp
    TCollection_AsciiString se=(Standard_CString ) ((n==1)?" edge :":" edges :");
//    TCollection_AsciiString se=(n==1)?" edge :":" edges :";
    TCollection_AsciiString s;s=s+"# face "+myfacename+" : "+n+se;
    TCollection_AsciiString sb(s.Length()-1,' ');
    cout<<endl<<s;
    for (Standard_Integer i=1;i<=n;i++) {
      TCollection_AsciiString stei;ename(edge(i),stei);cout<<" "<<stei;
      if ((i>1) && (i%4 == 0) && (i<n)) cout<<endl<<"#"<<sb;
    }
    cout<<endl;
    cout.flush();
  }
  
  Standard_Integer displayface(const TopoDS_Shape& S, const TCollection_AsciiString& sta);
  Standard_Integer displayedge(const TopoDS_Shape& S);

  void initedgeiter() { myiemap = 1; }
  Standard_Boolean moreedgeiter() const { return myiemap <= mynemap; }
  void nextedgeiter(const Standard_Integer incr = +1);
  Standard_Integer curredgeiter() const { return myiemap; }
  void setcurredgeiter(const Standard_Integer i) { if (i>=1 &&i<=mynemap) myiemap = i; }
  Standard_Integer edgedisplayed() const { return myedgedisplayed; }
  void setedgedisplayed(const Standard_Integer i) { if (i>=1 &&i<=mynemap) myedgedisplayed = i; }

  TopoDS_Face myface; TCollection_AsciiString myfacename;
  TColStd_ListOfAsciiString myvis;
  TopTools_IndexedDataMapOfShapeListOfShape mymapve;
  
  TopTools_DataMapOfOrientedShapeInteger mymapsi;
  TopTools_DataMapOfIntegerShape         mymapis;
  Standard_Integer mynemap;
  Standard_Integer myiemap;
  Standard_Integer myedgedisplayed;
  
  Draw_Interpretor* mypdi;
  TopoDS_Edge myemptyedge;
  TopTools_ListOfShape myemptylos;
}; // cvx2d

void cvx2d::nextedgeiter(const Standard_Integer incr) 
{
  if (!moreedgeiter()) return;
  myiemap += incr;
  if      (myiemap>mynemap) myiemap = 1;
  else if (myiemap<1) myiemap = mynemap;
}

Standard_Integer cvx2d::displayface(const TopoDS_Shape& S, const TCollection_AsciiString& sta) {
  TopoDS_Shape aLocalShape = S.Oriented(TopAbs_FORWARD) ;
  TopoDS_Face F = TopoDS::Face(aLocalShape); 
//  TopoDS_Face F = TopoDS::Face(S.Oriented(TopAbs_FORWARD)); 
  Standard_Boolean init = Standard_True;
  if (init) initface(F,sta);
  displayface();
  printedges();
  return 0;
} // displayface
  
Standard_Integer cvx2d::displayedge(const TopoDS_Shape& S) {
  if (S.IsNull()) return 0;
  const TopoDS_Edge& E = TopoDS::Edge(S);
  const TopoDS_Face& F = face();
  if (F.IsNull()) return 0;
  
  Draw_Color savecol = DrawTrSurf_CurveColor(Draw_Color(Draw_rouge));
  
  Standard_Real u,v,V,U;BRepTools::UVBounds(F,E,u,U,v,V);
  Standard_Boolean deg = BRep_Tool::Degenerated(E);
  gp_Pnt2d pe1(u,v);gp_Pnt2d pe2(U,V);
  gp_Pnt2d p1,p2;BRep_Tool::UVPoints(E,F,p1,p2);
  TopoDS_Vertex v1,v2; TopExp::Vertices(E,v1,v2);
  gp_Pnt P1 = BRep_Tool::Pnt(v1);
  gp_Pnt P2 = BRep_Tool::Pnt(v2);
  Standard_Real v1v2 = P1.Distance(P2);
  Standard_Real tole = BRep_Tool::Tolerance(E);
  Standard_Real tol1 = BRep_Tool::Tolerance(v1);
  Standard_Real tol2 = BRep_Tool::Tolerance(v2);
  const TopTools_ListOfShape& l1 = incidentedges(v1);
  const TopTools_ListOfShape& l2 = incidentedges(v2);
  Handle(Geom2d_Curve) c2d = curve2d(E);
 
//  char sdi[1000];
  char stol[1000];
  TCollection_AsciiString ste,stc,sv1,sv1a,sv2,sv2a,ste1,ste2;

  ename(E,ste);
  stc = "c"; stc = stc + ste;
  vname(v1,E,sv1); sv1a = "V"; sv1a = sv1a + sv1;
  vname(v2,E,sv2); sv2a = "V"; sv2a = sv2a + sv2;
  lenames(l1,ste1);
  lenames(l2,ste2);

  cout<<endl;
  Sprintf(stol,"%g",tole);
  cout<<"# "<<ste<<" : tole "<<stol<<" : uv "<<pe1.X()<<" "<<pe1.Y()<<" UV "<<pe2.X()<<" "<<pe2.Y();
  cout<<endl;
  Sprintf(stol,"%g",tol1);
  cout<<"# "<<sv1<<" : tol1 "<<stol<<" : uv "<<p1.X()<<" "<<p1.Y()<<" : edges "<<ste1;
  cout<<endl;
  Sprintf(stol,"%g",tol2);
  cout<<"# "<<sv2<<" : tol2 "<<stol<<" : uv "<<p2.X()<<" "<<p2.Y()<<" : edges "<<ste2;
  cout<<endl;
  Sprintf(stol,"%g",v1v2); cout<<"# v1v2 = "<<stol;
  cout<<endl;
  cout.flush();

  erasevisible();
  TestTopOpeDraw_C2DDisplayer TDC2D;
  TestTopOpeDraw_P2DDisplayer TDP2D;
  TestTopOpeDraw_Displayer TD;

  TDC2D.AllColors(color(E)); TDC2D.DisplayC2D(stc,ste,c2d);
  TDP2D.AllColors(color(v1));TDP2D.DisplayP2D(sv1,p1);
  TDP2D.AllColors(color(v2));TDP2D.DisplayP2D(sv2,p2);
  TD.AllColors(color(E));TD.DisplayShape(ste,ste,E);
  if (!deg) {
    TD.AllColors(color(v1));TD.DisplayShape(sv1a,sv1,v1);
    TD.AllColors(color(v2));TD.DisplayShape(sv2a,sv2,v2);
  }
  else {
    TD.AllColors(Draw_Color(Draw_blanc));TD.DisplayShape(sv1a,"",v1);
  }
  addvisible(stc);
  addvisible(sv1);
  addvisible(sv2);
  addvisible(ste);
  addvisible(sv1a);
  addvisible(sv2a);
  Standard_Integer ie = eindex(E);
  setcurredgeiter(ie);
  setedgedisplayed(ie);
  
  DrawTrSurf_CurveColor(savecol);
  return 0;
} // displayedge

//=======================================================================
// vx2d
//=======================================================================
Standard_Integer vx2d(Draw_Interpretor& di, Standard_Integer na, const char** a)
{
  static cvx2d *pv2d = NULL;
#define ISINTEGER(MMstr) ((strspn((MMstr),"0123456789") == strlen((MMstr))))
  
  BoopReadInitFile(di,"vx2d.tcl");
  if (na < 2) return 0;
  if (pv2d == NULL) pv2d = (cvx2d*) new cvx2d(di);
  
  Standard_Integer dostep = 0;Standard_Integer doiedge= 0;
  for(Standard_Integer ia=1;ia<na;ia++) {
    if     (!strcasecmp(a[1],"-n")) { dostep=+1; DRAWsuppressarg(na,a,ia); }
    else if(!strcasecmp(a[1],"-p")) { dostep=-1; DRAWsuppressarg(na,a,ia); }
    else if (ISINTEGER(a[1])) { doiedge=Draw::Atoi(a[1]); DRAWsuppressarg(na,a,ia); }
  }
  
  if (dostep) {
    if (pv2d->edgedisplayed()) pv2d->nextedgeiter(dostep);
    if (pv2d->moreedgeiter()) pv2d->displayedge(pv2d->edge(pv2d->curredgeiter()));
    return 0;
  }
  else if (doiedge) {
    pv2d->setcurredgeiter(doiedge);
    if (pv2d->moreedgeiter()) pv2d->displayedge(pv2d->edge(pv2d->curredgeiter()));
    return 0;
  }
  
  TopoDS_Shape S = DBRep::Get(a[1]); if (S.IsNull()) return 0;
  TCollection_AsciiString sta1(a[1]); TopAbs_ShapeEnum t = S.ShapeType();
  TopAbs_ShapeEnum tt = ( t == TopAbs_FACE) ? TopAbs_EDGE : TopAbs_FACE;

  Standard_Integer iearg = (na >= 3) ? Draw::Atoi(a[2]) : 0;
  TCollection_AsciiString stss; Standard_Integer i = 1;
  TopExp_Explorer ex;
  for (ex.Init(S,tt);ex.More();ex.Next(),i++) {
//  for (TopExp_Explorer ex(S,tt);ex.More();ex.Next(),i++) {
    Standard_Boolean cond = Standard_False;
    cond = cond || (!iearg);
    cond = cond || (iearg && (iearg == i));
    if (cond) {
      pv2d->subshapename(sta1,tt,i,stss);
      DBRep::Set(stss.ToCString(),ex.Current());
      di<<"vx2d "<<stss.ToCString()<<"\n";
      //cout.flush();
    }
  }

  Standard_Integer r = 0;
  if      (t == TopAbs_FACE) r = pv2d->displayface(S,sta1);
  else if (t == TopAbs_EDGE) r = pv2d->displayedge(S);
  return r;
} // vx2d

//=======================================================================
//function : OtherCommands
//purpose  : 
//=======================================================================

void TestTopOpeDraw::OtherCommands(Draw_Interpretor& theCommands)
{
  const char* g = "Topological Operation other commands";
  theCommands.Add("cdinp","cdinp p x y z",__FILE__,testtopopedraw_cdinp,g);
  theCommands.Add("cdins","cdins s1 ... ",__FILE__,testtopopedraw_cdins,g);
  theCommands.Add("vx2d","vx2d f",__FILE__,vx2d,g);
  BoopReadInitFile(theCommands,getenv("BOOPGLOB"));
  theCommands.Add("ttab","",__FILE__,ttab,g);
}
