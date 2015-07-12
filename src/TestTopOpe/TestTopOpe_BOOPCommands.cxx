// Created on: 1996-10-21
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


#include <BRep_Tool.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TestTopOpe.hxx>
#include <TestTopOpe_BOOP.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

extern void suppressarg(Standard_Integer& na,const char** a,const Standard_Integer d);
#ifdef OCCT_DEBUG
extern void TopOpeBRepTool_SettraceEND(const Standard_Boolean);
extern Standard_Boolean TopOpeBRepTool_GettraceEND();
#endif

static const char* kboo[TESTTOPOPE_NKEYS] = {"topo","secc","sece","sec","com","c12","c21","fus"};
static TestTopOpe_BOOP LOCALBOOP(kboo, "r");
TestTopOpe_BOOP* PBOOP=&LOCALBOOP;

void CATIND
(TCollection_AsciiString& s,const Standard_Integer I,const TCollection_AsciiString& a,const TCollection_AsciiString& b) {s=s+a+TCollection_AsciiString(I)+b;}
void CATSHA
(TCollection_AsciiString& s,const TopoDS_Shape& S,const TopOpeBRepDS_DataStructure& BDS,const TCollection_AsciiString& a,const TCollection_AsciiString& b)
{if (S.IsNull()) return;::CATIND(s,BDS.Shape(S),a,b);}
void CATLOSHA
(TCollection_AsciiString& s,const TopTools_ListOfShape& L,const TopOpeBRepDS_DataStructure& BDS,const TCollection_AsciiString& a,const TCollection_AsciiString& aa,const TCollection_AsciiString& bb,const TCollection_AsciiString& b)
{ 
  if (!L.Extent()) return;
  s=s+a;for(TopTools_ListIteratorOfListOfShape i(L);i.More();i.Next())::CATSHA(s,i.Value(),BDS,aa,bb);s=s+b;
}

Standard_Integer TOPOCHKCOMMANDS(TestTopOpe_BOOP& P,Standard_Integer na,const char**a);
Standard_Integer TOPOSETCOMMANDS(TestTopOpe_BOOP& P,Standard_Integer na,const char**a, Draw_Interpretor& di);
Standard_Integer TOPOCOMMANDS(TestTopOpe_BOOP& PT,Standard_Integer na,const char**a, Draw_Interpretor& di);

// ----------------------------------------------------------------------
Standard_Integer TOPOC(Draw_Interpretor& interpretor,Standard_Integer na,const char** a)
{
  if (PBOOP == NULL) return 0;
  Standard_Integer err=0, i;

  if      (!strncmp(a[0],kboo[BOOP_TOPX],4)) {
    err = TOPOCOMMANDS((*PBOOP),na,a,interpretor);
    return err;
  }  
  else if (PBOOP->IsBooope(a[0])) {
    PBOOP->Booope(a[0],(na >= 2) ? a[1] : NULL);
    return 0;
  }
  else if (!strcmp(a[0],"toposet")) {
    err = TOPOSETCOMMANDS((*PBOOP),na,a,interpretor);
    return err;
  }
  else if (!strcmp(a[0],"tsp") ) {
    TopAbs_State sta = TopAbs_UNKNOWN;
    if (na >= 2) {
      if      (!strcmp(a[1],"IN"))  sta = TopAbs_IN;
      else if (!strcmp(a[1],"OUT")) sta = TopAbs_OUT;
      else if (!strcmp(a[1],"ON"))  sta = TopAbs_ON;
      
      if (sta != TopAbs_UNKNOWN) {
	if (na==2) { PBOOP->GetSplit(sta); } // tsp IN/ON/OUT
	else if ( na > 2 ) {
	  TopAbs_ShapeEnum typ = TopAbs_SHAPE;
	  if      (!strcmp(a[2],"e")) typ = TopAbs_EDGE;
	  else if (!strcmp(a[2],"f")) typ = TopAbs_FACE;
	  else if (!strcmp(a[2],"s")) typ = TopAbs_SOLID;
	  if (typ != TopAbs_SHAPE) {
	    if (na == 3) { 
	      // tsp IN/ON/OUT e/f/s
	      PBOOP->GetSplit(typ,sta);
	    }
	    else if (na > 3) { 
	      // tsp IN/ON/OUT e/f/s i1 i2 ...
	      for(i=3;i<na;i++) PBOOP->GetSplit(typ,sta,Draw::Atoi(a[i])); 
	    }
	  }
	  else {
	    // tsp IN/ON/OUT i1 i2 ...
	    for(i=2;i<na;i++) PBOOP->GetSplit(sta,Draw::Atoi(a[i])); 
	  }
	}
      }
      else { // tsp i1 i2 ...
	for (i = 1; i < na; i++) {
	  if ( ISINTEGER(a[i]) ) PBOOP->GetSplit(Draw::Atoi(a[i]));
	}
      }
    }
    else { 
      PBOOP->GetSplit(TopAbs_IN);
      PBOOP->GetSplit(TopAbs_ON);
      PBOOP->GetSplit(TopAbs_OUT); 
    } // tsp
  } // arg0 = tsp
  
  else if (!strcmp(a[0],"tme") ) {
    TopAbs_State sta = TopAbs_UNKNOWN;
    if (na >= 2) {
      if      (!strcmp(a[1],"IN"))  sta = TopAbs_IN;
      else if (!strcmp(a[1],"OUT")) sta = TopAbs_OUT;
      else if (!strcmp(a[1],"ON"))  sta = TopAbs_ON;
      
      if (sta != TopAbs_UNKNOWN) {
	if (na==2) { PBOOP->GetMerged(sta); } // tme IN/ON/OUT
	else if ( na > 2 ) {
	  TopAbs_ShapeEnum typ = TopAbs_SHAPE;
	  if      (!strcmp(a[2],"e")) typ = TopAbs_EDGE;
	  else if (!strcmp(a[2],"f")) typ = TopAbs_FACE;
	  else if (!strcmp(a[2],"s")) typ = TopAbs_SOLID;
	  if (typ != TopAbs_SHAPE) {
	    if (na == 3) { 
	      // tme IN/ON/OUT e/f/s
	      PBOOP->GetMerged(typ,sta);
	    }
	    else if (na > 3) { 
	      // tme IN/ON/OUT e/f/s i1 i2 ...
	      for(i=3;i<na;i++) PBOOP->GetMerged(typ,sta,Draw::Atoi(a[i])); 
	    }
	  }
	  else {
	    // tme IN/ON/OUT i1 i2 ...
	    for(i=2;i<na;i++) PBOOP->GetMerged(sta,Draw::Atoi(a[i])); 
	  }
	}
      }
      else { // tme i1 i2 ...
	for (i = 1; i < na; i++) {
	  if ( ISINTEGER(a[i]) ) PBOOP->GetMerged(Draw::Atoi(a[i]));
	}
      }
    }
    else { 
      PBOOP->GetMerged(TopAbs_IN);
      PBOOP->GetMerged(TopAbs_ON);
      PBOOP->GetMerged(TopAbs_OUT); 
    } // tme
  } // arg0 = tme
  
  else if (!strcmp(a[0],"csecanc") ) {
    Standard_Integer ia;

    Standard_Boolean outdraw = Standard_False,outclear = Standard_False,outAB = Standard_False;
    for (ia=1; ia<na; ia++ ) {
      if      (!strcmp(a[ia],"-d")) outdraw = Standard_True;
      else if (!strcmp(a[ia],"-c")) outclear = Standard_True;
      else if (!strcmp(a[ia],"-AB")) outAB = Standard_True;
    }
    
    Handle(TopOpeBRepBuild_HBuilder) HB = PBOOP->HBuilder();
    Handle(TopOpeBRepDS_HDataStructure) HDS = HB->DataStructure();
    const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
    const TopTools_ListOfShape& loe = HB->Section();

    Standard_Boolean onedef = Standard_False;
    Standard_Integer n = loe.Extent();
    Handle(TColStd_HArray1OfBoolean) tabloe = new TColStd_HArray1OfBoolean(1,n,Standard_False);
    for (ia=1; ia<na; ia++ ) {
      if (!strcmp(a[ia],"-d")) continue;
      if (!strcmp(a[ia],"-c")) continue;
      if (!strcmp(a[ia],"-AB")) continue;
      Standard_Integer ie = Draw::Atoi(a[ia]);
      if (ie>=1 && ie<=n) { onedef = Standard_True; tabloe->SetValue(ie,Standard_True); }
    }
    if (!onedef) tabloe->Init(Standard_True);
    
    TopTools_ListIteratorOfListOfShape itloe(loe);
    for (Standard_Integer ie=1;itloe.More();itloe.Next(),ie++) {
      if (!tabloe->Value(ie)) continue;
      const TopoDS_Shape& es = itloe.Value();
      TopoDS_Shape F1,F2; Standard_Integer IC; TopTools_ListOfShape LF1,LF2,LE1,LE2;
      Standard_Boolean eca = HB->EdgeCurveAncestors(es,F1,F2,IC);
      Standard_Boolean esa = HB->EdgeSectionAncestors(es,LF1,LF2,LE1,LE2);
      TCollection_AsciiString namie("se"); namie = namie + TCollection_AsciiString(ie); 
      DBRep::Set(namie.ToCString(),es);
      if (eca) {
	TCollection_AsciiString s;

	if (outdraw) {
	  if (outclear) s = s + "clear; ";
	  ::CATSHA(s,F1,BDS,"tsee f ","; ");
	  ::CATSHA(s,F2,BDS,"tsee f ","; ");
	  ::CATIND(s,IC,"tsee c ","; ");
	  s = s + "cdins " + namie;
	  interpretor<<s.ToCString()<<"\n";
	  if (outAB) {
	    s = "     ";
	    ::CATSHA(s,F1,BDS,"mksol A f_","; ");
	    ::CATSHA(s,F2,BDS,"mksol B f_","; ");
	    s = s + "topopeload A B; ";
	    s = s + "# " + namie;
	    interpretor<<s.ToCString()<<"\n";
	  }
	}
	else {
	  s = s + "section edge " + TCollection_AsciiString(ie) + " ";
	  ::CATSHA(s,F1,BDS,"  f1 : ","");
	  ::CATSHA(s,F2,BDS,"  f2 : ","");
	  ::CATIND(s,IC,"  c : ","");
	  interpretor<<s.ToCString()<<"\n";
	}
      }

      if (esa) { 
	TCollection_AsciiString s;
#ifdef OCCT_DEBUG
	TopTools_ListIteratorOfListOfShape ils;
#endif
	if (outdraw) { 
	  if (outclear) s = s + "clear; ";
	  ::CATLOSHA(s,LF1,BDS,"tsee f ",""," ","; ");
	  ::CATLOSHA(s,LF2,BDS,"tsee f ",""," ","; ");
	  ::CATLOSHA(s,LE1,BDS,"tsee e ",""," ","; ");
	  ::CATLOSHA(s,LE2,BDS,"tsee e ",""," ","; ");
	  s = s + "cdins " + "-p 0.5 " + namie;
	  interpretor<<s.ToCString()<<"\n";
	  if (outAB) {
	    s = "     ";
	    ::CATLOSHA(s,LF1,BDS,"mksol A ","f_"," ","; ");
	    ::CATLOSHA(s,LF2,BDS,"mksol B ","f_"," ","; ");
	    s = s + "topopeload A B;";
	    s = s + " # " + namie;
	    interpretor<<s.ToCString()<<"\n";
	  }
	}
	else {
	  s = s + "section edge " + TCollection_AsciiString(ie) + " ";
	  ::CATLOSHA(s,LF1,BDS,"  f1 : ",""," ","");
	  ::CATLOSHA(s,LF2,BDS,"  f2 : ",""," ","");
	  ::CATLOSHA(s,LE1,BDS,"  e1 : ",""," ","");
	  ::CATLOSHA(s,LE2,BDS,"  e2 : ",""," ","");
	  interpretor<<s.ToCString()<<"\n";
	}
      }
      if ( !eca && !esa ) {
	interpretor<<"edge "<<ie<<" has no ancestors"<<"\n";
      }
    }
  }
  else if (!strcmp(a[0],"parvi") ) {
    TopoDS_Shape S = DBRep::Get(a[1]);
    TopExp_Explorer ee(S,TopAbs_EDGE);
    for(Standard_Integer ie=1; ee.More(); ee.Next(),ie++) {
      TopoDS_Edge e = TopoDS::Edge(ee.Current());
      e.Orientation(TopAbs_FORWARD);
      TCollection_AsciiString enam("VIe");enam=enam+TCollection_AsciiString(ie);
      TopExp_Explorer ev(e,TopAbs_VERTEX);
      for(Standard_Integer iv=1; ev.More(); ev.Next(),iv++) {
	const TopoDS_Vertex v = TopoDS::Vertex(ev.Current());
	if (v.Orientation()!=TopAbs_INTERNAL) continue;
	TCollection_AsciiString vnam;vnam=enam+"i"+TCollection_AsciiString(iv);
	DBRep::Set(enam.ToCString(),e);
	DBRep::Set(vnam.ToCString(),v);
	interpretor<<"din "<<enam.ToCString()<<" "<<vnam.ToCString();
	interpretor<<" ;#par("<<vnam.ToCString()<<"/"<<enam.ToCString()<<") ";
	//cout.flush();
	interpretor<<"\n";
	Standard_Real p = BRep_Tool::Parameter(v,e);
	interpretor<<p<<"\n"; 
      }
    }
  }
  else if (!strcmp(a[0],"parvi2d") ) {
    TopoDS_Shape S = DBRep::Get(a[1]);
    TopExp_Explorer ef(S,TopAbs_FACE);
    for(Standard_Integer ifa=1; ef.More(); ef.Next(),ifa++) {
      TopoDS_Face f = TopoDS::Face(ef.Current());
      f.Orientation(TopAbs_FORWARD);
      TCollection_AsciiString fnam("VIf");fnam=fnam+TCollection_AsciiString(ifa);
      DBRep::Set(fnam.ToCString(),f);
      TopExp_Explorer ee(f,TopAbs_EDGE);
      for(Standard_Integer ie=1; ee.More(); ee.Next(),ie++) {
	TopoDS_Edge e = TopoDS::Edge(ee.Current());
	e.Orientation(TopAbs_FORWARD);
	TCollection_AsciiString enam;enam=fnam+"e"+TCollection_AsciiString(ie);
	TopExp_Explorer ev(e,TopAbs_VERTEX);
	for(Standard_Integer iv=1; ev.More(); ev.Next(),iv++) {
	  const TopoDS_Vertex v = TopoDS::Vertex(ev.Current());
	  if (v.Orientation()!=TopAbs_INTERNAL) continue;
	  TCollection_AsciiString vnam;vnam=enam+"vi"+TCollection_AsciiString(iv);
	  DBRep::Set(enam.ToCString(),e);
	  DBRep::Set(vnam.ToCString(),v);
	  interpretor<<"din "<<fnam.ToCString()<<" "<<enam.ToCString()<<" "<<vnam.ToCString();
	  interpretor<<" ;#par("<<vnam.ToCString()<<"/"<<enam.ToCString()<<","<<fnam.ToCString()<<") ";
	  //cout.flush();
	  interpretor<<"\n";
	  Standard_Real p=BRep_Tool::Parameter(v,e,f);
	  interpretor<<p<<"\n"; 
	}
      }
    }
  }
  return 0 ;
} // TOPO

// ----------------------------------------------------------------------
#ifdef OCCT_DEBUG
Standard_Integer BOOPCHK(Draw_Interpretor& ,Standard_Integer na,const char** a)
{
  if (!strcmp(a[0],"tchk")) { 
//    Standard_IMPORT extern Standard_Integer TopOpeBRepTool_BOOOPE_CHECK_DEB;
    Standard_IMPORT Standard_Integer TopOpeBRepTool_BOOOPE_CHECK_DEB;
    if ( na >= 2 ) TopOpeBRepTool_BOOOPE_CHECK_DEB = (Draw::Atoi(a[1])) ? 1 : 0;
    return 0;
  }
  return 0;
}
#else
Standard_Integer BOOPCHK(Draw_Interpretor& ,Standard_Integer,const char**) {return 0;}
#endif

// ----------------------------------------------------
Standard_Integer TOPOSETCOMMANDS(TestTopOpe_BOOP& PT,Standard_Integer na,const char** a, Draw_Interpretor& di)
{ 
  switch(na) {
  case 1:
    di<<"toposet <kind> <index> <tol>"<<"\n";
    di<<"toposet <kind = Point> <index> <x> <y> <z>"<<"\n";
    di<<"kind = p | c | s | v | e | f | w "<<"\n";
    return 1;
  case 4:
    if(!strcasecmp(a[1],"p"))
      PT.SetTol(TopOpeBRepDS_POINT,Draw::Atoi(a[2]),Draw::Atof(a[3]));
    else if(!strcasecmp(a[1],"c"))// c ou C
      PT.SetTol(TopOpeBRepDS_CURVE,Draw::Atoi(a[2]),Draw::Atof(a[3]));
    else if(!strcasecmp(a[1],"s"))
      PT.SetTol(TopOpeBRepDS_SURFACE,Draw::Atoi(a[2]),Draw::Atof(a[3]));
    else if(!strcasecmp(a[1],"sh") ||
	    !strcasecmp(a[1],"v") ||
	    !strcasecmp(a[1],"e") ||
	    !strcasecmp(a[1],"f") ||
	    !strcasecmp(a[1],"w"))
      PT.SetTol(Draw::Atoi(a[2]),Draw::Atof(a[3])); 
    break;
  case 6:
    if(!strcasecmp(a[1],"p"))
      PT.SetPnt(Draw::Atoi(a[2]),Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
    else
      return 1;
    break;
  default:
    break;
  } // switch na
  return 0;
}

// -----------------------------------------------------
void TOPOHELP(TestTopOpe_BOOP& PT, Draw_Interpretor& di)
{
  di<<"\n";
  di<<"Help on topo command :"<<"\n";
  di<<"----------------------"<<"\n";
  for (Standard_Integer ic=0;ic<PT.Tnb();ic++) di<<PT.Tkeys(ic).ToCString()<<" : "<<PT.Thelp(ic).ToCString()<<"\n";
  di<<"args :"<<"\n";
  di<<"-2d -no2d : produce (or not) 2d curves in section"<<"\n";
  di<<"-a : section curves approximated (default)"<<"\n";
  di<<"-p : section curve discretisated"<<"\n";
  di<<"-tx [default] | tol1 tol2 : set approximation tolerances"<<"\n";
  di<<"-ti [shape] | tol1 tol2 : set intersection tolerances"<<"\n";
  di<<"-c 0/[1] : clear view before displaying result"<<"\n";
  di<<"-m [0]/1 : execute methods / explode methods"<<"\n";
  di<<"-f [name] : retrieve shape/index <name> among input shapes (exploration index)"<<"\n";
  di<<"-v [0]/1 : do not trace / trace methods name"<<"\n";
  di<<"-d : dump all control variables"<<"\n";
  di<<"-i : initialize control variables to default values"<<"\n";
  di<<"-h : help"<<"\n";
  di<<"----------------------"<<"\n";
}

// -------------------------------------------------------
Standard_Integer VARSTOPOARGS(TestTopOpe_BOOP& PT,Standard_Integer& na,const char** a, Draw_Interpretor& di)
{
  VarsTopo& VT = PT.ChangeVarsTopo();

  Standard_Boolean dump = Standard_False;
  Standard_Boolean help = Standard_False;
  Standard_Integer ia = 1;
  while (ia < na) {
    const char *thea = a[ia];
    if (!strcmp(thea,"-f")) {
      suppressarg(na,a,ia);
      if (ia < na) {
	PT.FindShape(a[ia]);
	suppressarg(na,a,ia);
      }
      else {
	PT.FindShape(0);
      }
    }
    else if      (!strcasecmp(thea,"-2d")) {
      VT.SetC2D(Standard_True);
      suppressarg(na,a,ia);
    } 
    else if (!strcasecmp(thea,"-no2d")){
      VT.SetC2D(Standard_False);
      suppressarg(na,a,ia);
    } 
    else if (!strcasecmp(thea,"-a")) {
      VT.SetOCT(TopOpeBRepTool_APPROX);
      suppressarg(na,a,ia);
    }
    else if (!strcasecmp(thea,"-p")) {
      VT.SetOCT(TopOpeBRepTool_BSPLINE1);
      suppressarg(na,a,ia);
    }
    else if (!strcmp(thea,"-tx")) {
      suppressarg(na,a,ia);
      if (ia + 1 < na) {
	VT.SetTolx(Draw::Atof(a[ia]),Draw::Atof(a[ia + 1]));
	suppressarg(na,a,ia);
	suppressarg(na,a,ia);
      }
      else if (ia < na) {
	if (!strcmp(a[ia],"default")) {
	  VT.SetTolxDef();
	  suppressarg(na,a,ia);
	}
	else {
	  VT.SetTolx(Draw::Atof(a[ia]),Draw::Atof(a[ia]));
	  suppressarg(na,a,ia);
	}
      }
    }
    else if (!strcmp(thea,"-ti")) {
      suppressarg(na,a,ia);
      if (ia + 1 < na) {
	VT.SetToli(Draw::Atof(a[ia]),Draw::Atof(a[ia + 1]));
	suppressarg(na,a,ia);
	suppressarg(na,a,ia);
      }
      else if (ia < na) {
	if (!strcmp(a[ia],"shape")) {
	  VT.SetToliDef();
	  suppressarg(na,a,ia);
	}
	else {
	  VT.SetToli(Draw::Atof(a[ia]),Draw::Atof(a[ia]));
	  suppressarg(na,a,ia);
	}
      }
    }
    else if (!strcmp(thea,"-c")) {
      suppressarg(na,a,ia);
      if (ia < na) {
	VT.SetClear((Standard_Boolean)Draw::Atoi(a[ia]));
	suppressarg(na,a,ia);
      }
      else VT.SetClear(Standard_True);
    }
    else if (!strcasecmp(thea,"-d")) {
      dump = Standard_True;
      suppressarg(na,a,ia);
    }
    else if (!strcasecmp(thea,"-i")) {
      VT.Init();
      suppressarg(na,a,ia);
    }
    else if (!strcmp(thea,"-m")) {
      suppressarg(na,a,ia);
      if (ia < na) {
	VT.SetMode(Draw::Atoi(a[ia]));
	suppressarg(na,a,ia);
      }
      else VT.SetMode(0);
    }
    else if (!strcmp(thea,"-v")) {
      suppressarg(na,a,ia);
      if (ia < na) {
	VT.SetVerbose(Draw::Atoi(a[ia]));
	suppressarg(na,a,ia);
      }
      else VT.SetVerbose(0);
    }
    else if (!strcasecmp(thea,"-h")) {
      help = Standard_True;
      suppressarg(na,a,ia);
    }
    else {
      ia++;
    }
  } // while (ia < na)
  if (dump) VT.Dump(cout);
  if (help) TOPOHELP(PT, di);
  return 0;

} // VARSTOPOARGS

// ----------------------------------------------------
Standard_Integer SETTOPOCOMMANDS(TestTopOpe_BOOP& PT)
{
  PT.Tinit();
  PT.Tadd("-def",0  ,"[a b] : load shapes");
  PT.Tadd("-iss",100,"[a b] : intersection");
  PT.Tadd("-gap",110,"[a b] : gap filler");
  PT.Tadd("-cds",115,"[a b] : complete DS");
  PT.Tadd("-fil",120,"[a b] : filter");
  PT.Tadd("-red",130,"[a b] : reducer");
  PT.Tadd("-rug",140,"[a b] : remove unshared geometry");
  PT.Tadd("-chk",150,"[a b] : check DS");
  PT.Tadd("-bui",200,"[a b] : section building");
  PT.Tadd("-all",290,"[a b] : all");
  return 0;
}

// ----------------------------------------------------
Standard_Integer SETTOPOPREP(TestTopOpe_BOOP& PT,Standard_Integer& na,const char** a)
{
  if (na < 2) return 0;
  const char* thea = a[0];
  PT.mytodoPREP = 999;
  if (!strcasecmp(thea,"topo") && na == 3) {
    PT.mylastPREP = -1;
    return 0;
  }
  if (na == 4) {
    PT.mylastPREP = -1;
  }

  Standard_Integer ia = 1;
  while (ia < na) {
    thea = a[ia];
    Standard_Integer ik = PT.Tstep(thea);
    if (ik != -1) {
      PT.mytodoPREP = ik; 
      suppressarg(na,a,ia); 
    }
    else {
      ia++;
    }
  }
  return 0;
}

// ----------------------------------------------------
Standard_Integer TOPOCOMMANDS(TestTopOpe_BOOP& PT,Standard_Integer na,const char** a, Draw_Interpretor& di)
{ 
  Standard_Integer err = 0;
  if (strncmp(a[0],"topo",4)) return 1;
  if (na == 2 && !strcasecmp(a[1],"-h")) { TOPOHELP(PT, di); return 0; }
  err = VARSTOPOARGS(PT,na,a, di); if (err) return err;
  if (na == 1) return 0; 
  err = SETTOPOPREP(PT,na,a); if (err) return err;
  
#ifdef OCCT_DEBUG
  Standard_Boolean tend = TopOpeBRepTool_GettraceEND();
  TopOpeBRepTool_SettraceEND(PT.ChangeVarsTopo().GetVerbose());
#endif

  err = 0;
  TopOpeBRep_DSFiller* PDSF = PT.myPDSF;
  while (PT.mylastPREP <= PT.mytodoPREP) {
    switch (PT.mylastPREP) {
    case -1   : if (na >= 3) err = PT.LoadShapes(a[1],a[2]);break;
    case  0   : err = PT.LoadnewDS(); PDSF = PT.myPDSF; break;
    case  100 : err = PT.DSF_Insert_InsertIntersection(*PDSF); break;
    case  110 : err = PT.DSF_Insert_Complete_GapFiller(*PDSF);break;
    case  115 : err = PT.DSF_Insert_Complete_CompleteDS(*PDSF); break;
    case  120 : err = PT.DSF_Insert_Complete_Filter(*PDSF); break;
    case  130 : err = PT.DSF_Insert_Complete_Reducer(*PDSF); break;
    case  140 : err = PT.DSF_Insert_Complete_RemoveUnsharedGeometry(*PDSF); break;
    case  150 : err = PT.DSF_Insert_Complete_Checker(*PDSF); break;
    case  160 : err = PT.LoadnewHB(); break;
    case  200 : err = PT.Builder_Perform(); break;
    case  290 : PT.mylastPREP = 1000; break;
    default   : PT.mylastPREP = 1000; break;
    }
    if (err) return err;
  }

#ifdef OCCT_DEBUG
  TopOpeBRepTool_SettraceEND(tend);
#endif
  return 0;
}

//=======================================================================
//function : BOOPCommands
//purpose  : 
//=======================================================================
void TestTopOpe::BOOPCommands(Draw_Interpretor& theCommands)
{
  SETTOPOCOMMANDS(*PBOOP);
  theCommands.Add(kboo[BOOP_TOPX],"-h for help",__FILE__,TOPOC);
  theCommands.Add(kboo[BOOP_SECC],"secc [r]",__FILE__,TOPOC);
  theCommands.Add(kboo[BOOP_SECE],"sece [r]",__FILE__,TOPOC);
  theCommands.Add(kboo[BOOP_SEC],"sec [r]",__FILE__,TOPOC);
  theCommands.Add(kboo[BOOP_C12],"c12 [r]",__FILE__,TOPOC);
  theCommands.Add(kboo[BOOP_C21],"c21 [r]",__FILE__,TOPOC);
  theCommands.Add(kboo[BOOP_COM],"com [r]",__FILE__,TOPOC);
  theCommands.Add(kboo[BOOP_FUS],"fus [r]",__FILE__,TOPOC);
  theCommands.Add("toposet","",__FILE__,TOPOC);
  theCommands.Add("tsp","tsp [state] iS1 : get split parts",__FILE__,TOPOC);
  theCommands.Add("tme","tme [state] iS1 : get merged parts",__FILE__,TOPOC);
  theCommands.Add("csecanc","section edge ancestors. use : puts [csecanc]",__FILE__,TOPOC);
  theCommands.Add("parvi","parvi <shape> : get parameter of internal vertices",__FILE__,TOPOC);
  theCommands.Add("parvi2d","parvi <shape> : get parameter of internal vertices",__FILE__,TOPOC);
  theCommands.Add("tchk","tchk 1/0 : check SameParameter edges or not in BRepBuilderAPI",__FILE__,BOOPCHK);
}

#if 0
    if      (!strcasecmp(thea,"-def")) { PT.mytodoPREP = 0;   suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-iss")) { PT.mytodoPREP = 100; suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-gap")) { PT.mytodoPREP = 110; suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-cds")) { PT.mytodoPREP = 115; suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-fil")) { PT.mytodoPREP = 120; suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-red")) { PT.mytodoPREP = 130; suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-rug")) { PT.mytodoPREP = 140; suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-chk")) { PT.mytodoPREP = 150; suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-bui")) { PT.mytodoPREP = 200; suppressarg(na,a,ia); }
    else if (!strcasecmp(thea,"-all")) { PT.mytodoPREP = 290; suppressarg(na,a,ia); }
#endif
