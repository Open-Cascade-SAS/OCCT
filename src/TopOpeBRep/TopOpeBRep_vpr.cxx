// Created on: 1995-08-04
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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


#include <gp_Pnt.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_PointClassifier.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Transition.hxx>

#ifdef DRAW
#include <TopOpeBRepDS_DRAW.hxx>
#endif

#include <Standard_DomainError.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <BRep_Tool.hxx>
#include <gp_Vec.hxx>

#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>

#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_Dumper.hxx>

#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_PointGeomTool.hxx>
#include <TopOpeBRep.hxx>

#define M_ON(st)       (st == TopAbs_ON) 
#define M_UNKNOWN(st)  (st == TopAbs_UNKNOWN) 
#define M_REVERSED(st) (st == TopAbs_REVERSED) 

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRep_GettraceNVP(Standard_Integer a,Standard_Integer b,Standard_Integer c,Standard_Integer d,Standard_Integer e);

Standard_Boolean GLOBAL_bvpr = Standard_False;

void debvpr(){};
void debvprmess(Standard_Integer f1,Standard_Integer f2,Standard_Integer il,Standard_Integer vp,Standard_Integer si)
{cout<<"f1,f2,il,vp,si : "<<f1<<","<<f2<<","<<il<<","<<vp<<","<<si<<endl;cout.flush();debvpr();}
void debpoint(Standard_Integer i) {cout<<"+ debpoint"<<i<<endl;}
void debvertex(Standard_Integer i){cout<<"+ debvertex"<<i<<endl;}

Standard_EXPORT void debarc(const Standard_Integer i)   {cout<<"+ debarc "<<i<<endl;}
Standard_EXPORT void debooarc(const Standard_Integer i) {cout<<"+ debooarc "<<i<<endl;}
#endif

Standard_EXPORT Standard_Boolean FDS_LOIinfsup(const TopOpeBRepDS_DataStructure& BDS,const TopoDS_Edge& E,const Standard_Real pE,const Standard_Integer GIP,
				    const TopOpeBRepDS_ListOfInterference& LOI, Standard_Real& pbef, Standard_Real& paft, Standard_Boolean& isonboundper);
Standard_EXPORT Standard_Boolean FUNBREP_topokpart
(const Handle(TopOpeBRepDS_Interference)& Ifound,const TopOpeBRepDS_ListOfInterference& DSCIL,
 const TopOpeBRep_LineInter& L,const TopOpeBRep_VPointInter& VP,
 const TopOpeBRepDS_DataStructure& BDS,const TopoDS_Shape& E,const TopoDS_Shape& F,const Standard_Real toluv,
 Standard_Real& parline,TopOpeBRepDS_Transition& transLine);

//-----------------------------------------------------------------------
// Search, among a list of interferences accessed by the iterator <IT>,
// a geometry whose parameter on edge point is identical to <par>.
// return True if such an interference has been found, False else.
// if True, iterator <IT> points (by the Value() method) on the first 
// interference found.
//-----------------------------------------------------------------------

Standard_EXPORT Standard_Boolean FUN_GetGonParameter
(TopOpeBRepDS_ListIteratorOfListOfInterference& it, const Standard_Real& par, const Standard_Real& tolp,
 Standard_Integer& G, TopOpeBRepDS_Kind& GT)
{
  while (it.More()) {
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    Standard_Real ipar; Standard_Boolean haspar = FDS_Parameter(I,ipar);
    if (!haspar) {it.Next(); continue;}
    Standard_Boolean samepar = (Abs(par-ipar) < tolp);
    if (!samepar){it.Next(); continue;}
    TopOpeBRepDS_Kind ST; Standard_Integer S; FDS_data(I,GT,G,ST,S);
    return Standard_True;
  }
  return Standard_False;
}   

static Standard_Boolean FUN_INlos(const TopoDS_Shape& S, const TopTools_ListOfShape& loS)
{
  TopTools_ListIteratorOfListOfShape it(loS);
  for (; it.More(); it.Next())
    if (it.Value().IsSame(S)) return Standard_True;
  return Standard_False;
}
 
//=======================================================================
//function : ProcessVPIonR
//purpose  : 
//=======================================================================

void TopOpeBRep_FacesFiller::ProcessVPIonR
(TopOpeBRep_VPointInterIterator& VPI,
 const TopOpeBRepDS_Transition& Trans,
 const TopoDS_Shape& Face,
 const Standard_Integer ShapeIndex) //1,2
{
  const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
  ProcessVPonR(VP,Trans,Face,ShapeIndex);
} // ProcessVPIonR

//-----------------------------------------------------------------------
static void FUN_transForWL
(const TopOpeBRep_LineInter& L,
 const Standard_Integer iVP,
 const Standard_Integer ShapeIndex,
 TopOpeBRepDS_Transition& transLine)
//-----------------------------------------------------------------------
{
  // premier VP avec indetermine : on prend le complement 
  // du suivant determine
  TopOpeBRep_VPointInterIterator VPIbis;
  for (VPIbis.Init(L); 
       VPIbis.More(); VPIbis.Next()) {
    const TopOpeBRep_VPointInter& VPbis = VPIbis.CurrentVP();
    Standard_Boolean tokeep = VPbis.Keep();
    if ( !tokeep ) continue;
    Standard_Integer iVPbis = VPIbis.CurrentVPIndex();
    if ( iVPbis <= iVP ) continue;
    Standard_Integer absindexbis = VPbis.ShapeIndex(); // 0,1,2,3
    Standard_Integer shapeindexbis = (absindexbis == 3) ? ShapeIndex : absindexbis;
    if ( shapeindexbis == 0 ) continue;
    const TopoDS_Shape& edgebis = VPbis.Edge(shapeindexbis);
    TopAbs_Orientation edgeoribis = edgebis.Orientation();
    TopOpeBRepDS_Transition transLinebis;
    transLinebis = 
      TopOpeBRep_FFTransitionTool::ProcessLineTransition
	(VPbis,shapeindexbis,edgeoribis);
    Standard_Boolean trliunkbis = transLinebis.IsUnknown();
    if ( trliunkbis ) continue;
    transLine = transLinebis.Complement();
    break;
  }
}

//-----------------------------------------------------------------------
static void FUN_VPgeometryfound
(TopOpeBRep_FacesFiller& FF,
 const TopOpeBRep_LineInter& L,
 const TopOpeBRep_VPointInter& VP,
 const Standard_Integer ShapeIndex,
 const Handle(TopOpeBRepDS_HDataStructure)& HDS,
 const TopOpeBRepDS_ListOfInterference& DSCIL,
 TopOpeBRepDS_Kind& PVKind, Standard_Integer& PVIndex,
 Standard_Boolean& EPIfound, Handle(TopOpeBRepDS_Interference)& IEPI,
 Standard_Boolean& CPIfound, Handle(TopOpeBRepDS_Interference)& ICPI,
 Standard_Boolean& OOEPIfound, Handle(TopOpeBRepDS_Interference)& IOOEPI) // (only if on2edges)
//-----------------------------------------------------------------------
{
  Standard_Boolean Lrest = (L.TypeLineCurve() == TopOpeBRep_RESTRICTION);
  TopoDS_Shape Erest; Standard_Real parErest=0; Standard_Integer rkErest=0;
  if (Lrest) {
    Erest = L.Arc(); parErest = VP.ParameterOnLine();
    Standard_Boolean isedge1 = L.ArcIsEdge(1); Standard_Boolean isedge2 = L.ArcIsEdge(2);
    rkErest = (isedge1) ? 1 : (isedge2) ? 2 : 0;
  }
  
  Standard_Integer absindex = VP.ShapeIndex();
  Standard_Integer OOabsindex = (absindex == 1) ? 2 : 1; 
  Standard_Integer OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  Standard_Boolean on2edges = (absindex == 3) || (Lrest && (rkErest == OOabsindex));
  TopoDS_Shape edge = (rkErest == ShapeIndex)? Erest : VP.Edge(ShapeIndex);

  PVIndex = 0;  // POINT or VERTEX index
  EPIfound = CPIfound = OOEPIfound = Standard_False;
  Standard_Real par = (rkErest == ShapeIndex)? parErest : VP.EdgeParameter(ShapeIndex); 
  Standard_Real tole = FUN_tool_maxtol(edge);
  Standard_Real tolp =  Precision::Parametric(tole);
  
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  if (BDS.HasShape(edge)) {
    const TopOpeBRepDS_ListOfInterference& EPIL = BDS.ShapeInterferences(edge);
    TopOpeBRepDS_ListIteratorOfListOfInterference itEPIL(EPIL);
    EPIfound = FF.GetGeometry(itEPIL,VP,PVIndex,PVKind);
    if (!EPIfound) {
      itEPIL.Initialize(EPIL);
      EPIfound = FUN_GetGonParameter(itEPIL,par,tolp,PVIndex,PVKind);
    }
    if (EPIfound) IEPI = itEPIL.Value();
  }

  TopOpeBRepDS_ListIteratorOfListOfInterference itCPIL(DSCIL);
  CPIfound = FF.GetGeometry(itCPIL,VP,PVIndex,PVKind);
  if (CPIfound) ICPI = itCPIL.Value();
    
  // - <VP> is of shapeindex 3 : is on <edge> and <OOedge>,
  // - <VP> is of shapeindex <ShapeIndex> and <VP> is given ON another edge <OOedge>
  // If <OOedge> is defined, we look among the list of interferences attached 
  // to the other edge <OOedge> for an interference of geometry falling into <VP>'s.

  Standard_Boolean hasOOedge = Standard_True;
  if (on2edges) hasOOedge = Standard_True;
  else          hasOOedge = (VP.State(OOShapeIndex) == TopAbs_ON);
  if ( hasOOedge ) {  
    TopoDS_Shape OOedge;

    if (on2edges) OOedge = (rkErest == OOShapeIndex)? Erest : VP.Edge(OOShapeIndex);
    else          OOedge = VP.EdgeON(OOShapeIndex);    

    Standard_Real OOpar = 0.;

    if (on2edges) OOpar = (rkErest == OOShapeIndex)? parErest : VP.EdgeParameter(OOShapeIndex);
    else          OOpar = VP.EdgeONParameter(OOShapeIndex); 

    Standard_Real tolOOe = FUN_tool_maxtol(OOedge);
    Standard_Real OOtolp = Precision::Parametric(tolOOe);
    if (BDS.HasShape(OOedge)) {
      const TopOpeBRepDS_ListOfInterference& OOEPIL = BDS.ShapeInterferences(OOedge);
      TopOpeBRepDS_ListIteratorOfListOfInterference OOitEPIL(OOEPIL);
      OOEPIfound = FF.GetGeometry(OOitEPIL,VP,PVIndex,PVKind);
      if (!OOEPIfound) {
	OOitEPIL.Initialize(OOEPIL);
	FUN_GetGonParameter(OOitEPIL,OOpar,OOtolp,PVIndex,PVKind);
      }
      if (OOEPIfound) IOOEPI = OOitEPIL.Value();
    }
  }        
}

#define M_FINDVP  (0) // only look for new vp
#define M_MKNEWVP (1) // only make newvp
#define M_GETVP   (2) // steps (0) [+(1) if (O) fails]

//-----------------------------------------------------------------------
Standard_EXPORT void FUN_VPIndex
(TopOpeBRep_FacesFiller& FF,
 const TopOpeBRep_LineInter& L,
 const TopOpeBRep_VPointInter& VP,
 const Standard_Integer ShapeIndex,
 const Handle(TopOpeBRepDS_HDataStructure)& HDS,
 const TopOpeBRepDS_ListOfInterference& DSCIL,
 TopOpeBRepDS_Kind& PVKind, Standard_Integer& PVIndex, // out
 Standard_Boolean& EPIfound, Handle(TopOpeBRepDS_Interference)& IEPI, // out 
 Standard_Boolean& CPIfound, Handle(TopOpeBRepDS_Interference)& ICPI, // out
 const Standard_Integer mkVP)
//-----------------------------------------------------------------------
{
  PVIndex = 0;  // POINT or VERTEX index
  Standard_Integer OOShapeIndex = (ShapeIndex == 1) ? 2 : 1; 
  Standard_Boolean SIisvertex = VP.IsVertex(ShapeIndex);
  Standard_Boolean OOisvertex = VP.IsVertex(OOShapeIndex);
  
  // search for an interference with a equal 3D geometry
  // if found, set PVIndex to index of geometry found
  // if not found, make a new geometry PVIndex with 3d point or vertex  
    
  Standard_Boolean OOEPIfound = Standard_False; 
  Handle(TopOpeBRepDS_Interference) IOOEPI;
  if ((mkVP == M_FINDVP)||(mkVP == M_GETVP)) {
    FUN_VPgeometryfound (FF,L,VP,ShapeIndex,HDS,DSCIL, //in
			 PVKind,PVIndex, // out
			 EPIfound,IEPI, // out
			 CPIfound,ICPI, // out
			 OOEPIfound,IOOEPI); // out (only if on2edges)
    if (mkVP == M_FINDVP) {
      //JMB 27 Dec 1999
      //modified by NIZHNY-MZV  Tue Apr 25 09:27:15 2000
      if (!EPIfound && !CPIfound && !OOEPIfound)
	PVIndex = 0; // if we just want to find (M_FINDVP) then we must readjust PVIndex to 0
                                   // because OOEPIfound is not treated in the upper function. The upper function
                                   // will detect that we found a geometry because PVIndex != 0 but as EPIfound and
                                   // CPIfound are FALSE, the resulting VP geometry give unpredictable results.
      return;
    }
  }
  // Gfound = VP corresponds with an existing geometry of ShapeIndex
  Standard_Boolean Gfound = ( EPIfound || CPIfound);
  // Gfound =             or with an existing geometry of OOShapeIndex
  Gfound = Gfound || OOEPIfound;
  
  Standard_Boolean on2edges = (VP.ShapeIndex() == 3);
  Standard_Boolean hasOOedge = Standard_True;
  if (on2edges) hasOOedge = Standard_True;
  else          hasOOedge = (VP.State(OOShapeIndex) == TopAbs_ON);
  // If v shares same domain with a vertex of the other shape,
  // v has already been stored in the DS
  
  if (PVIndex == 0) PVKind = (SIisvertex || OOisvertex) ? TopOpeBRepDS_VERTEX : TopOpeBRepDS_POINT;

  if ( hasOOedge && !Gfound ) {
    if ( !OOEPIfound )  { 
      if      ( SIisvertex ) PVIndex = FF.MakeGeometry(VP,ShapeIndex,PVKind);
      else if ( OOisvertex ) PVIndex = FF.MakeGeometry(VP,OOShapeIndex,PVKind);
      else                   PVIndex = FF.MakeGeometry(VP,ShapeIndex,PVKind);
    }
  }
  if ( !hasOOedge && !Gfound ) {
    Standard_Boolean found = FF.GetFFGeometry(VP,PVKind,PVIndex);
    if ( !found) {
      if      ( SIisvertex ) PVIndex = FF.MakeGeometry(VP,ShapeIndex,PVKind);
      else if ( OOisvertex ) PVIndex = FF.MakeGeometry(VP,OOShapeIndex,PVKind);
      else                   PVIndex = FF.MakeGeometry(VP,ShapeIndex,PVKind);
    }
  }
} // FUN_VPIndex

//-----------------------------------------------------------------------
static Standard_Boolean FUN_LineRestF
(const TopoDS_Face& F, const TopOpeBRep_LineInter& L, 
 const TopTools_ListOfShape& ERL, TopoDS_Edge& ER)
//-----------------------------------------------------------------------
{
  // returns true if <L> is ON a restriction <ER> of <F>
  // <ERL> is the list of the faces intersector.
  // prequesitory : <L> is on edge
  TopTools_IndexedMapOfShape mapE;
  TopExp::MapShapes(F,TopAbs_EDGE,mapE);
  TopTools_ListIteratorOfListOfShape itER(ERL);
  TopTools_ListOfShape ERLonF;
  for (; itER.More(); itER.Next()){
    const TopoDS_Shape& e = itER.Value();
    if (mapE.Contains(e)) ERLonF.Append(e);
  }
  itER.Initialize(ERLonF);
  TopTools_ListOfShape ERLonFonL;
  for (; itER.More(); itER.Next()){
    const TopoDS_Shape& e = itER.Value();
    TopTools_ListOfShape eL; eL.Append(e);
    Standard_Boolean isonL = TopOpeBRep_FacesFiller::LSameDomainERL(L,eL);
    if (isonL) ERLonFonL.Append(e);
  }
  // <L> is on at most one edge restriction.
  if (ERLonFonL.Extent() != 1) return Standard_False;
  ER = TopoDS::Edge(ERLonFonL.First());
  return Standard_True;
}

//-----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_newtransEdge
(const Handle(TopOpeBRepDS_HDataStructure) HDS,
 const TopOpeBRep_FacesFiller& FF,
 const TopOpeBRep_LineInter& L,
 const Standard_Boolean& Lonrest,
 const TopOpeBRep_VPointInter& VP, 
 const TopOpeBRepDS_Kind PVKind,const Standard_Integer PVIndex,
 const Standard_Integer& OOShapeIndex,
 const TopoDS_Edge& edge, const TopTools_ListOfShape& ERL, TopOpeBRepDS_Transition& T)
//-----------------------------------------------------------------------
{
  T.Before(TopAbs_UNKNOWN); T.After(TopAbs_UNKNOWN);
  const TopoDS_Face& OOface = FF.Face(OOShapeIndex);
  TopoDS_Face FIE = OOface;
  {
    TopAbs_Orientation oFIE = FIE.Orientation();
    if (oFIE == TopAbs_INTERNAL || oFIE == TopAbs_EXTERNAL) {
      T.Set(oFIE);
      return Standard_True;
    }
  }

  // compute of transition on edge <edge> while crossing <VP>.
  // <VP> given by <paredge> on <edge>, <uv> on <OOface>, 
  //               <paronline> on Line.

  // <C>, <pf>, <pl>, <paredge> :
  Standard_Real paredge; Standard_Boolean ok = VP.ParonE(edge,paredge); if (!ok) return Standard_False;

  Standard_Real par1,par2; 
  if (HDS->HasShape(edge)) {
    Standard_Boolean isonper;
    if (PVIndex == 0) FDS_getupperlower(HDS,HDS->DS().Shape(edge),paredge,par1,par2);
    else              FDS_LOIinfsup(HDS->DS(),edge,paredge,PVKind,PVIndex,
				      HDS->DS().ShapeInterferences(edge),
				      par1,par2,isonper); 
  }
  else
    FUN_tool_bounds(edge,par1,par2);
  
  gp_Pnt2d uv = VP.SurfaceParameters(OOShapeIndex);
  
#ifdef OCCT_DEBUG
  TopOpeBRepDS_Transition Tr; 
#endif
  //       <Tr> relative to 3d <OOface> matter,
  //       we take into account <Tr> / 2d <OOface> only if <edge> is normal to <OOface>  
  Standard_Real tola = Precision::Angular()*1.e+4; //dealing with tolerances
  Standard_Boolean EtgOOF = FUN_tool_EtgF(paredge,edge,uv,OOface,tola);   
  Standard_Boolean inERL = FUN_INlos(edge,ERL);
  Standard_Boolean isse = HDS->DS().IsSectionEdge(edge);
  Standard_Boolean rest = inERL || isse;
  Standard_Boolean interf2d = EtgOOF && Lonrest && rest;
  Standard_Boolean interf3dtg = EtgOOF && rest && !interf2d; // xpu260898 :cto902D6,(e15,p3,f9)

  Standard_Real factor = 1.e-2; 
  TopOpeBRepTool_makeTransition MKT; 
  ok = MKT.Initialize(edge,par1,par2,paredge, OOface,uv, factor);
  if (!ok) return Standard_False;
  Standard_Boolean isT2d = MKT.IsT2d();
  interf2d = interf2d && isT2d;
  
  TopAbs_State stb,sta; 
  if (interf2d) {
    // <tgLine> :
    TopoDS_Edge OOER; Standard_Boolean onOOface = Standard_False;
    TopOpeBRep_TypeLineCurve typL = L.TypeLineCurve();
    if (typL == TopOpeBRep_RESTRICTION) {onOOface = Standard_True; OOER = TopoDS::Edge(L.Arc());}
    else                                {onOOface = ::FUN_LineRestF(OOface,L,ERL,OOER);}
    if (!onOOface) return Standard_False;
    
    Standard_Real OOpar; ok = VP.ParonE(OOER,OOpar);
    if (!ok) ok = FUN_tool_parE(edge,paredge,OOER,OOpar);
    if (!ok) return Standard_False;  

    //xpu051098 : cto900L4 (edge18,OOface5)
    ok = MKT.SetRest(OOER,OOpar);
    if (!ok) return Standard_False; 
  }
  else if (interf3dtg) {
    Standard_Integer absindex = VP.ShapeIndex(); // 0,1,2,3
    Standard_Boolean on2edges = (absindex == 3);
    Standard_Boolean hasONedge = (VP.State(OOShapeIndex) == TopAbs_ON);
    Standard_Boolean hasOOedge = (on2edges) ? Standard_True : hasONedge;
    
    if ( hasOOedge ) {
      TopoDS_Edge OOedge; Standard_Real OOpar = 1.e7;
      if (on2edges) 
	{OOedge = TopoDS::Edge(VP.Edge(OOShapeIndex)); OOpar = VP.EdgeParameter(OOShapeIndex);}
      else          
	{OOedge = TopoDS::Edge(VP.EdgeON(OOShapeIndex)); OOpar = VP.EdgeONParameter(OOShapeIndex);}

      ok = MKT.SetRest(OOedge,OOpar);
      if (!ok) return Standard_False;       
    }
  }
 
  ok = MKT.MkTonE(stb,sta);
  if (!ok) return Standard_False;
  T.Before(stb); T.After(sta);
  return Standard_True;
} // FUN_newtransEdge

//-----------------------------------------------------------------------
static void FUN_ScanInterfList(const TopOpeBRepDS_Point& PDS, const Handle(TopOpeBRepDS_HDataStructure) HDS,
			       const TopOpeBRepDS_ListOfInterference& loI, TopOpeBRepDS_ListOfInterference& loIfound)
//-----------------------------------------------------------------------
{
  // looks among the list of interferences <loI> for interferences
  // of geometry falling into <PDS>, add them to <loIfound>
  TopOpeBRepDS_ListIteratorOfListOfInterference it(loI);
  while ( it.More()) {
    Standard_Boolean found = HDS->ScanInterfList(it,PDS);
    if (found) {
      loIfound.Append(it.Value());
      if (it.More()) it.Next();
    }
    else return;
  }
}

static Standard_Boolean FUN_selectTRAISHAinterference(const TopOpeBRepDS_ListOfInterference& lI, const Standard_Integer ITRASHA,
					 TopOpeBRepDS_ListOfInterference& lITRAonISHA)
// purpose : <lITRAonISHA> = {I = (T on ITRASHA,G,S)}
{
  lITRAonISHA.Clear();
  TopOpeBRepDS_ListIteratorOfListOfInterference it(lI);
  for (; it.More(); it.Next()) {
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    const TopOpeBRepDS_Transition& T = I->Transition(); 
    Standard_Integer iTRASHA = T.Index();
// BUG :
//POP : pb  : comparaison entre 2 enum differentes : on prend la valeur correspondante 
    if (T.Orientation(TopAbs_IN) == TopAbs_EXTERNAL) continue; //xpu030998
//    if (T.Orientation(TopAbs_IN) == TopAbs_UNKNOWN) continue; //xpu030998
    if (iTRASHA == ITRASHA) lITRAonISHA.Append(I);
  }
  Standard_Boolean noIfound = lITRAonISHA.IsEmpty();
  return !noIfound;
}

static Standard_Boolean FUN_selectGinterference(const TopOpeBRepDS_ListOfInterference& lI, const Standard_Integer G,
				   TopOpeBRepDS_ListOfInterference& lIonG)
{
  lIonG.Clear();
  TopOpeBRepDS_ListIteratorOfListOfInterference it(lI);
  for (; it.More(); it.Next()) {
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    if (I->Geometry() == G) lIonG.Append(I);
  }
  Standard_Boolean noIfound = lIonG.IsEmpty();
  return !noIfound;  
}

static Standard_Boolean FUN_sameGsameS(const TopOpeBRepDS_ListOfInterference& loI, const Standard_Integer& G, const Standard_Integer& S,
			  TopOpeBRepDS_ListOfInterference& loIfound)
{
  loIfound.Clear();
  // Gets among the list <loI> the interferences of :
  //  geometry <G>, and support <S>
  TopOpeBRepDS_PointIterator PI(loI);   
  for (; PI.More(); PI.Next()) {
    Handle(TopOpeBRepDS_Interference) EPI = PI.Value();    
    Standard_Integer GEPI = EPI->Geometry(); Standard_Integer SEPI = EPI->Support();      
    if (GEPI == G && SEPI == S) loIfound.Append(EPI);
  }
  return (loIfound.Extent() > 0);
}

//-----------------------------------------------------------------------
static void FUN_processCPI
(TopOpeBRep_FacesFiller& FF,
 const TopOpeBRep_VPointInter& VP,
 const TopoDS_Shape& F,
 const Standard_Integer ShapeIndex, 
 const TopOpeBRep_LineInter& L,
 TopOpeBRepDS_PDataStructure pDS,
 const TopOpeBRepDS_Transition& transLine,
 const TopOpeBRepDS_ListOfInterference& DSCIL,
 const Handle(TopOpeBRepDS_Interference)& Ifound,
 const Standard_Boolean& Gfound,
 const TopOpeBRepDS_Kind& PVKind,const Standard_Integer& PVIndex,
 Standard_Integer& keptVPnbr)
//-----------------------------------------------------------------------
{    
  Standard_Integer OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;

  TopOpeBRepDS_Transition ttransLine = transLine;
  // prequesitory : current line is not on edge.  
  Standard_Real parline = VP.ParameterOnLine();
  Standard_Boolean SIisvertex = VP.IsVertex(ShapeIndex);
  Standard_Boolean OOisvertex = VP.IsVertex(OOShapeIndex);
  const TopoDS_Shape& E = VP.Edge(ShapeIndex);
  
  // xpu010299 : we do not keep interferences with same parameters on curve 
  //             PRO16120(f3,f4 -> null c1)
  if (!DSCIL.IsEmpty()) {
    Standard_Real par = FDS_Parameter(DSCIL.Last()); // parameter on curve
    Standard_Real dd = Abs(par-parline); // en fait, ce sont des entiers
    if (dd == 0) return;
  }

  // dist(p2d1,p2d2) < toluv => p2d1, p2d2 are considered equal.
  // NYI : compute uvtol with the original faces. By default, we set toluv = TolClass
  Standard_Real toluv = 1.e-8;
  Standard_Boolean keep = FUNBREP_topokpart(Ifound,DSCIL,L,VP,(*pDS),E,F,toluv,parline,ttransLine);
  
  if (keep) {
    keptVPnbr++;
    if (keptVPnbr > 2) keep = Standard_False;
  }
  if (!keep) return;
  
  Handle(TopOpeBRepDS_Interference) CPI;
  {
    TopOpeBRepDS_Kind GKCPV;
    if (Gfound) GKCPV = PVKind;
    else GKCPV = (SIisvertex || OOisvertex) ? TopOpeBRepDS_VERTEX : TopOpeBRepDS_POINT;

    CPI = ::MakeCPVInterference(ttransLine,0,PVIndex,parline,GKCPV);
    FF.StoreCurveInterference(CPI);
  }
}

static Standard_Boolean FUN_onedge(const TopOpeBRepDS_Point& PDS, const TopoDS_Edge& E)
{
  gp_Pnt P = PDS.Point();
  Standard_Real tolP = PDS.Tolerance(); Standard_Real tolE = BRep_Tool::Tolerance(E);
  Standard_Real tol = Max(tolP,tolE);
  TopoDS_Vertex vf,vl; TopExp::Vertices(E,vf,vl);
  gp_Pnt pf = BRep_Tool::Pnt(vf); Standard_Boolean isonf = P.IsEqual(pf,tol);
  gp_Pnt pl = BRep_Tool::Pnt(vl); Standard_Boolean isonl = P.IsEqual(pl,tol);
  return isonf || isonl;
}

#ifdef OCCT_DEBUG
Standard_EXPORT void funraise() {cout<<"!!!!!!!!!! PVIndex = 0 !!!!!!!!!!"<<endl;}
#endif

//=======================================================================
//function : ProcessVPonR
//purpose  : 
//=======================================================================

void TopOpeBRep_FacesFiller::ProcessVPonR
(const TopOpeBRep_VPointInter& VP,
 const TopOpeBRepDS_Transition& Trans,
// const TopoDS_Shape& GFace,
 const TopoDS_Shape& ,
 const Standard_Integer ShapeIndex) //1,2
{
  Standard_Integer absindex = VP.ShapeIndex(); // 0,1,2,3
  Standard_Integer iVP = VP.Index();
  Standard_Integer OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  Standard_Boolean on2edges = (absindex == 3);
  Standard_Boolean hasONedge = (VP.State(OOShapeIndex) == TopAbs_ON);
  Standard_Boolean hasOOedge = (on2edges) ? Standard_True : hasONedge;

  TopoDS_Face Face = (*this).Face(ShapeIndex);
  Standard_Integer iSIFace = myDS->Shape(Face);
  if (iSIFace == 0) iSIFace = myDS->AddShape(Face,ShapeIndex);
  TopoDS_Face OOFace = (*this).Face(OOShapeIndex);
  Standard_Integer iOOFace = myDS->Shape(OOFace);
  if (iOOFace == 0)  iOOFace = myDS->AddShape(OOFace,OOShapeIndex);

  // current VPoint is on <edge>
  Standard_Integer SIedgeIndex = 0;
  const TopoDS_Edge& edge = TopoDS::Edge(VP.Edge(ShapeIndex));
  if (myDS->HasShape(edge)) SIedgeIndex = myDS->Shape(edge);
  Standard_Real paredge = VP.EdgeParameter(ShapeIndex);
  Standard_Boolean isrest = myDS->IsSectionEdge(edge);
  Standard_Boolean closing = TopOpeBRepTool_ShapeTool::Closed(edge,Face);
  Standard_Boolean dge = BRep_Tool::Degenerated(edge);
  
  // dummy if !<hasOOedge>
  Standard_Integer OOedgeIndex = 0; 
  Standard_Boolean OOclosing,OOisrest; OOclosing = OOisrest = Standard_False;
  TopoDS_Edge OOedge; Standard_Real OOparedge = 0.; Standard_Boolean dgOOe = Standard_False;
  if ( hasOOedge ) {
    if (on2edges) OOparedge = VP.EdgeParameter(OOShapeIndex);
    else          OOparedge = VP.EdgeONParameter(OOShapeIndex);
    TopoDS_Shape OOe;
    if (on2edges) OOe = VP.Edge(OOShapeIndex);
    else          OOe = VP.EdgeON(OOShapeIndex);
    OOedge = TopoDS::Edge(OOe);
    if (myDS->HasShape(OOedge)) OOedgeIndex = myDS->Shape(OOedge);
    OOisrest = myDS->IsSectionEdge(OOedge);
    OOclosing = TopOpeBRepTool_ShapeTool::Closed(OOedge,OOFace);
    dgOOe = BRep_Tool::Degenerated(OOedge);
  }

#ifdef OCCT_DEBUG
  Standard_Integer ili=myLine->Index(),ivp=iVP,isi=ShapeIndex;
  GLOBAL_bvpr = TopOpeBRep_GettraceNVP(myexF1,myexF2,ili,ivp,isi);
  if (GLOBAL_bvpr) debvprmess(myexF1,myexF2,ili,ivp,isi);
#endif

  // degenerated edge processing 
  // ---------------------------
  Standard_Integer PVIndex = 0; // POINT or VERTEX index
  TopOpeBRepDS_Kind PVKind;
  Standard_Boolean EPIfound,CPIfound;
  EPIfound = CPIfound = Standard_False;
  Handle(TopOpeBRepDS_Interference) IEPI,ICPI;
  ProcessVPondgE(VP, ShapeIndex,
		 PVKind,PVIndex, // out
		 EPIfound,IEPI,  // out
		 CPIfound,ICPI); // out
  Standard_Boolean foundPVIndex = (PVIndex != 0);
  
  
  // ===================================================================
  //              <TransLine>, <transEdge>
  // ===================================================================
  
  Standard_Boolean wline = (myLine->TypeLineCurve() == TopOpeBRep_WALKING);
  Standard_Boolean grestriction = (myLine->TypeLineCurve() == TopOpeBRep_RESTRICTION);
  Standard_Boolean glinenotoned = !wline && !grestriction && !myLineIsonEdge;
  
  // lasttransLine (for walking line)
  // -------------
  // set lasttransLine for a WALKING line
  Standard_Boolean dscilempty = myDSCIL.IsEmpty();
  Standard_Boolean setlastonwl = wline && !dscilempty;
  if (setlastonwl) { //xpu171198, FRA61896 (f7,f13-> null DSC1)
    Standard_Real parline = VP.ParameterOnLine();
    Standard_Real par = FDS_Parameter(myDSCIL.Last()); // parameter on curve
    Standard_Real dd = Abs(par-parline); // en fait, ce sont des entiers
    if (dd == 0) setlastonwl=Standard_False;
  }
  TopOpeBRepDS_Transition lasttransLine; if (setlastonwl) lasttransLine = myDSCIL.Last()->Transition();
  
  // edgeori, transLine
  // ------------------
  TopAbs_Orientation edgeori = edge.Orientation();
  TopOpeBRepDS_Transition transLine; 
  transLine = TopOpeBRep_FFTransitionTool::ProcessLineTransition
    (VP,ShapeIndex,edgeori);
  Standard_Boolean trliunk = transLine.IsUnknown();
  
  // 1_ If vpmin has transition OUT/IN, and vpmax is UNKNOWN, 
  //    we change vpmax transition as IN/OUT
  //
  // 2_ If vpmin is UNKNOWN (if vp is first on line and transition is UNKNOWN,
  //    vpmin's transition is ON/ON the OOshape)
  //    we change vpmin transition as OUT/IN
  //
  // (kpart : sphere/box, with the sphere's sewing edge lying on one boxe's 
  //  face and one of the edge's vertices IN the same face)
  
  Standard_Integer iINON1,iINONn,nINON; myLine->VPBounds(iINON1,iINONn,nINON);
  Standard_Boolean islastvp = (iVP == iINONn);
  Standard_Boolean isfirstvp = (iVP == iINON1);
  
  Standard_Boolean keepvp = Standard_False;
  Standard_Boolean ret1 = Standard_False; 
  if ( trliunk ) {
    // <transLine> is unknown :
    //  for a walking -> 
    //     - if <myDSCIL> is not empty,
    //                   we set it as the last transition complemented
    //     - else, 
    //          we look after an determinate transition on VP(i>iVP)
    //          and set transLine as this last complemented.
    //
    //  for a gline not on edge -> 
    //     - if the transition on edge is unknown too and !<keepvp>,
    //       we do not keep it.
    //  elsewhere -> we do not keep <VP>
    
    if      ( wline ) {
      if (setlastonwl) transLine = lasttransLine.Complement();
      else             ::FUN_transForWL(*myLine,iVP,ShapeIndex,transLine);

      //  walki vpfirst on 3, vplast on 0, nvpkept = 2 kept
      if (transLine.IsUnknown()) {
	//modified by NIZHNY-MKK  Mon Jul  3 11:30:03 2000.BEGIN
	Standard_Boolean keepvpfirst = dscilempty && isfirstvp && (nINON == 2);
	if(absindex==3)
	  keepvpfirst = keepvpfirst && myLastVPison0;
	//modified by NIZHNY-MKK  Mon Jul  3 11:30:21 2000.END
	if (keepvpfirst) transLine.Set(TopAbs_FORWARD);
	ret1 = Standard_False;
      }
    }
    else if ( glinenotoned ) {
      //       if (islastvp)  keepvp = !dscilempty;
      //       if (isfirstvp) keepvp = Standard_True;
      if (isfirstvp) keepvp = Standard_True;
      else {
	if (islastvp)  keepvp = !dscilempty;
	else {
	  if(!dge && !dgOOe && (VP.IsVertexOnS1() || VP.IsVertexOnS2())) {
	    // If VP is on vertex we should compute at least one interference for the edge.
	    // This interference is necessary at least to indicate that the edge intersect something.
	    const TopOpeBRep_VPointInter& aFirstPoint = myLine->VPoint(iINON1);
	    const TopOpeBRep_VPointInter& aLastPoint = myLine->VPoint(iINONn);

	    for(Standard_Integer faceindex = 1; !keepvp && faceindex <=2; faceindex++) {
	      Standard_Boolean VPIsVertex = (faceindex==1) ? VP.IsVertexOnS1() : VP.IsVertexOnS2();
	      Standard_Boolean FirstPointIsVertex = (faceindex==1) ? aFirstPoint.IsVertexOnS1() : aFirstPoint.IsVertexOnS2();
	      Standard_Boolean LastPointIsVertex =  (faceindex==1) ? aLastPoint.IsVertexOnS1() : aLastPoint.IsVertexOnS2();
	      if(VPIsVertex) {
		const TopoDS_Shape& aV1 =  (faceindex==1) ? VP.VertexOnS1() : VP.VertexOnS2();
		if(FirstPointIsVertex) {		  
		  const TopoDS_Shape& aV2 =  (faceindex==1) ? aFirstPoint.VertexOnS1(): aFirstPoint.VertexOnS2();
		  if(aV1.IsSame(aV2)) {
		    keepvp = Standard_True;
		  }
		}
		if(!keepvp && LastPointIsVertex) {
		  const TopoDS_Shape& aV2 =  (faceindex==1) ? aLastPoint.VertexOnS1() : aLastPoint.VertexOnS2();
		  if(aV1.IsSame(aV2)) {
		    keepvp = !dscilempty;
		  }
		}
	      }
	    }
	  }
	}
      }      
      ret1 = !keepvp;
    }
    else 
      ret1 = Standard_True;
  }
  trliunk = transLine.IsUnknown();
  if (ret1) return;

  // Transori, transEdge
  // -------------------
  TopAbs_Orientation Transori = Trans.Orientation(TopAbs_IN);
  TopOpeBRepDS_Transition transEdge = TopOpeBRep_FFTransitionTool::ProcessEdgeTransition(VP,ShapeIndex,Transori);  
  Standard_Boolean Tunknown = FDS_hasUNK(transEdge);
  TopOpeBRepDS_Point PDS = TopOpeBRep_PointGeomTool::MakePoint(VP);// <VP>'s geometry  
  TopOpeBRepDS_ListOfInterference lITOOFonVP; // {I on <edge> = (T on <OOface>, G on <VP>, S)}
  Standard_Boolean found = Standard_False;
  if (SIedgeIndex != 0) {
    TopOpeBRepDS_ListOfInterference lI; 
    const TopOpeBRepDS_ListOfInterference& lIedge = myDS->ShapeInterferences(edge);
    if (PVIndex == 0) ::FUN_ScanInterfList(PDS,myHDS,lIedge,lI);
    else              ::FUN_selectGinterference(lIedge,PVIndex,lI);
    found = ::FUN_selectTRAISHAinterference(lI,iOOFace,lITOOFonVP);    
  }

//  if (found && myLineINL && Tunknown) return; //xpu220998 : cto cylcong A1 (edge8,OOface4)

  // <Transori> = INTERNAL or EXTERNAL (tangent cases), compute <transEdge>
  Standard_Boolean newtransEdge = (Transori == TopAbs_INTERNAL) || (Transori == TopAbs_EXTERNAL);
  TopAbs_Orientation otransEdge = transEdge.Orientation(TopAbs_IN);
  Standard_Boolean allINT = (Transori == TopAbs_INTERNAL) || (otransEdge == TopAbs_INTERNAL);
  Standard_Boolean allEXT = (Transori == TopAbs_EXTERNAL) || (otransEdge == TopAbs_EXTERNAL);


  newtransEdge = newtransEdge && (!allINT) && (!allEXT);
  newtransEdge = newtransEdge || Tunknown;
  // -> intersection fails for closing edges
  // 1. <edge> touches closing <OOedge> at <VP> && <OOedge> is tangent to <OOFace>,
  // intersection -> 1 forward && 1 reversed instead of internal/external.
  // 2. if <edge> is tangent to <OOFace> at <VP> on walking
  newtransEdge = newtransEdge || closing || OOclosing;
  newtransEdge = newtransEdge && (!myLineINL); 
  if (newtransEdge){
    myDS->Shape(edge);
    newtransEdge = !found;
    if (found) { 
      // Getting first transition found
      // prequesitory : transition on edge / <OOF> on same geometry point is unchanged
      TopOpeBRepDS_Transition Tr = lITOOFonVP.First()->Transition();
      transEdge.Before(Tr.Before()); transEdge.After(Tr.After());
    }
    if (newtransEdge) {
      // Compute of <transEdge> : transition on <edge> at geometry <VP> / <OOface>
      // if line on a restriction OOedge of <OOface> : gets <edge> transition/<OOface> when
      //                                               at <VP> on OOedge.
      // if line is not on restriction : gets <edge> transition/<OOface>.
      TopOpeBRepDS_Transition Tr;
      Standard_Boolean ok = FUN_newtransEdge(myHDS,(*this),(*myLine),myLineIsonEdge,VP,
				PVKind,PVIndex,OOShapeIndex,edge,myERL,Tr);
      if (ok) {transEdge.Before(Tr.Before()); transEdge.After(Tr.After());}
      newtransEdge = ok;
    }
  } // newtransEdge

  Standard_Boolean tredunk = transEdge.IsUnknown();
  Standard_Boolean ret2 = Standard_False;
  if ( tredunk ) {
    if (!trliunk) transEdge = transLine.Complement();
    if (trliunk && !keepvp) ret2 = Standard_True;
  }
  if (ret2) return;
  tredunk = transEdge.IsUnknown();

  // ===================================================================
  //              DS geometry Management
  // ===================================================================
  // SI*** : data issued from shape ShapeIndex
  // OO*** : data issued from other shape
    
  if (SIedgeIndex == 0) SIedgeIndex = myDS->AddShape(edge,ShapeIndex);

  Standard_Boolean SIisvertex = VP.IsVertex(ShapeIndex);
  Standard_Boolean OOisvertex = VP.IsVertex(OOShapeIndex);
    
  // <PVIndex>, <PVKind> :
  // --------------------
  // search for an interference with a equal 3D geometry
  // if found, set <PVIndex> to index of geometry found
  // if not found, make a new geometry PVIndex with 3d point or vertex      

  // modified by NIZHNY-MKK  Tue Apr  3 12:08:38 2001.BEGIN
  Standard_Boolean ismultiplekind = foundPVIndex && !EPIfound && !CPIfound && 
    (SIisvertex || OOisvertex) && (PVKind == TopOpeBRepDS_POINT);

  //   if (!foundPVIndex) FUN_VPIndex ((*this),(*myLine),VP,ShapeIndex,myHDS,myDSCIL, //in
  if (!foundPVIndex || ismultiplekind) FUN_VPIndex ((*this),(*myLine),VP,ShapeIndex,myHDS,myDSCIL, //in
  // modified by NIZHNY-MKK  Tue Apr  3 12:13:17 2001.END
						    PVKind,PVIndex, // out
						    EPIfound,IEPI,  // out
						    CPIfound,ICPI,  // out
						    M_MKNEWVP);
  if (PVIndex == 0){
#ifdef OCCT_DEBUG
    funraise();
#endif
  }
  
  Standard_Boolean VPonedge=Standard_False; if (PVKind == TopOpeBRepDS_VERTEX) VPonedge=::FUN_onedge(PDS,edge);
  if (myLineINL) {
    Standard_Real tolang = Precision::Angular()*1.e5;//=1.e-7 NYITOLXPU

    gp_Vec tgE = FUN_tool_tggeomE(paredge,edge);
    gp_Pnt2d OOuv; Standard_Boolean ok = Standard_False;
    if (VPonedge) {OOuv = VP.SurfaceParameters(OOShapeIndex); ok = Standard_True;}
    else          {ok = FUN_tool_paronEF(OOedge,OOparedge,OOFace, OOuv);} 
    gp_Vec ntOOF;
    if (ok) ntOOF = FUN_tool_nggeomF(OOuv,OOFace);
    if (OOFace.Orientation() == TopAbs_REVERSED) ntOOF.Reverse();       
    
    Standard_Real tol = 1.e-7;
    if (ok) ok = (tgE.Magnitude() > tol)&&(ntOOF.Magnitude() > tol);
    Standard_Real dot = 1.e7; if (ok) dot = gp_Dir(tgE).Dot(gp_Dir(ntOOF));
      

    Handle(Geom_Surface) su = BRep_Tool::Surface(OOFace);
    Standard_Boolean apex = FUN_tool_onapex(OOuv,su);      
    TopOpeBRepDS_Transition T;
    if (!apex && ok && (Abs(dot) > tolang)) {  
      TopAbs_Orientation ori = (dot < 0.) ? TopAbs_FORWARD : TopAbs_REVERSED;
      T.Set(ori);
    }

    if (VPonedge && (!dge)) {
      //xpu231098 : cto904C8(edge11)
      // xpu131198 : CTS21802(edge31)
      if (iOOFace == 0) iOOFace = myDS->AddShape(OOFace,OOShapeIndex);
      Handle(TopOpeBRepDS_Interference) EPIf;
      {
	T.Index(iOOFace);
	EPIf = MakeEPVInterference(T,iOOFace,PVIndex,paredge,PVKind,TopOpeBRepDS_FACE,SIisvertex);
      }
      myHDS->StoreInterference(EPIf,edge);
      if (on2edges || hasONedge) {
	if (OOedgeIndex == 0) OOedgeIndex = myDS->AddShape(OOedge,OOShapeIndex);
	Handle(TopOpeBRepDS_Interference) EPI;
	{
	  T.Index(iOOFace);
	  EPI = MakeEPVInterference(T,OOedgeIndex,PVIndex,paredge,PVKind,SIisvertex);
	}
	myHDS->StoreInterference(EPI,edge);
      }
      return;
    }//VPonedge
    else {
      // compute interferences later on
      //modified by NIZHNY-MZV  Thu Dec 23 13:27:10 1999
      if(!T.IsUnknown()) {
	transEdge.Before(T.Before()); 
	transEdge.After(T.After());
      }
    }
  }//myLineINL

  // Gfound = VP corresponds with an existing geometry of ShapeIndex
  Standard_Boolean Gfound = ( EPIfound || CPIfound );  
#ifdef OCCT_DEBUG
  if (GLOBAL_bvpr) debvprmess(myexF1,myexF2,ili,ivp,isi);
#endif

  // ===================================================================
  //          Current VPoint VP is kept
  // ===================================================================  

  // ------------------------------------------
  // -- Curve/(POINT,VERTEX) Interference (CPI)
  // ------------------------------------------
  
  Standard_Boolean noCPI = myLineIsonEdge;
  noCPI = noCPI || (!on2edges && hasOOedge && (OOisrest || isrest)); 

  Standard_Boolean condi = (!noCPI);
  condi = condi && (!myLineINL); // INL
  if (condi) {
    Standard_Integer keptVPnbr = mykeptVPnbr;
    FUN_processCPI((*this),VP,Face,ShapeIndex,(*myLine),myDS,
		   transLine,myDSCIL,ICPI,Gfound,PVKind,PVIndex,
		   keptVPnbr);
    mykeptVPnbr = keptVPnbr;
  }
  
  // ------------------------------------------
  // --- Edge/(POINT,VERTEX) Interference (EPI) 
  // ------------------------------------------

//  if (on2edges && !Gfound && !closing) {
  Standard_Boolean condi2 = (on2edges && !closing);
  condi2 = condi2 || (hasONedge && !closing); 
  if (condi2 && (!dge)) {
    if (OOedgeIndex == 0) OOedgeIndex = myDS->AddShape(OOedge,OOShapeIndex);
    
    Handle(TopOpeBRepDS_Interference) EPI;
    {
      TopOpeBRepDS_Transition T = transEdge;
      if (iOOFace == 0) iOOFace = myDS->AddShape(OOFace,OOShapeIndex);
      T.Index(iOOFace);
      EPI = MakeEPVInterference(T,OOedgeIndex,PVIndex,paredge,PVKind,SIisvertex);
    }
    myHDS->StoreInterference(EPI,edge);
  } //condi2

  // ===================================================================
  // manip corrective d'un pb. d'intersection
  // - le VPoint est donne sur une restriction de ShapeIndex (1 ou 2),
  // - le VPoint est ON une restriction de l'autre shape (OOShapeIndex)
  // --> le VPoint n'est PAS donne sur restriction de OOShapeIndex NYI
  //     par les intersections (a ameliorer). NYI             
  // L'etat ON sur OOShapeIndex indique que le point est sur une 
  // de ses restrictions :
  // - on ne met PAS le point dans les CPIs
  // - on met le point dans les EPIs de l'arete de OOShapeIndex
  // ===================================================================

  Standard_Boolean correctON = !on2edges && hasONedge && !dgOOe;
  Standard_Boolean correctedON = Standard_False;
  if ( correctON ) {  
    TopOpeBRepDS_ListOfInterference lITFonVP; Standard_Boolean OOfound = Standard_False;
    if (OOedgeIndex != 0) {
      const TopOpeBRepDS_ListOfInterference& lIOOedge = myDS->ShapeInterferences(OOedge); 
      TopOpeBRepDS_ListOfInterference lI; ::FUN_ScanInterfList(PDS,myHDS,lIOOedge,lI);
      OOfound = ::FUN_selectTRAISHAinterference(lI,iSIFace,lITFonVP);
      correctON = !OOfound;        
    }
  }   
  if ( correctON ) {  
    if (OOedgeIndex == 0) OOedgeIndex = myDS->AddShape(OOedge,OOShapeIndex);
    
    // VP a ete classifie ON sur l'edge <OOedge>.
    // calcul de la transition <tOOedge> sur l'arete <OOedge> 
    // (de l'autre face en jeu, OOShapeIndex) ou le VP est donne ON.
    // On tient compte de l'orientation de <edge> dans <Face>.
    // (bug IntPatch_Line : VP n'a pas de donnees en OOShapeIndex 
    // alors qu'il est dessus)
    
    TopOpeBRepDS_Transition tOOedge;
    // distinguish wether OOedge is the edge on which geometric line lies.
    // OOedge == edge(line) ==> tOOedge = f(orientation of <edge> in <Face> FORWARD)
    // OOedge != edge(line) ==> tOOedge = f(orientation of <Face>)    
    Standard_Real OOpar1,OOpar2; Standard_Boolean isonper; FDS_LOIinfsup((*myDS),OOedge,OOparedge,PVKind,PVIndex,
						    myDS->ShapeInterferences(OOedge),
						    OOpar1,OOpar2,isonper);
    //FDS_getupperlower(myHDS,OOedgeIndex,OOparedge,par1,par2);
    gp_Pnt2d OOuv = VP.SurfaceParameters(ShapeIndex);
    
    //       <Tr> relative to 3d <OOface> matter,
    //       we take into account <Tr> / 2d <OOface> only if <edge> is normal to <OOface> 
    Standard_Real tola = Precision::Angular()*1.e+2; //dealing with tolerances 

    // KK : supplying tolerances pbm (tola too small)
    Standard_Boolean EsdmEofF = myHDS->HasSameDomain(OOedge);
    if (EsdmEofF) {
      TopExp_Explorer ex;
      for (ex.Init(Face, TopAbs_EDGE); ex.More(); ex.Next())
	if (FUN_ds_sdm(*myDS,ex.Current(),OOedge)) {EsdmEofF = Standard_True; break;}
    }
    Standard_Boolean OOEtgF = Standard_True;
    if (!EsdmEofF) OOEtgF = FUN_tool_EtgF(OOparedge,OOedge,OOuv,Face,tola);  
    Standard_Boolean OOrest = FUN_INlos(edge,myERL);
    Standard_Boolean interf2d = OOEtgF && (OOisrest || OOrest);
    
    Standard_Real factor = 1.e-2;
    TopOpeBRepTool_makeTransition MKT; 
    Standard_Boolean ok = MKT.Initialize(OOedge,OOpar1,OOpar2,OOparedge,Face,OOuv, factor);

    if (ok && !(interf2d && !MKT.IsT2d())) {
      MKT.SetRest(edge,paredge);
      TopAbs_State stb,sta; ok = MKT.MkTonE(stb,sta); 
      if (ok) {
        tOOedge.Before(stb); tOOedge.After(sta);
        Handle(TopOpeBRepDS_Interference) OOEPIe;
        {
          if (iSIFace == 0) iSIFace = myDS->AddShape(Face,ShapeIndex); 
          TopOpeBRepDS_Transition OOT = tOOedge; OOT.Index(iSIFace);      
          OOEPIe = MakeEPVInterference(OOT,SIedgeIndex,PVIndex,OOparedge,PVKind,OOisvertex);
        }
        myHDS->StoreInterference(OOEPIe,OOedge);
      
        // xpu : 09-03-98
        // hsd3d => interf2d  : only IwithSkEDGE interf 
        // elsewhere            : add an IwithSkFACE interference.
        Standard_Boolean addEPIf = !myLineIsonEdge;
        TopTools_ListOfShape dummy; Standard_Boolean hsd3d = FDS_HasSameDomain3d(*myDS,OOedge,&dummy);
        if (hsd3d) addEPIf = Standard_False;
        if (addEPIf) {
          TopOpeBRepDS_Transition OOT = tOOedge; OOT.Index(iSIFace); 
          Handle(TopOpeBRepDS_Interference) OOEPIf = MakeEPVInterference(OOT,iSIFace,PVIndex,OOparedge,PVKind,
                                                                         TopOpeBRepDS_FACE,OOisvertex);
          myHDS->StoreInterference(OOEPIf,OOedge);
        }
        // xpu : 09-03-98    
        correctedON = Standard_True;
      } // ok
    }
  } // correctON

  if (correctON && !correctedON && noCPI && !myLineIsonEdge) {
    // MSV: correct ON failed, so store CPI
    Standard_Integer keptVPnbr = mykeptVPnbr;
    FUN_processCPI((*this),VP,Face,ShapeIndex,(*myLine),myDS,
		   transLine,myDSCIL,ICPI,Gfound,PVKind,PVIndex,
		   keptVPnbr);
    mykeptVPnbr = keptVPnbr;
  }

  // closing edge processing 
  // -----------------------
  if ((OOclosing || closing)&& !found) {
    ProcessVPonclosingR(VP,Face,ShapeIndex,
			transEdge,
			PVKind,PVIndex,
			EPIfound,IEPI);
    return;
  }

  // VP processing 
  // -------------
  
  Standard_Boolean addEPI = Standard_False;
  if (!Gfound) {
    addEPI = Standard_True; 
  } 
  else { // EPIfound
    TopAbs_Orientation anOtransEdge = transEdge.Orientation(TopAbs_IN);

    Standard_Boolean opporifound,memorifound; opporifound = memorifound = Standard_False;
    TopOpeBRepDS_ListOfInterference loIfound; 
    const TopOpeBRepDS_ListOfInterference& EPIL = myDS->ShapeInterferences(edge);
    Standard_Boolean ok = FUN_sameGsameS(EPIL,PVIndex,iOOFace,loIfound);
    if (ok) {
      TopOpeBRepDS_PointIterator PI(loIfound); 
      // on cree une EPI orientee <transEdge> ssi :
      // - il en existe deja une d'orientation opposee a TransEdge
      // - il n'en existe pas deja une d'orientation identique a TransEdge
      for (; PI.More(); PI.Next()){
	TopAbs_Orientation oEPI = PI.Value()->Transition().Orientation(TopAbs_IN);
	if (!memorifound) memorifound = ( oEPI == anOtransEdge);
	if (!opporifound) opporifound = ( oEPI == TopAbs::Complement(anOtransEdge) );
	addEPI = (opporifound && ! memorifound);
	if (addEPI) break;
      }
    }
    if (!ok) addEPI = Standard_True;
  } // EPIfound
  
#ifdef OCCT_DEBUG
  if (GLOBAL_bvpr) debvprmess(myexF1,myexF2,ili,ivp,isi);
#endif  

  // xpu030998 : edge has restriction on OOface, do NOT append EPIf
  //     cto904A3 (edge19,OOface14,vG16), 
  if (myLineINL) {
    Standard_Real tola = Precision::Angular()*1.e+4; //dealing with tolerances
    gp_Pnt2d uv = VP.SurfaceParameters(OOShapeIndex);
    Standard_Boolean EtgOOF = FUN_tool_EtgF(paredge,edge,uv,OOFace,tola);    
    Standard_Boolean inERL = FUN_INlos(edge,myERL);
    if (EtgOOF && inERL) return; // cto904A3
  }
  
  if ( addEPI && (!dge)) {
    // ShapeIndex = 1,2 --> OOShapeIndex = 2,1
    // point est sur une seule arete <edge> de <ShapeIndex>
    // le Support de l'interference est l'autre 
    // face (OOShapeIndex) / ShapeIndex
    if (iOOFace == 0) iOOFace = myDS->AddShape(OOFace,OOShapeIndex);
    Handle(TopOpeBRepDS_Interference) EPIf;
    {
      TopOpeBRepDS_Transition T = transEdge; T.Index(iOOFace);
      EPIf = MakeEPVInterference(T,iOOFace,PVIndex,paredge,PVKind,TopOpeBRepDS_FACE,SIisvertex);
    }
    myHDS->StoreInterference(EPIf,edge);
  } // addEPI  

} // ProcessVPonR
