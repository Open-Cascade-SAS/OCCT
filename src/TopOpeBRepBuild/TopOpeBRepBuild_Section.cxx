// Created on: 1997-01-14
// Created by: Jean Yves LEBEY
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define MGhc3 Handle(Geom_Curve)
#define MGhc2 Handle(Geom2d_Curve)

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceSPS();
Standard_EXPORT void debsplitse(const Standard_Integer) {}
Standard_EXPORT void debsplitsemess(const Standard_Integer i,const TCollection_AsciiString& s = "");
Standard_EXPORT void debsplitsemess(const Standard_Integer i,const TCollection_AsciiString& s){cout<<"+++ debsplitse "<<s<<" E"<<i<<endl;debsplitse(i);}
Standard_EXPORT void debspseou(const Standard_Integer i) {debsplitsemess(i,"OUT");}
Standard_EXPORT void debspsein(const Standard_Integer i) {debsplitsemess(i,"IN ");}
Standard_EXPORT void debspseon(const Standard_Integer i) {debsplitsemess(i,"ON ");}
extern Standard_Boolean TopOpeBRepTool_GettraceC2D();
#endif

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

//Standard_IMPORT void FUN_tool_ttranslate(const gp_Vec2d& tvector, const TopoDS_Face& fF, TopoDS_Edge& fyE);

#include <TopOpeBRepTool_ShapeTool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
//---------------------------------------------
static Standard_Boolean FUN_periodicS(const TopoDS_Shape& F)
//---------------------------------------------
{
  if (F.IsNull()) return Standard_False;
  if (F.ShapeType() != TopAbs_FACE) return Standard_False;
  Handle(Geom_Surface) SSS = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  GeomAdaptor_Surface GAS(SSS);  
  GeomAbs_SurfaceType styp = GAS.GetType();
  Standard_Boolean periodic = Standard_False;
  if (styp == GeomAbs_Cylinder) periodic = Standard_True;
  if (styp == GeomAbs_Cone) periodic = Standard_True;
  if (styp == GeomAbs_Sphere) periodic = Standard_True;
  // NYI : for Torus,SurfaceOfRevolution.. 
  return periodic;
}

static Standard_Boolean FUN_periodic(const TopoDS_Face& F,Standard_Boolean& uper,Standard_Boolean& vper)
{
  const Handle(Geom_Surface)& su = BRep_Tool::Surface(F);
  uper = su->IsUPeriodic(); 
  vper = su->IsVPeriodic();
  Standard_Boolean per = (uper || vper);
  return per;  
}

static Standard_Boolean FUN_onboundsper(const gp_Pnt2d& uv,const TopoDS_Face& F)
{  
  // 2d : 
  const Handle(Geom_Surface)& su = BRep_Tool::Surface(F);
  Standard_Boolean uclo = su->IsUPeriodic();  
  Standard_Boolean vclo = su->IsVPeriodic();
  if (!uclo && !vclo) return Standard_False;

  Standard_Real u1,u2,v1,v2; su->Bounds(u1,u2,v1,v2);
  Standard_Real toluv = 1.e-8*1.e-2; // nyinyitol
  if (uclo) {
    Standard_Real d1 = Abs(u1-uv.X()); Standard_Boolean on1 = (d1 < toluv);
    Standard_Real d2 = Abs(u2-uv.X()); Standard_Boolean on2 = (d2 < toluv);
    return (on1 || on2);
  }
  if (vclo) {
    Standard_Real d1 = Abs(v1-uv.Y()); Standard_Boolean on1 = (d1 < toluv);
    Standard_Real d2 = Abs(v2-uv.Y()); Standard_Boolean on2 = (d2 < toluv);
    return (on1 || on2);
  }
  return Standard_False;  
}
			   
static Standard_Boolean FUN_onboundsper(const TopoDS_Edge& E,const TopoDS_Face& F, gp_Pnt2d& uv)
// uv is found by projection pnt(E,par)
{  
  // 3d : 
  Standard_Real f,l; FUN_tool_bounds(E,f,l); Standard_Real x=0.45678; Standard_Real par=f*x+(1-x)*l;
  Standard_Real tolF = BRep_Tool::Tolerance(F)*1.e2; // nyitol
  Standard_Boolean ok = FUN_tool_parF(E,par,F,uv,tolF);
  if (!ok) return Standard_False;
  
  Standard_Boolean onbp = ::FUN_onboundsper(uv,F);
  return onbp;
}



//-----------------------------------------------------------------------
static Standard_Boolean FUN_PinC(const gp_Pnt& P,const Handle(Geom_Curve)& C,const Standard_Real pmin,const Standard_Real pmax,const Standard_Real tol)
//-----------------------------------------------------------------------
{
  Standard_Boolean PinC = Standard_False;
  GeomAPI_ProjectPointOnCurve mydm(P,C,pmin,pmax);
  Standard_Boolean dmdone = ( mydm.Extrema().IsDone() );
  if ( dmdone ) {
    if ( mydm.NbPoints() ) {
      Standard_Real d = mydm.LowerDistance();
      PinC = (d <= tol);
    }
  }
  return PinC;
}

//-----------------------------------------------------------------------
static Standard_Boolean FUN_PinE(const gp_Pnt& P, const TopoDS_Edge& E)
//-----------------------------------------------------------------------
{
  Standard_Boolean PinE = Standard_False; 
  Standard_Real f,l; Handle(Geom_Curve) CE = BRep_Tool::Curve(E,f,l);
  Standard_Real tolE = BRep_Tool::Tolerance(E);
  PinE = FUN_PinC(P,CE,f,l,tolE);
  return PinE;
}

#include <BRep_Tool.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
//-----------------------------------------------------------------------
static Standard_Boolean FUN_EstaEE(const TopoDS_Edge& E, const TopAbs_State sta, const TopoDS_Edge& EE)
//-----------------------------------------------------------------------
{
  Standard_Boolean EinEE = Standard_False; 
  Standard_Real f,l; 
  //modified by NIZNHY-PKV Wed Nov  3 11:40:14 1999 from

  if (BRep_Tool::Degenerated(E)) {
    if      (sta == TopAbs_IN)  return Standard_False;
    else  return  Standard_True;
  }
  //modified by NIZNHY-PKV Wed Nov  3 11:40:19 1999 to

  Handle(Geom_Curve) CE = BRep_Tool::Curve(E,f,l);
  Standard_Real t = 0.417789; // Oi blya... ???
  Standard_Real p = (1-t)*f + t*l; 
  gp_Pnt P = CE->Value(p);
  EinEE = FUN_PinE(P,EE);
  if      (sta == TopAbs_IN)  return EinEE;
  else if (sta == TopAbs_OUT) return !EinEE;
  else throw Standard_ProgramError("TopOpeBRepBuild FUN_EstaEE on invalid state");
}

//=======================================================================
//function : TopOpeBRepBuild_Builder::InitSection
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::InitSection()
{
  mySectionDone = Standard_False;
  mySection.Clear();
  mySplitSectionEdgesDone = Standard_False;
  mySplitON.Clear();
}

//-----------------------------------------------------------------------
// LE : list of edges from where is extracted LEsta = edges located <sta> / edge E
// N.B. : LEsta is expanded and NOT reinitialized
static void FUN_selLEE(TopTools_ListOfShape& LE,const TopoDS_Edge& E,const TopAbs_State sta,TopTools_ListOfShape& LEsta)
{
  TopTools_ListIteratorOfListOfShape it(LE);
  while (it.More()) {
    const TopoDS_Edge& Ecur = TopoDS::Edge(it.Value());
    Standard_Boolean ok = FUN_EstaEE(Ecur,sta,E);
    if (ok) {
      LEsta.Append(Ecur);
      LE.Remove(it);
    }
    else it.Next();
  }
}

//-----------------------------------------------------------------------
// search the existence of shape E in time as shape of a mesh cell
// (shape,listofshape) of the list loslos.
Standard_Boolean FUN_FindEinSLOS(const TopoDS_Shape& E,const TopOpeBRepBuild_ListOfShapeListOfShape& loslos)
{
  Standard_Boolean f = Standard_False;
  for (TopOpeBRepBuild_ListIteratorOfListOfShapeListOfShape it(loslos); it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value().Shape();
    Standard_Boolean issame = (S.IsSame(E));
    if (issame) {
      f = Standard_True;
      break;
    }
  }
  return f;
}

//=======================================================================
//function : SplitSectionEdges
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::SplitSectionEdges()
{
  if (mySplitSectionEdgesDone) return;

  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  Standard_Integer i,n = BDS.NbSectionEdges();

  for (i = 1; i <= n; i++) { // 1
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
    if(E.IsNull()) continue;
    SplitSectionEdge(E);  
  } // 1

  TopOpeBRepBuild_DataMapOfShapeListOfShapeListOfShape MEIN;
  TopTools_DataMapOfShapeListOfShape MEOUT;
  
  for (i = 1; i <= n; i++) { // 2
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
    if(E.IsNull()) continue;
    Standard_Integer iE = myDataStructure->Shape(E); 
    Standard_Integer rE = BDS.AncestorRank(E);

#ifdef OCCT_DEBUG
    Standard_Boolean tSPS = GtraceSPS(E,iE); if (tSPS) debsplitsemess(iE); Standard_Integer DEBiESD = 1;
#endif

    Standard_Boolean isspliton = IsSplit(E,TopAbs_ON);
    if (!isspliton) continue;

    const TopTools_ListOfShape& LESD = BDS.ShapeSameDomain(E);
    if ( LESD.IsEmpty() ) continue;

    const TopTools_ListOfShape& LEspon = Splits(E,TopAbs_ON);
    TopTools_ListOfShape LEoutLESD; GCopyList(LEspon,LEoutLESD);
    Standard_Integer iRef = BDS.SameDomainRef(E);

    // LEoutLESD = list of edges Split(E,ON) OUT of all 
    // edges of the list of edges same domain of E in 3d only
    // edges LEoutLESD are attached in Split(ON) to E only.
 
    for (TopTools_ListIteratorOfListOfShape itLESD(LESD); itLESD.More(); itLESD.Next()) { // 2.1
      const TopoDS_Edge& ESD = TopoDS::Edge(itLESD.Value()); 
      Standard_Integer iESD = myDataStructure->Shape(ESD);
      const TopTools_ListOfShape& LESDspon = Splits(ESD,TopAbs_ON);

#ifdef OCCT_DEBUG
      if (tSPS) {
	TCollection_AsciiString str("# edge ");str=str+iE+" sd3d edge "+iESD;
	TCollection_AsciiString stru(str.Length(),'-');
	cout<<endl;if(DEBiESD==1)cout<<stru<<endl;
	DEBiESD++;cout<<str<<endl;debsplitsemess(iE);
      }
#endif

      // reduction of LEoutLESD = edges OUT all ESD
      TopTools_ListOfShape dummylos;
      FUN_selLEE(LEoutLESD,ESD,TopAbs_IN,dummylos);

      const TopoDS_Edge    *pE1 = NULL,   *pE2 = NULL;
      const TopTools_ListOfShape *plos1 = NULL, *plos2 = NULL;
      Standard_Integer nLEspon   = LEspon.Extent();
      Standard_Integer nLESDspon = LESDspon.Extent();

      if ( nLEspon != 0 && nLESDspon != 0 ) {
        Standard_Boolean takeE = ((rE == 1 && iESD != iRef) || iE == iRef);
        pE1 =  takeE ? &E : &ESD;
        pE2 = !takeE ? &E : &ESD;
        plos1 =  takeE ? &LEspon : &LESDspon;
        plos2 = !takeE ? &LEspon : &LESDspon;
      }
      else if ( nLEspon != 0 ) {
        pE1 = &E;
        pE2 = &ESD;
        plos1 = &LEspon;
        plos2 = &LESDspon;
      }
      else if ( nLESDspon != 0 ) {
        pE1 = &ESD;
        pE2 = &E;
        plos1 = &LESDspon;
        plos2 = &LEspon;
      }
      
      if (pE1 == NULL || pE2 == NULL) continue;
      if (plos1 == NULL || plos2 == NULL) continue;

      const TopoDS_Edge& E1 = *pE1;
      const TopoDS_Edge& E2 = *pE2;
      const TopTools_ListOfShape& LE1 = *plos1;

      // selection of edges IN E2 = LEinE2
      TopTools_ListOfShape LE1loc; 
      GCopyList(LE1,LE1loc);
      TopTools_ListOfShape LE1inE2;
      FUN_selLEE(LE1loc,E2,TopAbs_IN,LE1inE2);
      Standard_Integer nLE1inE2 = LE1inE2.Extent();

#ifdef DRAW
      if (tSPS) {
	cout<<"# edges ON "<<iE<<" ";
	TopAbs::Print(TopAbs_IN,cout); cout<<" / esd";
	cout<<" "<<iESD;
	cout<<" : ("<<nLE1inE2<<")"<<endl;
	TCollection_AsciiString str("ON");str=str+iE+"IN"+iESD;
	FDRAW_DINLOE("   ",LE1inE2,str,"");
      }
#endif
      
      // edges E1 and E2 share LE1inE2
      if (nLE1inE2 != 0) { // 2.2

	Standard_Boolean E1b = MEIN.IsBound(E1);
        TopOpeBRepBuild_ListOfShapeListOfShape thelist;
	if (!E1b) MEIN.Bind(E1, thelist);
	TopOpeBRepBuild_ListOfShapeListOfShape& LE1loslos = MEIN.ChangeFind(E1);
#ifdef OCCT_DEBUG
//	Standard_Integer nLE1 = LE1loslos.Extent();
#endif
	
	Standard_Boolean E2b = MEIN.IsBound(E2);
        TopOpeBRepBuild_ListOfShapeListOfShape thelist2;
	if (!E2b) MEIN.Bind(E2,thelist2);
	TopOpeBRepBuild_ListOfShapeListOfShape& LE2loslos = MEIN.ChangeFind(E2);
#ifdef OCCT_DEBUG
//	Standard_Integer nLE2 = LE2loslos.Extent();
#endif
	
	Standard_Boolean E2elemofE1 = FUN_FindEinSLOS(E2,LE1loslos);     
	Standard_Boolean E1elemofE2 = FUN_FindEinSLOS(E1,LE2loslos);
	
	Standard_Boolean condadd = (!E2elemofE1 && !E1elemofE2);
	if (condadd) {
	  // regularization of edges of LE1inE2 -> LR
	  TopTools_ListOfShape LR;
	  for (TopTools_ListIteratorOfListOfShape ite(LE1inE2);ite.More();ite.Next()){
	    const TopoDS_Edge& e = TopoDS::Edge(ite.Value());
	    TopTools_ListOfShape newle; Standard_Boolean ok = TopOpeBRepTool_TOOL::SplitE(e,newle);
	    if (ok) LR.Append(newle);
	    else    LR.Append(e);
	  }
	  {
            TopOpeBRepBuild_ShapeListOfShape thelist3;
	    LE1loslos.Append(thelist3);
	    TopOpeBRepBuild_ShapeListOfShape& E1slos = LE1loslos.Last();
	    E1slos.ChangeShape() = E2;
	    GCopyList(LR,E1slos.ChangeList());
	  }
	  {
            TopOpeBRepBuild_ShapeListOfShape thelist4;
	    LE2loslos.Append(thelist4);
	    TopOpeBRepBuild_ShapeListOfShape& E2slos = LE2loslos.Last();
	    E2slos.ChangeShape() = E1;
	    GCopyList(LR,E2slos.ChangeList());
	  }
	}
      } // 2.2
    } 

#ifdef DRAW
    if (tSPS) {
      cout<<endl<<"# edges ON "<<iE<<" ";
      TopAbs::Print(TopAbs_OUT,cout);cout<<" / lesd";
      for(TopTools_ListIteratorOfListOfShape it(LESD);it.More();it.Next())
	cout<<" "<<myDataStructure->Shape(it.Value());
      Standard_Integer n=LEoutLESD.Extent();cout<<" : ("<<n<<")"<<endl;
      TCollection_AsciiString str("ON");str=str+iE+"OUT";
      FDRAW_DINLOE("   ",LEoutLESD,str,"");
    }
#endif
    
    if (!MEOUT.IsBound(E)) {
      TopTools_ListOfShape thelist5;
      MEOUT.Bind(E, thelist5);
    }
    GCopyList(LEoutLESD,MEOUT.ChangeFind(E));

  } // 2

  for (i = 1; i <= n; i++) { // 3
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
    if(E.IsNull()) continue;
#ifdef OCCT_DEBUG
    Standard_Integer iE = myDataStructure->Shape(E);
//    Standard_Integer rE = GShapeRank(E);
    Standard_Boolean tSPS = GtraceSPS(E,iE); 
    if (tSPS) debsplitsemess(iE);
#endif

    Standard_Boolean isspliton = IsSplit(E,TopAbs_ON);;
    if (!isspliton) continue;

    const TopTools_ListOfShape& LESD = BDS.ShapeSameDomain(E);
    if ( LESD.IsEmpty() ) continue;

    Standard_Boolean isbMEOUT = MEOUT.IsBound(E);
    Standard_Boolean isbMEIN = MEIN.IsBound(E);
    if (!isbMEOUT && !isbMEIN) continue;

    TopTools_ListOfShape& LEspon = ChangeSplit(E,TopAbs_ON);
    LEspon.Clear();
    
    if (isbMEOUT) {
      const TopTools_ListOfShape& LEOUT = MEOUT.Find(E);
#ifdef OCCT_DEBUG
//      Standard_Integer nOUT = LEOUT.Extent();
#endif
      GCopyList(LEOUT,LEspon);
    }

    if (isbMEIN) {
      const TopOpeBRepBuild_ListOfShapeListOfShape& loslos = MEIN.Find(E);
#ifdef OCCT_DEBUG
//      Standard_Integer nloslos = loslos.Extent();
#endif
      for (TopOpeBRepBuild_ListIteratorOfListOfShapeListOfShape it(loslos); it.More(); it.Next()) {
	const TopTools_ListOfShape& los = it.Value().List();
#ifdef OCCT_DEBUG
//	Standard_Integer nlos = los.Extent();
#endif
	GCopyList(los,LEspon);
      }
    }
  } // 3

  BRep_Builder BB;
  for (i = 1; i <= n; i++) { // 4
    const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i)); if(E.IsNull()) continue;
#ifdef OCCT_DEBUG
    Standard_Integer idebE; Standard_Boolean tSPS = GtraceSPS(E,idebE); if (tSPS) debsplitsemess(idebE);
#endif
    const TopTools_ListOfShape& lesd = BDS.ShapeSameDomain(E);
    if (lesd.IsEmpty()) continue;
    
    Standard_Integer iE = BDS.Shape(E);
#ifdef OCCT_DEBUG
//    Standard_Integer rE = BDS.AncestorRank(E); 
#endif
    Standard_Integer RE = BDS.SameDomainRef(E);
    if (iE != RE) continue;
    
    TopTools_ListOfShape lF; TopTools_ListIteratorOfListOfShape itlesd;
    for(itlesd.Initialize(lesd);itlesd.More();itlesd.Next()) {
      const TopoDS_Edge& esd = TopoDS::Edge(itlesd.Value());
#ifdef OCCT_DEBUG
//      Standard_Integer iesd = BDS.Shape(esd);
#endif
      const TopTools_ListOfShape& lf = FDSCNX_EdgeConnexitySameShape(esd,myDataStructure);
      GCopyList(lf,lF);
    }
#ifdef OCCT_DEBUG
//    Standard_Integer nlF = lF.Extent();
#endif
    
    TopTools_ListOfShape& lon = ChangeSplit(E,TopAbs_ON);
    Standard_Real tolE = BRep_Tool::Tolerance(E); 
    TopTools_ListIteratorOfListOfShape it(lF); for(;it.More();it.Next()) {
      const TopoDS_Face& F = TopoDS::Face(it.Value());
      Standard_Integer iF = BDS.Shape(F); Standard_Integer rF = BDS.AncestorRank(iF);

      TopoDS_Edge esdF; Standard_Boolean besdF = Standard_False; // NYI pointer on esdF
      for(itlesd.Initialize(lesd);itlesd.More();itlesd.Next()) {
	const TopoDS_Edge& esd = TopoDS::Edge(itlesd.Value()); 
	Standard_Integer iesd = BDS.Shape(esd); Standard_Integer resd = BDS.AncestorRank(iesd);
	if (resd == rF) { 
	  TopExp_Explorer ex;
	  for (ex.Init(F,TopAbs_EDGE);ex.More();ex.Next()) {
//	  for (TopExp_Explorer ex(F,TopAbs_EDGE);ex.More();ex.Next()) {
	    const TopoDS_Shape& ee = ex.Current();
	    Standard_Boolean eq = (ee.IsEqual(esd));
	    if (eq) { esdF = esd; besdF = Standard_True; break; }
	  }
	}
	if (besdF) break;
      }
      
      TopTools_ListIteratorOfListOfShape itlon(lon); for(;itlon.More();itlon.Next()) {
	TopoDS_Edge& eon = TopoDS::Edge(itlon.Value());
	Standard_Real f,l; Standard_Boolean hasPC = FC2D_HasCurveOnSurface(eon,F);
	if (hasPC) continue;
#ifdef OCCT_DEBUG
	if (TopOpeBRepTool_GettraceC2D()) {
	  cout<<"\n#TopOpeBRepBuild_Builder::SSE : hasPC = 0 ES"<<i<<" E"<<idebE<<" sur F"<<iF<<endl;
	  cout<<"tsee s "<<iF<<" "<<idebE<<";"<<endl;
	}
#endif
//	Standard_Real tolpc; MGhc2 PC = FC2D_CurveOnSurface(eon,F,esdF,f,l,tolpc);
	Standard_Real tolpc; MGhc2 PC = FC2D_CurveOnSurface(eon,F,esdF,f,l,tolpc,Standard_True);//xpu051198 :PRO15049
	hasPC = (!PC.IsNull());
	if (!hasPC) throw Standard_ProgramError("TopOpeBRepBuild_Builder::SSE null PC on F");
	Standard_Real tol = Max(tolE,tolpc);
	BB.UpdateEdge(eon,PC,F,tol);
      }
    }
  } // 4
  
  Standard_Integer nsha = BDS.NbShapes();
  for (i = 1; i <= nsha; i++) { // 5
    const TopoDS_Shape& FOR = myDataStructure->Shape(i);
    Standard_Boolean isface = (FOR.ShapeType() == TopAbs_FACE); 
    if (!isface) continue;
    const TopoDS_Face& FF = TopoDS::Face(FOR);
#ifdef OCCT_DEBUG
//    Standard_Integer iFF = BDS.Shape(FF);
#endif
    const TopOpeBRepDS_ListOfInterference& LI = BDS.ShapeInterferences(FF); Standard_Integer nLI = LI.Extent(); 
    if (nLI == 0) continue;
    for (TopOpeBRepDS_ListIteratorOfListOfInterference ILI(LI); ILI.More(); ILI.Next() ) {
      Handle(TopOpeBRepDS_ShapeShapeInterference) SSI (Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(ILI.Value())); 
      if (SSI.IsNull()) continue;      
      TopOpeBRepDS_Kind GT,ST;Standard_Integer GI,SI;FDS_data(SSI,GT,GI,ST,SI); 
      if (ST != TopOpeBRepDS_FACE) continue;      
      const TopOpeBRepDS_Transition& TFE = SSI->Transition(); 
      TopAbs_ShapeEnum shab = TFE.ShapeBefore(),shaa = TFE.ShapeAfter();
      if (shaa != TopAbs_FACE || shab != TopAbs_FACE) continue;
      const TopoDS_Face& FS = TopoDS::Face( myDataStructure->Shape(SI)); 
#ifdef OCCT_DEBUG
//      Standard_Integer iFS = myDataStructure->Shape(FS);       
#endif
      Standard_Boolean FSisper = FUN_periodicS(FS);
      if (!FSisper) continue;

      const TopoDS_Edge& EG = TopoDS::Edge(myDataStructure->Shape(GI)); 
#ifdef OCCT_DEBUG
//      Standard_Integer iEG = myDataStructure->Shape(EG);    
#endif
      Standard_Boolean isrest = myDataStructure->DS().IsSectionEdge(EG); if (!isrest) continue;
#ifdef OCCT_DEBUG
//      Standard_Real tolE = BRep_Tool::Tolerance(EG);
#endif
      Standard_Boolean haspc = FC2D_HasCurveOnSurface(EG,FS); if (haspc) continue;
      Standard_Boolean hasc3d = FC2D_HasC3D(EG);
      if (!hasc3d) throw Standard_ProgramError("TopOpeBRepBuild_Builder::SSE EG without C3D");
      Standard_Real pf,pl,tolpc; Handle(Geom2d_Curve) PC;
      Standard_Boolean trim3d = Standard_True; PC = FC2D_CurveOnSurface(EG,FS,pf,pl,tolpc,trim3d);
      if (PC.IsNull()) throw Standard_ProgramError("TopOpeBRepBuild_Builder::SSE EG without PC on FS");
    } 
  } //5
  
  for (i = 1; i <= nsha; i++) { // 6 
    // xpu201198, for all periodic surfaces
    const TopoDS_Shape& FOR = myDataStructure->Shape(i);
    Standard_Boolean isface = (FOR.ShapeType() == TopAbs_FACE); 
    if (!isface) continue;
    const TopoDS_Face& FF = TopoDS::Face(FOR);
#ifdef OCCT_DEBUG
//    Standard_Integer iFF = BDS.Shape(FF); 
#endif
    Standard_Boolean FFuper,FFvper; Standard_Boolean FFisper = FUN_periodic(FF,FFuper,FFvper);
    if (!FFisper) continue;

    const TopOpeBRepDS_ListOfInterference& LI = BDS.ShapeInterferences(FF); Standard_Integer nLI = LI.Extent(); 
    if (nLI == 0) continue;
    for (TopOpeBRepDS_ListIteratorOfListOfInterference ILI(LI); ILI.More(); ILI.Next() ) {
      Handle(TopOpeBRepDS_ShapeShapeInterference) SSI (Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(ILI.Value())); 
      if (SSI.IsNull()) continue;      
      TopOpeBRepDS_Kind GT,ST;Standard_Integer GI,SI;FDS_data(SSI,GT,GI,ST,SI); 
      if (ST != TopOpeBRepDS_FACE) continue;      
      Standard_Boolean GB = SSI->GBound();
      if (GB == 1) continue;
      
      const TopoDS_Edge& EG = TopoDS::Edge(myDataStructure->Shape(GI)); 
#ifdef OCCT_DEBUG
//      Standard_Integer iEG = myDataStructure->Shape(EG);    
#endif
      Standard_Boolean isrest = myDataStructure->DS().IsSectionEdge(EG); if (!isrest) continue;
      
      // xpu191198 : cto016*
      Standard_Real pf,pl,tolpc; Handle(Geom2d_Curve) PC;
      Standard_Boolean trim3d = Standard_True; PC = FC2D_CurveOnSurface(EG,FF,pf,pl,tolpc,trim3d);
      Standard_Boolean isoU,isoV; gp_Pnt2d o2d; gp_Dir2d d2d; 
      Standard_Boolean ISO = TopOpeBRepTool_TOOL::UVISO(PC,isoU,isoV,d2d,o2d);
      if (ISO) {
	TopTools_ListIteratorOfListOfShape itON(Splits(EG,TopAbs_ON));
	TopTools_ListOfShape newlON;
	for (; itON.More(); itON.Next()){
	  TopoDS_Edge eon = TopoDS::Edge(itON.Value());

	  Standard_Real pfon,plon,tolpcon; Handle(Geom2d_Curve) PCon;
	  PCon = FC2D_CurveOnSurface(eon,FF,pfon,plon,tolpcon,trim3d);

	  Standard_Boolean isouon,isovon; gp_Pnt2d o2don; gp_Dir2d d2don; 
	  Standard_Boolean ISOon = TopOpeBRepTool_TOOL::UVISO(PCon,isouon,isovon,d2don,o2don);
	  Standard_Boolean PCko = !ISOon || ((isoU && !isouon) || (isoV && !isovon));
	  if (PCko) throw Standard_ProgramError("TopOpeBRepBuild_Builder::splitON");

	  Standard_Boolean test = (FFuper && isoU) || (FFvper && isoV);
	  if (!test) { newlON.Append(eon); continue;}

	  gp_Pnt2d uvok; Standard_Boolean isonclo = FUN_onboundsper(eon,FF,uvok); //3d
	  if (isonclo) { newlON.Append(eon); continue;}
	  
	  Standard_Boolean isonclo2 = FUN_onboundsper(o2don,FF); //2d
	  if (isonclo2) {	    
	    gp_Vec2d tr;
	    if (isoU) {Standard_Real dtr = uvok.X() - o2don.X();tr = gp_Vec2d(dtr,0.);}
	    else      {Standard_Real dtr = uvok.Y() - o2don.Y();tr = gp_Vec2d(0.,dtr);}
	    Standard_Real mag = tr.Magnitude();
	    Standard_Real toluv = 1.e-8*1.e2; // NYINYI
	    if (mag > toluv) TopOpeBRepTool_TOOL::TrslUVModifE(tr,FF,eon);
	  }
	  newlON.Append(eon);	 
	} // itON
	TopTools_ListOfShape& nlON = ChangeSplit(EG,TopAbs_ON);
	nlON.Clear(); nlON.Append(newlON);	
      } // ISO
    } //ILI(LI)
  } //6
  mySplitSectionEdgesDone = Standard_True;  
} // SplitSectionEdges

#define TheIN (1)
#define TheON (2)
#define TheOUT (3)
Standard_Integer GLOBAL_issp = 0; //++

#define HASSD2d (2)
#define HASSD3d (3)
Standard_EXPORT Standard_Integer GLOBAL_hassd = 0; //++

//=======================================================================
//function : SplitSectionEdge
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::SplitSectionEdge(const TopoDS_Shape& EA)
{
#ifdef OCCT_DEBUG
  Standard_Integer iE; Standard_Boolean tSPS = GtraceSPS(EA,iE);
  if (tSPS) debsplitsemess(iE);
#endif

  TopOpeBRepDS_DataStructure& BDS = myDataStructure->ChangeDS();
  const TopoDS_Edge& EOR = TopoDS::Edge(EA);
  TopoDS_Edge EF = EOR; EF.Orientation(TopAbs_FORWARD);
  Standard_Integer rankEF = myDataStructure->DS().AncestorRank(EF); // GShapeRank <- GMapShapes, appele par Merge
  
//  FUN_removeonGB(myDataStructure,EOR); //xpu041198

  Standard_Boolean hg = myDataStructure->HasGeometry(EOR);
  Standard_Boolean hsd3d = FDS_HasSameDomain3d(BDS,EOR);
  Standard_Boolean hsd2d = FDS_HasSameDomain2d(BDS,EOR);
#ifdef OCCT_DEBUG
  Standard_Boolean issplit = IsSplit(EOR,TopAbs_ON);
#endif

  Standard_Boolean cond = (hg || hsd3d); //REST2 //( hg && (!hsd) );  

  GLOBAL_hassd = 0; // initializing
  if(hsd3d) GLOBAL_hassd=3; //++
  if(hsd2d) GLOBAL_hassd=2; //++

  if (mySplitSectionEdgesDone) {
#ifdef OCCT_DEBUG
    if(tSPS) {
      GdumpSHA(EF, (char *) "SplitSectionEdges done : ");
      if (issplit) cout<<" "<<Splits(EOR,TopAbs_ON).Extent()<<" edges splitON"<<endl;
      else cout<<" !IsSplit"<<endl;
    }
#endif
    return;
  }
  
#ifdef OCCT_DEBUG
  if(tSPS)GdumpSHASTA(EF,TopAbs_ON,"--- SplitSectionEdges ");
  if(tSPS)cout<<" (hg="<<hg<<"||hsd3d="<<hsd3d<<")="<<cond<<endl;
#endif   

  // xpu161198 BUC60382(e3on) SE EOR has all its interferences "on bounds"
  Standard_Boolean allGb1=Standard_False;   
  TopoDS_Vertex vf,vl; TopExp::Vertices(TopoDS::Edge(EOR),vf,vl);
  const TopOpeBRepDS_ListOfInterference& loi = BDS.ShapeInterferences(EOR);
  TopOpeBRepDS_ListIteratorOfListOfInterference it(loi);
  for (; it.More(); it.Next()){
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    TopOpeBRepDS_Kind GT,ST; Standard_Integer G,S; FDS_data(I,GT,G,ST,S);    
    if (GT == TopOpeBRepDS_POINT) {allGb1=Standard_False; break;}
    Standard_Integer rkG = BDS.AncestorRank(G);
    const TopoDS_Vertex& v = TopoDS::Vertex(BDS.Shape(G));
    if (rkG==rankEF) {
      allGb1 = (v.IsSame(vf) || v.IsSame(vl));      
    }
    else {
      TopoDS_Shape oov; Standard_Boolean hsdmv = FUN_ds_getoov(v,myDataStructure,oov);
      if (!hsdmv) allGb1=Standard_False;
      allGb1 = (oov.IsSame(vf) || oov.IsSame(vl)); 
    }
    if (!allGb1) break;
  } // tki
  

  Standard_Boolean mke = !cond;
  mke = mke || !hg; //xpu271098 cto002J2(e9on)
  mke = mke || allGb1; //xpu161198 BUC60382(e3on)
  if (mke) {
    MarkSplit(EOR,TopAbs_ON);
    TopTools_ListOfShape& LON = ChangeSplit(EOR,TopAbs_ON);
#ifdef OCCT_DEBUG
//    Standard_Integer non = LON.Extent(); // DEB
#endif

    TopoDS_Edge newEOR; 
    FUN_ds_CopyEdge(EOR,newEOR); 
    Standard_Boolean hasnewEOR=Standard_False;
    BRep_Builder BB;
    TopExp_Explorer exv(EOR, TopAbs_VERTEX);
    for (; exv.More(); exv.Next()){
      const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
      Standard_Real parv = BRep_Tool::Parameter(v,EOR);
      Standard_Integer iv = BDS.Shape(v); 
      Standard_Integer ivref = 0;
      Standard_Boolean hsd = Standard_False;
      if (iv != 0) 
	hsd = myDataStructure->HasSameDomain(v);      
      if (hsd) 
	ivref = myDataStructure->SameDomainReference(v);
      Standard_Boolean setref = hsd && (iv != ivref);
      TopoDS_Vertex vref = TopoDS::Vertex(BDS.Shape(ivref));
      if (!setref) 
	{BB.Add(newEOR,v); FUN_ds_Parameter(newEOR,v,parv); continue;}
      hasnewEOR = Standard_True;
      vref.Orientation(v.Orientation());
      BB.Add(newEOR,vref); 
      FUN_ds_Parameter(newEOR,vref,parv);
    }
    if (hasnewEOR) 
      LON.Append(newEOR);
    else           
      LON.Append(EOR);
    return;
  } // mke

  TopTools_ListOfShape LESD1,LESD2; GFindSamDom(EOR,LESD1,LESD2);
 
#ifdef OCCT_DEBUG
  if(tSPS)GdumpSHASTA(EF,TopAbs_ON,"--- SplitSectionEdges ");
  if(tSPS){ 
    cout<<" (hg="<<hg<<"||hsd3d="<<hsd3d<<")="<<cond<<endl;
    GdumpSHA(EOR, (char *) "SplitSectionEdge");
    cout<<endl;
    GdumpSAMDOM(LESD1, (char *) "LESD1 : ");
    GdumpSAMDOM(LESD2, (char *) "LESD2 : ");
  }
#endif

  {
#ifdef OCCT_DEBUG
    if (tSPS) debspseon(iE);
#endif
    TopOpeBRepBuild_GTopo G = TopOpeBRepBuild_GTool::GComUnsh(TopAbs_FACE,TopAbs_FACE);
    myEdgeReference = EF;   
    TopOpeBRepBuild_PaveSet PVS(EF);

    GLOBAL_issp = TheON; //++
    GFillEdgePVS(EF,myEmptyShapeList,G,PVS);
    GLOBAL_issp = 0; //++
    
    // Create an edge builder EDBU
    TopOpeBRepBuild_PaveClassifier VCL(EF);
    Standard_Boolean equalpar = PVS.HasEqualParameters();
    if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());
    TopOpeBRepBuild_EdgeBuilder EDBU;
    EDBU.InitEdgeBuilder(PVS,VCL);
    
    // Build the new edges LEM
    TopTools_ListOfShape LEM;
    GEDBUMakeEdges(EF,EDBU,LEM);

    // xpu : 18-03-98 (cto 016 B2)
    //       splits edges of LEM with internal vertices    
    TopTools_ListOfShape newLEM; TopTools_ListIteratorOfListOfShape ite(LEM);
    for (; ite.More(); ite.Next()){
      const TopoDS_Edge& esp = TopoDS::Edge(ite.Value());
      TopTools_ListOfShape lspe; Standard_Boolean ok = TopOpeBRepTool_TOOL::SplitE(esp,lspe);
      Standard_Boolean nonwesp = (!ok) || (lspe.Extent() < 2);
      if (nonwesp) newLEM.Append(esp); 
      else         newLEM.Append(lspe);
    }
    LEM.Clear(); LEM.Append(newLEM);
    
    // connect new edges LEM as split parts (ON,SOLID)
    MarkSplit(EOR,TopAbs_ON);
    TopTools_ListOfShape& LON = ChangeSplit(EOR,TopAbs_ON);
    GCopyList(LEM,LON);
  }

//modified by NIZHNY-MZV  Mon Apr 17 16:25:51 2000  TopTools_ListOfShape losOO;
//modified by NIZHNY-MZV  Mon Apr 17 16:25:52 2000  if      (rankEF == 1) losOO.Append(GLOBALDS_shape2);
//modified by NIZHNY-MZV  Mon Apr 17 16:25:53 2000  else if (rankEF == 2) losOO.Append(GLOBALDS_shape1);

  {
#ifdef OCCT_DEBUG
    if (tSPS) debspsein(iE);
#endif
    TopOpeBRepBuild_GTopo G = TopOpeBRepBuild_GTool::GCutUnsh(TopAbs_FACE,TopAbs_FACE);
    G = G.CopyPermuted(); // parts (IN,OUT) 3d are constructed
    myEdgeReference = EF;   
    TopOpeBRepBuild_PaveSet PVS(EF);
  
    GLOBAL_issp = TheIN; //++
    GFillEdgePVS(EF,myEmptyShapeList,G,PVS);
    GLOBAL_issp = 0; //++

    // Create an edge builder EDBU
    TopOpeBRepBuild_PaveClassifier VCL(EF);
    Standard_Boolean equalpar = PVS.HasEqualParameters();
    if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());
    TopOpeBRepBuild_EdgeBuilder EDBU;
    EDBU.InitEdgeBuilder(PVS,VCL);
    
    // Build the new edges LEM
    TopTools_ListOfShape LEMNC;
    GEDBUMakeEdges(EF,EDBU,LEMNC);

//modified by NIZHNY-MZV  Mon Apr 17 15:23:28 2000    TopTools_ListOfShape LEM;
//modified by NIZHNY-MZV  Mon Apr 17 15:23:16 2000    GKeepShapes(EF,losOO,TopAbs_IN,LEMNC,LEM);

    // connect new edges LEM as split parts (IN,SOLID)
    MarkSplit(EOR,TopAbs_IN);
    TopTools_ListOfShape& LINN = ChangeSplit(EOR,TopAbs_IN);
    GCopyList(LEMNC,LINN);
  }

  {
#ifdef OCCT_DEBUG
    if (tSPS) debspseou(iE);
#endif
    TopOpeBRepBuild_GTopo G = TopOpeBRepBuild_GTool::GCutUnsh(TopAbs_FACE,TopAbs_FACE);
    // parts (OUT,IN) 3d are constructed
    myEdgeReference = EF;   
    TopOpeBRepBuild_PaveSet PVS(EF);

    GLOBAL_issp = TheOUT; //++    
    GFillEdgePVS(EF,myEmptyShapeList,G,PVS);
    GLOBAL_issp = 0; //++    
    
    // Create an edge builder EDBU
    TopOpeBRepBuild_PaveClassifier VCL(EF);
    Standard_Boolean equalpar = PVS.HasEqualParameters();
    if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());
    TopOpeBRepBuild_EdgeBuilder EDBU;
    EDBU.InitEdgeBuilder(PVS,VCL);
    
    // Build the new edges LEM
    TopTools_ListOfShape LEM;
    GEDBUMakeEdges(EF,EDBU,LEM);

    // connect new edges LEM as split parts (OUT,SOLID)
    MarkSplit(EOR,TopAbs_OUT);
    TopTools_ListOfShape& LINN = ChangeSplit(EOR,TopAbs_OUT);
    GCopyList(LEM,LINN);
  }

  GLOBAL_hassd=0; //++
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& TopOpeBRepBuild_Builder::Section()
{
  if (mySectionDone) return mySection;
  mySectionDone = Standard_True;
  SectionCurves(mySection);
  SectionEdges(mySection);
  return mySection;
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::Section(TopTools_ListOfShape& L)
{
  L = Section();
}

//=======================================================================
//function : SectionCurves
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::SectionCurves(TopTools_ListOfShape& LSE)
{
  TopOpeBRepDS_CurveExplorer cex(myDataStructure->DS());
  for (; cex.More(); cex.Next()) {
    Standard_Integer ic = cex.Index();
    TopTools_ListIteratorOfListOfShape itloe(NewEdges(ic));
    for(;itloe.More();itloe.Next()) {
      LSE.Append(itloe.Value());
    }
  }
}

//=======================================================================
//function : SectionEdges
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::SectionEdges(TopTools_ListOfShape& LSE)
{
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  Standard_Integer i,nes = BDS.NbSectionEdges();
  
  Standard_Integer iskpart = IsKPart();
  if ( iskpart == 1 ) { // iskole
    for (i=1;i<=nes;i++ ) {
      const TopoDS_Edge& E = TopoDS::Edge(BDS.SectionEdge(i));
      // modif fbi 16-09-97: case possible if RemoveSectionEdge was called
      if(E.IsNull()) 
	continue;
      // end modif fbi
      LSE.Append(E);
    }
    return;
  }
  
  SplitSectionEdges();
  
  // common of edges of section

  TopTools_MapOfShape MOS;

  for (i=1;i<=nes;i++) {

    const TopoDS_Shape& es = BDS.SectionEdge(i);
    // modif fbi 16-09-97: case possible if RemoveSectionEdge was called
    if(es.IsNull()) 
      continue;
    // end modif fbi
    Standard_Boolean issplitIN = IsSplit(es,TopAbs_IN);
    Standard_Boolean issplitON = IsSplit(es,TopAbs_ON);
    TopAbs_State staspl=(issplitON)?TopAbs_ON:(issplitIN)?TopAbs_IN:TopAbs_UNKNOWN;

#ifdef OCCT_DEBUG
    Standard_Integer iii; Standard_Boolean tSPS = GtraceSPS(es,iii);
    if (tSPS) {
      GdumpSHA(es, (char *) "--- Section ");
      cout<<" splitIN "<<issplitIN<<" "<<Splits(es,TopAbs_IN).Extent()<<endl;
      cout<<" splitON "<<issplitON<<" "<<Splits(es,TopAbs_ON).Extent()<<endl;
    }
#endif
    
    if (staspl != TopAbs_UNKNOWN) {
      for(TopTools_ListIteratorOfListOfShape it(Splits(es,staspl));it.More();it.Next()) {
	const TopoDS_Shape& S = it.Value();
	if ( !MOS.Contains(S) ) {
	  MOS.Add(S);
	  LSE.Append(S);
	}
      }
    }
    else {
      Standard_Boolean hasgeom = myDataStructure->HasGeometry(es);
      Standard_Boolean hassame = myDataStructure->HasSameDomain(es);
      Standard_Boolean take = !(hasgeom || hassame);
      if (take) {
	if ( !MOS.Contains(es) ) {
	  MOS.Add(es);
	  LSE.Append(es);
	}
      }
    }
  } // for i [1..nes]
} // SectionEdges

//=======================================================================
//function : FillSecEdgeAncestorMap
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::FillSecEdgeAncestorMap(const Standard_Integer aShapeRank,
                                                     const TopTools_MapOfShape& aMapON,
                                                     TopTools_DataMapOfShapeShape& anAncMap)
     const
{
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();

  Standard_Integer i,nse = BDS.NbSectionEdges();
  for (i=1; i<=nse; i++) {
    const TopoDS_Shape& es = BDS.SectionEdge(i);
    if(es.IsNull() || ShapeRank(es) != aShapeRank)
      continue;
    if (aMapON.Contains(es)) {
      anAncMap.Bind(es,es);
      continue;
    }
    TopAbs_State states[3] = {TopAbs_IN, TopAbs_ON, TopAbs_OUT};
    for (Standard_Integer j=0; j < 3; j++) {
//      Standard_Boolean isSplit = IsSplit(es,states[j]);
      if (IsSplit(es,states[j])) {
        TopTools_ListIteratorOfListOfShape it(Splits(es,states[j]));
        for(;it.More();it.Next()) {
          const TopoDS_Shape& aS = it.Value();
          if (aMapON.Contains(aS))
            anAncMap.Bind(aS,es);
	}
      }
    }
  }
}
