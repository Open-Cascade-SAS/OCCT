// Created on: 1998-05-07
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_BuilderON.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>

#define M_OUT(st) (st == TopAbs_OUT)
#define M_IN( st) (st == TopAbs_IN)
#define M_FORWARD(st) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)

#ifdef OCCT_DEBUG
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextEINTERNAL();
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextEEXTERNAL();
Standard_EXPORT void debfillonf(const Standard_Integer iF);
Standard_EXPORT void debON2d(const Standard_Integer iF)
{cout<<"++ debON2d "<<iF<<" "<<endl;}
#endif

Standard_EXPORT TopAbs_State FUN_build_TB(const TopOpeBRepBuild_PBuilder& PB,const Standard_Integer rank); // xpu290698
Standard_EXPORT Standard_Boolean FUN_keepEON(const TopOpeBRepBuild_Builder& B,
				const TopoDS_Shape& sEG,const TopoDS_Shape& sFOR,const TopoDS_Shape& sFS,
				const Standard_Boolean EGBoundFOR,
				const TopOpeBRepDS_Transition& TFE,const TopAbs_State TB1,const TopAbs_State TB2);
Standard_EXPORT void FUN_coutmess(const TCollection_AsciiString& m);

Standard_EXPORTEXTERN  TopOpeBRepDS_PDataStructure GLOBAL_DS2d;

//=======================================================================
//function : Perform2d
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_BuilderON::Perform2d
(const TopOpeBRepBuild_PBuilder& PB,
 const TopoDS_Shape& FOR,const TopOpeBRepBuild_PGTopo& PG,
 const TopOpeBRepTool_Plos& PLSclass, const TopOpeBRepBuild_PWireEdgeSet& PWES)
{
  myPB = PB;
  myFace = FOR;
  myPG = PG;
  myPLSclass = PLSclass;
  myPWES = PWES;

  const TopOpeBRepDS_DataStructure& BDS=myPB->DataStructure()->DS();
  if (GLOBAL_DS2d == NULL) GLOBAL_DS2d = (TopOpeBRepDS_PDataStructure)new TopOpeBRepDS_DataStructure();
  const TopOpeBRepDS_ListOfInterference& lFEI = GLOBAL_DS2d->ShapeInterferences(FOR);

#ifdef OCCT_DEBUG
  Standard_Integer iFOR;Standard_Boolean tFOR=myPB->GtraceSPS(FOR,iFOR);
  if (tFOR) debfillonf(iFOR);
  if (tFOR) cout<<endl<<"LI on F"<<iFOR<<endl;
#endif

  for (TopOpeBRepDS_ListIteratorOfListOfInterference itI(lFEI); itI.More(); itI.Next()){
    const Handle(TopOpeBRepDS_Interference)& I = itI.Value();
    TopOpeBRepDS_Kind GT,ST;Standard_Integer GI,SI;FDS_data(I,GT,GI,ST,SI);

    const TopoDS_Edge& EG=TopoDS::Edge(BDS.Shape(GI));
#ifdef OCCT_DEBUG
//    Standard_Integer iEG=BDS.Shape(EG);
#endif
    const TopTools_ListOfShape& lEspON=myPB->Splits(EG,TopAbs_ON);
#ifdef OCCT_DEBUG
//    Standard_Integer nEspON=lEspON.Extent();
#endif
    for(TopTools_ListIteratorOfListOfShape it(lEspON);it.More();it.Next()) {
      const TopoDS_Shape& EspON=it.Value();
      GFillONParts2dWES2(I,EspON);
    }
  }
}

//=======================================================================
//function : GFillONPartsWES2
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_BuilderON::GFillONParts2dWES2(const Handle(TopOpeBRepDS_Interference)& I,const TopoDS_Shape& EspON)
{
  const Handle(TopOpeBRepDS_HDataStructure)& HDS=myPB->DataStructure();
  const TopOpeBRepDS_DataStructure& BDS= HDS->DS();
  Handle(TopOpeBRepDS_ShapeShapeInterference) SSI (Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I));
  TopAbs_State TB1,TB2;myPG->StatesON(TB1,TB2);
  TopOpeBRepDS_Kind GT,ST;Standard_Integer GI,SI;FDS_data(SSI,GT,GI,ST,SI);
//  const TopOpeBRepDS_Transition& TFE=SSI->Transition(); 
  Standard_Boolean EGBoundFOR=SSI->GBound();
  const TopoDS_Face& FOR=TopoDS::Face(myFace); Standard_Integer iFOR=BDS.Shape(FOR);
  const TopoDS_Edge& EG=TopoDS::Edge(BDS.Shape(GI));
#ifdef OCCT_DEBUG
//  Standard_Integer iEG=BDS.Shape(EG);
#endif
  const TopoDS_Face& FS=TopoDS::Face(BDS.Shape(SI)); 
#ifdef OCCT_DEBUG
//  Standard_Integer iFS=BDS.Shape(FS);
//  Standard_Boolean isclosedFF=BRep_Tool::IsClosed(EG,FOR);
//  Standard_Boolean isclosedFS=BRep_Tool::IsClosed(EG,FS);
//  Standard_Boolean isclosed=(isclosedFF || isclosedFS);
//  Standard_Boolean isrest=BDS.IsSectionEdge(EG);
//  Standard_Boolean issplit=myPB->IsSplit(EG,TopAbs_ON);
//  Standard_Integer rankFS=myPB->GShapeRank(FS);
#endif  
  Standard_Integer rankEG=myPB->GShapeRank(EG);
#ifdef OCCT_DEBUG
//  Standard_Integer rankFOR=myPB->GShapeRank(FOR);
#endif
              
//  TopAbs_State TBEG = (rankEG == 1) ? TB1 : TB2;
  TopAbs_State TBEG = FUN_build_TB(myPB,rankEG);
#ifdef OCCT_DEBUG
//  TopAbs_State TFEbef = TFE.Before();
//  TopAbs_State TFEaft = TFE.After();
//  Standard_Boolean EGboundFOR =
//                   Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I)->GBound();
#endif
  Standard_Boolean eghassd   = HDS->HasSameDomain(EG);

#ifdef OCCT_DEBUG
  Standard_Boolean tFOR=myPB->GtraceSPS(iFOR);
//  Standard_Boolean tE=myPB->GtraceSPS(GI);
//  Standard_Boolean tEFOR = (tE && tFOR);
  if (tFOR) {debON2d(iFOR);}
#endif    
  
  Standard_Integer iFCX=SI;
  Standard_Boolean FFinSDSO = Standard_True;
#ifdef OCCT_DEBUG
//  Standard_Boolean FFinSDDO = Standard_False;
#endif
  Standard_Boolean FFinSD= Standard_True;
  TopoDS_Face FCX = FS;
  
#ifdef OCCT_DEBUG
//  TopAbs_Orientation oFOR = BDS.Shape(iFOR).Orientation();
//  TopAbs_Orientation oFS  = BDS.Shape(iFS).Orientation();
//  TopAbs_Orientation oFCX = BDS.Shape(iFCX).Orientation();
//  Standard_Integer irefFOR = BDS.SameDomainRef(FOR);
//  Standard_Integer irefFCX = BDS.SameDomainRef(FCX);
//  Standard_Boolean FORisref = (irefFOR == iFOR);
//  Standard_Boolean FCXisref = (irefFCX == iFCX);
#endif
  
  TopAbs_Orientation oegFCXF;Standard_Boolean EGBoundFCX = FUN_tool_orientEinFFORWARD(EG,FCX,oegFCXF);
  TopAbs_Orientation oegFCX ;
#ifdef OCCT_DEBUG
//  Standard_Boolean ok2 =
#endif
            FUN_tool_orientEinF(EG,FCX,oegFCX);

  Standard_Boolean opeFus = myPB->Opefus(); 
  Standard_Boolean opeCut = myPB->Opec12() || myPB->Opec21();
  Standard_Boolean opeCom = myPB->Opecom();
  
    
  Standard_Boolean yap6 = Standard_True;
  yap6 = yap6 && FFinSD; 
//  yap6 = yap6 && (!EGBoundFOR);
//  yap6 = yap6 && EGBoundFCX;
  yap6 =  yap6 && (EGBoundFOR || EGBoundFCX);
  yap6 = yap6 && eghassd;
   
  //=========================================
  if ( yap6) {
#ifdef OCCT_DEBUG
    if (tFOR) cout<<"* yap6 = 1"<<endl;
#endif
    TopAbs_Orientation neworiE = TopAbs_FORWARD;
    // FF est samedomain avec FCX
    // on evalue la transition de FOR par rapport a la matiere 2d de la face FCX
    // au lieu de la transition par rapport a la matiere 3d de la face FS
    // EG est une arete de FCX, oegFCXF=O.T. de EG dans FCX orientee FORWARD
    
    Standard_Boolean b = Standard_False;

    Standard_Boolean SO = FFinSDSO;//(FFinSDSO && (oFOR == oFCX)) || (FFinSDDO && (oFOR != oFCX));
#ifdef OCCT_DEBUG
//    Standard_Integer rkToFill = BDS.AncestorRank(myFace); //DEB
#endif
    Standard_Boolean rk1 = (rankEG == 1);
    if (!rk1) return;

    TopAbs_Orientation oegFOR;
    Standard_Boolean shareG = Standard_False;
    Standard_Boolean ok = Standard_False;
    if      (EGBoundFCX) 
      ok = FUN_ds_shareG(myPB->DataStructure(),iFOR,iFCX,GI,TopoDS::Edge(EspON),shareG);
    else if (EGBoundFOR)
      ok = FUN_ds_shareG(myPB->DataStructure(),iFCX,iFOR,GI,TopoDS::Edge(EspON),shareG);
    if (!ok) return; // nyiFUNRAISE
#ifdef OCCT_DEBUG
    if (tFOR) cout<<" shareG="<<shareG<<endl;
#endif

    if (SO) {
      // FOR and FCX share geometric domain.
      if      (opeFus) b = shareG;
      else if (opeCut) b = (!shareG) && M_OUT(TBEG);
      else if (opeCom) b = shareG;
    }
    if (!b) return;    

    if      (!EGBoundFOR) {      
      TopAbs_Orientation oegFCXF1; FUN_tool_orientEinFFORWARD(EG,FCX,oegFCXF1);
      neworiE = oegFCXF;
    }
    else if (EGBoundFOR) {
      FUN_tool_orientEinFFORWARD(EG,TopoDS::Face(FOR),oegFOR);
      neworiE = oegFOR;
    }

//    Standard_Real f,l; FUN_tool_bounds(EG,f,l); Standard_Real parON = (f+l)*.4352;
    Standard_Real f,l; FUN_tool_bounds(TopoDS::Edge(EspON),f,l); Standard_Real parON = (f+l)*.4352; // xpu120698
    Standard_Boolean ESO; ok = FUN_tool_curvesSO(TopoDS::Edge(EspON),parON,EG,ESO);
    if (!ok) return; // xpu120698
    if (!ESO) neworiE = TopAbs::Complement(neworiE);

    TopAbs_Orientation oFOR = BDS.Shape(iFOR).Orientation();
    TopAbs_Orientation oFCX = BDS.Shape(iFCX).Orientation();
    if (oFOR != oFCX) neworiE = TopAbs::Complement(neworiE);	

    TopoDS_Shape newE = EspON;
    newE.Orientation(neworiE);  
    myPWES->AddStartElement(newE);
#ifdef OCCT_DEBUG
    if (tFOR) cout<<"  add spON e"<<GI<<endl;
#endif
    return;
  } // yap6

} // GFillONPartsWES2
