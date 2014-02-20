// Created on: 1994-07-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#ifdef DRAW
#include <DBRep.hxx>
#include <gp_Lin.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <Draw.hxx>
#include <Draw_MarkerShape.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Segment3D.hxx>
#include <ElCLib.hxx>
#include <TopAbs_State.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Draw_Color.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopOpeBRepTool_DRAW.hxx>

Standard_IMPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color col);
Standard_IMPORT void DBRep_WriteColorOrientation ();
Standard_IMPORT Draw_Color DBRep_ColorOrientation (const TopAbs_Orientation Or);

static Draw_MarkerShape MARKER3DSHAPE = Draw_Square;
static Draw_ColorKind   MARKER3DCOLOR = Draw_magenta;
static Standard_Integer MARKER3DSIZE = 1;
static Draw_ColorKind   SEGMENTCOLOR = Draw_vert;

//===========================================================================
void TopOpeBRepTool_DrawPoint
  (const gp_Pnt& P, const Draw_MarkerShape T, const Draw_ColorKind C) 
//===========================================================================
{ dout<<new Draw_Marker3D(P,T,C,MARKER3DSIZE); dout.Flush(); }
void TopOpeBRepTool_DrawPoint(const gp_Pnt& P, const Draw_ColorKind C) 
{ TopOpeBRepTool_DrawPoint(P,MARKER3DSHAPE,C); }
void TopOpeBRepTool_DrawPoint(const gp_Pnt& P)
{ TopOpeBRepTool_DrawPoint(P,MARKER3DSHAPE,MARKER3DCOLOR); }


//===========================================================================
void TopOpeBRepTool_DrawSegment
  (const gp_Pnt& P1,const gp_Pnt& P2, const Draw_ColorKind C) 
//===========================================================================
{ 
  dout << new Draw_Segment3D(P1,P2,C);
  TopOpeBRepTool_DrawPoint(P1); TopOpeBRepTool_DrawPoint(P2);
}

void TopOpeBRepTool_DrawSegment(const gp_Pnt& P1, const gp_Pnt& P2) 
{ TopOpeBRepTool_DrawSegment(P1,P2,SEGMENTCOLOR); }


//===========================================================================
void TopOpeBRepTool_DrawSegment
  (const gp_Pnt& P,const gp_Lin& L,
   const Standard_Real Par,const Draw_ColorKind C)
//===========================================================================
{ TopOpeBRepTool_DrawSegment(P,ElCLib::Value(Par,L),C); }


//===========================================================================
Draw_ColorKind TopOpeBRepTool_ColorOnState(const TopAbs_State S)
//===========================================================================
{  
  Draw_ColorKind c;
  switch (S) {
  case TopAbs_ON      : c = Draw_blanc; break;
  case TopAbs_UNKNOWN : c = Draw_rouge; break;
  case TopAbs_IN      : c = Draw_rose; break;
  case TopAbs_OUT     : c = Draw_cyan; break;
  }
  return c;
}

//===========================================================================
void TopOpeBRepTool_DrawSegment
  (const gp_Pnt& P,const gp_Lin& L,const Standard_Real Par,
   const TopAbs_State S) 
//===========================================================================
{ TopOpeBRepTool_DrawSegment(P,L,Par,TopOpeBRepTool_ColorOnState(S)); }
void TopOpeBRepTool_DrawSegment 
  (const gp_Pnt& P,const gp_Lin& L,const Standard_Real Par) 
{ TopOpeBRepTool_DrawSegment(P,L,Par,SEGMENTCOLOR); }


Standard_EXPORT void FDRAW_DINS(const TCollection_AsciiString pref,const TopoDS_Shape& SS,const TCollection_AsciiString Snam,const TCollection_AsciiString suff)
{
  DBRep::Set(Snam.ToCString(),SS);
  cout<<pref<<FUN_tool_PRODINS()<<" "<<Snam<<";"<<suff;
}

Standard_EXPORT void FDRAW_DINE(const TCollection_AsciiString pref,const TopoDS_Edge& EE,const TCollection_AsciiString Enam,const TCollection_AsciiString suff)
{
  TCollection_AsciiString VFnam,VRnam,VInam,VEnam;
  TCollection_AsciiString blancV("      ");
  TopoDS_Vertex VF,VR;TopExp::Vertices(EE,VF,VR);
  if ( ! VF.IsNull() && !VR.IsNull() && !EE.IsNull() ) {
    DBRep::Set(Enam.ToCString(),EE);
    Standard_Real f,l; BRep_Tool::Range(EE,f,l);
    cout<<pref<<FUN_tool_PRODINS()<<"-gO "<<Enam<<"; # f,l : "<<f<<","<<l<<suff;
    
#if 0
    Standard_Integer nfo=0,nre=0,nin=0,nex=0;
    TopExp_Explorer exx;
    for (exx.Init(EE,TopAbs_VERTEX);exx.More();exx.Next()) {
//    for (TopExp_Explorer exx(EE,TopAbs_VERTEX);exx.More();exx.Next()) {
      const TopoDS_Vertex& vv = TopoDS::Vertex(exx.Current());
      TopAbs_Orientation vvo = vv.Orientation();
      if      ( vvo == TopAbs_FORWARD ) {
	nfo++; VFnam = Enam + ".vf" + nfo;
	DBRep::Set(VFnam.ToCString(),vv);
	cout<<blancV<<FUN_tool_PRODINS()<<VFnam;
      }
      else if ( vvo == TopAbs_REVERSED ) {
	nre++; VRnam = Enam + ".vr" + nre;
	DBRep::Set(VRnam.ToCString(),vv);
	cout<<blancV<<FUN_tool_PRODINS()<<VRnam;
      }
      else if ( vvo == TopAbs_INTERNAL ) {
	nin++; VInam = Enam + ".vi" + nin;
	DBRep::Set(VInam.ToCString(),vv);
	cout<<blancV<<FUN_tool_PRODINS()<<VInam;
      }
      else if ( vvo == TopAbs_EXTERNAL ) {
	nex++; VEnam = Enam + ".ve" + nex;
	DBRep::Set(VEnam.ToCString(),vv);
	cout<<blancV<<FUN_tool_PRODINS()<<VEnam;
      }
      
      Standard_Real p = BRep_Tool::Parameter(vv,EE);
      cout<<"; #draw ; par/"<<Enam<<" : "<<p<<endl;	
    }
#endif
    
  }
}

Standard_EXPORT void FDRAW_DINLOE
(const TCollection_AsciiString pref,const TopTools_ListOfShape& LOE,const TCollection_AsciiString str1,const TCollection_AsciiString str2)
{
  TopTools_ListIteratorOfListOfShape it(LOE);
  for (Standard_Integer ned=1;it.More();it.Next(),ned++) {
    TCollection_AsciiString Enam  = str1 + str2 + "." + ned;
    FDRAW_DINE(pref,TopoDS::Edge(it.Value()),Enam,"\n");
  }
}

Standard_EXPORT void FUN_tool_draw (const TCollection_AsciiString& aa, const gp_Pnt& p, const gp_Dir& d)
{
  char* aaa = aa.ToCString();
  Handle(Geom_Line) L = new Geom_Line(p,d);
  Handle(Geom_TrimmedCurve) tL = new Geom_TrimmedCurve(L,0.,1.);
  DrawTrSurf::Set(aaa,tL);
}
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString& aa,const gp_Pnt2d& p, const gp_Dir2d& d,const Standard_Integer& i)
{  
  TCollection_AsciiString bb(aa); bb += TCollection_AsciiString(i); char* aaa = bb.ToCString();
  Handle(Geom2d_Line) L = new Geom2d_Line(p,d);
  Handle(Geom2d_TrimmedCurve) tL = new Geom2d_TrimmedCurve(L,0.,.5);
  DrawTrSurf::Set(aaa,tL);
}

Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const gp_Pnt2d& p2d)
{
  char* aaa = aa.ToCString(); DrawTrSurf::Set(aaa,p2d);
}
Standard_EXPORT void FUN_tool_draw (const TCollection_AsciiString aa,const gp_Pnt& p)
{
  char* aaa = aa.ToCString(); DrawTrSurf::Set(aaa,p);
}

Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const Handle(Geom2d_Curve) c2d)
{
  Draw_Color col(Draw_blanc); DrawTrSurf_CurveColor(col); char* aaa = aa.ToCString();
  DrawTrSurf::Set(aaa,c2d);
}
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const Handle(Geom2d_Curve) c2d, 
				   const Standard_Real f, const Standard_Real l)
{
  Standard_Real tol = 1.e-7; Handle(Geom2d_Curve) cc2d; 
  if (Abs(f)<=tol && Abs(l)<=tol) cc2d = c2d;
  else cc2d = new  Geom2d_TrimmedCurve(c2d,f,l);
  FUN_tool_draw(aa,cc2d);
}
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString& aa, const Handle(Geom_Curve)& C)
{
  char* aaa = aa.ToCString();
  DrawTrSurf::Set(aaa,C);
}
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const Handle(Geom_Curve) c, 
				   const Standard_Real f, const Standard_Real l)
{
  Standard_Real tol = 1.e-7; Handle(Geom_Curve) cc; 
  if (Abs(f)<=tol && Abs(l)<=tol) cc = c;
  else cc = new  Geom_TrimmedCurve(c,f,l);
  FUN_tool_draw(aa,cc);
}

Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa, const TopoDS_Shape& s)
{
  char* aaa = aa.ToCString(); DBRep::Set(aaa,s);
}
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const TopoDS_Shape& S,const Standard_Integer is)
{
  TCollection_AsciiString bb(aa); bb += TCollection_AsciiString(is); FUN_tool_draw(bb,S);
}
Standard_EXPORT void FUN_tool_draw(TCollection_AsciiString aa,const TopoDS_Edge& E, const TopoDS_Face& F,const Standard_Integer ie)
{  
  if (E.IsNull())  {cout<<"************* null edge\n"; return;} 
  Standard_Real f,l; const Handle(Geom2d_Curve)& PC = BRep_Tool::CurveOnSurface(E,F,f,l);  
  if (PC.IsNull()) {cout<<"************* no curv on surf\n"; return;}
  TCollection_AsciiString bb(aa); bb += TCollection_AsciiString(ie);  
  char* aaa = bb.ToCString();

  Standard_Boolean coldef = Standard_False;
  TopExp_Explorer ex(F,TopAbs_EDGE);
  Draw_Color col,savecol = DrawTrSurf_CurveColor(Draw_rouge);
  for (; ex.More(); ex.Next()) 
    if (E.IsEqual(ex.Current())) 
      {col = DBRep_ColorOrientation(ex.Current().Orientation()); 
       coldef = Standard_True;
       break;}
  if (!coldef) col = DBRep_ColorOrientation(E.Orientation());
    
  DrawTrSurf_CurveColor(col);  
  DrawTrSurf::Set(aaa,new Geom2d_TrimmedCurve(PC,f,l));
}

Standard_EXPORT const TCollection_AsciiString& FUN_tool_PRODINS() 
{
  static TCollection_AsciiString PRODINS("dins ");
  return PRODINS;
}

Standard_EXPORT const TCollection_AsciiString& FUN_tool_PRODINP() 
{
  static TCollection_AsciiString PRODINP("dinp ");
  return PRODINP;
}

#endif
