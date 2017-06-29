// Created on: 1996-03-07
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
#include <BRepAdaptor_Surface.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepLProp_SurfaceTool.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_BuilderON.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef OCCT_DEBUG
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextEINTERNAL();
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextEEXTERNAL();
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOSG();
Standard_EXPORT void debON(const Standard_Integer iF)
{cout<<"++ debON "<<iF<<" "<<endl;}
Standard_EXPORT void debON(const Standard_Integer iF, const TopAbs_State TB1,const TopAbs_State TB2)
{cout<<"++ debON "<<iF<<" :TB1=";TopAbs::Print(TB1,cout);cout<<",TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
Standard_EXPORT void debfillonf(const Standard_Integer iF)
{cout<<"++ debfillonf "<<iF<<" "<<endl;}
Standard_EXPORT void debfillone(const Standard_Integer iE)
{cout<<"++ debfillone "<<iE<<" "<<endl;}
Standard_EXPORT void debfillonfe(){}
Standard_EXPORT void debfillonfemess(const Standard_Integer f,const Standard_Integer e)
{cout<<"++ debfillonfe f"<<f<<" e"<<e<<endl;debfillonfe();}
Standard_EXPORT void debfillonfe3d(){}
Standard_EXPORT void debfillonfemess3d(const Standard_Integer f,const Standard_Integer e)
{cout<<"++ debfillonfe3d f"<<f<<" e"<<e<<endl;debfillonfe3d();}
Standard_EXPORT void debfillonfemess(const Standard_Integer iFOR,const Standard_Integer iEG,const TopOpeBRepBuild_PBuilder& PB,const TopOpeBRepBuild_PWireEdgeSet& PWES,const TCollection_AsciiString& str)
{PB->GdumpSHASTA(iEG,TopAbs_ON,*PWES,str); debfillonfemess(iFOR,iEG); }
Standard_EXPORT void debaddpwes(const Standard_Integer iFOR,const TopAbs_State TB1,const Standard_Integer iEG,const TopAbs_Orientation neworiE,const TopOpeBRepBuild_PBuilder& PB,const TopOpeBRepBuild_PWireEdgeSet& PWES,const TCollection_AsciiString& str1,const TCollection_AsciiString& str2)
{PB->GdumpSHASTA(iFOR,TB1,*PWES,str1,str2);cout<<iEG<<" : 1 edge ";TopAbs::Print(neworiE,cout);cout<<endl; }
Standard_EXPORT Standard_Boolean DEBTEFOR(const TopOpeBRepBuild_Builder& B,const Standard_Integer iFOR,const Standard_Integer GI)
{return B.GtraceSPS(iFOR,GI); }
#endif

#ifdef OCCT_DEBUG
Standard_EXPORT void FUN_RaiseON()
{throw Standard_Failure("BuilderON");}
static void FUN_cout(const TopoDS_Shape& eON)
{
  TopAbs_Orientation oE = eON.Orientation();
  if (oE == TopAbs_FORWARD)  cout<<"-> + eONF"<<endl;
  if (oE == TopAbs_REVERSED) cout<<"-> + eONR"<<endl;
  if (oE == TopAbs_INTERNAL) cout<<"-> + eONI"<<endl;
  if (oE == TopAbs_EXTERNAL) cout<<"-> + eONE"<<endl;
}
#endif

#define M_FORWARD(st)  (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

// NYI : TopOpeBRepTool_ShapeTool::ShapesSameOriented -> CurvesSameOriented NYI for E !line
//Standard_IMPORT Standard_Boolean TopOpeBRepBuild_FUN_aresamegeom(const TopoDS_Shape& S1,const TopoDS_Shape& S2);

//=======================================================================
//function : TopOpeBRepBuild_BuilderON
//purpose  : 
//=======================================================================
TopOpeBRepBuild_BuilderON::TopOpeBRepBuild_BuilderON()
{
}

//=======================================================================
//function : TopOpeBRepBuild_BuilderON
//purpose  : 
//=======================================================================
TopOpeBRepBuild_BuilderON::TopOpeBRepBuild_BuilderON(const TopOpeBRepBuild_PBuilder& PB, const TopoDS_Shape& FOR,const TopOpeBRepBuild_PGTopo& PG,
	       const TopOpeBRepTool_Plos& PLSclass, const TopOpeBRepBuild_PWireEdgeSet& PWES)
{
  Perform(PB,FOR,PG,PLSclass,PWES);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_BuilderON::Perform(const TopOpeBRepBuild_PBuilder& PB,const TopoDS_Shape& FOR,const TopOpeBRepBuild_PGTopo& PG,
		     const TopOpeBRepTool_Plos& PLSclass, const TopOpeBRepBuild_PWireEdgeSet& PWES)
{
  myPB = PB;
  myFace = FOR;
  myPG = PG;
  myPLSclass = PLSclass;
  myPWES = PWES;

#ifdef OCCT_DEBUG
  Standard_Integer iFOR;Standard_Boolean tFOR=myPB->GtraceSPS(FOR,iFOR);
  if (tFOR) debfillonf(iFOR);
#endif

  const TopOpeBRepDS_DataStructure& BDS=myPB->DataStructure()->DS();
  const TopOpeBRepDS_ListOfInterference& LI=BDS.ShapeInterferences(myFace);  
  for (TopOpeBRepDS_ListIteratorOfListOfInterference ILI(LI);ILI.More();ILI.Next() ) {
    const Handle(TopOpeBRepDS_Interference)& I=ILI.Value();
    GFillONPartsWES1(I);
  }
}

//=======================================================================
//function : GFillONCheckI
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_BuilderON::GFillONCheckI(const Handle(TopOpeBRepDS_Interference)& I) const
{
  const TopOpeBRepDS_DataStructure& BDS=myPB->DataStructure()->DS();
  
  Handle(TopOpeBRepDS_ShapeShapeInterference) SSI=Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I);
  if (SSI.IsNull()) return Standard_False;

  TopOpeBRepDS_Kind GT,ST;Standard_Integer GI,SI;FDS_data(SSI,GT,GI,ST,SI);
  if (GT != TopOpeBRepDS_EDGE || ST != TopOpeBRepDS_FACE) return Standard_False;
#ifdef OCCT_DEBUG
//  Standard_Integer iFOR=BDS.Shape(myFace);
#endif
  const TopoDS_Edge& EG=TopoDS::Edge(BDS.Shape(GI, Standard_False));
#ifdef OCCT_DEBUG
//  Standard_Integer iEG=BDS.Shape(EG, Standard_False);
#endif
  const TopoDS_Shape& FS=BDS.Shape(SI, Standard_False);
#ifdef OCCT_DEBUG
//  Standard_Integer iFS=BDS.Shape(FS, Standard_False);
#endif
  TopAbs_ShapeEnum ShapeInterf=TopAbs_FACE;
  const TopOpeBRepDS_Transition& TFE=SSI->Transition();
  TopAbs_ShapeEnum shab=TFE.ShapeBefore(),shaa=TFE.ShapeAfter();
  if (shaa != ShapeInterf || shab != ShapeInterf) return Standard_False;

  Standard_Boolean isrest=BDS.IsSectionEdge(EG);
  Standard_Boolean issplit=myPB->IsSplit(EG,TopAbs_ON);
  Standard_Boolean keep=(isrest && issplit);
  if ( !keep ) return Standard_False;

  const TopTools_ListOfShape& lEspON=myPB->Splits(EG,TopAbs_ON);
  if (lEspON.Extent() == 0) return Standard_False;

#ifdef OCCT_DEBUG
//  const TopoDS_Shape& EspON=lEspON.First();
#endif
  Standard_Integer rankFS=myPB->GShapeRank(FS);
  Standard_Integer rankFOR=myPB->GShapeRank(myFace);
  if (rankFS == 0 || rankFOR == 0) return Standard_False;

  return Standard_True;
} // GFillONCheckI

//=======================================================================
//function : GFillONPartsWES1
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_BuilderON::GFillONPartsWES1(const Handle(TopOpeBRepDS_Interference)& I)
{
  const TopOpeBRepDS_DataStructure& BDS=myPB->DataStructure()->DS();
#ifdef OCCT_DEBUG
  Standard_Integer iFOR=
#endif
           BDS.Shape(myFace);
  TopOpeBRepDS_Kind GT,ST;Standard_Integer GI,SI;FDS_data(I,GT,GI,ST,SI);

  Standard_Boolean Iok=GFillONCheckI(I); 
  if (!Iok) return;

  const TopoDS_Edge& EG=TopoDS::Edge(BDS.Shape(GI));
//  const TopoDS_Shape& FS=BDS.Shape(SI);
#ifdef OCCT_DEBUG
//  Standard_Integer iEG=BDS.Shape(EG);
//  Standard_Integer iFS=BDS.Shape(FS);
#endif
  
#ifdef OCCT_DEBUG
  Standard_Boolean tEFOR=DEBTEFOR(*myPB,iFOR,GI);
  if (tEFOR) debfillonfemess(iFOR,GI,myPB,myPWES,"--- GFillONPartsWES1");
#endif

  const TopTools_ListOfShape& lEspON=myPB->Splits(EG,TopAbs_ON);
#ifdef OCCT_DEBUG
//  Standard_Integer nEspON=lEspON.Extent();
#endif
  for(TopTools_ListIteratorOfListOfShape it(lEspON);it.More();it.Next()) {
    const TopoDS_Shape& EspON=it.Value();
    GFillONPartsWES2(I,EspON);
  }
} // GFillONPartsWES1

#ifdef OCCT_DEBUG
Standard_EXPORT void FUN_coutmess(const TCollection_AsciiString& m)
{
    cout<<m;
}
#else
Standard_EXPORT void FUN_coutmess(const TCollection_AsciiString&)
{
}
#endif

//------------------------------------------------------
#ifdef OCCT_DEBUG
Standard_Boolean FUN_keepEON(const TopOpeBRepBuild_Builder& B,
#else
Standard_Boolean FUN_keepEON(const TopOpeBRepBuild_Builder&,
#endif
                             const TopoDS_Shape& sEG,
                             const TopoDS_Shape& sFOR,
                             const TopoDS_Shape& sFS,
#ifdef OCCT_DEBUG
                             const Standard_Boolean EGBoundFOR,
#else
                             const Standard_Boolean,
#endif
                             const TopOpeBRepDS_Transition& TFE,
                             const TopAbs_State TB1,
                             const TopAbs_State )
//------------------------------------------------------
{
  // on construit l'etat TB1 de FOR par rapport a FS.
  // sur face FOR,on transitionne (TFE) en l'arete EG par rapport a la face FS.
  // TFE est defini sur la geometrie naturelle de la face orientee FOR,
  // c'est a dire sur FF=FOR orientee FORWARD,par rapport a la matiere 
  // definie par la face orientee FS.
  // on cherche oEGFF=orientation de EG dans FF
  TopoDS_Edge EG=TopoDS::Edge(sEG);
  TopoDS_Face FF=TopoDS::Face(sFOR);FF.Orientation(TopAbs_FORWARD);
  TopoDS_Face FS=TopoDS::Face(sFS);

#ifdef OCCT_DEBUG
  Standard_Integer iEG;/*Standard_Boolean tEG=*/B.GtraceSPS(EG,iEG);
  Standard_Integer iFF;/*Standard_Boolean tFF=*/B.GtraceSPS(FF,iFF);
  Standard_Integer iFS;/*Standard_Boolean tFS=*/B.GtraceSPS(FS,iFS);
  Standard_Boolean tFFEG=DEBTEFOR(B,iFF,iEG);if (tFFEG) debfillonfemess(iFF,iEG);
  Standard_Boolean tFSEG=DEBTEFOR(B,iFS,iEG);if (tFSEG) debfillonfemess(iFS,iEG);
#endif
  
#ifdef OCCT_DEBUG
  Standard_Boolean keep1=Standard_True;
  Standard_Boolean keep2=Standard_True;
#endif
  Standard_Boolean keep3=Standard_True;
  Standard_Boolean isclosedFF=BRep_Tool::IsClosed(EG,FF);
  if (isclosedFF) {
#ifdef OCCT_DEBUG
    keep1=Standard_True;
    keep2=Standard_True;
#endif
    keep3=Standard_True;
  }
  else {
    TopAbs_Orientation oEGFF=TopAbs_FORWARD;
    FUN_tool_orientEinF(EG,FF,oEGFF);

#ifdef OCCT_DEBUG
    TopAbs_Orientation omatFS1=TFE.Orientation(TB1);
    if (oEGFF == TopAbs_REVERSED) omatFS1=TopAbs::Complement(omatFS1);
    keep1=(omatFS1 == TopAbs_FORWARD);

    TopAbs_Orientation omatFS2=TFE.Orientation(TB1);
    keep2=(omatFS2 == oEGFF);
#endif

    TopAbs_State tfeb=TFE.Before();
    TopAbs_State tfea=TFE.After();
    if      (oEGFF == TopAbs_FORWARD ) keep3=(tfea == TB1);
    else if (oEGFF == TopAbs_REVERSED) keep3=(tfeb == TB1);
    else if (oEGFF == TopAbs_EXTERNAL) keep3=(tfea == TB1 || tfeb == TB1);
    else if (oEGFF == TopAbs_INTERNAL) keep3=(tfea == TB1 || tfeb == TB1);
  }
  
#ifdef OCCT_DEBUG
  if (tFFEG || tFSEG) {
    if ( keep1 != keep2 || keep1 != keep3 || keep2 != keep3 ) {
      cout<<"\nkeepEON : EGB "<<EGBoundFOR<<" EG "<<iEG<<" FOR "<<iFF<<" FS "<<iFS;
      cout<<" keep1 "<<keep1<<" keep2 "<<keep2<<" keep3 "<<keep3;
      cout<<" !=!=!=!=!=!=!=!=!=!=!=!=!=!=!=";
      cout<<endl;
    }
  }
#endif

  return keep3;
}

Standard_EXPORT TopAbs_State FUN_build_TB(const TopOpeBRepBuild_PBuilder& PB,const Standard_Integer rank)
{
  Standard_Boolean opeFus = PB->Opefus();
  Standard_Boolean opec12 = PB->Opec12();
  Standard_Boolean opec21 = PB->Opec21();
  Standard_Boolean opeCom = PB->Opecom();
  TopAbs_State sta = TopAbs_UNKNOWN;
  if (opeFus) sta = TopAbs_OUT;
  if (opeCom) sta = TopAbs_IN;
  if (opec12) sta = (rank == 1) ? TopAbs_OUT : TopAbs_IN;
  if (opec21) sta = (rank == 2) ? TopAbs_OUT : TopAbs_IN;
  return sta;
}

static Standard_Boolean FUN_Kpart0(const GeomAbs_SurfaceType& ST1, const GeomAbs_SurfaceType& ST2)
{
  Standard_Boolean plane1 = (ST1 == GeomAbs_Plane);
  Standard_Boolean cyli1  = (ST1 == GeomAbs_Cylinder);
  Standard_Boolean cone1  = (ST1 == GeomAbs_Cone);

  Standard_Boolean cyli2  = (ST2 == GeomAbs_Cylinder);
  Standard_Boolean cone2  = (ST2 == GeomAbs_Cone);
  Standard_Boolean sphe2  = (ST2 == GeomAbs_Sphere);
  Standard_Boolean quad2  = cyli2 || cone2 || sphe2 || (ST2 == GeomAbs_Torus);
  
  if (plane1 && quad2) return Standard_True;
  if (cyli1  && sphe2) return Standard_True;
  if (cone1  && sphe2) return Standard_True;
  return Standard_False;
}
static Standard_Integer FUN_Kpart(const GeomAbs_SurfaceType& ST1, const GeomAbs_SurfaceType& ST2)
{
  Standard_Boolean kpart0 =  FUN_Kpart0(ST1,ST2);
  if (kpart0) return 1;
  kpart0 = FUN_Kpart0(ST2,ST1);
  if (kpart0) return 2;
  return 0;  
}

static Standard_Integer FUN_findeSD
(const TopOpeBRepDS_DataStructure& BDS, const TopoDS_Edge& EspON, const TopoDS_Edge& EG, const TopoDS_Face& FOR,
 TopAbs_Orientation& oeSD, const Standard_Integer D)
{
  // chercher eSD = SameDomain3d/2d de EG, arete de FOR, qui contient EspON
  gp_Pnt ptON; Standard_Real parON;
  FUN_tool_findPinE(EspON,ptON,parON);
  TopTools_ListOfShape lesdSD; 
  if (D == 3) FDS_HasSameDomain3d(BDS,EG,&lesdSD);
  if (D == 2) FDS_HasSameDomain2d(BDS,EG,&lesdSD);
  for(TopTools_ListIteratorOfListOfShape it(lesdSD);it.More();it.Next()) {
    TopoDS_Edge eSD = TopoDS::Edge(it.Value());
    TopAbs_Orientation oesd; Standard_Boolean eSDofFOR = FUN_tool_orientEinFFORWARD(eSD,FOR,oesd);
    if (!eSDofFOR) continue;
    TopAbs_State staeSD = FUN_tool_staPinE(ptON,eSD);
    if (staeSD == TopAbs_IN) {
      Standard_Integer i = BDS.Shape(eSD); oeSD = oesd; 
      return i;
    }
  }
  return 0;
}

static Standard_Boolean ComputeFaceCrvtInSec(const TopoDS_Face& aFace,
                                             const gp_Pnt2d& aP2d,
                                             const gp_Dir& aSecDir,
                                             Standard_Real& aCrvt)
{
  BRepAdaptor_Surface aSurf(aFace);
  Standard_Integer cn = BRepLProp_SurfaceTool::Continuity(aSurf);
  if (cn < 2) return Standard_False;
  BRepLProp_SLProps aProp(aSurf, aP2d.X(), aP2d.Y(), 2, Precision::Confusion());
  if (!aProp.IsCurvatureDefined()) return Standard_False;

  if (aProp.IsUmbilic()) {
    aCrvt = aProp.MaxCurvature();
    return Standard_True;
  }

  Standard_Real maxCrv = aProp.MaxCurvature();
  Standard_Real minCrv = aProp.MinCurvature();
  gp_Dir maxDir, minDir;
  aProp.CurvatureDirections(maxDir, minDir);
  Standard_Real cosMax = aSecDir * maxDir;
  Standard_Real cosMin = aSecDir * minDir;
  aCrvt = maxCrv * cosMax + minCrv * cosMin;
  return Standard_True;
}

//=======================================================================
//function : GFillONPartsWES2
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_BuilderON::GFillONPartsWES2(const Handle(TopOpeBRepDS_Interference)& I,const TopoDS_Shape& EspON)
{
  const Handle(TopOpeBRepDS_HDataStructure)& HDS=myPB->DataStructure();
  const TopOpeBRepDS_DataStructure& BDS= HDS->DS();
  Handle(TopOpeBRepDS_ShapeShapeInterference) SSI (Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I));
  TopAbs_State TB1,TB2;myPG->StatesON(TB1,TB2); TopAbs_State TB=TB1;
  TopOpeBRepDS_Kind GT,ST;Standard_Integer GI,SI;FDS_data(SSI,GT,GI,ST,SI);
  const TopOpeBRepDS_Transition& TFE=SSI->Transition(); Standard_Boolean EGBoundFOR=SSI->GBound();
  const TopoDS_Face& FOR=TopoDS::Face(myFace); Standard_Integer iFOR=BDS.Shape(FOR);
  const TopoDS_Edge& eON=TopoDS::Edge(EspON);
  const TopoDS_Edge& EG=TopoDS::Edge(BDS.Shape(GI));
#ifdef OCCT_DEBUG
  Standard_Integer iEG=
#endif
          BDS.Shape(EG);
  const TopoDS_Face& FS=TopoDS::Face(BDS.Shape(SI)); Standard_Integer iFS=BDS.Shape(FS);

  Standard_Real tola = Precision::Angular()*1.e3;//nyitol  

  TopAbs_Orientation oFOR = BDS.Shape(iFOR).Orientation();
  TopAbs_Orientation oFS  = BDS.Shape(iFS).Orientation();

  Standard_Boolean isclosedFF=FUN_tool_IsClosingE(EG,FOR,FOR); //xpu240898 : cto900J5 faulty yapc2(FOR17,FS18,EG15)
  Standard_Boolean isclosedFS=FUN_tool_IsClosingE(EG,FS,FS); //xpu240898
  Standard_Boolean isclosed=(isclosedFF || isclosedFS);
  Standard_Boolean isrest=BDS.IsSectionEdge(EG);
#ifdef OCCT_DEBUG
//  Standard_Boolean issplit=myPB->IsSplit(EG,TopAbs_ON);
#endif
  Standard_Integer rankFS=myPB->GShapeRank(FS);
  Standard_Integer rankEG=myPB->GShapeRank(EG);
  Standard_Integer rankFOR=myPB->GShapeRank(FOR);
  TopAbs_Orientation OTFE = TFE.Orientation(TopAbs_IN);
  TopAbs_State TFEbef = TFE.Before();
  TopAbs_State TFEaft = TFE.After();
#ifdef OCCT_DEBUG
//  Standard_Boolean EGboundFOR =
//    Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I)->GBound();
#endif
  Standard_Boolean eghassd = HDS->HasSameDomain(EG);
  TopTools_ListOfShape le3;Standard_Boolean eghassd3d = FDS_HasSameDomain3d(BDS,EG,&le3);  
  Standard_Integer ie3d = 0; TopAbs_Orientation oe3d = TopAbs_EXTERNAL;
  Standard_Integer ie2d = 0; TopAbs_Orientation oe2d = TopAbs_EXTERNAL;
  TopTools_ListOfShape lfcx; FDSCNX_FaceEdgeConnexFaces(FS,EG,HDS,lfcx);
  Standard_Integer nlfcx=lfcx.Extent();

  Standard_Boolean hsdFOR = HDS->HasSameDomain(FOR);
  TopOpeBRepDS_Config cFOR = TopOpeBRepDS_UNSHGEOMETRY;
  Standard_Integer irefFOR = 0; 
  TopAbs_Orientation orefFOR = TopAbs_EXTERNAL; 
  Standard_Boolean FORisref = Standard_False;
  if (hsdFOR) {
    cFOR = BDS.SameDomainOri(iFOR);
    if (hsdFOR) irefFOR = BDS.SameDomainRef(FOR);
    if (irefFOR != 0) orefFOR = BDS.Shape(irefFOR).Orientation();
    FORisref = (irefFOR == iFOR);
  }

  Standard_Boolean opeFus = myPB->Opefus();
  Standard_Boolean opec12 = myPB->Opec12();
#ifdef OCCT_DEBUG
//  Standard_Boolean opec21 = myPB->Opec21();
#endif
  Standard_Boolean opeCut = myPB->Opec12() || myPB->Opec21();
  Standard_Boolean opeCom = myPB->Opecom();
  Standard_Boolean ComOfCut = opeCut && (TB1 == TB2) && (TB1 == TopAbs_IN); //xpu200598 only if FFSDSO

  TopAbs_State TBFOR = FUN_build_TB(myPB,rankFOR);  

#ifdef OCCT_DEBUG
  Standard_Boolean tFOR=myPB->GtraceSPS(iFOR);Standard_Boolean tE=myPB->GtraceSPS(GI);
  Standard_Boolean tEFOR=DEBTEFOR(*myPB,iFOR,GI);
  if (tFOR) debON(iFOR);
#endif 

  Standard_Real factor = 1.e2;   
  Standard_Real tolEG  = BRep_Tool::Tolerance(EG)*factor; //NYITOLXPU
  Standard_Real tolFOR = BRep_Tool::Tolerance(FOR)*factor; //NYITOLXPU
  Standard_Real tolFS = BRep_Tool::Tolerance(FS)*factor; //NYITOLXPU

  Standard_Real f,l; FUN_tool_bounds(eON,f,l); Standard_Real x=0.456189; Standard_Real parON = (1-x)*f + x*l;
  
    
  // xpu150698 : spON(EG)=spON1+spON2+.., spON1 ON EsdEG, spON2 shares no
  // geometry with any other edge.
  // FRA11018 (FOR=f5,EG=e18,EsdEG=e7)           
  Standard_Boolean espONesd = Standard_False; Standard_Integer Iesd=0;// xpu150698
  if (eghassd) espONesd = FUN_ds_ONesd(BDS,GI,EspON,Iesd);// xpu150698 
  Standard_Boolean eONsoEsd = Standard_False;
  if (eghassd && (Iesd != 0)) {     
    const TopoDS_Edge& Esd = TopoDS::Edge(BDS.Shape(Iesd));
    Standard_Boolean ok = FUN_tool_curvesSO(eON,Esd,eONsoEsd);
    if (!ok) return;
  }

  Standard_Boolean eONFS = Standard_True; // xpu240898
  if (eghassd && (!espONesd)) {
    gp_Pnt pON; Standard_Boolean ok = FUN_tool_value(parON,eON,pON);
    if (!ok) return;
    gp_Pnt2d uvFS; Standard_Real d=1.; ok = FUN_tool_projPonboundedF(pON,FS,uvFS,d);
    if (!ok) return;
    Standard_Real tolF = FUN_tool_maxtol(FS); eONFS = (d < tolF);    
  }
  // ------------------------------------
  Standard_Boolean eghassdON = eghassd && espONesd;
  Standard_Boolean eghassd3dON = eghassd3d && espONesd;
  // ------------------------------------

  Standard_Boolean yap00 = Standard_True; //xpu180998 : cto900Q2
  yap00 = yap00 && (!isclosed);
  yap00 = yap00 && (EGBoundFOR);
  yap00 = yap00 && (espONesd);
  yap00 = yap00 && (!hsdFOR); //xpu280998 : PRO14892 (FOR16,FS30,GI20)
  if (yap00) {
    const TopOpeBRepDS_ListOfInterference& lI = BDS.ShapeInterferences(FOR);
    TopOpeBRepDS_ListOfInterference lIcopy; FDS_assign(lI,lIcopy); 
    TopOpeBRepDS_ListOfInterference lIGesd; Standard_Integer nGesd = FUN_selectGIinterference(lIcopy,Iesd,lIGesd);
    if (nGesd != 0) yap00=Standard_False; // ES has interference on G=edge(Iesd)
  }

  Standard_Boolean yap0 = Standard_True;
  yap0 = yap0 && (!isclosed);
  yap0 = yap0 && (EGBoundFOR);
  yap0 = yap0 && (!eghassd);

  Standard_Boolean yap0bis = Standard_True; // xpu160698
  yap0bis = yap0bis && (!isclosed);
  yap0bis = yap0bis && (EGBoundFOR);
  yap0bis = yap0bis && (eghassd); // xpu300798 : cto 902 B7
  yap0bis = yap0bis && (!eghassdON);
  yap0bis = yap0bis && eONFS; //xpu240898 : cto900J3 (e7on_2 NOT ON FS, FOR18,FS17,EG7)
  yap0bis = yap0bis && !ComOfCut; // xpu270798 : boxes240798, f14in,GI=34  
  if ( yap0bis ) {
    yap0 = Standard_True;
  }

  Standard_Boolean hassd3dON = Standard_False;
//  if (isclosed && !EGBoundFOR && eghassd3d) { -xpu110898 -> yapc3
  if (!EGBoundFOR && eghassd3d) { 
    ie3d = ::FUN_findeSD(BDS,eON,EG,FOR,oe3d,3);
    hassd3dON = (ie3d != 0);
  }

  Standard_Boolean yapc1 = Standard_True;
  yapc1 = yapc1 && (isclosed);
  yapc1 = yapc1 && (!EGBoundFOR);
  yapc1 = yapc1 && (eghassd3d);
  yapc1 = yapc1 && hassd3dON; //xpu230798
  
  Standard_Boolean yapc2a = Standard_True;
  yapc2a = yapc2a && (isclosed);
  yapc2a = yapc2a && (!eghassd3d);

  //xpu240798 cto902A3 (iFOR=5,GI=3,iFCX=6) : complement yapc1
  Standard_Boolean yapc2b = Standard_True;  
  yapc2b = yapc2b && (isclosed);
  yapc2b = yapc2b && (!EGBoundFOR);
  yapc2b = yapc2b && (eghassd3d);
  yapc2b = yapc2b && (!hassd3dON);
  
  Standard_Boolean yapc2c = Standard_True;
  yapc2c = yapc2c && (isclosed);
  yapc2c = yapc2c && (EGBoundFOR);
  yapc2c = yapc2c && (eghassd3d);

  Standard_Boolean yapc2 = yapc2a;
  yapc2 = yapc2 || yapc2b || yapc2c;    //xpu240798

  Standard_Boolean yapc3 = Standard_True; 
  yapc3 = yapc3 && (!isclosed);
  yapc3 = yapc3 && (eghassd3d);
  yapc3 = yapc3 && (hassd3dON);
  Standard_Boolean e3closedFOR = Standard_False;
  if (hassd3dON) {
    const TopoDS_Edge& e3d = TopoDS::Edge(BDS.Shape(ie3d));
    e3closedFOR = BRep_Tool::IsClosed(e3d,FOR);
  }
  yapc3 = yapc3 && e3closedFOR; // => !EGBoundFOR
  
  //=========================================
  if ( yap00 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap0 GFillON");
    if (tE) {cout<<"yap00(FOR"<<iFOR<<" FS"<<iFS<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif

    TopAbs_Orientation oeff;
    FUN_tool_orientEinFFORWARD(EG,FOR,oeff);
    Standard_Boolean add = Standard_False;
    if      (oeff == TopAbs_FORWARD)  add = (TFEaft == TB);
    else if (oeff == TopAbs_REVERSED) add = (TFEbef == TB);
    else if (oeff == TopAbs_INTERNAL) add = Standard_True;
    else if (oeff == TopAbs_EXTERNAL) add = Standard_True;
    if (!add) return;

    const TopoDS_Shape& Esd = BDS.Shape(Iesd);
    Standard_Boolean isONFS=Standard_False; TopExp_Explorer ex(FS, TopAbs_EDGE);
    for (; ex.More(); ex.Next())
      if (ex.Current().IsSame(Esd)) {isONFS=Standard_True;break;}
    if (!isONFS) return;
    
    Standard_Real parEG; Standard_Boolean ok = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
    if (!ok) return;
    Standard_Boolean samegeom;  ok = FUN_tool_curvesSO(EG,parEG,eON,samegeom); 
    if (!ok) return;
    TopAbs_Orientation neworiE = oeff; 
    if (!samegeom && (M_FORWARD(neworiE) || M_REVERSED(neworiE))) neworiE = TopAbs::Complement(neworiE);
    
    TopoDS_Shape newE = EspON; 
    newE.Orientation(neworiE);
#ifdef OCCT_DEBUG
    if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap00 GFillON","WES+ EspON ");
    if (tE) FUN_cout(newE);
#endif
    myPWES->AddStartElement(newE);
    return;
  }
  
  //=========================================
  if ( yap0 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap0 GFillON");
    if (tE) {cout<<"yap0(FOR"<<iFOR<<" FS"<<iFS<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif

    TopAbs_Orientation oeff;
    FUN_tool_orientEinFFORWARD(EG,FOR,oeff);
    Standard_Boolean add = Standard_False;
    if      (oeff == TopAbs_FORWARD)  add = (TFEaft == TB);
    else if (oeff == TopAbs_REVERSED) add = (TFEbef == TB);
    else if (oeff == TopAbs_INTERNAL) add = Standard_True;
    else if (oeff == TopAbs_EXTERNAL) add = Standard_True;
    if (!add) return; //xpu100698

    // parEG :
    Standard_Real parEG; Standard_Boolean ok = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
    if (!ok) return;
    // ngFS, ngFOR, xxFOR :
    Standard_Real tolON = Max(tolFS,tolEG);
    tolON *= 1.e2;//*****CAREFULL***** : xpu040998, cto 904 A3 
    gp_Vec ngFS;  ok = FUN_tool_nggeomF(parEG,EG,FS,ngFS,tolON);
    if (!ok) return;
    tolON = Max(tolFOR,tolEG);
    tolON *= 1.e2;//*****CAREFULL***** : xpu040998, cto 904 A3 
    gp_Vec ngFOR; ok = FUN_tool_nggeomF(parEG,EG,FOR,ngFOR,tolON);
    if (!ok) return;
    gp_Dir xxFOR; ok = FUN_tool_getxx(FOR,EG,parEG,ngFOR,xxFOR);
    if (!ok) return;
    // ntFS, ntFOR, ntdot :
    gp_Dir ntFS(ngFS);   if (M_REVERSED(oFS)) ntFS.Reverse();
    gp_Dir ntFOR(ngFOR); if (M_REVERSED(oFOR)) ntFOR.Reverse();   
    Standard_Real ntdot = ntFOR.Dot(ntFS);

    // xpu180698 : Kpart=FOR tangent to FS at EG
    Standard_Boolean Kpart = (OTFE == TopAbs_EXTERNAL) || (OTFE == TopAbs_INTERNAL);
    if (Kpart) {   
      Standard_Boolean Ktg = (Abs(1-Abs(ntdot)) < tola*1.e2);
      Kpart = Ktg;
    }
    if (Kpart) { 
      gp_Pnt2d UVfor,UVfs; ok = FUN_tool_paronEF(EG,parEG,FOR,UVfor,tolFOR);
      if (!ok) return;
      gp_Pnt Pfor,Pfs; FUN_tool_value(UVfor,FOR,Pfor);
      tolON = Max(Max(tolFOR,tolFS),tolEG) * 10.;
      Standard_Real d;

      BRepAdaptor_Surface BSfor(FOR); GeomAbs_SurfaceType STfor = BSfor.GetType(); 
      BRepAdaptor_Surface BSfs(FS);   GeomAbs_SurfaceType STfs = BSfs.GetType(); 
      Standard_Integer kpart = FUN_Kpart(STfor,STfs);
      if (kpart == 0)  {
        // MSV: general case, such as BSpline or Bezier surface or two cylinders.
        //      We should detect if FOR and FS have different curvature in section
        //      by plane perpendicular to EG at point parEG.
        Standard_Real crvFOR,crvFS;
        if (!ComputeFaceCrvtInSec(FOR,UVfor,xxFOR,crvFOR)) return;
        if (!FUN_tool_projPonF(Pfor,FS,UVfs,d)) return;
        if (!ComputeFaceCrvtInSec(FS,UVfs,xxFOR,crvFS)) return;
        if (M_REVERSED(oFOR)) crvFOR = -crvFOR;
        if (M_REVERSED(oFS)) crvFS = -crvFS;
        if (ntdot < 0.) crvFS = -crvFS;
        Standard_Real eps = Precision::Confusion();
        Standard_Real absCrvFOR = Abs(crvFOR), absCrvFS = Abs(crvFS);
        if (absCrvFOR <= eps && absCrvFS <= eps) return;
        if (absCrvFOR > eps && absCrvFS > eps) {
          Standard_Real tolR = tolON;
          Standard_Real rcrvFOR = 1./crvFOR, rcrvFS = 1./crvFS;
          if (Abs(rcrvFOR - rcrvFS) <= tolR) return;
        }
        // if we are here the curvatures are different
      }
      // SO :
      Standard_Boolean SO = (ntdot > 0.);
      // Pfor :
      Pfor.Translate(xxFOR.XYZ()*tolON*10.);
      ok = FUN_tool_projPonF(Pfor,FOR,UVfor,d);
      if (!ok) return;
      FUN_tool_value(UVfor,FOR,Pfor); 
      // Pfs :
      ok = FUN_tool_projPonF(Pfor,FS,UVfs,d);
      if (!ok) return;
      FUN_tool_value(UVfs,FS,Pfs); 
      // Dforfs :
      gp_Dir Dforfs(gp_Vec(Pfor,Pfs));

      Standard_Boolean keep = Standard_True;
      Standard_Real dot = Dforfs.Dot(ntFOR); // xpu250698
      if (SO) {
	Standard_Boolean so2and3 = (dot > 0);     // xpu250698
	Standard_Boolean so2 = so2and3;           // xpu250698
	
	if (opeFus) keep = !so2;
	if (opeCom) keep = so2;
	if (opeCut) {
	  if (TBFOR == TopAbs_OUT) keep = !so2;
	  else                     keep = so2;
	}
      } // SO
      else {
	Standard_Boolean do1and2 = (dot > 0);     // xpu250698
	Standard_Boolean do1 = do1and2;           // xpu250698

	if (opeFus) keep = do1;
	if (opeCom) keep = !do1;
	if (opeCut) {
	  if (TBFOR == TopAbs_OUT) keep = do1;
	  else                     keep = !do1;
	}		
      } // !SO
      if (!keep) return;
    } //Kpart
    else {
      // dot :
      Standard_Real dot = ntFS.Dot(xxFOR);      
      Standard_Boolean positive = (dot > 0);

      Standard_Boolean keep = Standard_True;
      if (opeFus) {
	keep = positive;
      }
      if (opeCom) {
	keep = !positive;
      }
      if (opeCut) {
	if (positive) keep = (TBFOR == TopAbs_OUT);
	else          keep = (TBFOR != TopAbs_OUT);
      }
      if (!keep) return;
    }    

    TopoDS_Face newF = FOR;
    TopoDS_Shape newE = EspON;
    TopAbs_Orientation neworiE; FUN_tool_orientEinFFORWARD(EG,newF,neworiE);

    if (hsdFOR) {
      //xpu260698 : cto902A5 : fonds de poche (FOR=f14,GI=e24,refFOR=f13)
      Standard_Boolean reverse = Standard_False;
      if (!FORisref) {	
	// xpu170698 : PRO13555 (FOR=f19,GI=e10,FS=f15)
	Standard_Boolean FORDO = (cFOR == TopOpeBRepDS_DIFFORIENTED);
	if (FORDO) reverse = !reverse; 
	if (oFOR != orefFOR) reverse = !reverse;
      }  
         
      if (opeFus) {//nyixpu260698
      }
      if (opeCom) {//nyixpu260698
      }
      if (opeCut) {
	if (TBFOR == TopAbs_IN) {
	  // xpu290798 : cto902C1 (FOR19,FS16,GI22) f9ou is DO f19in
	  //    sp(f9)+sp(f19) are in same WES, e22on is OUT f9 => 
	  //    do NOT reverse  neworiE      
//	  reverse = Standard_True;
	}
	else {} 
      }
      if (reverse) neworiE = TopAbs::Complement(neworiE); 
    } // hsdFOR

    newE.Orientation(neworiE);
#ifdef OCCT_DEBUG
    if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap0 GFillON","WES+ EspON ");
    if (tE) FUN_cout(newE);
#endif
    myPWES->AddStartElement(newE);
    return;
  } //yap0
  
  //=========================================
  if ( yapc1 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yapc1 GFillON");
    if (tE) {cout<<"yapc1(FOR"<<iFOR<<" FS"<<iFS<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif
    const TopoDS_Edge& e3d = TopoDS::Edge(BDS.Shape(ie3d));
    
    // e3d = SameDomain3d de EG, arete de FOR, qui contient EspON
    TopOpeBRepDS_Config cf; Standard_Boolean cfok = FDS_Config3d(EspON,e3d,cf);
    if (!cfok) return;

    TopAbs_Orientation oe3dk = oe3d;
//    Standard_Boolean samegeom = ::TopOpeBRepBuild_FUN_aresamegeom(EG,e3d);
    Standard_Real parEG; Standard_Boolean ok = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
    if (!ok) return; //nyiRAISE
    Standard_Boolean samegeom; ok = FUN_tool_curvesSO(EG,parEG,e3d,samegeom);
    if (!ok) return; //nyiRAISE
#ifdef OCCT_DEBUG
    if (!TopOpeBRepBuild_GetcontextNOSG()) {
#endif
      if (!samegeom) oe3dk = TopAbs::Complement(oe3dk);
#ifdef OCCT_DEBUG
    }
#endif

    Standard_Boolean keep3d = Standard_False;
    if      (oe3dk == TopAbs_FORWARD)  keep3d = (TFEaft == TB1);
    else if (oe3dk == TopAbs_REVERSED) keep3d = (TFEbef == TB1);
    else if (oe3dk == TopAbs_INTERNAL) keep3d = Standard_True;
    else if (oe3dk == TopAbs_EXTERNAL) keep3d = Standard_False;

#ifdef OCCT_DEBUG
//    if(tEFOR) {cout<<endl<<"yapc1 keep3d : "<<keep3d<<endl;debfillonfemess3d(iFOR,iEG);}
#endif

    if (keep3d) {
      TopAbs_Orientation neworiE = oe3d;
      if (cf == TopOpeBRepDS_DIFFORIENTED) neworiE = TopAbs::Complement(neworiE);
      TopoDS_Shape newE = EspON;
      newE.Orientation(neworiE);
      
#ifdef OCCT_DEBUG
      if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yapc1 GFillON","WES+ EspON ");
      if (tE) FUN_cout(newE);
#endif
      myPWES->AddStartElement(newE);
    }

    return;
  } // yapc1

  //=========================================
  if ( yapc2 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yapc2 GFillON");
    if (tE) {cout<<"yapc2(FOR"<<iFOR<<" FS"<<iFS<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif

    Standard_Boolean keep = Standard_False;
    if (EGBoundFOR) keep=FUN_keepEON(*myPB,EG,FOR,FS,EGBoundFOR,TFE,TB1,TB2);
    else            keep=Standard_True;
    if ( !keep ) return;
    
    TopAbs_Orientation neworiE = TFE.Orientation(TB1);
    Standard_Boolean giveoEinFOR = EGBoundFOR && (!isclosedFF) && (M_EXTERNAL(neworiE) || M_INTERNAL(neworiE));
    if (giveoEinFOR) FUN_tool_orientEinFFORWARD(EG,FOR,neworiE);

    // xpu230798 : cto902A3 (iFOR=5,GI=3,iFCX=6) 
    //                      (iFOR=20,GI=3,iFCX=6)    
    if (yapc2b && (!M_EXTERNAL(neworiE))){
      TopOpeBRepTool_ShapeClassifier& PSC = FSC_GetPSC(FOR);
      TopAbs_State state2d = FSC_StateEonFace(EspON,0.345,FOR,PSC);
      if (state2d != TopAbs_IN) return;
    }

    TopoDS_Shape newE=EspON;
    Standard_Boolean addFORREV = Standard_False;
    TopAbs_Orientation neworiEk = TopAbs_EXTERNAL;
    if      (!isclosedFF)                               neworiEk = neworiE;
    else if (M_FORWARD(neworiE) || M_REVERSED(neworiE)) neworiEk = neworiE;
    else if (M_INTERNAL(neworiE))                       addFORREV = Standard_True;
    newE.Orientation(neworiEk);    

#ifdef OCCT_DEBUG
    if (tEFOR) {
      if      (!isclosedFF) 
	debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yapc2 GFillON","WES+ EspON not closed");
      else if (M_FORWARD(neworiE) || M_REVERSED(neworiE))
	debaddpwes(iFOR,TB1,iEG,neworiEk,myPB,myPWES,"yapc2 GFillON closed","WES+ EspON F|R");
      else if (M_INTERNAL(neworiE)) {
	debaddpwes(iFOR,TB1,iEG,TopAbs_FORWARD, myPB,myPWES,"yapc2 GFillON closed","WES+ EspON INTERNAL");
	debaddpwes(iFOR,TB1,iEG,TopAbs_REVERSED,myPB,myPWES,"yapc2 GFillON closed","WES+ EspON INTERNAL");
      }      
    }
    if (tE) {
      if (addFORREV) {
	newE.Orientation(TopAbs_FORWARD);  FUN_cout(newE);
	newE.Orientation(TopAbs_REVERSED); FUN_cout(newE);
      }
      else FUN_cout(newE);
    }
#endif

    if (addFORREV) { 
      newE.Orientation(TopAbs_FORWARD);  
      myPWES->AddStartElement(newE);

      newE.Orientation(TopAbs_REVERSED); 
      myPWES->AddStartElement(newE); 
    }
    else {
      myPWES->AddStartElement(newE);
    }
    return;
  } // yapc2

  //=========================================
  if ( yapc3 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yapc3 GFillON");
    if (tE) {cout<<"yapc3(FOR"<<iFOR<<" FS"<<iFS<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif

    Standard_Boolean keep = Standard_False;
    if (EGBoundFOR) keep=FUN_keepEON(*myPB,EG,FOR,FS,EGBoundFOR,TFE,TB1,TB2);
    else            keep=Standard_True;
    if ( !keep ) return;
    
    TopAbs_Orientation neworiE = TFE.Orientation(TB1);
    Standard_Boolean giveoEinFOR = EGBoundFOR && !isclosedFF && (M_EXTERNAL(neworiE) || M_INTERNAL(neworiE));
    if (giveoEinFOR) FUN_tool_orientEinFFORWARD(EG,FOR,neworiE);
    
    TopoDS_Shape newE=EspON;
    Standard_Boolean addFORREV = Standard_False;
    TopAbs_Orientation neworiEk = TopAbs_EXTERNAL;     

    // xpu110798 : cto902B4 (FOR6,EG15,FS17)
    if (M_INTERNAL(OTFE) || M_EXTERNAL(OTFE)) {
      Standard_Boolean SO = Standard_False; // "approximate same oriented (FOR,FS)"
      // parEG :
      Standard_Real parEG; Standard_Boolean ok = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
      if (!ok) return;// nyiRAISE
      // ntFS : 
      gp_Pnt2d uvFS;  ok = FUN_tool_paronEF(EG,parEG,FS,uvFS,tolFS); // !EGBoundFOR
      if (!ok) return;// nyiRAISE
      gp_Vec ngFS = FUN_tool_nggeomF(uvFS,FS);
      gp_Dir ntFS(ngFS); if (M_REVERSED(oFS)) ntFS.Reverse();
      // ntFOR :
      gp_Pnt2d uvFOR; ok = FUN_tool_parF(EG,parEG,FOR,uvFOR,tolFOR);
      if (!ok) return;// nyiRAISE
      gp_Vec ngFOR = FUN_tool_nggeomF(uvFOR,FOR);
      gp_Dir ntFOR(ngFOR); if (M_REVERSED(oFOR)) ntFOR.Reverse();
      
      Standard_Real dot = ntFOR.Dot(ntFS);
      Standard_Boolean nulldot = (Abs(dot) < tola);
      if (nulldot) {
	// xxFS :
	gp_Dir xxFS;  ok = FUN_tool_getxx(FS,EG,parEG,ngFS,xxFS);
	if (!ok) return;// nyiRAISE 
	Standard_Real dot2 = ntFOR.Dot(xxFS);
	SO = (dot2 < 0.);
      }
      else {
	SO = (dot > 0.);
      }
      
      Standard_Boolean unkeepclo = Standard_False;
      if (SO && M_EXTERNAL(OTFE) && opeCom) unkeepclo = Standard_True;
      if (SO && M_INTERNAL(OTFE) && opeFus) unkeepclo = Standard_True;
      addFORREV = !unkeepclo;
    }

    if (!addFORREV) {
      neworiEk = neworiE;
      Standard_Boolean samegeom; Standard_Boolean ok = FUN_tool_curvesSO(eON,EG,samegeom);
      if (!ok) return; // nyiRAISE
      Standard_Boolean reverse = (!samegeom);
#ifdef OCCT_DEBUG
      if (TopOpeBRepBuild_GetcontextNOSG()) 
        // MSV 21.03.2002: restore the genaral behaviour, since the function
        //                 FUN_UNKFstasta was corrected.
	reverse = Standard_False; //we exclude this line from #ifdef OCCT_DEBUG because 
      //in optimised mode this line will never be included , and that follows to regressions
      //MZV-12-05-2000
#endif

      if (reverse) neworiEk = TopAbs::Complement(neworiEk);
      newE.Orientation(neworiEk);
    }

#ifdef OCCT_DEBUG
    if (tEFOR) {
      if      (!addFORREV)
	debaddpwes(iFOR,TB1,iEG,neworiEk,myPB,myPWES,"yapc3 GFillON","WES+ EspON ");
      else {
	debaddpwes(iFOR,TB1,iEG,TopAbs_FORWARD, myPB,myPWES,"yapc3 GFillON closed","WES+ EspON ");
	debaddpwes(iFOR,TB1,iEG,TopAbs_REVERSED,myPB,myPWES,"yapc3 GFillON closed","WES+ EspON ");
      }
    }
#endif

    if (addFORREV) { 
      newE.Orientation(TopAbs_FORWARD);  myPWES->AddStartElement(newE);
      newE.Orientation(TopAbs_REVERSED); myPWES->AddStartElement(newE); 
    }
    else {
      myPWES->AddStartElement(newE);
    }
    return;
  } // yapc3

  if (nlfcx == 0) return;
  const TopoDS_Face& FCX=TopoDS::Face(lfcx.First()); Standard_Integer iFCX=BDS.Shape(FCX);
  
  // faces samedomain de FCX
  TopTools_ListOfShape LFSO,LFDO,LFSO1,LFDO1,LFSO2,LFDO2;
  myPB->GFindSamDomSODO(FCX,LFSO,LFDO);
  Standard_Integer rankFCX=myPB->GShapeRank(FCX),rankX=(rankFCX)?((rankFCX==1)?2:1):0;
  // DEB : rankFCX doit etre=rankFS
  
  // LFSO2,LFDO2=faces samedomain de FCX dans le shape oppose (rankX) 
  // rankX=shape oppose=shape de FOR.
  myPB->GFindSameRank(LFSO,rankFCX,LFSO1); myPB->GFindSameRank(LFDO,rankFCX,LFDO1);
  myPB->GFindSameRank(LFSO,rankX,LFSO2);   myPB->GFindSameRank(LFDO,rankX,LFDO2);
  
  // FFinSO=appartenance de FOR a l'ensemble des faces sameoriented de FCX=LFSO2
  Standard_Boolean FFinSDSO=Standard_False;
  {
    for(TopTools_ListIteratorOfListOfShape i(LFSO2);i.More();i.Next()) {
      const TopoDS_Shape& F=i.Value();
      if (F.IsSame(FOR)) {FFinSDSO=Standard_True;break;}
    }
  }
  // FFinDO=appartenance de FOR a l'ensemble des faces difforiented de FCX=LFDO2
  Standard_Boolean FFinSDDO=Standard_False;
  {
    for(TopTools_ListIteratorOfListOfShape i(LFDO2);i.More();i.Next()) {
      const TopoDS_Shape& F=i.Value();
      if (F.IsSame(FOR)) {FFinSDDO=Standard_True;break;}
    }
  }
  // FFinSD=appartenance de FOR a l'ensemble des faces samedomain de FCX
  Standard_Boolean FFinSD=(FFinSDSO || FFinSDDO);

  TopAbs_Orientation oFCX = BDS.Shape(iFCX).Orientation();
  TopOpeBRepDS_Config cFCX = BDS.SameDomainOri(iFCX);
  Standard_Integer irefFCX = BDS.SameDomainRef(FCX);
  TopAbs_Orientation orefFCX = BDS.Shape(irefFCX).Orientation();
  Standard_Boolean FCXisref = (irefFCX == iFCX);

#ifdef OCCT_DEBUG
//  Standard_Real tolFCX = factor*BRep_Tool::Tolerance(FCX); //NYITOLXPU
#endif
    
  TopAbs_Orientation oegFCXF;Standard_Boolean EGBoundFCX = FUN_tool_orientEinFFORWARD(EG,FCX,oegFCXF);
  TopAbs_Orientation oegFCX ;
  FUN_tool_orientEinF(EG,FCX,oegFCX);

  if (!EGBoundFOR && !espONesd) { //xpu220998 : ctocylcongA1 (FOR10,FS19,EG8)
    gp_Pnt ptON; Standard_Boolean ok = FUN_tool_value(parON,eON,ptON);
    if (!ok) return; //nyiRAISE
    Standard_Real d=1.; gp_Pnt2d uvFOR; ok = FUN_tool_projPonboundedF(ptON,FOR,uvFOR,d);
    if (!ok) return; //nyiRAISE
    Standard_Real tolON = Max(tolEG,tolFOR);//xpu291098 cto900L7(f7,e7on)
                                  //xpu051198 PRO12953(f6,e4on)
    tolON *= 1.e2;//*****CAREFULL***** : xpu040998, cto 904 A3 
    Standard_Boolean eONFOR = (d < tolON);
    if (!eONFOR) return; 
  }

  Standard_Boolean yap1 = Standard_True;
  yap1 = yap1 && FFinSD; 
  yap1 = yap1 && (!EGBoundFOR);
  yap1 = yap1 && EGBoundFCX;
  yap1 = yap1 && eghassd3dON;

  Standard_Boolean yap2 = Standard_True;
  yap2 = yap2 && FFinSD; 
  yap2 = yap2 && (!EGBoundFOR);
  yap2 = yap2 && EGBoundFCX;//  yap2 = yap2 && !eghassdON;
  yap2 = yap2 && !eghassd;//  yap2 = yap2 && !eghassd3dON;
  
  Standard_Boolean yap1b = Standard_True;
  yap1b = yap1b && FFinSD; 
  yap1b = yap1b && (!EGBoundFOR);
  yap1b = yap1b && EGBoundFCX;
  yap1b = yap1b && eghassd3d;
  yap1b = yap1b && !eghassd3dON; 
  yap1b = yap1b && eONFS; //xpu240898
  if (yap1b) {
    yap2 = Standard_True; // xpu220998 : ctocylcongA1 yap1b(FOR12,FS5,EG8)
  }
      
  Standard_Boolean yap6 = Standard_True; // cto001F3 : f18ou,EG=e23
  yap6 = yap6 && FFinSD; 
  yap6 = yap6 && (!EGBoundFOR);
  yap6 = yap6 && EGBoundFCX;
  yap6 = yap6 && eghassdON;
  yap6 = yap6 && !eghassd3d;//  yap6 = yap6 && !eghassd3dON;

  Standard_Boolean yap6b = Standard_True;
  yap6b = yap6b && FFinSD; 
  yap6b = yap6b && (!EGBoundFOR);
  yap6b = yap6b && EGBoundFCX;
  yap6b = yap6b && eghassd;
  yap6b = yap6b && !eghassdON;
  yap6b = yap6b && eONFS; //xpu240898
  yap6b = yap6b && !eghassd3d;//  yap6b = yap6b && !eghassd3dON; 
  if (yap6b) {
#ifdef OCCT_DEBUG
//    FUN_RaiseON();
#endif
    yap2 = Standard_True;
  }
 
  // CTS20205 spOUT(f30), e4 = eghassd3dON
  // mais f30 !sdm lfcx(e4)
  Standard_Boolean eghassd3fcx = eghassdON;
  eghassd3fcx = eghassd3fcx && !FUN_ds_sdm(BDS,FOR,FS);
  eghassd3fcx = eghassd3fcx && !FFinSD; //!FUN_ds_sdm(BDS,FOR,FCX)
  Standard_Boolean yap5 = Standard_True; 
  yap5 = yap5 && !EGBoundFOR;
  yap5 = yap5 && eghassd3fcx;

  Standard_Boolean yap3 = Standard_True;
  yap3 = yap3 && !FFinSD; 
  yap3 = yap3 && !EGBoundFOR;
  yap3 = yap3 && eghassd3dON;   
  
  Standard_Boolean yap4 = Standard_True;
  yap4 = yap4 && !FFinSD;
  yap4 = yap4 && isrest;
  yap4 = yap4 && !yap5;
  yap4 = yap4 && !hsdFOR; //xpu290598

  Standard_Boolean yap3b = Standard_True;
  yap3b = yap3b && !FFinSD; 
  yap3b = yap3b && !EGBoundFOR;
  yap3b = yap3b && eghassd3d;   
  yap3b = yap3b && !eghassd3dON;   
  yap3b = yap3b && eONFS; //xpu240898
  if (yap3b) {
    yap4 = Standard_True; //xpu191098 : cto016F3(yap3b(FOR7,EG4))
  }
  
  //xpu290598 
  Standard_Boolean yap7 = Standard_True;
  yap7 = yap7 && !FFinSD;
  yap7 = yap7 && isrest;
  yap7 = yap7 && !yap5;
  yap7 = yap7 && hsdFOR;

  TopAbs_State staFOR = TB1;
  TopAbs_State staFS = (rankFS == rankFOR) ? TB1 : TB2;
  
#ifdef OCCT_DEBUG
//  if(tEFOR) cout<<endl<<"yap1 yap2 yap3 yap4 = ";
//  if(tEFOR) cout<<yap1<<" "<<yap2<<" "<<yap3<<" "<<yap4<<endl<<endl;
//  if(tEFOR) debfillonfemess(iFOR,iEG);
#endif
  
  //=========================================
  if ( yap1 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap1 GFillON");
    if (tE) {cout<<"yap1(FOR"<<iFOR<<" FCX"<<iFCX<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
    
#endif
    
    // FF est samedomain avec FCX
    // on evalue la transition de FOR par rapport a la matiere 2d de la face FCX
    // au lieu de la transition par rapport a la matiere 3d de la face FS
    // EG est une arete de FCX, oegFCXF=O.T. de EG dans FCX orientee FORWARD
    // EG a des aretes 3d same domain : le3
    
    TopAbs_State staFCX = staFS; // FS et FCX connexes par EG => meme shape origine => meme etat
    Standard_Boolean b3d = Standard_False; Standard_Boolean b2d = Standard_False;

    // staFOR : etat demande sur FOR
    Standard_Boolean b3de3 = Standard_False; Standard_Boolean b2de3 = Standard_False;

    const TopoDS_Edge& e3 = TopoDS::Edge(le3.First()); Standard_Integer ie3 = BDS.Shape(e3);    
    Standard_Boolean ssif = Standard_False; Handle(TopOpeBRepDS_ShapeShapeInterference) ssie3;
    TopOpeBRepDS_ListIteratorOfListOfInterference itssi(BDS.ShapeInterferences(FCX));
    for (;itssi.More();itssi.Next()) {
      Handle(TopOpeBRepDS_ShapeShapeInterference) ssi (Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(itssi.Value())); if (ssi.IsNull()) continue;
      TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(ssi,GT1,G1,ST1,S1);
      Standard_Boolean cond = (GT1 == TopOpeBRepDS_EDGE && ST1 == TopOpeBRepDS_FACE);
      cond = cond && (G1 == ie3); 
      // NYI cond = cond && e(S1 est une face connexe a iFOR par ie3)
      if (cond) { ssif = Standard_True; ssie3 = ssi; break; }
    }

    TopOpeBRepDS_Transition TFEe3; TopAbs_State TFEe3bef = TFEe3.Before(); TopAbs_State TFEe3aft = TFEe3.After();
    TopAbs_Orientation oe3FORF = TopAbs_FORWARD;
    if (ssif) {
      TFEe3 = ssie3->Transition();
      TFEe3bef = TFEe3.Before();
      FUN_tool_orientEinFFORWARD(e3,FOR,oe3FORF);	
    }

    if (FFinSDSO) {
      if ( oFOR != oFCX ) {
	oegFCXF = TopAbs::Complement(oegFCXF);
	oe3FORF = TopAbs::Complement(oe3FORF);
      }      
      TopOpeBRepDS_Transition T1(oegFCXF); TopAbs_State T1aft = T1.After(); TopAbs_State T1bef = T1.Before();
      if      (oegFCXF == TopAbs_FORWARD)  b3d = (TFEaft == staFCX);
      else if (oegFCXF == TopAbs_REVERSED) b3d = (TFEbef == staFCX);
      else    FUN_coutmess("DEBUG GFillONPartsWES2_1 orientation != F,R\n");
      if      (oegFCXF == TopAbs_FORWARD)  b2d = (T1aft == TopAbs_IN);
      else if (oegFCXF == TopAbs_REVERSED) b2d = (T1bef == TopAbs_IN);
      else    FUN_coutmess("DEBUG GFillONPartsWES2_2 orientation != F,R\n");
      
      if (ssif) {
	TopOpeBRepDS_Transition T11(oe3FORF); TopAbs_State T11aft = T11.After(); TopAbs_State T11bef = T11.Before();	
	if      (oe3FORF == TopAbs_FORWARD)  b3de3 = (TFEe3aft == staFOR);
	else if (oe3FORF == TopAbs_REVERSED) b3de3 = (TFEe3bef == staFOR);
	else    FUN_coutmess("DEBUG GFillONPartsWES2_1 orientation != F,R\n");
	if      (oe3FORF == TopAbs_FORWARD)  b2de3 = (T11aft == TopAbs_IN);
	else if (oe3FORF == TopAbs_REVERSED) b2de3 = (T11bef == TopAbs_IN);
	else    FUN_coutmess("DEBUG GFillONPartsWES2_2 orientation != F,R\n");
      }
    }
    
    if (FFinSDDO) {
      if ( oFOR == oFCX ) {
	oegFCXF = TopAbs::Complement(oegFCXF);
	oe3FORF = TopAbs::Complement(oe3FORF);
      }    
      TopOpeBRepDS_Transition T2(oegFCXF); TopAbs_State Taft = T2.After(); TopAbs_State Tbef = T2.Before();
      if      (oegFCXF == TopAbs_FORWARD)  b3d = (TFEaft == staFCX);
      else if (oegFCXF == TopAbs_REVERSED) b3d = (TFEbef == staFCX);
      else    FUN_coutmess("DEBUG GFillONPartsWES2_3 orientation != F,R\n");
      if      (oegFCXF == TopAbs_FORWARD)  b2d = (Taft == TopAbs_IN);
      else if (oegFCXF == TopAbs_REVERSED) b2d = (Tbef == TopAbs_IN);
      else    FUN_coutmess("DEBUG GFillONPartsWES2_4 orientation != F,R\n");

      if (ssif) {
	TopOpeBRepDS_Transition T22(oe3FORF); TopAbs_State T22aft = T22.After(); TopAbs_State T22bef = T22.Before();
	if      (oe3FORF == TopAbs_FORWARD)  b3de3 = (TFEe3aft == staFOR);
	else if (oe3FORF == TopAbs_REVERSED) b3de3 = (TFEe3bef == staFOR);
	else    FUN_coutmess("DEBUG GFillONPartsWES2_3 orientation != F,R\n");
	if      (oe3FORF == TopAbs_FORWARD)  b2de3 = (T22aft == TopAbs_IN);
	else if (oe3FORF == TopAbs_REVERSED) b2de3 = (T22bef == TopAbs_IN);
	else    FUN_coutmess("DEBUG GFillONPartsWES2_4 orientation != F,R\n");
      }
    }
    
    Standard_Boolean b = (b3d && b2d && b3de3 && b2de3);    
    if (!b) return;
    
    // on garde EspON pour la reconstruction de la face FOR
    // si elle est IN/ON la face FOR (EGBoundFOR = 0 ==> projection)
    TopOpeBRepTool_ShapeClassifier& PSC = FSC_GetPSC(FOR);
    TopAbs_State state2d = FSC_StateEonFace(EspON,0.345,FOR,PSC);
    Standard_Boolean isin = (state2d == TopAbs_IN);
    if (!isin) return;
    
    TopAbs_State TBEG = TB1;
    TopAbs_Orientation neworiE = TFE.Orientation(TBEG);
    
    //  if (FCXisref && FFinSDDO) {
    if      (FCXisref && !EGBoundFOR) {
      FUN_tool_orientEinFFORWARD(EG,FCX,neworiE);
      Standard_Boolean rev = myPB->Reverse(staFCX,staFOR);
      neworiE = myPB->Orient(neworiE,rev);
    }
//    xpu280798 : never occurs as yap1 -> !EGBoundFOR
//    else if (FORisref && EGBoundFOR) {
//      FUN_tool_orientEinFFORWARD(EG,FOR,neworiE);
//      Standard_Boolean rev = myPB->Reverse(staFOR,staFCX);
//      neworiE = myPB->Orient(neworiE,rev);
//    } 
    else if (FORisref && !EGBoundFOR) {   
      const TopoDS_Edge& Esd = TopoDS::Edge(BDS.Shape(Iesd));
      FUN_tool_orientEinFFORWARD(Esd,TopoDS::Face(FOR),neworiE);
      Standard_Boolean reverse = (M_FORWARD(neworiE) || M_REVERSED(neworiE)) && (!eONsoEsd);
      if (reverse) neworiE = TopAbs::Complement(neworiE);
      Standard_Boolean rev = myPB->Reverse(staFOR,staFCX);
      neworiE = myPB->Orient(neworiE,rev);
    }
    else if (!EGBoundFOR) { // xpu210898 
      Standard_Integer rankrefFOR = BDS.AncestorRank(irefFOR);
      if (rankrefFOR == rankFCX) 
	FUN_tool_orientEinFFORWARD(EG,FCX,neworiE);
      else if (rankrefFOR == rankFOR) {     
	const TopoDS_Edge& Esd = TopoDS::Edge(BDS.Shape(Iesd));
	FUN_tool_orientEinFFORWARD(Esd,TopoDS::Face(FOR),neworiE);
	Standard_Boolean reverse = (M_FORWARD(neworiE) || M_REVERSED(neworiE)) && (!eONsoEsd);
	if (reverse) neworiE = TopAbs::Complement(neworiE);	
      }
      Standard_Boolean rev = myPB->Reverse(staFOR,staFCX);
      neworiE = myPB->Orient(neworiE,rev);      
    }
    
    TopoDS_Shape newE = EspON;
    newE.Orientation(neworiE);
    
#ifdef OCCT_DEBUG
    if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap1 GFillON","WES+ EspON ");
    if (tE) FUN_cout(newE);
#endif
    myPWES->AddStartElement(newE);

    return;
  } // yap1

  //=========================================
  if ( yap2 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap2 GFillON");
    if (tE) {cout<<"yap2(FOR"<<iFOR<<" FCX"<<iFCX<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif
    
    // FF est samedomain avec FCX
    // on evalue la transition de FOR par rapport a la matiere 2d de la face FCX
    // au lieu de la transition par rapport a la matiere 3d de la face FS
    // EG est une arete de FCX, oegFCXF=O.T. de EG dans FCX orientee FORWARD

    TopAbs_State staFCX = staFS; // FS et FCX connexes par EG => meme shape origine => meme etat
    Standard_Boolean b3d = Standard_False; Standard_Boolean b2d = Standard_False;
    Standard_Boolean b = (b3d && b2d);

    if (FFinSDSO) {
      if ( oFOR != oFCX ) {
	// orientation topologique de EG dans la face mere
	oegFCXF = TopAbs::Complement(oegFCXF);
      }    
      TopOpeBRepDS_Transition T1(oegFCXF); TopAbs_State Taft = T1.After(); TopAbs_State Tbef = T1.Before();
      if      (oegFCXF == TopAbs_FORWARD)  b3d = (TFEaft == staFCX);
      else if (oegFCXF == TopAbs_REVERSED) b3d = (TFEbef == staFCX);
      else    FUN_coutmess("DEBUG GFillONPartsWES2_1 orientation != F,R\n");
      if      (oegFCXF == TopAbs_FORWARD)  b2d = (Taft == TopAbs_IN);
      else if (oegFCXF == TopAbs_REVERSED) b2d = (Tbef == TopAbs_IN);
      else    FUN_coutmess("DEBUG GFillONPartsWES2_2 orientation != F,R\n");
    }
    if (FFinSDDO) {
      if ( oFOR == oFCX ) {
	// orientation topologique de EG dans la face mere
	oegFCXF = TopAbs::Complement(oegFCXF);
      }    
      TopOpeBRepDS_Transition T2(oegFCXF); TopAbs_State Taft = T2.After(); TopAbs_State Tbef = T2.Before();
      if      (oegFCXF == TopAbs_FORWARD)  b3d = (TFEaft == staFCX);
      else if (oegFCXF == TopAbs_REVERSED) b3d = (TFEbef == staFCX);
      else    FUN_coutmess("DEBUG GFillONPartsWES2_3 orientation != F,R\n");
      if      (oegFCXF == TopAbs_FORWARD)  b2d = (Taft == TopAbs_IN);
      else if (oegFCXF == TopAbs_REVERSED) b2d = (Tbef == TopAbs_IN);
      else    FUN_coutmess("DEBUG GFillONPartsWES2_4 orientation != F,R\n");
    }    
//#ifdef OCCT_DEBUG
//    if(tEFOR) {cout<<endl<<"yap2 : b3d,b2d "<<b3d<<","<<b2d<<endl;debfillonfemess(iFOR,iEG);}
//#endif
    
    // bcl1;bcl2; c12;tsp(f9),tspON(e7)
    if (ComOfCut) {b3d=Standard_True; b2d=Standard_True;} //xpu200598
     
    gp_Vec ngFS,ngFOR;
    Standard_Real parEG = 0.;
    // bcl1;bcl2; tsp(f9),tspON(e7)
    Standard_Boolean sdm = FUN_ds_sdm(BDS,BDS.Shape(iFOR),BDS.Shape(iFS));
    Standard_Boolean isfafa = sdm; // such interferences are computed IN fafa case
    if (!isfafa ) {
      // parEG :
      Standard_Boolean ok = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
      if (!ok) return;
      // xxFS :
      gp_Pnt2d uvFS;  ok = FUN_tool_parF(EG,parEG,FS,uvFS,tolFS);
      if (!ok) return;// nyiRAISE
      ngFS = FUN_tool_nggeomF(uvFS,FS);
      gp_Dir xxFS;    ok = FUN_tool_getxx(FS,EG,parEG,ngFS,xxFS);
      if (!ok) return;// nyiRAISE      
      // ntFOR :
      gp_Pnt2d uvFOR; ok = FUN_tool_parF(EG,parEG,FOR,uvFOR,tolFOR);
      if (!ok) return;// nyiRAISE
      ngFOR = FUN_tool_nggeomF(uvFOR,FOR);
      gp_Dir ntFOR(ngFOR); if (M_REVERSED(oFOR)) ntFOR.Reverse();
      
      Standard_Real dot = xxFS.Dot(ntFOR);
      //xpu040698 :  FS tg to FCX at EG - CTS20578 (FOR=F8,EG=E6,FS=F4) -
      Standard_Boolean nulldot = (Abs(dot) < tola);
      if (nulldot) { // approximate xxFS :
	gp_Pnt2d newuvFS;
	gp_Vec2d dxx; ok = FUN_tool_getdxx(FS,EG,parEG,dxx);
	if (!ok) return;
	newuvFS = uvFS.Translated(dxx * 0.25);	  
	gp_Dir newngFS = FUN_tool_nggeomF(newuvFS,FS);
	ok = FUN_tool_getxx(FS,EG,parEG,newngFS,xxFS);
	if (!ok) return;
	dot = xxFS.Dot(ntFOR);
      }
      //xpu040698

      TopAbs_State TBFS = FUN_build_TB(myPB,rankFS);
      Standard_Boolean positive = (dot > 0);
      
      Standard_Boolean keep = Standard_True;
      if (FFinSDSO) {
	if (opeFus) keep = positive; //xpu090698 : PRO14033 (iFOR=15,GI=10,iFS=9)
	if (opeCom) keep = Standard_True;
	if (opeCut) {
	  if (positive) keep = (TBFS == TopAbs_OUT); //xpu230698 cto100G1
	  else          keep = (TBFS == TopAbs_IN);
	}	
      }
      if (FFinSDDO) {
	if (opeFus) {
	  keep = positive;
	  //xpu050698 : (fuse = OU/IN + IN/OU) -CTS20578-
	  if (keep) {
	    Standard_Integer rkOU = (TB1 == TopAbs_OUT) ? 1 : 2;
	    Standard_Integer rkfspOU = (rankFOR == rkOU) ? 1 : 2;
	    Standard_Boolean spOUFOR = (rankFOR == rkfspOU);
	    keep = spOUFOR;
	  }
	}
	if (opeCom) keep = !positive;
	if (opeCut) {
	  Standard_Boolean FORmoinsFS = (TBFS == TopAbs_IN);
	  if (positive) {
	    if (FORmoinsFS) keep = Standard_True;
	    else            keep = ComOfCut;
//	    if (TBFS == TopAbs_OUT) keep = Standard_True;
//	    else                    keep = (rankFS == 1); 
	  }
	  else {
//	    keep = !FORmoinsFS; // xpu230698 : cto100A1
//	    keep = Standard_False;
	    if (!FORmoinsFS) keep = Standard_True; // xpu230698 : cto100A1
	    else             keep = ComOfCut; //xpu120898 PRO12859 (FOR26,FS12,FCX18,EG14)
	  }
	}
      }
//#ifdef OCCT_DEBUG
//      if(tEFOR) {cout<<endl<<"yap2 : keep "<<keep<<endl;debfillonfemess(iFOR,iEG);}
//#endif
      if (!keep) return;
    } // !isfafa
    else {
      if (!b) return;
    }

    // on garde EspON pour la reconstruction de la face FOR
    // si elle est IN/ON la face FOR (EGBoundFOR = 0 ==> projection)
    TopOpeBRepTool_ShapeClassifier& PSC = FSC_GetPSC(FOR);
    TopAbs_State state2d = FSC_StateEonFace(EspON,0.345,FOR,PSC);
    Standard_Boolean isin = (state2d == TopAbs_IN);
    if (!isin) return;
    
    TopAbs_State TBEG = TB1;
    TopAbs_Orientation neworiE = TFE.Orientation(TBEG);
    
    FUN_tool_orientEinFFORWARD(EG,FCX,neworiE);   
    Standard_Boolean forw=M_FORWARD(neworiE), reve=M_REVERSED(neworiE);
    Standard_Boolean ok = forw || reve;
    if (FFinSDSO && ok) {
      if (opeCut) {
	neworiE = TopAbs::Complement(neworiE);
      }
    } // FFinSDSO && ok
    
    // bcl1;bcl2; tsp(f9)
    if (FFinSDDO && ok) {
      if (opeCut) {
	if (!ComOfCut) { 
//	  TopAbs_State TBEG = (rankEG == 1)? TB1 : TB2;
//	  if (TBEG == TopAbs_IN) neworiE = TopAbs::Complement(neworiE);
	  neworiE = TopAbs::Complement(neworiE); // xpu270898 : cto905E2 (FOR33,FCX16,EG19)
	}
	else if (ComOfCut) {
	  // rien a faire
	} 
      }
      if (opeFus) {
	// en principe , l'operation locale est tjrs = OUT,IN
	neworiE = TopAbs::Complement(neworiE); 
      }
      if (opeCom) { 
	// SDDO et oT anti// => oG //	
	// xxFCX :
	gp_Dir xxFCX; ok = FUN_tool_getxx(FCX,EG,parEG,xxFCX);
	if (!ok) return;// nyiRAISE
	// ntFS :
	gp_Dir ntFS(ngFS);
	if (M_REVERSED(oFS)) ntFS.Reverse();
	
	Standard_Real dot = xxFCX.Dot(ntFS);
	Standard_Boolean toreverse = (dot > 0);
	
	if (toreverse) neworiE = TopAbs::Complement(neworiE);
      }
    } // FFinSDDO && ok

    Standard_Boolean reverse = Standard_False; //xpu270598 : reverse=false=>FCX has same geom ori
    if (!FCXisref)  { //xpu270598   
      if (cFCX == TopOpeBRepDS_DIFFORIENTED) reverse = Standard_True; 
      if (oFCX != orefFCX) reverse = !reverse;
    } //xpu270598   
    if (reverse) neworiE = TopAbs::Complement(neworiE); // xpu270598
        
    TopoDS_Shape newE = EspON;
    newE.Orientation(neworiE);
    
#ifdef OCCT_DEBUG
    if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap2 GFillON","WES+ EspON ");
    if (tE) FUN_cout(newE);
#endif    
    myPWES->AddStartElement(newE);

    return;
  } // yap2
    
  //=========================================
  if ( yap6 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap6 GFillON");
    if (tE) {cout<<"yap6(FOR"<<iFOR<<" FCX"<<iFCX<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif
    TopAbs_Orientation neworiE;
    // FF est samedomain avec FCX
    // on evalue la transition de FOR par rapport a la matiere 2d de la face FCX
    // au lieu de la transition par rapport a la matiere 3d de la face FS
    // EG est une arete de FCX, oegFCXF=O.T. de EG dans FCX orientee FORWARD
    
    TopAbs_State staFCX = staFS; // FS et FCX connexes par EG => meme shape origine => meme etat
    Standard_Boolean b3d = Standard_False; Standard_Boolean b2d = Standard_False; 
    Standard_Boolean b = Standard_False;

    Standard_Boolean SO = FFinSDSO;//(FFinSDSO && (oFOR == oFCX)) || (FFinSDDO && (oFOR != oFCX));
    Standard_Boolean DO = FFinSDDO;//(FFinSDSO && (oFOR != oFCX)) || (FFinSDDO && (oFOR == oFCX));

#ifdef OCCT_DEBUG
//    Standard_Integer rkToFill = BDS.AncestorRank(myFace);
#endif
//    Standard_Boolean samerk = (rankEG == rkToFill);

//    TopAbs_Orientation oegFOR;
    Standard_Boolean shareG;
    Standard_Boolean ok = FUN_ds_shareG(myPB->DataStructure(),iFOR,iFCX,GI,
			   TopoDS::Edge(EspON),shareG);
    if (!ok) return; // nyiFUNRAISE

    if (SO) {
      // FOR and FCX share geometric domain.
      Standard_Boolean forcekeep = opeFus && shareG && (rankEG ==1); // newnew
      
      if      (opeFus) {
//	b = shareG;
	// xpu231198 :PRO15946 (FOR11,EG24,FCX20)
	// xpu090299 (JAP60247, FOR6,FCX33,EG34)
	const TopoDS_Edge& Esd = TopoDS::Edge(BDS.Shape(Iesd));
	TopTools_ListOfShape lfor; FDSCNX_FaceEdgeConnexFaces(FOR,Esd,HDS,lfor);
	Standard_Integer nfor = lfor.Extent(); 
	if (nfor < 1) b = shareG; //Esd is FOR's closing edge
	else if (nfor > 1) return;//NYIRaise (unvalid shape)
	else {
	  const TopoDS_Face& FF = TopoDS::Face(lfor.First());
	  Standard_Real tola1 = Precision::Angular()*1.e2;//nyitolxpu
	  Standard_Real parEG; Standard_Boolean ok1 = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
	  if (!ok1) return;
	  Standard_Real matfs; ok1 = TopOpeBRepTool_TOOL::Matter(FS,FCX,EG,parEG,tola1,matfs);
	  if (!ok1) return;
	  
	  Standard_Real tolEsd = BRep_Tool::Tolerance(Esd)*factor; //NYITOLXPU
	  Standard_Real parEsd; Standard_Boolean ok2 = FUN_tool_parE(eON,parON,Esd,parEsd,tolEsd);
	  if (!ok2) return;
	  Standard_Real matfor; ok2 = TopOpeBRepTool_TOOL::Matter(FOR,FF,Esd,parEsd,tola1,matfor);
	  if (!ok2) return;
	  
	  Standard_Real sum = matfs+matfor;
	  Standard_Boolean sumisPI = (Abs(sum-M_PI) < tola1);
	  Standard_Boolean fsinfPI  = (matfs < M_PI);
	  Standard_Boolean forinfPI = (matfor < M_PI);
	  if      (sumisPI)  b = Standard_False;
	  else if (sum < M_PI) b = Standard_True;
	  else { //sum > M_PI
	    if (fsinfPI && forinfPI) b = Standard_False;
	    else { // (!fsinfPI) || (!forinfPI)
	      Standard_Boolean sammat = (Abs(matfs-matfor)<tola1);
	      if (sammat) b = Standard_False;
	      else        b = (matfs < matfor);
	    }
	  }
	  if (b) forcekeep = Standard_True;
	}
	// NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNYIXPU
	/*if (shareG) b = Standard_True;
	else {
	  // xpu231198 :PRO15946 (FOR11,EG24,FCX20)
	  Standard_Real parEG; Standard_Boolean ok = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
	  if (!ok) return;
	  gp_Dir xxFCX; ok = FUN_tool_getxx(FCX,EG,parEG,xxFCX);
	  if (!ok) return;// nyiRAISE
	  gp_Vec ntFS; ok = FUN_tool_nggeomF(parEG,EG,FS,ntFS,tolFS);
	  if (!ok) return;// nyiRAISE
	  if (M_REVERSED(oFS)) ntFS.Reverse();
	  
	  Standard_Real dot = ntFS.Dot(xxFCX);
	  if (Abs(dot) < tola) b = Standard_False; // xpu231198nyi tangent config
	  else                 {b = (dot <0.); if (b) forcekeep=Standard_True;}
	}//!shareG*/
      }
      else if (opeCut) b = (!shareG);
      else if (opeCom) b = shareG && (rankEG ==1);
      if (b) { // nyi : a revoir
	if ( oFOR != oFCX ) {
	  // orientation topologique de EG dans la face mere
	  oegFCXF = TopAbs::Complement(oegFCXF);
	}    
	TopOpeBRepDS_Transition T1(oegFCXF); TopAbs_State Taft = T1.After(); TopAbs_State Tbef = T1.Before();
	if      (oegFCXF == TopAbs_FORWARD)  b3d = (TFEaft == staFCX);
	else if (oegFCXF == TopAbs_REVERSED) b3d = (TFEbef == staFCX);
	if      (oegFCXF == TopAbs_FORWARD)  b2d = (Taft == TopAbs_IN);
	else if (oegFCXF == TopAbs_REVERSED) b2d = (Tbef == TopAbs_IN);
	b = (b3d && b2d);
      }
      if (forcekeep) b = Standard_True; // newnew
    }
    if (DO) { // we do NOT keep the splitON
      if (opeFus)   b = (!shareG); // cto001D2 spOU(f18) 
      if (opeCut) { 
	Standard_Integer rankREF = opec12? 1 : 2;
	Standard_Boolean binin=Standard_False, binou=Standard_False;
	if (TB1 == TB2) binin = (rankEG == rankREF); // partie IN/IN
	else {
	  //xpu291098 cto001D3 (f17ou)
	  binou = (rankFOR == 1)&&(TBFOR == TopAbs_OUT)&&(!shareG); 
	  if (binou) {
	    // xpu181198 CTS21302(FOR22,EG9,FCX5)
	    // ntOOFOR :
	    const TopoDS_Edge& Esd = TopoDS::Edge(BDS.Shape(Iesd)); // rkEsd=rkFOR
	    TopTools_ListOfShape lfcFk; FDSCNX_FaceEdgeConnexFaces(FOR,Esd,HDS,lfcFk);
	    if (lfcFk.Extent() != 1) return; // shape bizarre
	    Standard_Real parEsd; ok = FUN_tool_parE(eON,parON,Esd,parEsd,tolEG);
	    if (!ok) return;
	    const TopoDS_Face& OOFOR = TopoDS::Face(lfcFk.First());
	    gp_Pnt2d uv; Standard_Real dummy=0; 
	    ok = FUN_tool_paronEF(Esd,parEsd,OOFOR,uv,dummy); //rkEsd=rkOOFOR
	    gp_Vec ntOOFOR = FUN_tool_nggeomF(uv,OOFOR);
	    if (OOFOR.Orientation() == TopAbs_REVERSED) ntOOFOR.Reverse();
	    // xxFCX :
#ifdef OCCT_DEBUG
//	    Standard_Real t1 =factor*BRep_Tool::Tolerance(Esd);
#endif
	    Standard_Real parEG; ok = FUN_tool_parE(Esd,parEsd,EG,parEG);
	    if (!ok) return;
	    gp_Dir xxFCX; ok = FUN_tool_getxx(FCX,EG,parEG,xxFCX);
	    if (!ok) return;
	    // dot : 
	    Standard_Real dot = ntOOFOR.Dot(xxFCX);
	    if      (Abs(dot) < tola) binou = Standard_True;// nyixpu181198
	    else if (dot < 0.) binou = Standard_False;
	  }
	}
	b = binin || binou;
      } 
    }
    if (!b) return;
    
    TopAbs_State TBEG = TB1;
    neworiE = TFE.Orientation(TBEG);    
    if      (FCXisref && !EGBoundFOR) {
      FUN_tool_orientEinFFORWARD(EG,FCX,neworiE);
      Standard_Boolean rev = myPB->Reverse(staFCX,staFOR);
      neworiE = myPB->Orient(neworiE,rev);
    }   
    else if (FORisref && !EGBoundFOR) {
      // xpu280798 : cto902C1 (FOR=f9,FCX=f19,FS,f33,EG,e18)
      // as eghassdON : Esd = edge(Iesd) = bound FOR; eON is IN1d(Esd)      
      const TopoDS_Edge& Esd = TopoDS::Edge(BDS.Shape(Iesd));
      FUN_tool_orientEinFFORWARD(Esd,TopoDS::Face(FOR),neworiE);
      Standard_Boolean reverse = (M_FORWARD(neworiE) || M_REVERSED(neworiE)) && (!eONsoEsd);
      if (reverse) neworiE = TopAbs::Complement(neworiE);
      Standard_Boolean rev = myPB->Reverse(staFOR,staFCX);
      neworiE = myPB->Orient(neworiE,rev);
    }
    else if (!EGBoundFOR) {
      // xpu210898 : CTS20875 : yap6(FOR13,GI15,FCX6), !FCXisref && !FORisref
      Standard_Integer rankrefFOR = BDS.AncestorRank(irefFOR);
      if (rankrefFOR == rankFCX) 
	FUN_tool_orientEinFFORWARD(EG,FCX,neworiE);
      else if (rankrefFOR == rankFOR) {     
	const TopoDS_Edge& Esd = TopoDS::Edge(BDS.Shape(Iesd));
	FUN_tool_orientEinFFORWARD(Esd,TopoDS::Face(FOR),neworiE);
	Standard_Boolean reverse = (M_FORWARD(neworiE) || M_REVERSED(neworiE)) && (!eONsoEsd);
	if (reverse) neworiE = TopAbs::Complement(neworiE);	
      }
      Standard_Boolean rev = myPB->Reverse(staFOR,staFCX);
      neworiE = myPB->Orient(neworiE,rev);      
    }

    TopoDS_Shape newE = EspON;
    newE.Orientation(neworiE);    
#ifdef OCCT_DEBUG
    if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap6 GFillON","WES+ EspON ");
    if (tE) FUN_cout(newE);
#endif    
    myPWES->AddStartElement(newE);

    // xpu290798 : cto 902 C1, f9ou,f19in, spON(e7) = spON(e18)
    //    splits IN and OUT generated on reference surface should be diff oriented
    //    they share same edge eON which is FORWARD in one, REVERSED in the other
    Standard_Boolean addtwice = SO && !shareG && opeCut;        
    addtwice = addtwice && (M_FORWARD(neworiE) || M_REVERSED(neworiE));
    if (addtwice) {
      // we have splitIN 
      TopoDS_Shape FtospIN = (TBFOR == TopAbs_IN) ? FOR : FCX;      
      addtwice = Standard_False;      
      TopExp_Explorer ex;
      for (ex.Init(FtospIN,TopAbs_EDGE); ex.More(); ex.Next()){
//      for (TopExp_Explorer ex(FtospIN,TopAbs_EDGE); ex.More(); ex.Next()){
	const TopoDS_Shape& ee = ex.Current();
	if (!BDS.HasShape(ee)) continue;
	Standard_Boolean issp = myPB->IsSplit(ee,TopAbs_IN);
	if (issp) addtwice = !myPB->Splits(ee,TopAbs_IN).IsEmpty();
	if (addtwice) break;
      }
    }
    if (addtwice) {
      neworiE = TopAbs::Complement(neworiE);
      newE.Orientation(neworiE);    
#ifdef OCCT_DEBUG
      if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap6 GFillON","WES+ EspON ");
      if (tE) FUN_cout(newE);
#endif    
      myPWES->AddStartElement(newE);
    }
    return;
  } // yap6
    
  //=========================================
  if ( yap3 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap3 GFillON");
    if (tE) {cout<<"yap3(FOR"<<iFOR<<" FCX"<<iFCX<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif
    
    ie3d = ::FUN_findeSD(BDS,eON,EG,FOR,oe3d,3);
    if (ie3d == 0) return;
    const TopoDS_Edge& e3d = TopoDS::Edge(BDS.Shape(ie3d));

    TopOpeBRepDS_Config cf; Standard_Boolean cfok = FDS_Config3d(EspON,e3d,cf);
    if (!cfok) return;

    TopAbs_Orientation oe3dk = oe3d;
//    Standard_Boolean samegeom = ::TopOpeBRepBuild_FUN_aresamegeom(EG,e3d);
    Standard_Real parEG; Standard_Boolean ok = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
    if (!ok) return; //nyiRAISE
    Standard_Boolean samegeom; ok = FUN_tool_curvesSO(EG,parEG,e3d,samegeom);
    if (!ok) return; //nyiRAISE
#ifdef OCCT_DEBUG
    if (!TopOpeBRepBuild_GetcontextNOSG()) {
#endif
      if (!samegeom) oe3dk = TopAbs::Complement(oe3dk);
#ifdef OCCT_DEBUG
    }
#endif

    Standard_Boolean keep3d = Standard_False;
    // l'etat a construire sur FOR est TB1,
    if      (oe3dk == TopAbs_FORWARD)  keep3d = (TFEaft == TB1);
    else if (oe3dk == TopAbs_REVERSED) keep3d = (TFEbef == TB1);
    else if (oe3dk == TopAbs_INTERNAL) keep3d = Standard_True;
    else if (oe3dk == TopAbs_EXTERNAL) keep3d = Standard_False;

#ifdef OCCT_DEBUG
    if(tEFOR) {cout<<endl<<"yap3 keep3d : "<<keep3d<<endl;debfillonfemess3d(iFOR,iEG);}
#endif
    
    if (keep3d) {
      TopAbs_Orientation neworiE = oe3d;
      if (cf == TopOpeBRepDS_DIFFORIENTED) neworiE = TopAbs::Complement(neworiE);
      TopoDS_Shape newE = EspON;
      newE.Orientation(neworiE);
      
#ifdef OCCT_DEBUG
      if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap3 GFillON","WES+ EspON ");
      if (tE) FUN_cout(newE);
#endif
      myPWES->AddStartElement(newE);
    }

    return;
  } // yap3

  //=========================================
  if ( yap5 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap5 GFillON");
    if (tE) {cout<<"yap5(FOR"<<iFOR<<" FCX"<<iFCX<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif
    ie2d = ::FUN_findeSD(BDS,eON,EG,FOR,oe2d,2);
    if (ie2d == 0) return; 
    
    const TopoDS_Edge& e2d = TopoDS::Edge(BDS.Shape(ie2d));
    TopOpeBRepDS_Config cf; Standard_Boolean cfok = FDS_Config3d(EspON,e2d,cf);
    if (!cfok) return;

    TopAbs_Orientation oe2dk = oe2d;
    Standard_Real parEG; Standard_Boolean ok = FUN_tool_parE(eON,parON,EG,parEG,tolEG);
    if (!ok) return; // nyiRAISE
    Standard_Boolean samegeom; ok = FUN_tool_curvesSO(EG,parEG,e2d,samegeom);
    if (!ok) return; // nyiRAISE
#ifdef OCCT_DEBUG
    if (!TopOpeBRepBuild_GetcontextNOSG()) {
#endif
      if (!samegeom) oe2dk = TopAbs::Complement(oe2dk);
#ifdef OCCT_DEBUG
    }
#endif

    Standard_Boolean keep2d = Standard_False;
    // l'etat a construire sur FOR est TB1,
    if      (oe2dk == TopAbs_FORWARD)  keep2d = (TFEaft == TB1);
    else if (oe2dk == TopAbs_REVERSED) keep2d = (TFEbef == TB1);
    else if (oe2dk == TopAbs_INTERNAL) keep2d = Standard_True;
    else if (oe2dk == TopAbs_EXTERNAL) keep2d = Standard_False;
   
    if (keep2d) {
      TopAbs_Orientation neworiE = oe2d;
      if (cf == TopOpeBRepDS_DIFFORIENTED) neworiE = TopAbs::Complement(neworiE);
      TopoDS_Shape newE = EspON;
      newE.Orientation(neworiE);
      
#ifdef OCCT_DEBUG
      if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap5 GFillON","WES+ EspON ");
      if (tE) FUN_cout(newE);
#endif
      myPWES->AddStartElement(newE);
    }

    return;
  } // yap5
  
  //=========================================
  if ( yap4 ) {
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap4 GFillON");
    if (tE) {cout<<"yap4(FOR"<<iFOR<<" FCX"<<iFCX<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif    

    TopAbs_Orientation oTFE = TFE.Orientation(TB1);

    TopAbs_Orientation neworiE = oTFE;
    TopoDS_Shape newE = EspON;
    newE.Orientation(neworiE);
#ifdef OCCT_DEBUG
    if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap4 GFillON","WES+ EspON ");
    if (tE) FUN_cout(newE);
#endif
    myPWES->AddStartElement(newE);
        
    return;
  } // yap4

  //=========================================
  if ( yap7 ) {
    // xpu290598 : CTS20212
#ifdef OCCT_DEBUG
    if (tEFOR) debfillonfemess(iFOR,iEG,myPB,myPWES,"yap7 GFillON");
    if (tE) {cout<<"yap7(FOR"<<iFOR<<" FCX"<<iFCX<<" EG"<<GI<<") ";
	     cout<<"TB1=";TopAbs::Print(TB1,cout);cout<<" TB2=";TopAbs::Print(TB2,cout);cout<<endl;}
#endif
    Standard_Boolean isbound = Standard_False;
    for (TopTools_ListIteratorOfListOfShape it(BDS.ShapeSameDomain(iFOR)); it.More(); it.Next()){
      TopExp_Explorer ex(it.Value(), TopAbs_EDGE);
      for (; ex.More(); ex.Next()) {
	const TopoDS_Shape& E = ex.Current();
	if (E.IsSame(EG)) {
	  isbound = Standard_True;
	  break;
	}
      }
    }
    TopAbs_Orientation oTFE = TFE.Orientation(TB1);
    TopAbs_Orientation neworiE = oTFE;

    if (ComOfCut) { //CTS20212 : tspIN(f7),tspON(e4)
      if (!EGBoundFOR) {
	if (!isbound) return;
      }
    }

    //CTS20212 : tspIN(f7),tspON(e5)    
    Standard_Boolean samegeomref = Standard_False;
    if (FORisref) {
      samegeomref = Standard_True;
    }
    else {
      samegeomref = (cFOR == TopOpeBRepDS_SAMEORIENTED);
      if (oFOR != orefFOR) samegeomref = !samegeomref;
    }
    if (!samegeomref) neworiE = TopAbs::Complement(neworiE);
    
    TopoDS_Shape newE = EspON;
    newE.Orientation(neworiE);
#ifdef OCCT_DEBUG
    if (tEFOR) debaddpwes(iFOR,TB1,iEG,neworiE,myPB,myPWES,"yap7 GFillON","WES+ EspON ");
    if (tE) FUN_cout(newE);
#endif
    myPWES->AddStartElement(newE);
    
    return;
  } // yap7
  
} // GFillONPartsWES2
