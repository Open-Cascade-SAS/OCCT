// Created on: 1997-07-24
// Created by: Xuan PHAM PHU
// Copyright (c) 1997-1999 Matra Datavision
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

#include <TestTopOpe.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <TopOpeBRepTool_CORRISO.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_REGUS.hxx>
#include <TopOpeBRepTool_CLASSI.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepBuild_define.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepLProp_SLProps.hxx>
#include <Geom_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_IntCS.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <Precision.hxx>
#include <Bnd_Box2d.hxx>
#include <Bnd_Array1OfBox2d.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_ShellToSolid.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SolidClassifier.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Color.hxx>
#include <DBRep.hxx>

#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

Standard_IMPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color col);
Standard_IMPORT void DBRep_WriteColorOrientation ();
Standard_IMPORT Draw_Color DBRep_ColorOrientation (const TopAbs_Orientation Or);

Standard_IMPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color col);
Standard_IMPORT void DBRep_WriteColorOrientation ();
Standard_IMPORT Draw_Color DBRep_ColorOrientation (const TopAbs_Orientation Or);
Standard_EXPORT void FUN_tool_mkBnd2d(const TopoDS_Shape& W, const TopoDS_Shape& F,Bnd_Box2d& B2d);
Standard_EXPORT void FUN_tool_UpdateBnd2d(Bnd_Box2d& B2d, const Bnd_Box2d& newB2d);
Standard_IMPORT Standard_Integer FUN_tool_classiBnd2d(const Bnd_Array1OfBox2d& B,const Standard_Boolean chklarge = Standard_True);
Standard_IMPORT Standard_Boolean FUN_tool_chkp2dFORinE(const gp_Pnt2d& p2d,const TopoDS_Edge& E, 
					  const TopoDS_Face& F, Standard_Real& tol2d);
Standard_IMPORT TopoDS_Vertex FUN_tool_getv(const Standard_Integer Index,const TopoDS_Edge& E);


#define SAME     (-1)
#define DIFF     (-2)
#define UNKNOWN  ( 0)
#define oneINtwo ( 1)
#define twoINone ( 2)

static void FUN_test_draw(TCollection_AsciiString aa,
			  const TopoDS_Edge& E, const TopoDS_Face& F,
			  const Standard_Integer ie,
			  Draw_Interpretor& di)
{  
  if (E.IsNull())  {di<<"************* null edge\n"; return;} 
  Standard_Real f,l; const Handle(Geom2d_Curve)& PC = BRep_Tool::CurveOnSurface(E,F,f,l);  
  if (PC.IsNull()) {di<<"************* no curv on surf\n"; return;}
  TCollection_AsciiString bb(aa); bb += TCollection_AsciiString(ie);  
  char* aaa = (char *)bb.ToCString();
  
  Standard_Boolean coldef = Standard_False;
  TopExp_Explorer ex(F,TopAbs_EDGE);
  Draw_Color col;
  DrawTrSurf_CurveColor(Draw_Color(Draw_rouge));
  for (; ex.More(); ex.Next()) 
    if (E.IsEqual(ex.Current())) 
      {col = DBRep_ColorOrientation(ex.Current().Orientation()); 
       coldef = Standard_True;
       break;}
  if (!coldef) col = DBRep_ColorOrientation(E.Orientation());
  
  DrawTrSurf_CurveColor(col);  
  DrawTrSurf::Set(aaa,new Geom2d_TrimmedCurve(PC,f,l));
}

void FUN_draw(const TCollection_AsciiString aa,const TopoDS_Shape& s)
{char* aaa = (char *)aa.ToCString(); DBRep::Set(aaa,s);}

void FUN_cout(const gp_Pnt2d& p2d, Draw_Interpretor& di)
{di <<" = ("<<p2d.X()<<" "<<p2d.Y()<<") ";}

#ifdef OCCT_DEBUG
Standard_IMPORT void FUN_tool_coutsta(const Standard_Integer& sta, const Standard_Integer& i1, const Standard_Integer& i2);
#endif


Standard_Integer regularize(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face fa = TopoDS::Face(aLocalShape);
//  TopoDS_Face fa = TopoDS::Face(DBRep::Get(a[1]));
  if (fa.IsNull()) {di<<"null face"<<"\n"; return 1;}
  
  TopTools_DataMapOfShapeListOfShape ESplits;
  TopTools_ListOfShape lof;  
  TopOpeBRepTool::Regularize(fa, lof, ESplits);  
  Standard_Integer nfa = lof.Extent();
  di<<"face gives "<<nfa<<" newfaces"<<"\n";
  
  Standard_Integer i = 0;
  TopTools_ListIteratorOfListOfShape itlof(lof) ;
  for ( ; itlof.More(); itlof.Next()) {
    i++;
    TCollection_AsciiString aa = TCollection_AsciiString("fa_");
    aa += TCollection_AsciiString(i);
    FUN_draw(aa,itlof.Value());        
  }
  BRep_Builder BB;
  TopoDS_Compound CC; BB.MakeCompound(CC);
  for (itlof.Initialize(lof); itlof.More(); itlof.Next()) BB.Add(CC, itlof.Value());
  di<<"resulting compound is cmp"<<"\n";
  TCollection_AsciiString aa = TCollection_AsciiString("cmp");
  FUN_draw(aa,CC);  
  
  return 0;
}

static Standard_Integer splitF(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  const TopoDS_Face& fa = TopoDS::Face(aLocalShape);
//  const TopoDS_Face& fa = TopoDS::Face(DBRep::Get(a[1]));
  if (fa.IsNull()) {di<<"null face"<<"\n"; return 1;}
  
  TopTools_ListOfShape fsplits;
//  Standard_Boolean splitok = FUN_tool_SplitF(fa,fsplits);
  Standard_Boolean splitok = TopOpeBRepTool_REGUS::SplitF(fa,fsplits);
  
  if (!splitok) {di<<"no splits"<<"\n"; return 0;}
  di<<"fa gives "<<fsplits.Extent()<<" splits"<<"\n";
  
  BRep_Builder BB;
  TopoDS_Compound CC; BB.MakeCompound(CC);
  for (TopTools_ListIteratorOfListOfShape it(fsplits); it.More(); it.Next()) BB.Add(CC, it.Value());
  
  di<<"resulting compound is cmp"<<"\n";
  TCollection_AsciiString aa = TCollection_AsciiString("cmp");
  FUN_draw(aa,CC); 
  return 0;
}


static Standard_Integer regush(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  const TopoDS_Shape& sha = DBRep::Get(a[1]);
  if (sha.IsNull()) {di<<"null shape"<<"\n"; return 1;}
  TopExp_Explorer ex(sha, TopAbs_SOLID);
  const TopoDS_Solid& so = TopoDS::Solid(ex.Current());
  if (so.IsNull()) {di<<"no solid"<<"\n"; return 1;}
  
  TopTools_DataMapOfShapeListOfShape FSplits;
  TopTools_DataMapOfShapeListOfShape OldSheNewShe;
  
  BRep_Builder BB;
  TopoDS_Compound CC; BB.MakeCompound(CC);
  Standard_Integer nshe = 0;
  Standard_Boolean regu = TopOpeBRepTool::RegularizeShells(so,OldSheNewShe,FSplits);
  if (!regu) {di<<"solid gives no new shell"<<"\n";}
  else {  
    Standard_Integer noldshe = OldSheNewShe.Extent();
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape ite(OldSheNewShe);
    for (; ite.More(); ite.Next()){
      const TopoDS_Shape& oldshe = ite.Key();
      const TopTools_ListOfShape& newshells =  ite.Value();
      if (newshells.IsEmpty()) {BB.Add(CC,oldshe); nshe++;}
      else {
	for (TopTools_ListIteratorOfListOfShape it(newshells); it.More(); it.Next()){
	  BB.Add(CC,it.Value()); nshe++;
	}
      }
    } // i = 1..noldshe
    di <<"noldshe = "<<noldshe<<" gives nshe = "<<nshe<<"\n";
    di<<"resulting compound is cmp"<<"\n";
    TCollection_AsciiString aa = TCollection_AsciiString("cmp");
    FUN_draw(aa,CC);  
  }
  return 0;
}

Standard_Integer reguso(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  const TopoDS_Shape& sha = DBRep::Get(a[1]);
  if (sha.IsNull()) {di<<"null shape"<<"\n"; return 1;}
  TopExp_Explorer ex(sha, TopAbs_SOLID);
  const TopoDS_Solid& so = TopoDS::Solid(ex.Current());
  if (so.IsNull()) {di<<"no solid"<<"\n"; return 1;}
  
  TopTools_DataMapOfShapeListOfShape FSplits;
  TopTools_DataMapOfShapeListOfShape OldSheNewShe;
  
  BRep_Builder BB;
  TopoDS_Compound CC; BB.MakeCompound(CC);
//  Standard_Integer nshe = 0;
  Standard_Boolean regu = TopOpeBRepTool::RegularizeShells(so,OldSheNewShe,FSplits);
  if (!regu) {di<<"solid gives no new shell"<<"\n";}
  else {  
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(OldSheNewShe);
    
    TopOpeBRepBuild_ShellToSolid SheToSo;      
    for (; itm.More(); itm.Next()) {
      const TopTools_ListOfShape& lns = itm.Value();
      TopTools_ListIteratorOfListOfShape itsh(lns);
      for (; itsh.More(); itsh.Next()) {
	const TopoDS_Shell& she = TopoDS::Shell(itsh.Value());
	SheToSo.AddShell(she);
      }
    }
    TopTools_ListOfShape splits; 
    SheToSo.MakeSolids(so,splits);
    
    BRep_Builder BB;
    TopoDS_Compound CC; BB.MakeCompound(CC);
    Standard_Integer nSo = 0;
    TopTools_ListIteratorOfListOfShape itSo(splits);
    for (; itSo.More(); itSo.Next()) {
      const TopoDS_Shape& spli = itSo.Value();
      BB.Add(CC,spli);
      nSo++;
    } 
    di<<"so gives "<<nSo<<" new solids"<<"\n";
    di<<"resulting compound is cmp"<<"\n";
    TCollection_AsciiString aa = TCollection_AsciiString("cmp");
    FUN_draw(aa,CC);  
  }
  return 0;
}

static Standard_Integer purge(Draw_Interpretor& di, Standard_Integer n, const char** a)
{  

  if (n < 2) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face fa = TopoDS::Face(aLocalShape);
//  TopoDS_Face fa = TopoDS::Face(DBRep::Get(a[1]));
  if (fa.IsNull()) return 1;
    TopOpeBRepTool_CORRISO CORRISO(fa); 
  Standard_Real tolF = BRep_Tool::Tolerance(fa);
  Standard_Real uperiod; Standard_Boolean uclosed = CORRISO.Refclosed(1,uperiod);
  Standard_Real vperiod; Standard_Boolean vclosed = CORRISO.Refclosed(2,vperiod);
  if (!uclosed && !vclosed) return 1;
  Standard_Boolean inU = uclosed ? Standard_True : Standard_False;  
  Standard_Real xmin = inU ? (CORRISO.GASref().FirstUParameter()) : (CORRISO.GASref().LastUParameter());
  Standard_Real xper = inU ? uperiod : vperiod;
  Standard_Real tolx = inU ? (CORRISO.Tol(1,tolF)) : (CORRISO.Tol(2,tolF));

  BRep_Builder BB; 
  TopTools_ListOfShape lWs; Standard_Boolean hasnew = Standard_False;
  TopExp_Explorer exw(fa, TopAbs_WIRE);
  for (; exw.More(); exw.Next()){
    const TopoDS_Shape& W = exw.Current();

    CORRISO.Init(W);
    Standard_Boolean ok = CORRISO.UVClosed();
    if (ok) {lWs.Append(W); continue;}

    TopTools_ListOfShape cEds; 
    TopTools_ListIteratorOfListOfShape ite(CORRISO.Eds());
    for (; ite.More(); ite.Next()){
      const TopoDS_Edge& E = TopoDS::Edge(ite.Value());
      Standard_Boolean closing = BRep_Tool::IsClosed(E,fa); 
      if (!closing) {// xpu231198 : pcurve modified, the information is lost
	TopOpeBRepTool_C2DF C2DF; Standard_Boolean isb = CORRISO.UVRep(E,C2DF);
	if (!isb) return 1;//NYIRAISE
	Standard_Boolean onclo = TopOpeBRepTool_TOOL::IsonCLO(C2DF,inU,xmin,xper,tolx);
	if (onclo) closing=Standard_True;
      }
      if (closing) cEds.Append(E);
    }          
    Standard_Integer ncE = cEds.Extent();
    Standard_Boolean nopurge = (ncE <= 1);
    if (nopurge) {lWs.Append(W); continue;}
    
    // Checking <W>
    TopTools_ListOfShape lfyE; Standard_Boolean topurge = CORRISO.PurgeFyClosingE(cEds,lfyE);
    if (!topurge) {lWs.Append(W); continue;}
    
    TopoDS_Wire Wi; BB.MakeWire(Wi); // Add une TShape
    Wi.Free(Standard_True);
    ite.Initialize(CORRISO.Eds());
    for (; ite.More(); ite.Next()) {
      const TopoDS_Edge& ed = TopoDS::Edge(ite.Value());
      Standard_Boolean equ = Standard_False;
      for (TopTools_ListIteratorOfListOfShape itlfyE(lfyE);itlfyE.More();itlfyE.Next()) {
	const TopoDS_Shape& fyE = itlfyE.Value();
	if (ed.IsEqual(fyE)) {
	  equ = Standard_True; 
	  break;
	}
      }
      if (equ) {
	continue;
      }
      BB.Add(Wi,ed);
    }
    lWs.Append(Wi);
  } // exw
  
  if (!hasnew) return 1;

  // Building up the new face :
  aLocalShape = fa.EmptyCopied();
  TopoDS_Face newF = TopoDS::Face(aLocalShape);
//  TopoDS_Face newF = TopoDS::Face(fa.EmptyCopied());
  TopTools_ListIteratorOfListOfShape itw(lWs);  
  for (; itw.More(); itw.Next()) BB.Add(newF, TopoDS::Wire(itw.Value()));

  di <<"New face built is newF"<<"\n";
  TCollection_AsciiString aa("newF");
  FUN_draw(aa, newF);
  return 0;  
}


Standard_Integer correctONISO(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face F   = TopoDS::Face(aLocalShape);
  aLocalShape = DBRep::Get(a[2]);
  TopoDS_Face Fsp = TopoDS::Face(aLocalShape);
//  TopoDS_Face F   = TopoDS::Face(DBRep::Get(a[1]));
//  TopoDS_Face Fsp = TopoDS::Face(DBRep::Get(a[2]));

  FC2D_Prepare(F,F);

  if (F.IsNull() || Fsp.IsNull()) {di<<"NULL shape(s)"<<"\n";return 1;}

  TopOpeBRepTool::CorrectONUVISO(F,Fsp);
  TCollection_AsciiString aa("newFsp");
  FUN_draw(aa,Fsp);
  di<<"-> newFsp"<<"\n";
  
  return 0;  
}

// ======================================================================
//                useful commands :
// ======================================================================

static Standard_Integer isclosingE(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]) ;
  TopoDS_Edge ed = TopoDS::Edge(aLocalShape);
  aLocalShape = DBRep::Get(a[2]);
  TopoDS_Face fa = TopoDS::Face(aLocalShape);
//  TopoDS_Edge ed = TopoDS::Edge(DBRep::Get(a[1]));
//  TopoDS_Face fa = TopoDS::Face(DBRep::Get(a[2]));
  Standard_Boolean isclosing = BRep_Tool::IsClosed(ed,fa);
  TopAbs_Orientation oriE = ed.Orientation();
  if (isclosing) {di <<"edge is ";
		  //TopAbs::Print(oriE,cout);
		  Standard_SStream aSStream;
		  TopAbs::Print(oriE,aSStream);
		  di << aSStream;
		  di<<" closing edge"<<"\n";}
  else di <<"edge is NOT closing edge"<<"\n";
  return 0;
}

static Standard_Integer compareshsh(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape sh1 = DBRep::Get(a[1]);
  TopoDS_Shape sh2 = DBRep::Get(a[2]);
  Standard_Boolean issame = sh1.IsSame(sh2); 
  if (issame) di<<" same shapes"<<"\n";
  else        di <<" shapes are not same"<<"\n";
  Standard_Boolean isequal = sh1.IsEqual(sh2); if (isequal) di<<" equal shapes"<<"\n";
  return 0;
}
static Standard_Integer pcurveedgeonface(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape ed = DBRep::Get(a[1]);
  TopoDS_Shape fa = DBRep::Get(a[2]);
  TCollection_AsciiString aa("edonfa_");
  FUN_test_draw(aa,TopoDS::Edge(ed),TopoDS::Face(fa),0,di);
  return 0;
}

static Standard_Integer pcurvesonface(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape sh = DBRep::Get(a[2]);
  TopoDS_Shape fa = DBRep::Get(a[3]);
  TopExp_Explorer exe(sh, TopAbs_EDGE);
  Standard_Integer i = 0;
  for (; exe.More(); exe.Next()){
    i++;
    TCollection_AsciiString aa(a[1]);
    FUN_test_draw(aa,TopoDS::Edge(exe.Current()),TopoDS::Face(fa),i,di);    
  }
  return 0;
}

static Standard_Integer orivine(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Vertex v = TopoDS::Vertex(aLocalShape);
  aLocalShape = DBRep::Get(a[2]);
  TopoDS_Edge ed = TopoDS::Edge(aLocalShape);
//  TopoDS_Vertex v = TopoDS::Vertex(DBRep::Get(a[1]));
//  TopoDS_Edge ed = TopoDS::Edge(DBRep::Get(a[2]));

  Standard_Integer ori = TopOpeBRepTool_TOOL::OriinSor(v,ed);
  if      (ori==0) di<<"v not in ed"<<"\n";
  else if (ori==1)  di<<"v FORWARD in ed"<<"\n";
  else if (ori==2)  di<<"v REVERSED in ed"<<"\n";
  else if (ori==3)  di<<"v INTERNAL in ed"<<"\n";
  else if (ori==4)  di<<"v EXTERNAL in ed"<<"\n";
  else if (ori==5)  di<<"v CLOSING in ed"<<"\n";
  return 0;
}

static Standard_Integer vine(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Vertex v = TopoDS::Vertex(aLocalShape);
  aLocalShape = DBRep::Get(a[2]);
  TopoDS_Edge ed = TopoDS::Edge(aLocalShape);
  aLocalShape = DBRep::Get(a[3]);
  TopoDS_Face fa = TopoDS::Face(aLocalShape);
//  TopoDS_Vertex v = TopoDS::Vertex(DBRep::Get(a[1]));
//  TopoDS_Edge ed = TopoDS::Edge(DBRep::Get(a[2]));
//  TopoDS_Face fa = TopoDS::Face(DBRep::Get(a[3]));
  
  Standard_Real pf,pl;
  const Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(ed,fa,pf,pl);
  gp_Pnt2d p2df, p2dl; PC->D0(pf,p2df); PC->D0(pl,p2dl);
  di << "p2df";FUN_cout(p2df,di); di << "p2dl";FUN_cout(p2dl,di);di<<"\n";  
  
  Standard_Integer ori = TopOpeBRepTool_TOOL::OriinSor(v,ed);
  if      (ori==0)  {di<<"v not in ed"<<"\n"; return 0;}
  else if (ori==1)  di<<"v FORWARD in ed"<<"\n";
  else if (ori==2)  di<<"v REVERSED in ed"<<"\n";
  else if (ori==3)  di<<"v INTERNAL in ed"<<"\n";
  else if (ori==4)  di<<"v EXTERNAL in ed"<<"\n";
  else if (ori==5)  di<<"v CLOSING in ed"<<"\n";

  if ((ori == 1) || (ori == 2)) {
    Standard_Real par = TopOpeBRepTool_TOOL::ParE(ori,ed);//FUN_tool_parOnE(ind,ed,fa);
    gp_Pnt2d p2d; PC->D0(par,p2d);
    di <<"p2d : with TopOpeBRepTool_TOOL::ParE";FUN_cout(p2d,di);di<<"\n"; 
  }
  
  Standard_Real par = BRep_Tool::Parameter(v,ed,fa);
  gp_Pnt2d pp2d; PC->D0(par,pp2d);
  di <<"p2d computed with BRep_Tool::Parameter";FUN_cout(pp2d,di);di<<"\n";
  
  return 0;
}

static Standard_Integer issubshape(Draw_Interpretor& di, Standard_Integer n, const char** a)
{  
  if (n < 3) return 1;
  TopoDS_Shape subshape = DBRep::Get(a[1]);
  TopoDS_Shape shape    = DBRep::Get(a[2]);
  TopExp_Explorer ex(shape, subshape.ShapeType());
  Standard_Boolean issubs = Standard_False;
  for (; ex.More(); ex.Next())
    if (ex.Current().IsSame(subshape)) {issubs = Standard_True; break;}
  
  if (issubs) di<<" is subshape"<<"\n";
  else        di<<" is NOT subshape"<<"\n";
  return 0;
}

void FUN_mkBnd2dBREP(const TopoDS_Shape& W, const TopoDS_Shape& F,Bnd_Box2d& B2d,const Standard_Integer& i)
{
  // greater <B> with <W>'s UV representation on <F>
  Standard_Real tol = 1.e-8;  
  TopExp_Explorer ex;
  for (ex.Init(W, TopAbs_EDGE); ex.More(); ex.Next()) {
//  for (TopExp_Explorer ex(W, TopAbs_EDGE); ex.More(); ex.Next()) {
    if (i == 0) {
      FUN_tool_mkBnd2d(W,F,B2d);
    }
    if (i == 1) {
      BRepAdaptor_Curve2d BC2d(TopoDS::Edge(ex.Current()), TopoDS::Face(F));
      BndLib_Add2dCurve::Add(BC2d, tol, B2d);
    }
    if (i == 2) {
      Standard_Real f,l;
      Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(TopoDS::Edge(ex.Current()),TopoDS::Face(F),f,l);
      Geom2dAdaptor_Curve GC2d(PC);
      BndLib_Add2dCurve::Add(GC2d, tol, B2d);
    }
    if (i == 3) {
      TopLoc_Location L; Standard_Real f,l;
      const Handle(Geom_Surface)& S = BRep_Tool::Surface(TopoDS::Face(F),L);
      const Handle(Geom2d_Curve)& PC = BRep_Tool::CurveOnSurface(TopoDS::Edge(ex.Current()),S,L,f,l); 
      Geom2dAdaptor_Curve GC2d(PC);
      BndLib_Add2dCurve::Add(GC2d, tol, B2d);
    }
    
  } //ex(W,EDGE)
}

static Standard_Integer drawbnd2d(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 5) return 1;
  Standard_Integer i = Draw::Atoi(a[4]);
  TopoDS_Shape W = DBRep::Get(a[2]);
  TopoDS_Shape F = DBRep::Get(a[3]);
  if (W.IsNull() || F.IsNull()) return 1;
  Bnd_Box2d B2d;
  Standard_Real umin,vmin,umax,vmax;
  FUN_mkBnd2dBREP(W,F,B2d,i);
  B2d.Get(umin,vmin,umax,vmax);
  
  Handle(Geom2d_Line) cx = new Geom2d_Line(gp_Pnt2d(umin,vmin),gp_Dir2d(1.,0.));
  Handle(Geom2d_Line) cy = new Geom2d_Line(gp_Pnt2d(umin,vmin),gp_Dir2d(0.,1.));
  gp_Trsf2d tx; gp_Vec2d vx(umax-umin,0.); tx.SetTranslation(vx);
  gp_Trsf2d ty; gp_Vec2d vy(0.,vmax-vmin); ty.SetTranslation(vy);
  
  Handle(Geom2d_TrimmedCurve) tcx = new Geom2d_TrimmedCurve(cx,0.,umax-umin);
  Handle(Geom2d_TrimmedCurve) tcy = new Geom2d_TrimmedCurve(cy,0.,vmax-vmin);
  Handle(Geom2d_TrimmedCurve) tccx = Handle(Geom2d_TrimmedCurve)::DownCast(tcx->Copy()); tccx->Transform(ty);
  Handle(Geom2d_TrimmedCurve) tccy = Handle(Geom2d_TrimmedCurve)::DownCast(tcy->Copy()); tccy->Transform(tx);
  
  Draw_Color col(Draw_blanc);
  DrawTrSurf_CurveColor(col);
  
  TCollection_AsciiString aa3 = TCollection_AsciiString(a[1]); aa3 += TCollection_AsciiString(3);
  TCollection_AsciiString aa4 = TCollection_AsciiString(a[1]); aa4 += TCollection_AsciiString(4);
  
  TCollection_AsciiString aa;
  aa=TCollection_AsciiString(a[1]); aa+=TCollection_AsciiString(1); char* aaa = (char *)aa.ToCString(); DrawTrSurf::Set(aaa,tcx);
  aa=TCollection_AsciiString(a[1]); aa+=TCollection_AsciiString(2); aaa=(char *)aa.ToCString();         DrawTrSurf::Set(aaa,tcy);
  aa=TCollection_AsciiString(a[1]); aa+=TCollection_AsciiString(3); aaa=(char *)aa.ToCString();         DrawTrSurf::Set(aaa,tccx);
  aa=TCollection_AsciiString(a[1]); aa+=TCollection_AsciiString(4); aaa=(char *)aa.ToCString();         DrawTrSurf::Set(aaa,tccy);
  return 0;
}

static Standard_Integer classifBnd2d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 5) return 1;
  TopoDS_Shape W1 = DBRep::Get(a[1]);
  TopoDS_Shape W2 = DBRep::Get(a[2]);
  TopoDS_Shape F = DBRep::Get(a[3]);
  
  TopoDS_Wire w1 = TopoDS::Wire(W1);
  TopoDS_Wire w2 = TopoDS::Wire(W2);
  TopoDS_Face Fref = TopoDS::Face(F);

  TopOpeBRepTool_CLASSI classi; classi.Init2d(Fref);
  Standard_Real tolF = BRep_Tool::Tolerance(Fref);
  Standard_Real toluv = TopOpeBRepTool_TOOL::TolUV(Fref,tolF);

  if (w1.IsNull() || w2.IsNull() || Fref.IsNull()) return 1;

#ifdef OCCT_DEBUG
  Standard_Integer sta =
#endif
    classi.ClassiBnd2d(w1,w2,toluv,Standard_True);
  di <<"wires classification : checklarge=true ";
#ifdef OCCT_DEBUG
  FUN_tool_coutsta(sta,1,2);
#endif

#ifdef OCCT_DEBUG
  sta =
#endif
    classi.ClassiBnd2d(w1,w2,toluv,Standard_False);
  di <<"wires classification : checklarge=false ";
#ifdef OCCT_DEBUG
  FUN_tool_coutsta(sta,1,2);
#endif
  
  return 0;
}

static Standard_Integer pntonc(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  Standard_Real x = Draw::Atof(a[1]);
  Handle(Geom_Curve) C = DrawTrSurf::GetCurve(a[2]);
  if (C.IsNull()) {di<<"null curve"<<"\n"; return 1;}
  gp_Pnt p = C->Value(x);
  di<<"point on curve of parameter "<<x<<" =("<<p.X()<<",";
  di<<p.Y()<<","<<p.Z()<<")"<<"\n";
  return 0;
}

static Standard_Integer pntonc2d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4) return 1;
  Standard_Real x = Draw::Atof(a[1]);
  Handle(Geom2d_Curve) C2d = DrawTrSurf::GetCurve2d(a[2]);
  if (C2d.IsNull()) {di<<"null curve"<<"\n"; return 1;}
  Handle(Geom_Surface) S   = DrawTrSurf::GetSurface(a[3]);
  if (S.IsNull()) {di<<"null surface"<<"\n"; return 1;}
  gp_Pnt2d p2d = C2d->Value(x);
  di<<"point on curve of parameter "<<x<<" =("<<p2d.X()<<","<<p2d.Y()<<")"<<"\n";
  gp_Pnt p = S->Value(p2d.X(),p2d.Y());
  di<<"point on curve of parameter "<<x<<" =("<<p.X()<<",";
  di<<p.Y()<<","<<p.Z()<<")"<<"\n";	
  return 0;						          
}

static Standard_Integer projponf(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) {
    di << "projponf f pnt [extrema flag: -min/-max/-minmax] [extrema algo: -g(grad)/-t(tree)]\n";
    return 1;
  }
  //
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face f = TopoDS::Face(aLocalShape);
  //
  if (f.IsNull()) {
    di<<"null shape"<<"\n";
    return 1;
  }
  //
  Standard_Real dist=0.;
  Standard_Boolean ok;
  gp_Pnt2d uv;
  gp_Pnt p, pproj; 
  Extrema_ExtAlgo anExtAlgo = Extrema_ExtAlgo_Grad;
  Extrema_ExtFlag anExtFlag = Extrema_ExtFlag_MINMAX;
  //
  DrawTrSurf::GetPoint(a[2], p);
  //
  if (n > 3) {
    const char* key1 = a[3];
    const char* key2 = (n > 4) ? a[4] : NULL;
    if (key1) {
      if (!strcasecmp(key1,"-min")) {
        anExtFlag = Extrema_ExtFlag_MIN;
      } else if (!strcasecmp(key1,"-max")) {
        anExtFlag = Extrema_ExtFlag_MAX;
      } else {
        anExtAlgo = (!strcasecmp(key1,"-t")) ? Extrema_ExtAlgo_Tree : anExtAlgo;
      }
    }
    if (key2) {
      anExtAlgo = (!strcasecmp(key2,"-t")) ? Extrema_ExtAlgo_Tree : anExtAlgo;
    }
  }
  ok = FUN_tool_projPonF(p, f, uv, dist, anExtFlag, anExtAlgo);
  //
  if (!ok) {
    di<<"projection failed"<<"\n"; 
    return 1;
  }
  //
  ok = FUN_tool_value(uv,f,pproj);
  if (!ok) {
    di<<"projection failed"<<"\n"; 
    return 1;
  }
  //
  di<<"proj dist = "<<dist<<" uvproj = ("<<uv.X()<<" "<<uv.Y();
  di<<"); pproj = ("<<pproj.X()<<" "<<pproj.Y()<<" "<<pproj.Z()<<")"<<"\n";
  return 0;
}  

static Standard_Integer tolmax(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  TopoDS_Shape s = DBRep::Get(a[1]);
  if (s.IsNull()) {di<<"null shape"<<"\n"; return 1;}  
  Standard_Real tol = FUN_tool_maxtol(s);
  di<<"max tol = "<<tol<<"\n";
  return 0;
}

static Standard_Integer solidclassifier(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4) return 1;
  TopoDS_Shape s = DBRep::Get(a[1]);
  if (s.IsNull()) {di<<"null shape"<<"\n";return 1;}
  gp_Pnt p; DrawTrSurf::GetPoint(a[2], p);
  Standard_Real tol = Draw::Atof(a[3]);
  
  TopOpeBRepTool_SolidClassifier soclassi;
  TopAbs_State sta = TopAbs_UNKNOWN;
  if      (s.ShapeType() == TopAbs_SOLID) soclassi.Classify(TopoDS::Solid(s),p,tol);
  else if (s.ShapeType() == TopAbs_SHELL) soclassi.Classify(TopoDS::Shell(s),p,tol);
  else {di<<"shape is not a solid nor a shell => KO"<<"\n"; return 1;}
  
  sta = soclassi.State();
  di<<"point is ";
  //TopAbs::Print(sta,cout);
  Standard_SStream aSStream;
  TopAbs::Print(sta,aSStream);
  di << aSStream;
  di<<" shape s"<<"\n";
  return 0;
}

static Standard_Integer class3dclassifier(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4) return 1;
  TopoDS_Shape s = DBRep::Get(a[1]);
  if (s.IsNull()) {di<<"null shape"<<"\n";return 1;}
  gp_Pnt p; DrawTrSurf::GetPoint(a[2], p);
  Standard_Real tol = Draw::Atof(a[3]);
  
  BRepClass3d_SolidClassifier soclassi(s);
  TopAbs_State sta = TopAbs_UNKNOWN;
  soclassi.Perform(p,tol);
  
  sta = soclassi.State();
  di<<"point is ";
  //TopAbs::Print(sta,cout);
  Standard_SStream aSStream;
  TopAbs::Print(sta,aSStream);
  di << aSStream;
  di<<" shape s"<<"\n";
  return 0;
}
static Standard_Integer shapeclassifier(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape sh = DBRep::Get(a[1]);
  if (sh.IsNull()) {di<<"null shape"<<"\n";return 1;}
  TopoDS_Shape shref = DBRep::Get(a[2]);
  if (shref.IsNull()) {di<<"null reference shape"<<"\n";return 1;}
  Standard_Boolean hastoavoid = (n > 3);
  TopTools_ListOfShape toavoid;
  if (hastoavoid) {
    for (Standard_Integer i=3; i<=n; i++) {
      TopoDS_Shape shtoavoid = DBRep::Get(a[i]); 
      if (shtoavoid.IsNull()) {di<<"null toavoid shape"<<"\n";return 1;}    
      toavoid.Append(shtoavoid);
    }
  }
  
  TopOpeBRepTool_ShapeClassifier shclassi;
  TopAbs_State sta = TopAbs_UNKNOWN;
  if (hastoavoid) sta = shclassi.StateShapeShape(sh,shref);
  else if (toavoid.Extent() == 1) sta = shclassi.StateShapeShape(sh,toavoid.First(),shref);
  else sta = shclassi.StateShapeShape(sh,toavoid,shref);
  
  di<<"shape is ";
  //TopAbs::Print(sta,cout);
  Standard_SStream aSStream;
  TopAbs::Print(sta,aSStream);
  di << aSStream;
  di<<" shape ref"<<"\n";
  return 0;
}


// normals ..
static Standard_Integer normal(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face f = TopoDS::Face(aLocalShape);
//  TopoDS_Face f = TopoDS::Face(DBRep::Get(a[1]));
  if (f.IsNull()) {di<<"null shape"<<"\n";return 1;}
  gp_Pnt p; DrawTrSurf::GetPoint(a[2], p);

  Standard_Real dist=0.; gp_Pnt2d uv; Standard_Boolean ok = FUN_tool_projPonF(p,f,uv,dist);
  if (!ok) {di<<"projection failed"<<"\n"; return 1;}
#ifdef OCCT_DEBUG
  gp_Vec ngf =
#endif
               FUN_tool_nggeomF(uv,f);
  TCollection_AsciiString aa("ngS"); 
#ifdef DRAW
  FUN_tool_draw(aa,p,ngf,length);
#endif
  return 0;
}  

static Standard_Integer curvature(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 5) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face f = TopoDS::Face(aLocalShape);
//  TopoDS_Face f = TopoDS::Face(DBRep::Get(a[1]));
  if (f.IsNull()) {di<<"null shape"<<"\n";return 1;}
  Standard_Real x = Draw::Atof(a[2]);
  Standard_Real y = Draw::Atof(a[3]);
  Standard_Real z = Draw::Atof(a[4]);
  Handle(Geom_Line) line = new Geom_Line(gp_Ax1(gp_Pnt(0.,0.,0.), gp_Dir(x,y,z)));
  BRepAdaptor_Surface BS(f);
  Handle(Geom_Surface) su = BRep_Tool::Surface(f);
  GeomAPI_IntCS intcs(line,su);
  Standard_Boolean done = intcs.IsDone();
  if (!done) {di<<"intersection point on surface not found"<<"\n"; return 1;}
  Standard_Integer npnts = intcs.NbPoints();
  if (npnts < 1) {di<<"intersection point on surface not found"<<"\n"; return 1;}
  
  Standard_Real tol = Precision::Confusion();
  BRepLProp_SLProps props(BS,2,tol);
  gp_Dir Norm,D1,D2;  
  for (Standard_Integer i = 1; i <= npnts; i++) {
    gp_Pnt p = intcs.Point(i);
    Standard_Real u,v,w; intcs.Parameters(i,u,v,w);
    di<<"point("<<i<<") = { ("<<p.X()<<" "<<p.Y()<<" "<<p.Z()<<"), ("<<u<<" "<<v<<") }"<<"\n"; 
    props.SetParameters(u,v);
    Standard_Boolean curdef = props.IsCurvatureDefined();
    if (!curdef) {di<<"!IsCurvatureDefined"<<"\n"; continue;}
    Standard_Boolean umbilic = props.IsUmbilic();
    if (umbilic) { 
      D1 = gp_Dir(0,0,1);
      D2 = gp_Dir(0,1,0);   
      di<<"umbilic";
    }
    else {
      props.CurvatureDirections(D1,D2);
    }
    Norm = gp_Dir(D1^D2); 
    di<<"D1  = ("<<D1.X()<<" "<<D1.Y()<<" "<<D1.Z()<<" "<<")"<<"\n";
    di<<"D2  = ("<<D2.X()<<" "<<D2.Y()<<" "<<D2.Z()<<" "<<")"<<"\n";
    di<<"Norm  = ("<<Norm.X()<<" "<<Norm.Y()<<" "<<Norm.Z()<<" "<<")"<<"\n";
  } // i
  return 0;
}

void TestTopOpe::CORCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return; done = Standard_True;
  const char* g = "TestTopOpe CORCommands";
  // purge, regularization commands :
  theCommands.Add("purge",   "purge f",                  __FILE__, purge, g);
  theCommands.Add("corrISO", "corrISO f Fsp",            __FILE__, correctONISO, g);
  theCommands.Add("regufa",  "regufa f",                 __FILE__, regularize, g);
  theCommands.Add("splitf",  "splitf f",                 __FILE__, splitF, g);
  theCommands.Add("regush",  "regush so",                __FILE__, regush, g);
  theCommands.Add("reguso",  "reguso so",                __FILE__, reguso, g);
  // builder commands :
  theCommands.Add("soclass",  "soclass sh pnt tol",         __FILE__, solidclassifier, g);
  theCommands.Add("shclass",  "shclass sh shref <toavoid>", __FILE__, shapeclassifier, g);
  theCommands.Add("clclass",  "clclass sh shref <toavoid>", __FILE__, class3dclassifier, g);
  
  // useful commands :
  theCommands.Add("cled",       "cled ed f",                __FILE__, isclosingE, g);
  theCommands.Add("compare",    "compare s1 s2",            __FILE__, compareshsh, g);
  theCommands.Add("edonfa",     "edonfa ed f",              __FILE__, pcurveedgeonface, g);
  theCommands.Add("pconfa",     "pconfa name s f",          __FILE__, pcurvesonface, g);
  theCommands.Add("orivine",    "orivine v ed",             __FILE__, orivine, g);
  theCommands.Add("vine",       "vine v ed fa",             __FILE__, vine, g);
  theCommands.Add("issubsh",    "issubsh subsh sh",         __FILE__, issubshape, g);
  theCommands.Add("bnd2d",      "bnd2d name W F i",         __FILE__, drawbnd2d, g);
  theCommands.Add("classibnd2d","classibnd2d W1 W2 F i",    __FILE__, classifBnd2d, g);
  theCommands.Add("pntonc",     "pntonc par C3d",           __FILE__, pntonc, g);
  theCommands.Add("pntonc2d",   "pntonc2d par C2d S",       __FILE__, pntonc2d, g);
  theCommands.Add("projponf",   
                  "projponf f pnt [extrema flag: -min/-max/-minmax] [extrema algo: -g(grad)/-t(tree)]",
                                                            __FILE__, projponf, g);
  theCommands.Add("tolmax",     "tolmax s",                 __FILE__, tolmax, g);
  theCommands.Add("normal",     "normal f p3d length",      __FILE__, normal, g);
  theCommands.Add("curvature",  "curvature f x y z",        __FILE__, curvature , g);
  
}
