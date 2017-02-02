// Created on: 1998-12-08
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


#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <TopOpeBRepTool_connexity.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_REGUW.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#define FORWARD  (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING  (5)

#define M_FORWARD(ori)  (ori == TopAbs_FORWARD) 
#define M_REVERSED(ori) (ori == TopAbs_REVERSED) 
#define M_INTERNAL(ori) (ori == TopAbs_INTERNAL) 
#define M_EXTERNAL(ori) (ori == TopAbs_EXTERNAL) 

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepTool_GettraceREGUFA(); 
static TopTools_IndexedMapOfShape STATIC_mapf, STATIC_mapw, STATIC_mapv;
static TopTools_IndexedMapOfOrientedShape STATIC_mapeds;
void FUN_tro(const Standard_Integer i)
{
  if      (i==1) cout<<"FORWARD";
  else if (i==2) cout<<"REVERSED";
  else if (i==3) cout<<"INTERNAL";
  else if (i==4) cout<<"EXTERNAL";
  else if (i==5) cout<<"CLOSING";
}
Standard_EXPORT Standard_Integer FUN_adds(const TopoDS_Shape& s) {
  TopAbs_ShapeEnum typ = s.ShapeType();
  TCollection_AsciiString aa; Standard_Integer is = 0;
  if (typ == TopAbs_VERTEX) {aa = TCollection_AsciiString("v"); is = STATIC_mapv.Add(s);}
  if (typ == TopAbs_EDGE)   {aa = TCollection_AsciiString("e"); is = STATIC_mapeds.Add(s);}
  if (typ == TopAbs_WIRE)   {aa = TCollection_AsciiString("wi"); is = STATIC_mapw.Add(s);}
  if (typ == TopAbs_FACE)   {aa = TCollection_AsciiString("f"); is = STATIC_mapf.Add(s);}
#ifdef DRAW
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA(); 
  if (trc) FUN_tool_draw(aa,s,is);
#endif
  return is;
}
#endif

extern void FUN_tool_tori(const TopAbs_Orientation Or);

void FUN_tool_Add(TopTools_DataMapOfShapeListOfShape& map,const TopoDS_Shape& key, const TopoDS_Shape& subitem)
{
  if (map.IsBound(key)) map.ChangeFind(key).Append(subitem);
  else {TopTools_ListOfShape los; los.Append(subitem); map.Bind(key,los);}
}
static void FUN_Raise() {
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) cout<<"*** Raise REGUW"<<endl;
//  throw Standard_Failure("REGUW");
#endif  
}

//=======================================================================
//function : CORRISO
//purpose  : 
//=======================================================================

TopOpeBRepTool_REGUW::TopOpeBRepTool_REGUW(const TopoDS_Face& Fref)
 : myCORRISO(Fref)
{  
  myS.Nullify(); 
  hasnewsplits = Standard_False;

  myEsplits.Clear();
  myOwNw.Clear();

  mymapvEds.Clear();
  mymapvmultiple.Clear();
  myListVmultiple.Clear();

  iStep = 0;
}

//=======================================================================
//function : Fref
//purpose  : 
//=======================================================================

const TopoDS_Face& TopOpeBRepTool_REGUW::Fref() const
{
  return (myCORRISO.Fref());
}

//=======================================================================
//function : SetEsplits
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUW::SetEsplits(TopTools_DataMapOfShapeListOfShape& Esplits)
{
  myEsplits = Esplits;
}
//=======================================================================
//function : GetEsplits
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUW::GetEsplits(TopTools_DataMapOfShapeListOfShape& Esplits) const 
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  Esplits = myEsplits;
}

//=======================================================================
//function : SetOwNw
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUW::SetOwNw(TopTools_DataMapOfShapeListOfShape& OwNw)
{
  myOwNw = OwNw;
}
//=======================================================================
//function : GetOwNw
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUW::GetOwNw(TopTools_DataMapOfShapeListOfShape& OwNw) const
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  OwNw = myOwNw;
}

//=======================================================================
//function : SplitEds
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::SplitEds()
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) cout<<"**    SPLITTING EDGES    **"<<endl;
#endif

  TopTools_IndexedMapOfShape mehasIv;
  Standard_Integer i;
  for (i = 1; i <= mymapvEds.Extent(); i++) {
    const TopOpeBRepTool_connexity& co = mymapvEds(i);
    TopTools_ListOfShape loe; Standard_Integer ni = co.IsInternal(loe);
    if (ni == 0) continue;
    TopTools_ListIteratorOfListOfShape ite(loe);
    for (; ite.More(); ite.Next()) mehasIv.Add(ite.Value());
  }

  for (i = 1; i <= mehasIv.Extent(); i++) {
    const TopoDS_Edge& e = TopoDS::Edge(mehasIv.FindKey(i));
    TopTools_ListOfShape splits; Standard_Boolean issp = Standard_False;
    Standard_Boolean isdone = myEsplits.IsBound(e);
    if (isdone) splits.Assign(myEsplits.Find(e));
    else        issp = TopOpeBRepTool_TOOL::SplitE(e,splits);
    if (issp) hasnewsplits = Standard_True;
#ifdef OCCT_DEBUG
    if (trc) cout<<"e"<<FUN_adds(e)<<" gives splits ="<<endl;
#endif
    if (!(issp || isdone)) continue; //nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnyi

    // e gives splits = {esp has vertices {vv}}
    TopTools_ListIteratorOfListOfShape ite(splits);
    for (; ite.More(); ite.Next()){
      const TopoDS_Shape& esp = ite.Value();
#ifdef OCCT_DEBUG
      if (trc) {cout<<" e"<<FUN_adds(esp);}
#endif
      TopExp_Explorer exvv(esp, TopAbs_VERTEX);
      for (; exvv.More(); exvv.Next()){
	const TopoDS_Shape& vv = exvv.Current();
	TopOpeBRepTool_connexity& co = mymapvEds.ChangeFromKey(vv);
	Standard_Boolean isb = co.RemoveItem(INTERNAL,e);
	if (!isb) continue;
	Standard_Integer ivv = TopOpeBRepTool_TOOL::OriinSorclosed(vv,esp);
	co.AddItem(ivv,esp);
      }//exvv(exvv,VERTEX)
    }//ite(splits)
#ifdef OCCT_DEBUG
    if (trc) cout<<endl;
#endif
  }
  return Standard_True;
}


//=======================================================================
//function : S
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepTool_REGUW::S() const
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  return myS;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUW::Init(const TopoDS_Shape& S)
{
  myS = S;
  InitStep(S);
}

//=======================================================================
//function : HasInit
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::HasInit() const
{
  return (!myS.IsNull());
}



//=======================================================================
//function : InitStep
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUW::InitStep(const TopoDS_Shape& S)
{  
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  TopoDS_Shape null;myCORRISO.Init(null); myCORRISO.Init(S);

  mymapvEds.Clear();
  mymapvmultiple.Clear();
  myListVmultiple.Clear();
}



//=======================================================================
//function : MapS
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::MapS()
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) cout<<"**    MAPPING    **"<<endl;
#endif
  
  // Prequesitories :
  //        0) an edge with an INTERNAL vertex binded in <mapVedges> must be
  //           splitted 
  //           After splitting such edges, we deal only with FORWARD and       
  //           REVERSED vertices.
  //        1) a vertex belonging to a CLOSING edge is represented by 2 distinct
  //          2d points in the UV space.
  //        2) a vertex belonging to a  DEGENERATED edge has at least 2 UV rep.
  //          (if the original degenerated edge has been splitted).
  //          the bounds of the degenerated edge share the same TShape.
  //        3) a CLOSED edge binds the same vertex oriented FORWARD and REVERSED. 
  
  // For each vertex get non-"singular" edges in which the vertex is oriented 
  // FORWARD and REVERSED.
  // <mymapvmultiple(1)> binds v and ed if : 
  //            v is FORWARD in ed FORWARD   : oriFF(v,ed) = FORWARD
  //            v is REVERSED in ed REVERSED : oriFF(v,ed) = FORWARD
  // oriFF(v,ed) = (ori(ed) == FORWARD) ? ori(ed) : ori(ed).Reverse.
  
  // A "singular" edge is a closed, a closing or a degenerated edge.
  // In <mymapvmultiple(5)>, we bind with the key <v>:
  // - the FORWARD and the REVERSED CLOSING ancestor edges of <v>
  // - the DEGENERATED ancestor edge of <v> 
  // - the CLOSED edge ancestor edge of <v>   
  
  // A MULTIPLE vertex has its single UV representation's connexity > 2
  
  const TopoDS_Shape& CS = myCORRISO.S();
  TopExp_Explorer exe(CS, TopAbs_EDGE);
  for (; exe.More(); exe.Next()){
    const TopoDS_Edge& ed = TopoDS::Edge(exe.Current());

    Standard_Boolean isdgE = BRep_Tool::Degenerated(ed); 
    Standard_Boolean iscE = TopOpeBRepTool_TOOL::IsClosingE(ed,myCORRISO.S(),Fref()); 
    iscE = iscE && !isdgE; // closing ed
    TopoDS_Shape vcl; Standard_Boolean isvcl = TopOpeBRepTool_TOOL::ClosedE(ed,TopoDS::Vertex(vcl)); isvcl = isvcl && !isdgE; 

    TopExp_Explorer exv(ed, TopAbs_VERTEX);
    for (; exv.More(); exv.Next()){
      const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
      Standard_Boolean isb = mymapvEds.Contains(v);
      TopOpeBRepTool_connexity theconnexity(v);
      if (!isb) mymapvEds.Add(v, theconnexity);
      TopOpeBRepTool_connexity& co = mymapvEds.ChangeFromKey(v);

      if      (isdgE)                   {
	// avoid adding it twice	
	co.RemoveItem(CLOSING,ed);
	co.AddItem(CLOSING,ed);   
	break;
      }
      else if (iscE) {
	// FORWARD and REVERSED edge is added
	co.AddItem(CLOSING,ed);
      }
      else if (isvcl && (v.IsSame(vcl))) {              
	// avoid adding it twice	
	co.RemoveItem(CLOSING,ed);
	co.AddItem(CLOSING,ed);
      }
      else {
	Standard_Integer iov = TopOpeBRepTool_TOOL::OriinSor(v,ed,Standard_False); //iov != 0
	co.AddItem(iov,ed);
      }

    }//exv    
  }//exe

#ifdef OCCT_DEBUG
  if (trc) {
    for (Standard_Integer iv = 1; iv <= mymapvEds.Extent(); iv++) {
      const TopoDS_Vertex& v = TopoDS::Vertex(mymapvEds.FindKey(iv));
      cout <<"#v"<<FUN_adds(v)<<" :\n";
      const TopOpeBRepTool_connexity& co = mymapvEds(iv);
      for (Standard_Integer i=1; i<=5; i++)    {
	TopTools_ListOfShape eds; Standard_Integer ieds = co.Item(i,eds);
	if (ieds == 0) continue;
	cout<<" is ";FUN_tro(i);cout<<" in ";
	TopTools_ListIteratorOfListOfShape ite(eds);
	for (; ite.More(); ite.Next())
	  {const TopoDS_Edge& e = TopoDS::Edge(ite.Value());
	   cout<<"e"<<FUN_adds(e);FUN_tool_tori(e.Orientation());
	   TopoDS_Vertex vclo; Standard_Boolean cloE = TopOpeBRepTool_TOOL::ClosedE(e,vclo); if (cloE) cout<<"closed";
	   Standard_Boolean dgE = BRep_Tool::Degenerated(e); if (dgE) cout<<"degenerated";
	   Standard_Boolean iscE = TopOpeBRepTool_TOOL::IsClosingE(e,myCORRISO.S(),Fref()); 
	   if (iscE) cout<<"closing";
	   cout<<" ";}
	cout<<endl;	
      }//i=1..5
    }
  }//trc
#endif

  Standard_Integer nV = mymapvEds.Extent();
  if (nV < 1) return Standard_False;

  Standard_Integer i;
  for (i = 1; i <= mymapvEds.Extent(); i++) {
    const TopoDS_Shape& v = mymapvEds.FindKey(i);
    const TopOpeBRepTool_connexity& co = mymapvEds(i);
    Standard_Boolean faulty = co.IsFaulty();
    if (faulty) return Standard_False;
    Standard_Boolean multiple = co.IsMultiple();
    if (multiple)
      if (mymapvmultiple.Add(v))
        myListVmultiple.Append(v);
  } 
  return Standard_True;    
}//MapS

static void FUN_nextdata(const Standard_Integer iStep,const TopoDS_Edge& e,const Handle(Geom2d_Curve)& pc,
			 TopoDS_Vertex& v,gp_Pnt2d& p2d,gp_Dir2d& tg)
// prequesitory : pc = 2drep(e),
// ori(v,e)=iv1e1 -> v, p2d=pt2d(v,e), tg=tg2d(v,e).
{  
  Standard_Integer iv1e1 = (iStep == 1) ? FORWARD : REVERSED;
  TopoDS_Shape aLocalShape = TopOpeBRepTool_TOOL::Vertex(iv1e1,e);
  v = TopoDS::Vertex(aLocalShape);
//  v = TopoDS::Vertex(TopOpeBRepTool_TOOL::Vertex(iv1e1,e));
  Standard_Real par1 = TopOpeBRepTool_TOOL::ParE(iv1e1,e);
  Standard_Boolean line = FUN_tool_line(pc);
  Standard_Boolean quad = FUN_tool_quad(pc);// mytg2d is reapproximated if PCquad
  Standard_Boolean approx = quad && (!line);
  
  gp_Vec2d tg2d; 
  if (approx) {
    p2d = pc->Value(par1);

    Standard_Integer iv0e1 = (iStep == 1) ? REVERSED : FORWARD;
    Standard_Real par0 = TopOpeBRepTool_TOOL::ParE(iv0e1,e);
    // Getting a point near point<Index> of <E>
    Standard_Real x = 0.2345; Standard_Real par = (1-x)*par1+x*par0;
    gp_Pnt2d pmil; pc->D1(par,pmil,tg2d);     
  }
  else {
    pc->D1(par1,p2d,tg2d);  
  }
  tg = gp_Dir2d(tg2d); 
  if (M_REVERSED(e.Orientation())) tg.Reverse();
}

//=======================================================================
//function : InitBlock
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::InitBlock()
{  
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
#endif

  myv0.Nullify(); myp2d0=gp_Pnt2d(1.e7,1.e7); 
  myed.Nullify();
  Standard_Integer iv0e1 = (iStep == 1) ? REVERSED : FORWARD;

  // updating <mymapvmultiple> and myListVmultiple
  TopTools_ListIteratorOfListOfShape itmu(myListVmultiple);
  while (itmu.More()) {
    const TopoDS_Shape& vmu = itmu.Value(); 
    const TopOpeBRepTool_connexity& cmu = mymapvEds.FindFromKey(vmu);
    Standard_Boolean mult = cmu.IsMultiple();
    if (!mult) {
      myListVmultiple.Remove(itmu);
      mymapvmultiple.Remove(vmu);
    }
    else itmu.Next();
  }
  
  // myv0 :
  if (myListVmultiple.IsEmpty()) {
    Standard_Integer i;
    for (i = 1; i <= mymapvEds.Extent(); i++) {
      const TopoDS_Vertex& v = TopoDS::Vertex(mymapvEds.FindKey(i));
      const TopOpeBRepTool_connexity& co = mymapvEds(i);
      TopTools_ListOfShape lea; Standard_Integer nea = co.Item(iv0e1,lea);
      TopTools_ListOfShape leb; Standard_Integer neb = co.Item(CLOSING,leb);
      TopTools_ListOfShape le;  le.Append(lea); le.Append(leb); Standard_Integer ne = nea+neb;
      if (ne != 0) {myv0 = v; break;}
    }
  }
  else {
    myv0 = TopoDS::Vertex(myListVmultiple.First());  
  }
  if (myv0.IsNull()) return Standard_False;
  // myed : 
  const TopOpeBRepTool_connexity& co = mymapvEds.FindFromKey(myv0);
   
  TopTools_ListOfShape lea; Standard_Integer nea = co.Item(iv0e1,lea);
  TopTools_ListOfShape leb; Standard_Integer neb = co.Item(CLOSING,leb);
  TopTools_ListOfShape le;  le.Append(lea); le.Append(leb); Standard_Integer ne = nea+neb;
  if (ne == 0) return Standard_False;

  if (nea > 0) myed = TopoDS::Edge(le.First());
  else {// <myv0> CLOSING in <myed>
    TopTools_ListIteratorOfListOfShape itb(le);
    for (; itb.More(); itb.Next()){
      const TopoDS_Edge& eb = TopoDS::Edge(itb.Value());
      Standard_Boolean iscE = TopOpeBRepTool_TOOL::IsClosingE(myed,myCORRISO.S(),Fref()); 
      if (!iscE) {myed = eb; break;}
      Standard_Integer iov0 = TopOpeBRepTool_TOOL::OriinSorclosed(myv0,myed);
      if (iov0 != iv0e1) continue;
    }//itb(leb)
  }
  if (myed.IsNull()) return Standard_False;

  TopExp_Explorer exv(myed, TopAbs_VERTEX);
  for (; exv.More(); exv.Next()){
    const TopoDS_Shape& vcur = exv.Current();
    TopOpeBRepTool_connexity& cco = mymapvEds.ChangeFromKey(vcur);
//#ifdef OCCT_DEBUG
//    Standard_Boolean ok =
//#endif
             cco.RemoveItem(myed);
//    if (!ok) return Standard_False; see for closing vertices
  }

  // myp2d0 :
  TopOpeBRepTool_C2DF c2df;Standard_Boolean bound = myCORRISO.UVRep(myed,c2df);
  if (!bound) return Standard_False;
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& pc = c2df.PC(f,l,tol);
  Standard_Real par0 = TopOpeBRepTool_TOOL::ParE(iv0e1,myed);
  pc->D0(par0,myp2d0);

  // myv, myp2d, mytg2d :
  ::FUN_nextdata(iStep,myed,pc, myv,myp2d,mytg2d);  

#ifdef OCCT_DEBUG
  if (trc) {
    cout<<endl<<" v0   = v"<<FUN_adds(myv0)<<" p2d0 = ("<<myp2d0.X()<<" "<<myp2d0.Y()<<")"<<endl;
    cout<<" vcur = v"<<FUN_adds(myv)<<"  p2d = ("<<myp2d.X()<<" "<<myp2d.Y()<<")"<<endl;
    cout<<" ecur = e"<<FUN_adds(myed)<<endl;
  }
#endif
  return Standard_True;
}

//=======================================================================
//function : NearestE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::NearestE(const TopTools_ListOfShape& loe, TopoDS_Edge& efound) const
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
#endif

  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  efound.Nullify();
  Standard_Real fac = 0.45678;
  Standard_Real tola = Precision::Angular();
  Standard_Integer iv0e1 = (iStep == 1) ? REVERSED : FORWARD;

  // initializing 
  TopTools_ListIteratorOfListOfShape ite(loe);
  efound = TopoDS::Edge(ite.Value());  
  if (ite.More()) ite.Next();
  else            return Standard_True;

  TopOpeBRepTool_C2DF c2defound;Standard_Boolean isbfound = myCORRISO.UVRep(efound,c2defound);
  if (!isbfound) return Standard_False;
  
  gp_Vec2d tg2dfound = TopOpeBRepTool_TOOL::tryTg2dApp(iv0e1,efound,c2defound,fac);
  if (M_REVERSED(efound.Orientation())) tg2dfound.Reverse();
  Standard_Real angfound = 1.e7;
  if (iStep == 1) angfound = TopOpeBRepTool_TOOL::Matter(mytg2d,tg2dfound);
  else            angfound = 2.*M_PI - TopOpeBRepTool_TOOL::Matter(tg2dfound,mytg2d);
#ifdef OCCT_DEBUG
  if (trc) cout<<"ang(e"<<FUN_adds(myed)<<",e"<<FUN_adds(efound)<<")="<<angfound<<endl;
#endif

  // purpose : finding out <efound> /
  //     ecur is given by <tg2d>
  //     le = {ei}
  //     iStep=1 : matterang2d(efound,ecur) = min(ei,ecur)
  //     iStep=2 : oppomatterang2d(efound,ecur) = min(ei,ecur)
  for (; ite.More(); ite.Next()){
    const TopoDS_Edge& ei = TopoDS::Edge(ite.Value());
    // for INTERNAL edge eI -> eF+eR
    if (ei.IsSame(myed)) continue; 

    TopOpeBRepTool_C2DF c2dei;Standard_Boolean isbi = myCORRISO.UVRep(ei,c2dei);
    if (!isbi) return Standard_False;
    gp_Vec2d tg2di = TopOpeBRepTool_TOOL::tryTg2dApp(iv0e1,ei,c2dei,fac);
    if (M_REVERSED(ei.Orientation())) tg2di.Reverse();
    
    Standard_Real angi = 1.e7;
    if (iStep == 1) angi = TopOpeBRepTool_TOOL::Matter(mytg2d,tg2di);
    else            angi = 2.*M_PI - TopOpeBRepTool_TOOL::Matter(tg2di,mytg2d);
    Standard_Boolean eq = Abs(angi-angfound) < tola;
#ifdef OCCT_DEBUG
    if (trc) cout<<"ang(e"<<FUN_adds(myed)<<",e"<<FUN_adds(ei)<<")="<<angi<<endl;
#endif
    if (eq) {
      FUN_Raise(); 
      return Standard_False;
    }
    
    if (angi < angfound) {efound = ei; angfound = angi;}
  }  
  return Standard_True;
}

//=======================================================================
//function : NextinBlock
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::NextinBlock()
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) cout<<endl<<" vcur = v"<<FUN_adds(myv)<<"   p2d = ("<<myp2d.X()<<" "<<myp2d.Y()<<")"<<endl;
#endif

  Standard_Integer iv0e1 = (iStep == 1) ? REVERSED : FORWARD;
  const TopOpeBRepTool_connexity& co = mymapvEds.FindFromKey(myv);

  // {e} : e is connexed to <myv> && ori(<myv>,e)=iv0e1
  TopTools_ListOfShape lea; Standard_Integer nea = co.Item(iv0e1,lea);
  TopTools_ListOfShape leb; Standard_Integer neb = co.Item(CLOSING,leb);
  TopTools_ListOfShape le; le.Append(lea); le.Append(leb); Standard_Integer ne = nea + neb;

  TopTools_ListIteratorOfListOfShape ite(le);
  while (ite.More()) {
    const TopoDS_Edge& e = TopoDS::Edge(ite.Value());
#ifdef OCCT_DEBUG
    if (trc) cout<<" e"<<FUN_adds(e);
#endif

    Standard_Boolean issame = e.IsSame(myed);
    if (issame) {//xpu240299 FRA60275, fsp4
#ifdef OCCT_DEBUG
      if (trc) cout<<" is same : not valid"<<endl;
#endif
      le.Remove(ite); continue;
    }

    TopOpeBRepTool_C2DF c2df;Standard_Boolean bound = myCORRISO.UVRep(e,c2df);
    if (!bound) {FUN_Raise(); return Standard_False;}
    Standard_Real f,l,tol; const Handle(Geom2d_Curve)& pc = c2df.PC(f,l,tol);
    Standard_Real par = TopOpeBRepTool_TOOL::ParE(iv0e1,e);
    gp_Pnt2d p2de; pc->D0(par,p2de);
    
    // p2d(myv,ed)=p2d(myv,e)
    Standard_Boolean samep2d = p2de.IsEqual(myp2d,mytol2d);
    if (!samep2d) le.Remove(ite); 
    else ite.Next();
#ifdef OCCT_DEBUG
    if (trc) {if (samep2d) cout<<" valid"<<endl;
    else         cout<<" not valid"<<endl;}
#endif
  }//ite(le)
  ne = le.Extent();
  if (ne == 0) {FUN_Raise(); return Standard_False;}
  
  // myed :
  ne = le.Extent();
  if (ne == 1) myed = TopoDS::Edge(le.First());
  else {
    TopoDS_Edge efound; Standard_Boolean found = NearestE(le,efound);
    if (!found) {FUN_Raise(); return Standard_False;}
    myed = efound;
  }

  TopExp_Explorer exv(myed, TopAbs_VERTEX);
  for (; exv.More(); exv.Next()){
    TopOpeBRepTool_connexity& cco = mymapvEds.ChangeFromKey(exv.Current());
//#ifdef OCCT_DEBUG
//    Standard_Boolean ok =
//#endif
             cco.RemoveItem(myed);
//    if (!ok) {FUN_Raise(); return Standard_False;} closed edges
  }

  TopOpeBRepTool_C2DF c2df; myCORRISO.UVRep(myed,c2df);
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& pc = c2df.PC(f,l,tol);
  ::FUN_nextdata(iStep,myed,pc, myv,myp2d,mytg2d);

#ifdef OCCT_DEBUG
  if (trc) {
    cout<<" vcur = v"<<FUN_adds(myv)<<"  p2d = ("<<myp2d.X()<<" "<<myp2d.Y()<<")"<<endl;
    cout<<" ecur = e"<<FUN_adds(myed)<<endl;
  }
#endif
  return Standard_True;
}//NextinBlock

//=======================================================================
//function : REGU
//purpose  : step <iStep> for regularization 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::REGU(const Standard_Integer istep,
#ifdef OCCT_DEBUG
                                            const TopoDS_Shape& Scur,
#else
                                            const TopoDS_Shape& ,
#endif
                                            TopTools_ListOfShape& Splits)
{   
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  iStep = istep;
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) cout<<endl<<"**    REGU    istep=**"<<iStep<<endl;
#endif

  Splits.Clear();
  // A valid face is built on "valid" wires.
  // A valid wire has for UV representation a set of 2d edges of connexity = 2.
  
  // Starting element is a vertex of single UV representation with connexity greater
  // than 2.
  // Given <v0m>, we get :
  //  - <e1> an edge where <v0m> is oriented REVERSED(step1)/FORWARD(step2)
  //  - <v1> the other vertex of <e1> oriented FORWARD(step1)/REVERSED(step2)
  //  - <p2d1> is <v1>'s UV representation attached to <e1>, 
  //  - tg2d<e1> = Tgt(p2d1,e1)  if <e1> FORWARD(step1)
  //             = -Tgt(p2d1,e1) if <e1> is REVERSED(step1) in the face with
  //    Tgt(p2d1,e1) = tangent to <e1>'s pcurve at point <p2d1>
  
  // step1 :
  // -------
  // Given a starting couple (<vi>, <p2di>, tg2d<e1>, <ei>) with :
  //  - <vi> oriented FORWARD in <ei>,  
  //  - <vi>'s 2d representation falling on an UV bound of <ei>'s pcurve (if <vi> is 
  //    on a closing edge),
  // We get the edge <ei+1> such that :
  //  - <ei+1> is not already touched,
  //  - <vi> is oriented REVERSED(step1) in <ei+1>,
  //  - <ei+1> has an UV bound falling into geometry <p2di>.
  //  - The oriented angle (-tg2d<ei>, tg2d<ei+1>) is the lowest,
  // ( The angle is computed in the anti-trigonometric sense; 
  //   the tangents's orientation are reversed if the edge is oriented REVERSED in the face )
  //   = Smaller oriented (following trigonometric sense) angle (tg2d<ei+1>, -tg2d<ei>) 
  //
  // step2 :  reverse the orientations and the angle describes non-matter.
  // -------
  
  // tol2d = 1.e-2*(lowest distance (p2df,p2dl) between all non-closed  
  //                and non-degenerated pcurves seen during the regularization).
  // This value is updated in method chkp2dFORinE.
  // The comparision of p2d points is coupled with comparision of vertices
  // We should in fact use bounding boxes on pcurves but this is too expensive!!!
  mytol2d = 1.e-5;
  isinit0 = Standard_True;
  TopTools_ListOfShape loEcur, loW;

  Standard_Integer nite = 0; 
  Standard_Integer nE = myCORRISO.Eds().Extent();// recall myCORRISO.Init(myS);
  TopTools_ListIteratorOfListOfShape ite(myCORRISO.Eds());
  for (; ite.More(); ite.Next()) {
    TopAbs_Orientation oe = ite.Value().Orientation();
    if (M_INTERNAL(oe) || M_EXTERNAL(oe)) nE--;
  }

  while (nite <= nE) {       
    Standard_Boolean FINI = (nite == nE);    

    //*** Getting starts elements if not defined 
    // -----------------------------------------
    if (isinit0 && !FINI) {
      Standard_Boolean ok = InitBlock();
      if (!ok) {
#ifdef OCCT_DEBUG
      if (trc) cout<<"** InitBlock fails"<<endl;
#endif    
	{FUN_Raise(); return Standard_False;}      
      }
      loEcur.Append(myed);// add it to the current wire
      nite++;             // increment nite  
      isinit0 = Standard_False;
      continue;
    } 

    //*** Checking the wire is not already closed
    // ------------------------------------------
    // If last vertex <v> is the first one <v0> and <p2d> == <p2d0>, the wire is closed
    Standard_Boolean wireisclosed = myp2d.IsEqual(myp2d0,mytol2d);
    if (wireisclosed) {

      //* Adding INTERNAL/EXTERNAL edges to wires
      // ----------------------------------------      
      TopTools_ListIteratorOfListOfShape ite1(loEcur);
      for (; ite1.More(); ite1.Next()){
	const TopoDS_Shape& e = ite1.Value();
	TopExp_Explorer exv(e, TopAbs_VERTEX);
	for (; exv.More(); exv.Next()){
	  const TopoDS_Shape& v = exv.Current();
	  TopOpeBRepTool_connexity& co = mymapvEds.ChangeFromKey(v);
	  TopTools_ListOfShape& le = co.ChangeItem(INTERNAL);
	  TopTools_ListIteratorOfListOfShape itte(le);
	  while (itte.More()) {
	    const TopoDS_Shape& ee = itte.Value();
	    TopAbs_Orientation oe = ee.Orientation();
	    if (M_INTERNAL(oe) || M_EXTERNAL(oe)) {loEcur.Append(ee);le.Remove(itte);}
	    else                                  itte.Next();
	  }//itte(le)
	}//exv(e)
      }//ite(loEcur)
      FINI = (nite == nE);
            
      // if Scur = <currentW> gives only one new wire, and has no new splitted edges, <currentW>
      // is valid and unchanged.
      Standard_Boolean onewok = FINI && loW.IsEmpty() && !hasnewsplits;
      if (onewok){
#ifdef OCCT_DEBUG	
	if (trc) cout<<"wire "<<FUN_adds(Scur)<<" is found valid\n";
#endif
	return Standard_True;
      }//onewok

      TopoDS_Wire newW; Standard_Boolean wiok = FUN_tool_MakeWire(loEcur, newW);
      if (wiok) loW.Append(newW);
      else      {
#ifdef OCCT_DEBUG
      if (trc) cout<<"** FUN_tool_MakeWire fails"<<endl;
#endif   
	{FUN_Raise(); return Standard_False;} 
      }
#ifdef OCCT_DEBUG
      if (trc) {cout<<"#->new wire = wi"<<FUN_adds(newW)<<" = ";
		for (TopTools_ListIteratorOfListOfShape it(loEcur); it.More(); it.Next())
		  cout<<" e"<<FUN_adds(it.Value());
		cout<<"\n\n";}  
#endif      
      isinit0 = Standard_True; 
      loEcur.Clear(); 

      if (FINI) {
	Splits.Append(loW);
	return Standard_True;
      }
      continue;
    } // wireisclosed            
    
    //*** Iteration on UV connexed edges 
    // ---------------------------------
    // <v> is in <ed>, <p2d> = <v> current UV representation
    Standard_Boolean gotonext = NextinBlock();
    if (!gotonext) {
#ifdef OCCT_DEBUG
      if (trc) cout<<"** NextinBlock fails"<<endl;
#endif   
      {FUN_Raise(); return Standard_False;}
    }
    loEcur.Append(myed);
    nite++;
  } // nite

  return Standard_True;   
}

//=======================================================================
//function : REGU
//purpose  : complete regularization
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::REGU() 
{  
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  TopTools_ListOfShape null;

  Standard_Boolean toregu = !myListVmultiple.IsEmpty();
  toregu = toregu || hasnewsplits;
  TopTools_ListOfShape Splits;
  if (!toregu) {
    myOwNw.Bind(S(),Splits); 
    return Standard_True;
  }

  // iStep = 1;
  TopTools_ListOfShape loS; Standard_Boolean ok = REGU(1,S(),loS);
  if (!ok) {FUN_Raise(); return Standard_False;}

  // iStep = 2;
  if (loS.IsEmpty()) loS.Append(S());// no new shape

  TopTools_ListIteratorOfListOfShape it(loS);
  for (; it.More(); it.Next()){
    const TopoDS_Shape& Scur = it.Value();
    InitStep(Scur);
    MapS();
    Standard_Boolean toregu1 = !myListVmultiple.IsEmpty();
    if (!toregu1) {Splits.Append(Scur); continue;}

    TopTools_ListOfShape sp; ok = REGU(2,Scur,sp);
    if (!ok) {FUN_Raise(); return Standard_False;}
    if (sp.IsEmpty()) sp.Append(Scur);// no new shape
    Splits.Append(sp);
  }
  myOwNw.Bind(S(),Splits);
  return Standard_True;
}

//=======================================================================
//function : GetSplits
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::GetSplits(TopTools_ListOfShape& Splits) const
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  Standard_Boolean isb = myOwNw.IsBound(S());
  if (!isb) return Standard_False;
  Splits = myOwNw.Find(S());
  return Standard_True;
}

//=======================================================================
//function : Connexity
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::Connexity(const TopoDS_Vertex& v, TopOpeBRepTool_connexity& co) const
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  Standard_Boolean isb = mymapvEds.Contains(v);
  if (!isb) return Standard_False;
  co = mymapvEds.FindFromKey(v);
  return Standard_True;
}

//=======================================================================
//function : AddNewConnexity
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::AddNewConnexity(const TopoDS_Vertex& v, const Standard_Integer OriKey,const TopoDS_Edge& e)
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  Standard_Boolean isb = mymapvEds.Contains(v);
  if (!isb) return Standard_False;

  Standard_Boolean ok = myCORRISO.AddNewConnexity(v,e);
  if (!ok) return Standard_False;

  TopOpeBRepTool_connexity& co = mymapvEds.ChangeFromKey(v);
  co.AddItem(OriKey,e);

#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA(); 
  if (trc) 
    {cout<<"** setting new connexity : v"<<FUN_adds(v)<<" is ";FUN_tro(OriKey);
     cout<<" in  e"<<FUN_adds(e);FUN_tool_tori(e.Orientation());
     TopoDS_Vertex vclo; Standard_Boolean cloE = TopOpeBRepTool_TOOL::ClosedE(e,vclo); if (cloE) cout<<" closed";
     Standard_Boolean dgE = BRep_Tool::Degenerated(e); if (dgE) cout<<" degenerated";cout<<endl;}
#endif
  return Standard_True;  
}

//=======================================================================
//function : RemoveOldConnexity
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::RemoveOldConnexity(const TopoDS_Vertex& v, const Standard_Integer OriKey,const TopoDS_Edge& e)
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  Standard_Boolean isb = mymapvEds.Contains(v);
  if (!isb) return Standard_False;

  Standard_Boolean ok = myCORRISO.RemoveOldConnexity(v,e);
//  if (!ok) return Standard_False;

  TopOpeBRepTool_connexity& co = mymapvEds.ChangeFromKey(v);
  ok = co.RemoveItem(OriKey,e);
  if (!ok) return Standard_False;

#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA(); 
  if (trc) 
    {cout<<"** removing old connexity : v"<<FUN_adds(v)<<" for e"<<FUN_adds(e);FUN_tool_tori(e.Orientation());
     TopoDS_Vertex vclo; Standard_Boolean cloE = TopOpeBRepTool_TOOL::ClosedE(e,vclo); if (cloE) cout<<" closed";
     Standard_Boolean dgE = BRep_Tool::Degenerated(e); if (dgE) cout<<" degenerated";cout<<endl;}
#endif
  return Standard_True;  
}
  
//=======================================================================
//function : UpdateMultiple
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUW::UpdateMultiple(const TopoDS_Vertex& v)
{
  if (!HasInit()) throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  Standard_Boolean isb = mymapvEds.Contains(v);
  if (!isb) return Standard_False;
  const TopOpeBRepTool_connexity& co = mymapvEds.FindFromKey(v);
  Standard_Boolean ismult = co.IsMultiple();
  if (ismult)
    if (mymapvmultiple.Add(v))
      myListVmultiple.Append(v);
  return Standard_True;
  
}
