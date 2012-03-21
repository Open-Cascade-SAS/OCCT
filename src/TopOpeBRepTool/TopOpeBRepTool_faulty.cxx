// Created on: 1998-11-24
// Created by: Xuan PHAM PHU
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


#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>

#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#ifdef DEB
//Standard_IMPORT extern TopTools_IndexedMapOfShape STATIC_PURGE_mapv;
Standard_IMPORT TopTools_IndexedMapOfShape STATIC_PURGE_mapv;
//Standard_IMPORT extern TopTools_IndexedMapOfOrientedShape STATIC_PURGE_mapeds;
Standard_IMPORT TopTools_IndexedMapOfOrientedShape STATIC_PURGE_mapeds;
Standard_IMPORT Standard_Boolean TopOpeBRepTool_GettracePURGE();
Standard_EXPORT void FUN_REINIT()
{STATIC_PURGE_mapv.Clear(); STATIC_PURGE_mapeds.Clear();}
static void FUN_RaiseError()
{
#ifdef DEB
  Standard_Boolean trc = TopOpeBRepTool_GettracePURGE();
  //  Standard_Failure::Raise("TopOpeBRepTool::PurgeClosingEdges");
  FUN_REINIT(); 
  if (trc) cout <<"*********failure in TopOpeBRepTool::PurgeClosingEdges***********\n";
#endif
}
Standard_EXPORT Standard_Integer FUN_addepc(const TopoDS_Shape& ed,const TopoDS_Shape& f){
  Standard_Integer ie = STATIC_PURGE_mapeds.Add(ed);
#ifdef DRAW
//  TCollection_AsciiString aa = TCollection_AsciiString("pc_"); FUN_tool_draw(aa,TopoDS::Edge(ed),TopoDS::Face(f),ie);
//  TCollection_AsciiString bb = TCollection_AsciiString("ed_"); FUN_tool_draw(bb,ed,ie);
#endif  
  return ie;
}
Standard_EXPORT Standard_Integer FUN_addcheckepc(const TopoDS_Shape& ed,const TopoDS_Shape& f) {
  Standard_Integer ie = 0;
  ie = STATIC_PURGE_mapeds.FindIndex(ed);
  if (ie == 0) ie = FUN_addepc(ed,f);
  return ie;
}
/*Standard_EXPORT Standard_Integer FUN_adds(const TopoDS_Shape& s) {
  TopAbs_ShapeEnum typ = s.ShapeType();
  TCollection_AsciiString aa; Standard_Integer is;
  if (typ == TopAbs_VERTEX) {aa = TCollection_AsciiString("v_"); is = STATIC_PURGE_mapv.Add(s);}
#ifdef DRAW
  Standard_Boolean trc = TopOpeBRepTool_GettracePURGE();
  if (trc) FUN_tool_draw(aa,s,is);
#endif
  return is;
}*/
Standard_IMPORT Standard_Integer FUN_adds(const TopoDS_Shape& s);
Standard_EXPORT void FUN_tool_tori(const TopAbs_Orientation Or)
{
  switch (Or) {
  case TopAbs_FORWARD:
    cout<<"FOR";break;
  case TopAbs_REVERSED:
    cout<<"REV";break;
  case TopAbs_INTERNAL:
    cout<<"INT";break;
  case TopAbs_EXTERNAL:
    cout<<"EXT";break;
  }    
}
#endif

Standard_EXPORT void FUN_tool_trace(const Standard_Integer Index)
{if (Index == 1) cout <<"FORWARD ";
 if (Index == 2) cout <<"REVERSED ";}
Standard_EXPORT  void FUN_tool_trace(const gp_Pnt2d p2d)
{cout<<" = ("<<p2d.X()<<" "<<p2d.Y()<<")"<<endl;}

/*Standard_IMPORT Standard_Real FUN_tool_parOnE(const Standard_Integer Index,const TopoDS_Edge& E,const TopoDS_Face& F);
Standard_IMPORT void FUN_tool_addTomap
(const TopoDS_Shape& key,const TopoDS_Shape& elementOfitem,TopTools_DataMapOfShapeListOfShape& map,const Standard_Integer check);
Standard_IMPORT void FUN_tool_addTomap(const TopoDS_Shape& key,const TopoDS_Shape& elementOfitem,
				       TopTools_IndexedDataMapOfShapeListOfShape& map,const Standard_Integer check);

// ----------------------------------------------------------------------
Standard_EXPORT gp_Pnt2d FUN_GetVParonF(const TopoDS_Edge& E, const TopoDS_Face& F, const Standard_Integer Index)
// ----------------------------------------------------------------------
{
  Standard_Real par = FUN_tool_parOnE(Index, E, F);
  Standard_Real pf,pl,tol; Handle(Geom2d_Curve) PC = FUNTOOLC2D_CurveOnSurface(E,F,pf,pl,tol); 
  if (PC.IsNull()) Standard_Failure::Raise(" FUN_GetVParonF : no 2d curve");
  gp_Pnt2d  p2d; PC->D0(par,p2d);
  return p2d;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Real FUN_toluv(const GeomAdaptor_Surface& GAS, const Standard_Real& tol3d)
// ----------------------------------------------------------------------
{
  Standard_Real tolu = GAS.UResolution(tol3d);
  Standard_Real tolv = GAS.VResolution(tol3d); 
  Standard_Real toluv = Max(tolu,tolv);
  return toluv;
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_Vertices(const TopoDS_Edge& E, TopTools_Array1OfShape& vertices)
// ----------------------------------------------------------------------
{  
  // Returns vertices (F,R) if E is FORWARD
  //                  (R,V) if E is REVERSED
  TopAbs_Orientation oriE = E.Orientation();
  TopoDS_Vertex vF, vR; TopExp::Vertices(E,vF,vR);
  Standard_Integer iF, iR;
  if (oriE == TopAbs_FORWARD) {iF = 1; iR = 2;}
  else                        {iF = 2; iR = 1;}
  vertices.ChangeValue(iF) = vF; 
  vertices.ChangeValue(iR) = vR;
} 
// ----------------------------------------------------------------------
Standard_EXPORT void FUN_mapVloe(const TopoDS_Shape& F, TopTools_IndexedDataMapOfShapeListOfShape& mapVloe)
// ----------------------------------------------------------------------
{
  mapVloe.Clear();
//  TopExp::MapShapesAndAncestors(F,TopAbs_VERTEX,TopAbs_EDGE,mapVloe);
  TopExp_Explorer exe(F, TopAbs_EDGE);
  for (; exe.More(); exe.Next()){
    const TopoDS_Shape e = exe.Current();
    TopExp_Explorer exv(e, TopAbs_VERTEX);
    for (; exv.More(); exv.Next()){
      const TopoDS_Shape& v = exv.Current();
      FUN_tool_addTomap(v, e, mapVloe, 0);      
    }
  } 
}*/

//======================================================================

/*Standard_EXPORT Standard_Boolean FUN_DetectEdgeswithfaultyUV
(const TopoDS_Face& Fin, const TopoDS_Shape& fF, const TopTools_ListOfShape& ISOEds,
 const Standard_Boolean has2fybounds, TopTools_ListOfShape& lfyE, Standard_Integer& Ivfaulty,
 const Standard_Boolean& stopatfirst=Standard_False)
// purpose : finding out edges <lfyE> / its UV rep. on <Fin>
//           is unconnected to the other pcurves.
//           if (has2fybounds), the 2 bounds of the pcurve are faulty.
//           else : Ivfaulty is index of faulty vertex.
// return True if at least one edge is faulty
{
  // <ISOEds> : list of pcurves to check among <fF>'s 2d rep. edges.
  // <ISOEds> are edges of <fF>

  lfyE.Clear();
  Ivfaulty = 0; // dummy if has2faultybounds
  TopTools_ListOfShape Eds;
  TopExp_Explorer exe(fF, TopAbs_EDGE);
  for (; exe.More(); exe.Next()) Eds.Append(exe.Current());

  // <mapVloe> :
  // ----------
  //    key  = a vertex boundary of a closing edge,
  //    item = the list of edges connexed to the vertex.
  TopTools_IndexedDataMapOfShapeListOfShape mapVloe; ::FUN_mapVloe(fF,mapVloe);

  const Handle(Geom_Surface)& SU = BRep_Tool::Surface(Fin);
  GeomAdaptor_Surface GAS(SU);
  Standard_Real tttol = 1.e-8;
  Standard_Real tttolF = BRep_Tool::Tolerance(Fin);
  Standard_Real tttuvF = FUN_toluv(GAS,tttolF);
  
  // fF's checking :
  // -----------------
  // An edge is valid if the first and last vertices are valid:
  // vertex <vISOe> is valid if there is an edge with bound <ve> such that :
  //   <vISOe> and <ve> share same UV geometry
  //   <vISOe> and <ve> are of opposite orientation.   
  TopTools_ListIteratorOfListOfShape itISOEd(ISOEds);
  for (; itISOEd.More(); itISOEd.Next()) {
    const TopoDS_Edge& ISOe = TopoDS::Edge(itISOEd.Value());
    TopAbs_Orientation oriISO = ISOe.Orientation();
    if (M_INTERNAL(oriISO) || M_EXTERNAL(oriISO)) continue;
    
    Standard_Real tttolISOe = BRep_Tool::Tolerance(ISOe);
    Standard_Real tttuvISOe = FUN_toluv(GAS,tttolISOe);
    
    TopTools_Array1OfShape vISOE(1,2); FUN_tool_Vertices(ISOe, vISOE);

    Standard_Boolean closed = vISOE(1).IsSame(vISOE(2));
    if (closed) continue; // closed edge is ass
    
    Standard_Integer nfyv = 0;
    for (Standard_Integer ivISOe = 1; ivISOe <=2; ivISOe++) {

      // <vISOe> (boundary of <ISOe>):      
      const TopoDS_Vertex& vISOe = TopoDS::Vertex(vISOE(ivISOe));
      gp_Pnt2d UVvISOe = FUN_GetVParonF(ISOe, Fin, ivISOe);
      
      Standard_Real tttolvISOe = BRep_Tool::Tolerance(vISOe);
      Standard_Real tttuvvISOe = FUN_toluv(GAS,tttolvISOe);

      Standard_Boolean isbound = mapVloe.Contains(vISOe);
      if (!isbound) {FUN_RaiseError(); return Standard_False;}
      
      // <vISOeok> :
      Standard_Boolean vISOeok = Standard_False;
      const TopTools_ListOfShape& loe = mapVloe.FindFromKey(vISOe);
      Standard_Integer nloe = loe.Extent();
      for (TopTools_ListIteratorOfListOfShape ite(loe); ite.More(); ite.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(ite.Value());
	if (E.IsSame(ISOe)) continue;
	Standard_Real tttolE = BRep_Tool::Tolerance(E);
	Standard_Real tttuvE = FUN_toluv(GAS,tttolE);

	TopTools_Array1OfShape vE(1,2); FUN_tool_Vertices(E,vE);	
	for (Standard_Integer ive = 1; ive <=2; ive++) {	  
	  const TopoDS_Vertex& ve = TopoDS::Vertex(vE(ive));	  
	  Standard_Boolean samev = ve.IsSame(vISOe);
	  if (!samev) continue;
	  
	  if (ive == ivISOe) continue;
	  gp_Pnt2d UVve = FUN_GetVParonF(E, Fin, ive);	
	  
	  Standard_Real tttolve = BRep_Tool::Tolerance(ve);
	  Standard_Real tttuvve = FUN_toluv(GAS,tttolve);
 
	  tttol = Max(tttol,Max(tttuvF,Max(tttuvE,Max(tttuvISOe,Max(tttuvve,tttuvvISOe)))));
	  Standard_Boolean isequal = UVvISOe.IsEqual(UVve,tttol);
	  if (isequal) {vISOeok = Standard_True; break;}
	} // ive	
	if (vISOeok) break;
      } // ite(loe)
      
      if (!vISOeok) nfyv++;
      
      Standard_Boolean stop = (!has2fybounds && (nfyv > 0));
      if (stop) {
	if (Ivfaulty == 0) Ivfaulty = ivISOe;//xpu121098 (cto900J4,f4ou) : first faulty edge
	break;
      }
    } // ivISOe = 1..2
    
    Standard_Boolean found = has2fybounds && (nfyv == 2);
    found = found || (!has2fybounds && (nfyv == 1));
    if (found) {
      lfyE.Append(ISOe);
    }
  }

  Standard_Integer n = lfyE.Extent();
  Standard_Boolean det = (n != 0);
  return det;
} // FUN_DetectEdgeswithfaultyUV

Standard_EXPORT Standard_Boolean FUN_DetectEdgewithfaultyUV
(const TopoDS_Face& Fin, const TopoDS_Shape& fF, const TopTools_ListOfShape& ISOEds,
 const Standard_Boolean has2fybounds, TopoDS_Shape& fyE, Standard_Integer& Ivfaulty)
// purpose : finding out first edge <fyE> / its UV rep. on <Fin>
//           is unconnected to the other pcurves.
//           if (has2fybounds), the 2 bounds of the pcurve are faulty.
//           else : Ivfaulty is index of faulty vertex.
// return True if at least one edge is faulty
{
  // <ISOEds> : list of pcurves to check among <fF>'s 2d rep. edges.
  // <ISOEds> are edges of <fF>
  TopTools_ListOfShape lfyE;
  Standard_Boolean det = FUN_DetectEdgeswithfaultyUV(Fin,fF,ISOEds,has2fybounds,lfyE,Ivfaulty);
  if (det) fyE = lfyE.First();
  return det;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_DetectFaultyClosingEdge
(const TopoDS_Face& Fin,const TopTools_ListOfShape& Eds,const TopTools_ListOfShape& cEds,TopTools_ListOfShape& fyE)
// ----------------------------------------------------------------------
{
#ifdef DEB
  Standard_Boolean trc = TopOpeBRepTool_GettracePURGE();
  for (TopTools_ListIteratorOfListOfShape it(Eds); it.More(); it.Next()){
    const TopoDS_Shape& ed = it.Value();
    TopExp_Explorer ex;
    for (ex.Init(ed, TopAbs_VERTEX); ex.More(); ex.Next())
//    for (TopExp_Explorer ex(ed, TopAbs_VERTEX); ex.More(); ex.Next())
      FUN_adds(ex.Current());}
#endif

  // <cEds> and <Eds> are the lists of closed and non-"closing" edges 
  // describing a wire boundary on face <Fin>. 
  // ("closing" edge = edge built on the geometry of a closing edge of <Fin> )  
  
  // If <W> has UV non-connexed edges, returns <True> if <fyE> is the list of faulty cEds
  // fyE = {e / e has its 2 bounds non-UVconnexed}
  // Returns <False> elsewhere.
  // prequesitory : wire <W> is 3d-connexed, so a faulty edge must be a 
  //                closing edge.

  // <mapVloe> :
  // ----------
  //    key  = a vertex boundary of a closing edge,
  //    item = the list of edges connexed to the vertex.
  TopTools_DataMapOfShapeListOfShape mapVloe;
  for (TopTools_ListIteratorOfListOfShape itEd(Eds); itEd.More(); itEd.Next()) { 
    const TopoDS_Edge& E = TopoDS::Edge(itEd.Value());
    TopExp_Explorer ex;
    for (ex.Init(E, TopAbs_VERTEX); ex.More(); ex.Next()) { 
//    for (TopExp_Explorer ex(E, TopAbs_VERTEX); ex.More(); ex.Next()) { 
      const TopoDS_Shape& v = ex.Current();
      FUN_tool_addTomap(v, E, mapVloe, 0);
    }
  }    
  const Handle(Geom_Surface)& SU = BRep_Tool::Surface(TopoDS::Face(Fin));
  GeomAdaptor_Surface GAS(SU);
  Standard_Real tttol = 1.e-8;
  Standard_Real tttolF = BRep_Tool::Tolerance(TopoDS::Face(Fin));
  Standard_Real tttuvF = FUN_toluv(GAS,tttolF);  

  Standard_Boolean uclosed,vclosed; Standard_Real uperiod,vperiod; 
  Standard_Boolean uvclosed = FUN_tool_closedS(Fin,uclosed,uperiod,vclosed,vperiod);
  
  // wire's checking :
  // -----------------
  // An edge is valid if the first and last vertices are valid:
  // vertex <vce> is valid if there is an edge with bound <ve> verifying :
  //   <vce> and <ve> share same UV geometry
  //   <vce> and <ve> are of opposite orientation.   
  TopTools_ListIteratorOfListOfShape itCEd(cEds);
  for (; itCEd.More(); itCEd.Next()) {
    const TopoDS_Edge& cE = TopoDS::Edge(itCEd.Value());   
    Standard_Boolean isou,isov; gp_Pnt2d o2d; gp_Dir2d d2d; 
    Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FUNTOOLC2D_CurveOnSurface(cE,Fin,f,l,tol);
    Standard_Boolean isouv = FUN_tool_IsUViso(PC,isou,isov,d2d,o2d);
    
#ifdef DEB 
    Standard_Integer icE = FUN_addcheckepc(cE,Fin);
    if (trc)  {cout<<"closing edge "<<icE<<" "; TopAbs::Print(cE.Orientation(),cout);
	       cout<<" : \n";}
#endif
    
    Standard_Real tttolcE = BRep_Tool::Tolerance(cE);
    Standard_Real tttuvcE = FUN_toluv(GAS,tttolcE);
    
    Standard_Boolean cEKO = Standard_True;
    TopTools_Array1OfShape vcE(1,2); FUN_tool_Vertices(cE, vcE);    
    for (Standard_Integer ivce = 1; ivce <=2; ivce++) {
      // <vce> (boundary of <cE>):      
      const TopoDS_Vertex& vce = TopoDS::Vertex(vcE(ivce));
      gp_Pnt2d UVvce = FUN_GetVParonF(cE, Fin, ivce);
#ifdef DEB
      // recall in one wire, there are 2 vertices for one non-degenerated closing edge
      Standard_Integer ivmapv = STATIC_PURGE_mapv.Add(vce);
      if (trc) {
	cout<<"v"<<ivmapv<<" ";FUN_tool_trace(ivce);cout<<" in closing edge "<<icE;
	FUN_tool_trace(UVvce);
#ifdef DRAW	
	TCollection_AsciiString bb("uv_");bb += TCollection_AsciiString(ivmapv);FUN_tool_draw(bb,UVvce);
#endif	
      }
#endif
      
      Standard_Real tttolvce = BRep_Tool::Tolerance(vce);
      Standard_Real tttuvvce = FUN_toluv(GAS,tttolvce);

      Standard_Boolean isbound = mapVloe.IsBound(vce);
      if (!isbound) continue; //xpu201198 cto016E2 (e3on)
      
      // <vceok> :
      Standard_Boolean vceok = Standard_False;
      const TopTools_ListOfShape& loe = mapVloe.Find(vce);
      Standard_Integer nloe = loe.Extent();
#ifdef DRAW
      Standard_Boolean tdr = Standard_False;
      if (tdr) {
	Standard_Integer inde = 1; 
	for (TopTools_ListIteratorOfListOfShape ite(loe); ite.More(); ite.Next()) {
	  const TopoDS_Shape& eee = ite.Value(); FUN_drawe(eee,inde); inde++;
	}
      }
#endif
      for (TopTools_ListIteratorOfListOfShape ite(loe); ite.More(); ite.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(ite.Value());
//	if (E.IsSame(cE)) continue; // closing FORWARD and REVERSED EDGES are UV disjoint	
#ifdef DEB
	Standard_Integer iE = FUN_addcheckepc(E,Fin);
#endif

	Standard_Real tttolE = BRep_Tool::Tolerance(E);
	Standard_Real tttuvE = FUN_toluv(GAS,tttolE);

	TopTools_Array1OfShape vE(1,2); FUN_tool_Vertices(E,vE);	
	for (Standard_Integer ive = 1; ive <=2; ive++) {	  
	  const TopoDS_Vertex& ve = TopoDS::Vertex(vE(ive));	  
	  Standard_Boolean samev = ve.IsSame(vce);
	  if (!samev) continue;
	  
	  if (ive == ivce) continue;
	  gp_Pnt2d UVve = FUN_GetVParonF(E, Fin, ive);	
	  
	  Standard_Real tttolve = BRep_Tool::Tolerance(ve);
	  Standard_Real tttuvve = FUN_toluv(GAS,tttolve);
 
	  tttol = Max(tttol,Max(tttuvF,Max(tttuvE,Max(tttuvcE,Max(tttuvve,tttuvvce)))));
//	  Standard_Boolean isequal = UVvce.IsEqual(UVve,tttol);	  
	  Standard_Real dd=0., xperiod=0.;
	  if (uclosed && isou) {dd = UVve.X()-UVvce.X(); xperiod = uperiod;}
	  if (vclosed && isov) {dd = UVve.Y()-UVvce.Y(); xperiod = vperiod;}
	  Standard_Boolean xok = (Abs(dd)<tttol) || (Abs(Abs(dd)-xperiod)<tttol);
	  Standard_Boolean isequal = xok; 

	  if (isequal) {
	    vceok = Standard_True;
#ifdef DEB
	    if (trc) 
	      {cout<<"vertex ";FUN_tool_trace(ivce);cout<<" of closing edge "<<icE;
	       cout<<" is connexed to vertex ";FUN_tool_trace(ive);cout<<" of edge "<<iE<<endl;}
#endif	
	  }
	  break;
	} // ive	
	if (vceok) break;
      } // ite(loe)      
#ifdef DEB
      if (trc && !vceok) {cout<<" vertex ";FUN_tool_trace(ivce);
			  cout<<"of closing edge "<<icE<<" is faulty"<<endl;}	
#endif    
      cEKO = cEKO && (!vceok);
    } // ivce

#ifdef DEB 
    if (trc) {if (!cEKO) cout<<"-> valid edge"<<endl;
              else cout<<"-> faulty edge"<<endl;}
#endif    
    if (cEKO) {fyE.Append(cE);}
  }
  return (!fyE.IsEmpty());
}

Standard_EXPORT Standard_Boolean FUN_isUVClosed(const TopoDS_Face& Fin, const TopoDS_Face& fF)
// purpose : returns true is fF is UV connexed
{
  TopTools_ListOfShape efF; 
  TopExp_Explorer ex(fF, TopAbs_EDGE);
  for (; ex.More(); ex.Next()) efF.Append(ex.Current());
  Standard_Integer Ivfaulty; TopTools_ListOfShape lfyE; Standard_Boolean stopatfirst = Standard_True;
  Standard_Boolean foundfaulty = FUN_DetectEdgeswithfaultyUV(Fin,fF,efF,Standard_False,lfyE,Ivfaulty,stopatfirst);
  return !foundfaulty;
}*/
