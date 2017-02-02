// Created on: 1997-11-13
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

#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_CORRISO.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Surface.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>

#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <Standard_Failure.hxx>
#include <TopOpeBRepTool_PURGE.hxx>

#ifdef OCCT_DEBUG
//Standard_EXPORT Standard_Integer STATIC_PURGE_iwi = 0;
//Standard_EXPORT TopTools_IndexedMapOfShape STATIC_PURGE_mapw, STATIC_PURGE_mapv;
//Standard_EXPORT TopTools_IndexedMapOfOrientedShape STATIC_PURGE_mapeds, STATIC_CORR_mapeds;

Standard_EXPORT void debcorrUV(){};
extern Standard_Boolean TopOpeBRepTool_GettracePURGE();
extern Standard_Boolean TopOpeBRepTool_GettraceCORRISO();
#endif
// DEB

#define SPLITEDGE (0)
#define INCREASE  (1)
#define DECREASE (-1)

#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

static void FUN_addtomap(TopTools_DataMapOfShapeListOfShape& map, const TopoDS_Shape& key, const TopoDS_Shape& item)
{
  if (map.IsBound(key))               map.ChangeFind(key).Append(item);
  else {TopTools_ListOfShape los; los.Append(item); map.Bind(key,los);}
}

static Standard_Boolean FUN_getv(const TopAbs_Orientation& orivine, const TopoDS_Shape& e, TopoDS_Shape& v)
{
  v.Nullify();
  // gets <v> oriented <orivine> in <e>
  TopAbs_Orientation oe = e.Orientation();
  Standard_Boolean isnonO = M_INTERNAL(oe) || M_EXTERNAL(oe);
  TopoDS_Shape eO;
  if (isnonO) {
    eO = e.Oriented(TopAbs_FORWARD);
  }
  else {
    eO = e;
  }
  TopExp_Explorer exv(eO, TopAbs_VERTEX);
  for (; exv.More(); exv.Next()) {	
    const TopoDS_Shape& vcur = exv.Current();
    if (vcur.Orientation() == orivine) {v = vcur; return Standard_True;}    
  } // exv
  return Standard_False;
}

Standard_EXPORT Standard_Boolean FUN_tool_ClosedW(const TopoDS_Wire& W)
{
  // !! an edge oriented INTERNAL/EXTERNAL has all its vertices
  // oriented INTERNAL/EXTERNAL.

  // <mapvedsO> = {(v,loe)} / e is oriented :
  // <mapvedsO> = {(v,loe)} / e is not oriented (INTERNAL/EXTERNAL) :
  TopTools_DataMapOfShapeListOfShape mapvFine,mapvRine, mapvIine;

  TopExp_Explorer exe(W, TopAbs_EDGE);
  for (; exe.More(); exe.Next()){
    const TopoDS_Shape& e = exe.Current();
    TopAbs_Orientation oe = e.Orientation();
    Standard_Boolean isnonO = M_INTERNAL(oe) || M_EXTERNAL(oe);
    TopoDS_Shape eO;
    if (isnonO) {
      eO = e.Oriented(TopAbs_FORWARD);
    }
    else {
      eO = e;
    }

    TopExp_Explorer exv(eO, TopAbs_VERTEX); 
    for (; exv.More(); exv.Next()) {	
      const TopoDS_Shape& v = exv.Current();
      TopAbs_Orientation oriv = v.Orientation();
      if (M_FORWARD(oriv))  FUN_addtomap(mapvFine,v,e);
      if (M_REVERSED(oriv)) FUN_addtomap(mapvRine,v,e);
      if (M_INTERNAL(oriv)) FUN_addtomap(mapvIine,v,e);
    }      
  }

  if (mapvFine.Extent() == 0) return Standard_False;   // empty wire  

  TopTools_MapOfShape mapvok;    
  // a vertex is found valid if is - an internal vertex
  //                               - found FORWARD and REVERSED. 
  TopTools_MapOfShape mapvonlyFine; // {(v,e)} v F in e, v belongs to only one e
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itvFine(mapvFine);
  for (; itvFine.More(); itvFine.Next()){
    const TopoDS_Shape& vFine = itvFine.Key();
    Standard_Boolean vIine = mapvIine.IsBound(vFine);
    if (vIine) {mapvok.Add(vFine); continue;}
    Standard_Boolean vRine = mapvRine.IsBound(vFine);
    if (vRine) {mapvok.Add(vFine); continue;}
    mapvonlyFine.Add(vFine);
  }  
  // <mapvRinonee> = {(v,e)} v R in e, v belongs to only one e
  TopTools_MapOfShape mapvonlyRine; 
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itvRine(mapvRine);
  for (; itvRine.More(); itvRine.Next()){
    const TopoDS_Shape& vRine = itvRine.Key();
    Standard_Boolean vok = mapvok.Contains(vRine);
    if (vok) continue;
    Standard_Boolean vIine = mapvIine.IsBound(vRine);
    if (vIine) {mapvok.Add(vRine); continue;}    
    mapvonlyRine.Add(vRine);
  }
  
  // checking vertices <mapvonlyFine> and <mapvonlyRine>
  TopTools_MapIteratorOfMapOfShape itvonlyFRine;
  Standard_Integer nmap = 0;
  while (nmap <= 2) {
    nmap++;
    Standard_Boolean vFine = (nmap == 1);
    if (vFine) itvonlyFRine.Initialize(mapvonlyFine);
    else       itvonlyFRine.Initialize(mapvonlyRine);

    for (; itvonlyFRine.More(); itvonlyFRine.Next()){
      const TopoDS_Shape& vtocheck = itvonlyFRine.Key();
      TopTools_ListOfShape edsvFRine;
      if (vFine) edsvFRine = mapvFine.Find(vtocheck);
      else       edsvFRine = mapvRine.Find(vtocheck);
       
      if (edsvFRine.Extent() > 1) return Standard_False; // faulty wire
      const TopoDS_Shape& e = edsvFRine.First();

      TopAbs_Orientation ovori = vFine? TopAbs_REVERSED: TopAbs_FORWARD;
      TopoDS_Shape ov; Standard_Boolean ovfound = FUN_getv(ovori,e,ov);
      if (!ovfound) return Standard_False; // faulty edge

      // <vtocheck> is on only one edge <e>,
      // <vtocheck> is FORWARD/REVERSED in <e>,
      // <ovfound> is REVERSED/FORWARD in <e>.
      // <vtocheck> is ok if : - <ovfound> is INTERNAL in another edge
      //                       - <ovfound> is FORWARD and REVERSED in 
      //                         one or two other edges.
      //                      and e is not oriented
      TopAbs_Orientation oe = e.Orientation();
      if (M_FORWARD(oe) || M_REVERSED(oe)) return Standard_False;
      if (!mapvok.Contains(ov)) return Standard_False;

      Standard_Boolean ovIine = mapvIine.IsBound(ov); if (ovIine)  continue;
      Standard_Boolean ovFine = mapvRine.IsBound(ov); if (!ovFine) return Standard_False;
      Standard_Boolean ovRine = mapvRine.IsBound(ov); if (!ovRine) return Standard_False;
    
      const TopTools_ListOfShape& edsovFine = mapvFine.Find(ov);
      const TopTools_ListOfShape& edsovRine = mapvRine.Find(ov);
      if (edsovFine.Extent() > 1) continue;
      if (edsovRine.Extent() > 1) continue;
      if (edsovFine.First().IsEqual(e)) return Standard_False;
      if (edsovRine.First().IsEqual(e)) return Standard_False;
    }
  } // nmap
  return Standard_True; 
}

//=======================================================================
//function : PurgeClosingEdges
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool::PurgeClosingEdges(const TopoDS_Face& Fin, const TopoDS_Face& FF, 
//				      const TopTools_DataMapOfShapeInteger& MWisOld,
				      const TopTools_DataMapOfShapeInteger& ,
				      TopTools_IndexedMapOfOrientedShape& MshNOK)
{
  // Fin is the original face
  // FF  is the splitted face

  // prequesitory : splitted edges, of edge ancestor a closing edge
  //                keep in memory the geometry of the ancestor edge,
  //                they answer True to BRep_Tool::IsClosed.
  // elsewhere : we have to get this information using geometric 
  //             criteriums (TopOpeBRepTool_TOOL::IsonCLO)

#ifdef OCCT_DEBUG
  Standard_Boolean trc =  TopOpeBRepTool_GettracePURGE();
  if (trc) cout<<"\n* PurgeClosingEdges:\n\n";
#endif
  TopOpeBRepTool_CORRISO CORRISO(Fin);  
  Standard_Real tolF = BRep_Tool::Tolerance(Fin);
  Standard_Real uperiod; Standard_Boolean uclosed = CORRISO.Refclosed(1,uperiod);
  Standard_Real vperiod; Standard_Boolean vclosed = CORRISO.Refclosed(2,vperiod);
  if (!uclosed && !vclosed) return Standard_False;
  Standard_Boolean inU = uclosed ? Standard_True : Standard_False;  
  Standard_Real xmin = inU ? (CORRISO.GASref().FirstUParameter()) :
                             (CORRISO.GASref().FirstVParameter());
  Standard_Real xper = inU ? uperiod : vperiod;
  Standard_Real tolx = inU ? (CORRISO.Tol(1,tolF)) : (CORRISO.Tol(2,tolF));

  TopExp_Explorer exw(FF, TopAbs_WIRE);
  for (; exw.More(); exw.Next()){
    const TopoDS_Shape& W = exw.Current();

    CORRISO.Init(W);
    Standard_Boolean ok = CORRISO.UVClosed();
    if (ok) continue;

    TopTools_ListOfShape cEds; 
    TopTools_ListIteratorOfListOfShape ite(CORRISO.Eds());
    for (; ite.More(); ite.Next()){
      const TopoDS_Edge& E = TopoDS::Edge(ite.Value());
      Standard_Boolean closing = BRep_Tool::IsClosed(E,Fin); 
      if (!closing) {// xpu231198 : pcurve modified, the information is lost
	TopOpeBRepTool_C2DF C2DF; Standard_Boolean isb = CORRISO.UVRep(E,C2DF);
	if (!isb) return Standard_False;//NYIRAISE
	Standard_Boolean onclo = TopOpeBRepTool_TOOL::IsonCLO(C2DF,inU,xmin,xper,tolx);
	if (onclo) closing=Standard_True;
      }
      if (closing) cEds.Append(E);
    }          
    Standard_Integer ncE = cEds.Extent();
    Standard_Boolean nopurge = (ncE <= 1);
    if (nopurge) return Standard_True;
    
    // Checking <W>
    TopTools_ListOfShape fyEds; Standard_Boolean topurge = CORRISO.PurgeFyClosingE(cEds,fyEds);
    if (topurge) {
      TopTools_ListIteratorOfListOfShape it(fyEds);
      for (; it.More(); it.Next()) MshNOK.Add(it.Value()); 
      MshNOK.Add(W); 
      MshNOK.Add(FF);
    }
    
#ifdef OCCT_DEBUG
    if (trc && topurge) cout<<"found FAULTY edge = ed"<<endl;
#endif
  } // exw
  return Standard_True;
}

//=======================================================================
//function : PurgeClosingEdges
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool::PurgeClosingEdges(const TopoDS_Face& Fin, const TopTools_ListOfShape& LOF, 
				      const TopTools_DataMapOfShapeInteger& MWisOld,
				      TopTools_IndexedMapOfOrientedShape& MshNOK)
{  
  Standard_Boolean uvclosed = FUN_tool_closedS(Fin);
  if (!uvclosed) return Standard_True;
  
  TopTools_ListIteratorOfListOfShape it(LOF);
  for (; it.More(); it.Next()){
    const TopoDS_Face& FF = TopoDS::Face(it.Value());
    Standard_Boolean ok = TopOpeBRepTool::PurgeClosingEdges(Fin,FF,MWisOld,MshNOK);
    if (!ok) return Standard_False;    
  }
  return Standard_True;
}

/*static Standard_Boolean FUN_correctClosingE(TopoDS_Edge& newfE, TopoDS_Face& Fsp)
{    
  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FUNTOOLC2D_CurveOnSurface(newfE,Fsp,f,l,tol);
  gp_Dir2d d2d; gp_Pnt2d O2d; Standard_Boolean isuiso,isviso; 
  Standard_Boolean uviso = FUN_tool_IsUViso(PC,isuiso,isviso,d2d,O2d);
  if (!uviso) return Standard_False;

  Standard_Real period = 2*M_PI;
  Standard_Real piso = isuiso? O2d.X(): O2d.Y();
  Standard_Real tol2d = 1.e-6;
  Standard_Boolean is0   = Abs(piso) < tol2d;
  Standard_Boolean is2PI = Abs(period-piso) < tol2d;
  // --------------------------------------------------
  //  prequesitory :  Closed Surfaces have period 2PI
  if (!is0 && !is2PI) return Standard_False;
  // -------------------------------------------------- 
  Standard_Real factor = is0? period: -period;
  gp_Vec2d transl(1.,0.); if (isviso) transl = gp_Vec2d(0.,1.);
  transl.Multiply(factor);
  
  Standard_Integer ok = FUN_tool_translate(transl,Fsp,newfE);
  return Standard_True;   
}

static Standard_Boolean FUN_correctDegeneratedE
(const TopTools_IndexedDataMapOfShapeListOfShape& mve,TopoDS_Edge& Ein,TopoDS_Face& Fsp)
{
  TopoDS_Vertex v1,v2; TopExp::Vertices(Ein,v1,v2);
  TopAbs_Orientation ov1 = v1.Orientation();
  TopAbs_Orientation ov2 = v2.Orientation();
  Standard_Boolean ok1 = mve.Contains(v1); if (!ok1) return Standard_False;
  Standard_Boolean ok2 = mve.Contains(v2); if (!ok2) return Standard_False;

  const TopTools_ListOfShape& le= mve.FindFromKey(v1);

  TopoDS_Edge e1,e2; Standard_Boolean fe1 = Standard_False; Standard_Boolean fe2 = Standard_False;
  TopoDS_Vertex vfe1,vfe2;
  Standard_Boolean fEin = Standard_False;
  for(TopTools_ListIteratorOfListOfShape itle(le);itle.More();itle.Next()) {
    const TopoDS_Shape& ecx = TopoDS::Edge(itle.Value());
    if (ecx.IsEqual(Ein)) {
      fEin = Standard_True;
    }
    else {
      TopExp_Explorer exv;
      for (exv.Init(ecx,TopAbs_VERTEX);exv.More();exv.Next()) {
//      for (TopExp_Explorer exv(ecx,TopAbs_VERTEX);exv.More();exv.Next()) {
	const TopoDS_Vertex& vecx = TopoDS::Vertex(exv.Current());
	Standard_Boolean issam = vecx.IsSame(v1);
	if (issam) { 
	  Standard_Boolean iseq1 = vecx.IsEqual(v1);
	  Standard_Boolean iseq2 = vecx.IsEqual(v2);
	  if (!iseq1) {
	    e1 = TopoDS::Edge(ecx);
	    vfe1 = TopoDS::Vertex(vecx); 
	    fe1 = Standard_True;
	  }
	  if (!iseq2) {
	    e2 = TopoDS::Edge(ecx);
	    vfe2 = TopoDS::Vertex(vecx); 
	    fe2 = Standard_True;
	  }
	}
	if (fe1 && fe2) break;
      }
      if (fe1 && fe2) break;
    }
    if (fEin && fe1 && fe2) break;
  } // itle.More()

  Standard_Boolean ok = (fEin && fe1 && fe2);
  if (!ok) return Standard_False;
  
#ifdef OCCT_DEBUG
  debcorrUV(); // call Draw_Call("av2d;dy fyf;fit;ppcu fyf")
#endif

  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FUNTOOLC2D_CurveOnSurface(Ein,Fsp,f,l,tol);
  gp_Dir2d d2d; gp_Pnt2d O2d; Standard_Boolean isuiso = 0,isviso = 0; 
  Standard_Boolean uviso = FUN_tool_IsUViso(PC,isuiso,isviso,d2d,O2d);
  if (!uviso) return Standard_False;

  Standard_Real pfEin,plEin,tolEin; Handle(Geom2d_Curve) PCEin = FUNTOOLC2D_CurveOnSurface(Ein,Fsp,pfEin,plEin,tolEin); 
  if (PCEin.IsNull()) throw Standard_Failure(" FUN_correctDegeneratedE : no 2d curve Ein");
  Standard_Real pf1,pl1,tol1; Handle(Geom2d_Curve) PC1 = FUNTOOLC2D_CurveOnSurface(e1,Fsp,pf1,pl1,tol1); 
  if (PC1.IsNull()) throw Standard_Failure(" FUN_correctDegeneratedE : no 2d curve e1");
  Standard_Real pf2,pl2,tol2; Handle(Geom2d_Curve) PC2 = FUNTOOLC2D_CurveOnSurface(e2,Fsp,pf2,pl2,tol2); 
  if (PC2.IsNull()) throw Standard_Failure(" FUN_correctDegeneratedE : no 2d curve e2");
  
  Standard_Real parv1 = BRep_Tool::Parameter(v1,Ein,Fsp);
  Standard_Real parv2 = BRep_Tool::Parameter(v2,Ein,Fsp);
  gp_Pnt2d  pv1; PCEin->D0(parv1,pv1);
  gp_Pnt2d  pv2; PCEin->D0(parv2,pv2);

  Standard_Real par1 = BRep_Tool::Parameter(vfe1,e1,Fsp);
  Standard_Real par2 = BRep_Tool::Parameter(vfe2,e2,Fsp);
  gp_Pnt2d  p1; PC1->D0(par1,p1);
  gp_Pnt2d  p2; PC2->D0(par2,p2);

  Standard_Real cv1 = (isuiso) ? pv1.Y() : pv1.X();
  Standard_Real cv2 = (isuiso) ? pv2.Y() : pv2.X();
  Standard_Real c1 = (isuiso) ? p1.Y() : p1.X();
  Standard_Real c2 = (isuiso) ? p2.Y() : p2.X();

  Standard_Real d1 = (c1 - cv1);
  Standard_Real d2 = (c2 - cv2);
  Standard_Real adc = Abs(c1 - c2);
  Standard_Real adcv = Abs(cv1 - cv2);

  Standard_Real tol2d = 1.e-6;
  Standard_Boolean mmd = (Abs(adc-adcv) < tol2d);
  if (mmd) { // correction de CTS20973
    gp_Vec2d transl(0.,1.); if (isviso) transl = gp_Vec2d(1.,0.);
    transl.Multiply(d1); // ou d2
    ok = FUN_tool_translate(transl,Fsp,Ein);
  }
  else {
    // redefinition des parametres de v1,v2 de Ein tels que des parametres de 
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceCORRISO()) {
      cout<<"FUN_correctDegeneratedE : !mmd NYI"<<endl;
    }
#endif
    ok = Standard_False;
  }
  
  return ok;   
} //FUN_correctDegeneratedE*/

/*static Standard_Boolean FUN_tool_reguUV(const TopoDS_Face& FF, TopoDS_Face& fF)
{  

  const TopoDS_Face& F = TopoDS::Face(FF);
  Standard_Boolean uclosed,vclosed; Standard_Real uperiod,vperiod;
  Standard_Boolean closed = FUN_tool_closedS(F,uclosed,uperiod,vclosed,vperiod);
  if (!closed) return Standard_False;    
  Standard_Real tolu,tolv; FUN_tool_tolUV(TopoDS::Face(fF),tolu,tolv);

  TopTools_ListOfShape leko;
  // --------
  TopTools_IndexedMapOfOrientedShape mape;
  TopExp_Explorer ex(fF, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& ee = TopoDS::Edge(ex.Current());
    TopAbs_Orientation oee = ee.Orientation();
    TopAbs_Orientation oe = TopAbs::Complement(oee);
    TopoDS_Edge e = TopoDS::Edge(ee.Oriented(oe));
    Standard_Boolean hasoppo = mape.Contains(e); // e with complement ori was added to mape
    if (hasoppo) {
      // ee :
      Standard_Real ff,ll; Handle(Geom2d_Curve) PCee = BRep_Tool::CurveOnSurface(ee,F,ff,ll);
      Standard_Boolean uisoee,visoee; gp_Dir2d d2dee; gp_Pnt2d O2dee; 
      Standard_Boolean uvisoee = FUN_tool_IsUViso(PCee,uisoee,visoee,d2dee,O2dee);
      // e :
      Standard_Real f,l; Handle(Geom2d_Curve) PCe = BRep_Tool::CurveOnSurface(e,F,f,l);
      Standard_Boolean uisoe,visoe; gp_Dir2d d2de; gp_Pnt2d O2de; 
      Standard_Boolean uvisoe = FUN_tool_IsUViso(PCe,uisoe,visoe,d2de,O2de);

      // isfaulty :
      Standard_Boolean isfaulty = Standard_False;
      Standard_Real dd = O2dee.Distance(O2de);
      if (uisoee && uisoe) isfaulty = (dd < tolu);
      if (visoee && visoe) isfaulty = (dd < tolv);

      if (isfaulty) leko.Append(ee);
    }
    else mape.Add(ee);
  }
  
  Standard_Integer nko = leko.Extent();
  if (nko != 1) return Standard_False;

  // eko = edge with faulty pcurve :
  const TopoDS_Shape& eko = leko.First();
  TopAbs_Orientation oeko = eko.Orientation();
  TopTools_ListOfShape edges; ex.Init(fF, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Shape& ee = ex.Current();
    if (!ee.IsSame(eko)) edges.Append(ee);
  }    
  // fe = edge with vfe = vertex(ivfe) not uv-connexed :
  TopoDS_Shape fe; Standard_Integer ivfe=0; 
  Standard_Boolean det = ::FUN_DetectEdgewithfaultyUV(FF,fF,edges,Standard_False,fe,ivfe);
  if (!det) return Standard_False;

  TopTools_Array1OfShape Vfe(1,2); FUN_Vertices(TopoDS::Edge(fe),Vfe); 
  TopAbs_Orientation ofe = fe.Orientation(); 
  const TopoDS_Vertex& vfe = TopoDS::Vertex(Vfe(ivfe));
  Standard_Real parfe = BRep_Tool::Parameter(vfe,TopoDS::Edge(fe));
  gp_Pnt2d uvfe; Standard_Boolean ok = FUN_tool_paronEF(TopoDS::Edge(fe),parfe,F,uvfe);
  if (!ok) return Standard_False;
  // ivconnex :
  Standard_Integer ivconnex = (ivfe == 1) ? 2 : 1;
//  if (ofe == TopAbs_REVERSED) ivconnex = (ivconnex == 1) ? 2 : 1;
  
  // vertex(ivconnex) of eko FORWARD
  TopoDS_Edge ekoFOR = TopoDS::Edge(eko.Oriented(TopAbs_FORWARD));
  TopTools_Array1OfShape Veko(1,2); FUN_Vertices(TopoDS::Edge(ekoFOR),Veko);  
  const TopoDS_Vertex& veko1 = TopoDS::Vertex(Veko(1));
  const TopoDS_Vertex& veko2 = TopoDS::Vertex(Veko(2));
  Standard_Integer iveko = 0;
  if (veko1.IsSame(vfe)) iveko = 1;
  if (veko2.IsSame(vfe)) iveko = 2;
  if (iveko == 0) return Standard_False;

  // ett : edge same eko with pcurve to translate
  // isekoFOR=true : vfe should be connexed to vertex(ivconnex) of ekoFOR
  Standard_Boolean isekoFOR = (iveko == ivconnex); 
  TopAbs_Orientation oett = isekoFOR ? TopAbs_FORWARD : TopAbs_REVERSED; 
  TopoDS_Edge ett = TopoDS::Edge(eko.Oriented(oett));
  const TopoDS_Vertex& vtt = TopoDS::Vertex(Veko(iveko));

  Standard_Real parett = BRep_Tool::Parameter(vtt,ett);
  gp_Pnt2d uvtt; ok = FUN_tool_paronEF(ett,parett,F,uvtt);
  if (!ok) return Standard_False;
  
  Standard_Real du = uvfe.X()-uvtt.X(); 
  Standard_Real dv = uvfe.Y()-uvtt.Y(); 
  Standard_Boolean tru=Standard_False, trv=Standard_False;
  if (uclosed) tru = (Abs(Abs(du)-uperiod) < tolu);
  if (vclosed) trv = (Abs(Abs(dv)-vperiod) < tolv);
  if (!tru && !trv) return Standard_False;

  gp_Vec2d tt;
  if (tru) tt = gp_Vec2d(du,0.);
  if (trv) tt = gp_Vec2d(0.,dv);
  Standard_Real f,l; Handle(Geom2d_Curve) PC1 = BRep_Tool::CurveOnSurface(ett,F,f,l);
  Standard_Boolean uisoett,visoett; gp_Dir2d d2dett; gp_Pnt2d o2dett; 
  Standard_Boolean uvisoett = FUN_tool_IsUViso(PC1,uisoett,visoett,d2dett,o2dett);o2dett.Translate(tt);
  Handle(Geom2d_Line) L2d = new Geom2d_Line(o2dett,d2dett);
  Handle(Geom2d_TrimmedCurve) PC2 = new Geom2d_TrimmedCurve(L2d,f,l);

  BRep_Builder BB;
  Standard_Real toltt = BRep_Tool::Tolerance(ett);
//  BB.UpdateEdge(TopoDS::Edge(ett),PC2,PC1,fF,toltt);
  // xpu220998 : cto cylcong A1
  Handle(Geom2d_Curve) nullc2d;
  BB.UpdateEdge(TopoDS::Edge(ekoFOR),nullc2d,nullc2d,fF,toltt); // clear
  if (isekoFOR) BB.UpdateEdge(TopoDS::Edge(ekoFOR),PC2,PC1,fF,toltt);
  else          BB.UpdateEdge(TopoDS::Edge(ekoFOR),PC1,PC2,fF,toltt);

  return Standard_True;
}*/

static Standard_Boolean FUN_connexX(const Standard_Boolean onU, TopOpeBRepTool_CORRISO& CORRISO, 
		       const TopTools_ListOfShape& EdstoCheck, TopTools_DataMapOfOrientedShapeInteger& fyEds)
// purpose : Fref is x-periodic, 
//           <fyEds>={(fye,recadre)}, recadre = INCREASE,DECREASE
//                                    fye has its 2 bounds faulty
{
  fyEds.Clear();
  Standard_Real tolF = BRep_Tool::Tolerance(CORRISO.Fref());
  Standard_Integer Index = onU ? 1 : 2;
  Standard_Real xperiod; Standard_Boolean xclosed = CORRISO.Refclosed(Index,xperiod);
  if (!xclosed) return Standard_False;
  Standard_Real xtol = CORRISO.Tol(Index,tolF);
  
  // fy has its 2 uvbounds non-connexed       
  //nyixpu300998 : iterative (while ko) + map of "static" edges
  TopoDS_Shape fy; Standard_Integer Ify=0; Standard_Boolean hasfy = CORRISO.EdgeWithFaultyUV(EdstoCheck,2,fy,Ify); 
  if (!hasfy) return Standard_False;
  TopOpeBRepTool_C2DF C2DF; Standard_Boolean isb = CORRISO.UVRep(TopoDS::Edge(fy),C2DF);
  if (!isb) return Standard_False; // NYIRAISE

  TopTools_Array1OfShape vfy(1,2); TopOpeBRepTool_TOOL::Vertices(TopoDS::Edge(fy),vfy);
  for (Standard_Integer ii = 1; ii <=2; ii++) {
    // vff = vertex[ii] of fy
    const TopoDS_Vertex& vff = TopoDS::Vertex(vfy(ii));
    Standard_Real parvff = TopOpeBRepTool_TOOL::ParE(ii,TopoDS::Edge(fy));gp_Pnt2d uvff = TopOpeBRepTool_TOOL::UVF(parvff,C2DF);
    // loe list of edges connexed to faultE
    TopTools_ListOfShape loe; isb = CORRISO.Connexity(vff,loe);
    if (!isb) return Standard_False; // FUNRAISE

    TopTools_ListIteratorOfListOfShape ite(loe); // iteration on connex edges of vff
    for (; ite.More(); ite.Next()){
      const TopoDS_Edge& ee = TopoDS::Edge(ite.Value());
      TopTools_Array1OfShape vee(1,2); TopOpeBRepTool_TOOL::Vertices(ee,vee);
      for (Standard_Integer ive = 1; ive <=2; ive++) {	  
	// ve = vertex[ive] of ee
	const TopoDS_Vertex& ve = TopoDS::Vertex(vee(ive));	  
	Standard_Boolean samev = ve.IsSame(vff);
	if (!samev) continue;	
	if (ive == ii) continue;
	TopOpeBRepTool_C2DF C2DFe; isb = CORRISO.UVRep(ee,C2DFe);
	if (!isb) return Standard_False; // FUNRAISE
	Standard_Real paree = TopOpeBRepTool_TOOL::ParE(ive,ee); gp_Pnt2d uve = TopOpeBRepTool_TOOL::UVF(paree,C2DFe);	
	
	// xxtrsl :
	Standard_Real dxx = onU ? uve.X()-uvff.X() : uve.Y()-uvff.Y();
	Standard_Boolean isper =( Abs(xperiod-Abs(dxx)) < xtol);
	if (!isper) continue;
	
	Standard_Integer recadre = (dxx > 0) ? INCREASE : DECREASE; 
	fyEds.Bind(fy,recadre);
      } //ive=1..2
    }//ite(loe)
  }//ii=1..2    
  return !fyEds.IsEmpty();
} // FUN_connexX
 
//=======================================================================
//function : CorrectONUVISO
//purpose  :  
//=======================================================================

Standard_Boolean TopOpeBRepTool::CorrectONUVISO(const TopoDS_Face& Fin, TopoDS_Face& Fsp)
// <Fref> is x-periodic 
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceCORRISO();
  if (trc) cout<<"\n####    CorrectONUVISO    ####\n\n";
  debcorrUV();
#endif

  Standard_Real tolF = BRep_Tool::Tolerance(Fin);

  TopOpeBRepTool_CORRISO CORRISO(Fin);
  Standard_Real uperiod,vperiod;
  Standard_Boolean uclosed = CORRISO.Refclosed(1,uperiod);
  Standard_Boolean vclosed = CORRISO.Refclosed(2,vperiod);
  if (!uclosed && !vclosed) return Standard_False;
  
  CORRISO.Init(Fsp);
  Standard_Boolean ok = CORRISO.UVClosed();
  if (ok) return Standard_True; // Fsp is valid

  // 1. We check connexity among all edges of <Fsp>
  // if we find on edge with 2 faulty UVbounds, we try to UVconnect it.
//  for (Standard_Integer i=1; i<=2; i++) {
  Standard_Integer i ;
  for ( i=1; i<=2; i++) {
    Standard_Boolean onU = (i==1) ? Standard_True : Standard_False; 
    const TopTools_ListOfShape& Tocheck = CORRISO.Eds();
    TopTools_DataMapOfOrientedShapeInteger fyEds; ok =::FUN_connexX(onU,CORRISO,Tocheck,fyEds);
    if (!ok) continue;
    ok = CORRISO.TrslUV(onU,fyEds);
    if (!ok) continue;
    ok = CORRISO.UVClosed();
    if (!ok) continue;
    ok = CORRISO.GetnewS(Fsp);
    if (!ok) return Standard_False; //NYIRAISE
    return Standard_True;
  }

  // 2. x-2drep(edges) are in [xfirst,xfirst+xperiod]
  for (i = 1; i <=2; i++) {
    Standard_Boolean onU = (i==1);     
    Standard_Real xper=0.; Standard_Boolean xclosed = CORRISO.Refclosed(i,xper);
    if (!xclosed) continue;
    Standard_Real tolx = CORRISO.Tol(i,tolF);
    tolx *= 1.e2; // BUC60380
    TopTools_DataMapOfOrientedShapeInteger FyEds; Standard_Boolean hasfy = CORRISO.EdgesOUTofBoundsUV(CORRISO.Eds(),onU,tolx,FyEds);
    if (!hasfy) continue;
    ok = CORRISO.TrslUV(onU,FyEds);
    if (!ok) return Standard_False;
    ok = CORRISO.UVClosed();
    if (!ok) continue;
    ok = CORRISO.GetnewS(Fsp);
    if (!ok) return Standard_False; //NYIRAISE
    return Standard_True;
  }
  return Standard_False;

  /*// xpu310898 : eC closing ff, ff sdm F(reference face), proj(eC,F) gives ee with
  //             !closing(ee,Fsp) -> 2drep(Fsp) is not closed.
  //             purpose : translate pceCFOR or pceCREV
  //   cto902B7 (ff=f7,eC=e9,F=f14)
  ok = ::FUN_tool_reguUV(Fin,Fsp);*/

  /*// JYL 980909 : reecriture complete
  // 1/ traitement de TOUTES les aretes
  //    isou et isov fautives (et non la premiere trouvee);
  // 2/ traitement des aretes degenerees fautives : CTS20973

  TopTools_ListOfShape lisoe,ldege;
  TopExp_Explorer exe(Fsp, TopAbs_EDGE);
  for (; exe.More(); exe.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(exe.Current());
    Standard_Boolean degen = BRep_Tool::Degenerated(E);
    if ( degen ) {
      ldege.Append(E);
    }
    else {
      Standard_Real f,l; Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(E,Fin,f,l);
      Standard_Boolean uiso,viso; gp_Dir2d d2d; gp_Pnt2d O2d; 
      Standard_Boolean uviso = FUN_tool_IsUViso(PC,uiso,viso,d2d,O2d);
      Standard_Boolean onclosing = (uiso && uclosed) || (viso && vclosed);
      if      ( onclosing) {
	lisoe.Append(E);
      }
    }
  }
  
  Standard_Integer nisoe = lisoe.Extent();
  Standard_Integer ndege = ldege.Extent();
  if (nisoe ==0 && ndege == 0) return Standard_False;

  Standard_Integer tmpisoe;
  Standard_Integer tmpdege;

  TopTools_ListOfShape lfyisoe; Standard_Boolean tocorrectisoe = Standard_False;
  tocorrectisoe = FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,lisoe,Standard_True,lfyisoe,tmpisoe);

  TopTools_ListOfShape lfydege; Standard_Boolean tocorrectdege = Standard_False;
  tocorrectdege = FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,ldege,Standard_True,lfydege,tmpdege);

  tocorrect = (tocorrectisoe || tocorrectdege);
  if (!tocorrect) {
    return Standard_True;
  }
  
#ifdef OCCT_DEBUG
  if (trc) {
    cout<<"CorrectONUVISO ";
    cout<<"iso faulty "<<tocorrectisoe<<" deg faulty "<<tocorrectdege<<endl;;
  }
  debcorrUV();
#endif
  
  if (tocorrectisoe) {
    for (TopTools_ListIteratorOfListOfShape itiso(lfyisoe);itiso.More();itiso.Next()) {
      TopoDS_Edge& fyisoe = TopoDS::Edge(itiso.Value());
      
      // !! if the faulty edge ON closing appears twice in <Eds>, NOTHING is done!
      // -> see processing ::PurgeClosingEdges later called in WESMakeFaces    
      Standard_Integer nfoundisoe = 0;
      for (exe.Init(Fsp, TopAbs_EDGE); exe.More(); exe.Next()) {
	if (exe.Current().IsSame(fyisoe)) {
	  nfoundisoe++; 
	}
      }
      if (nfoundisoe > 1) {
	continue;
      }
      
#ifdef DRAW
      if (trc) { 
	cout<<"TopOpeBRepTool correctONUVISO : faulty iso edge"<<endl;
	FUN_tool_draw("fyf",Fsp);FUN_tool_draw("fyisoe",fyisoe);
      }
#endif
      
      Standard_Boolean ok = ::FUN_correctClosingE(fyisoe,Fsp);
      if (!ok) {
	continue;
      }
    }
    
    Standard_Integer tmpisoe;
    TopTools_ListOfShape lffyisoe; tocorrectisoe = FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,lfyisoe,Standard_False,lffyisoe,tmpisoe);
  } // end lffyisoe process
  
  if (tocorrectdege) {
    TopTools_IndexedDataMapOfShapeListOfShape mve;
    TopExp::MapShapesAndAncestors(Fsp,TopAbs_VERTEX,TopAbs_EDGE,mve);
    
    for (TopTools_ListIteratorOfListOfShape itdeg(lfydege);itdeg.More();itdeg.Next()) {
      TopoDS_Edge& fydege = TopoDS::Edge(itdeg.Value());
      
#ifdef DRAW
      if (trc) { 
	cout<<"TopOpeBRepTool correctONUVISO : faulty deg edge"<<endl;
	FUN_tool_draw("fyf",Fsp);FUN_tool_draw("fydege",fydege);
      }
#endif
      
      Standard_Boolean ok = ::FUN_correctDegeneratedE(mve,fydege,Fsp);
      if (!ok) {
	continue;
      }
    } // itdeg
    
    TopTools_ListOfShape lffydege; tocorrectdege = FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,lfydege,Standard_False,lffydege,tmpdege);
  } // end lfydege process

  TopTools_ListOfShape eFsp; FUN_tool_shapes(Fsp,TopAbs_EDGE,eFsp);
  TopTools_ListOfShape lffydege; tocorrect = FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,eFsp,Standard_False,lffydege,tmpdege);
  Standard_Boolean done = !tocorrect;
  return done;*/

} // correctONUVISO

//=======================================================================
//function : MakeFaces
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool::MakeFaces(const TopoDS_Face& Fin, const TopTools_ListOfShape& LOF, 
			      const TopTools_IndexedMapOfOrientedShape& MshNOK,
			      TopTools_ListOfShape& LOFF)
{
//  TopOpeBRepDS_BuildTool BT;
  BRep_Builder BB;
  LOFF.Clear();
  TopTools_ListIteratorOfListOfShape it(LOF);
  for (; it.More(); it.Next()){
    const TopoDS_Face& FF = TopoDS::Face(it.Value());
    Standard_Boolean valid = !MshNOK.Contains(FF);
    if (valid) {LOFF.Append(FF); continue;}
    
    TopoDS_Shape aLocalShape = Fin.EmptyCopied();
    TopoDS_Face newFace = TopoDS::Face(aLocalShape);// BT.CopyFace(Fin,newFace);
//    TopoDS_Face newFace = TopoDS::Face(Fin.EmptyCopied());// BT.CopyFace(Fin,newFace);
    TopExp_Explorer exw(FF, TopAbs_WIRE);
    
    for (; exw.More(); exw.Next()){
      const TopoDS_Wire& W = TopoDS::Wire(exw.Current());
      valid = !MshNOK.Contains(W);
      //      if (valid) {BT.AddFaceWire(newFace,W); continue;}
      if (valid) {BB.Add(newFace,W); continue;}

      TopoDS_Wire newWire; //BT.MakeWire(newWire);
      BB.MakeWire(newWire);
      TopExp_Explorer exe(W, TopAbs_EDGE);
      Standard_Integer ne = 0;
      for (; exe.More(); exe.Next()){
	const TopoDS_Edge& E = TopoDS::Edge(exe.Current());
	valid = !MshNOK.Contains(E);
	if (!valid) continue;
	//	BT.AddWireEdge(newWire,E);
	BB.Add(newWire,E);
	ne++;
      } // exe
      if (ne == 0) continue;
      Standard_Boolean closed = FUN_tool_ClosedW(newWire);
      //      BT.Closed(newWire,closed); 
      //      BT.AddFaceWire(newFace,newWire);
      newWire.Closed(closed);
      BB.Add(newFace,newWire);
    } // exw
    LOFF.Append(newFace);
  }
  return Standard_True;
}

/*static Handle(Geom2d_TrimmedCurve) FUN_translate(const gp_Vec2d& tvector, const TopoDS_Face& fF, TopoDS_Edge& fyE)
{
  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FUNTOOLC2D_CurveOnSurface(fyE,fF,f,l,tol);
  Standard_Boolean isou,isov; gp_Pnt2d o2d; gp_Dir2d d2d;
  Standard_Boolean isouv = FUN_tool_IsUViso(PC,isou,isov,d2d,o2d); o2d.Translate(tvector);
  Handle(Geom2d_Line) L2d = new Geom2d_Line(o2d,d2d);
  Handle(Geom2d_TrimmedCurve) C2d = new Geom2d_TrimmedCurve(L2d,f,l);
  return C2d;
}
Standard_EXPORT void FUN_tool_ttranslate(const gp_Vec2d& tvector, const TopoDS_Face& fF, TopoDS_Edge& fyE)
{  
  Handle(Geom2d_TrimmedCurve) C2d = ::FUN_translate(tvector,fF,fyE);
  Standard_Real tole = BRep_Tool::Tolerance(fyE);
  BRep_Builder BB;
//  Handle(Geom2d_Curve) toclear; BB.UpdateEdge(fyE,toclear,fF,tole);
  BB.UpdateEdge(fyE,C2d,fF,tole);
}

static Standard_Boolean FUN_tool_translate(const gp_Vec2d& tvector, TopoDS_Face& fF, TopoDS_Edge& fyE)
     // prequesitory : <fF> is on periodic surface, translates edge 
     //  <fyE>'s uorviso to have its uorvpar bounded in [0.,2PI].
{
  Handle(Geom2d_TrimmedCurve) C2d = ::FUN_translate(tvector,fF,fyE);
  Standard_Real tole = BRep_Tool::Tolerance(fyE);
  
  //xpu040598 : CTS20280 f37 modified when its split is modified!
  TopoDS_Face newf; Standard_Boolean ok = FUN_tool_pcurveonF(fF,fyE,C2d,newf);
  if (ok) fF = newf;
  return ok;
  //xpu040598
}
*/
