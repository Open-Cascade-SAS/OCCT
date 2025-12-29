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
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_BuilderON.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_SC.hxx>

#define M_OUT(st) (st == TopAbs_OUT)
#define M_IN(st) (st == TopAbs_IN)
#define M_FORWARD(st) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)

#ifdef OCCT_DEBUG
Standard_EXPORT bool TopOpeBRepBuild_GetcontextEINTERNAL();
Standard_EXPORT bool TopOpeBRepBuild_GetcontextEEXTERNAL();
Standard_EXPORT void debfillonf(const int iF);

Standard_EXPORT void debON2d(const int iF)
{
  std::cout << "++ debON2d " << iF << " " << std::endl;
}
#endif

// clang-format off
Standard_EXPORT TopAbs_State FUN_build_TB(const TopOpeBRepBuild_PBuilder& PB,const int rank); // xpu290698
// clang-format on
Standard_EXPORT bool FUN_keepEON(const TopOpeBRepBuild_Builder& B,
                                 const TopoDS_Shape&            sEG,
                                 const TopoDS_Shape&            sFOR,
                                 const TopoDS_Shape&            sFS,
                                 const bool                     EGBoundFOR,
                                 const TopOpeBRepDS_Transition& TFE,
                                 const TopAbs_State             TB1,
                                 const TopAbs_State             TB2);
Standard_EXPORT void FUN_coutmess(const TCollection_AsciiString& m);

Standard_EXPORTEXTERN TopOpeBRepDS_PDataStructure GLOBAL_DS2d;

//=================================================================================================

void TopOpeBRepBuild_BuilderON::Perform2d(const TopOpeBRepBuild_PBuilder&     PB,
                                          const TopoDS_Shape&                 FOR,
                                          const TopOpeBRepBuild_PGTopo&       PG,
                                          const TopOpeBRepTool_Plos&          PLSclass,
                                          const TopOpeBRepBuild_PWireEdgeSet& PWES)
{
  myPB       = PB;
  myFace     = FOR;
  myPG       = PG;
  myPLSclass = PLSclass;
  myPWES     = PWES;

  const TopOpeBRepDS_DataStructure& BDS = myPB->DataStructure()->DS();
  if (GLOBAL_DS2d == nullptr)
    GLOBAL_DS2d = (TopOpeBRepDS_PDataStructure) new TopOpeBRepDS_DataStructure();
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lFEI =
    GLOBAL_DS2d->ShapeInterferences(FOR);

#ifdef OCCT_DEBUG
  int  iFOR;
  bool tFOR = myPB->GtraceSPS(FOR, iFOR);
  if (tFOR)
    debfillonf(iFOR);
  if (tFOR)
    std::cout << std::endl << "LI on F" << iFOR << std::endl;
#endif

  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itI(lFEI); itI.More();
       itI.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = itI.Value();
    TopOpeBRepDS_Kind                             GT, ST;
    int                                           GI, SI;
    FDS_data(I, GT, GI, ST, SI);

    const TopoDS_Edge& EG = TopoDS::Edge(BDS.Shape(GI));
#ifdef OCCT_DEBUG
//    int iEG=BDS.Shape(EG);
#endif
    const NCollection_List<TopoDS_Shape>& lEspON = myPB->Splits(EG, TopAbs_ON);
#ifdef OCCT_DEBUG
//    int nEspON=lEspON.Extent();
#endif
    for (NCollection_List<TopoDS_Shape>::Iterator it(lEspON); it.More(); it.Next())
    {
      const TopoDS_Shape& EspON = it.Value();
      GFillONParts2dWES2(I, EspON);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_BuilderON::GFillONParts2dWES2(const occ::handle<TopOpeBRepDS_Interference>& I,
                                                   const TopoDS_Shape& EspON)
{
  const occ::handle<TopOpeBRepDS_HDataStructure>&  HDS = myPB->DataStructure();
  const TopOpeBRepDS_DataStructure&                BDS = HDS->DS();
  occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI(
    occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(I));
  TopAbs_State TB1, TB2;
  myPG->StatesON(TB1, TB2);
  TopOpeBRepDS_Kind GT, ST;
  int               GI, SI;
  FDS_data(SSI, GT, GI, ST, SI);
  //  const TopOpeBRepDS_Transition& TFE=SSI->Transition();
  bool               EGBoundFOR = SSI->GBound();
  const TopoDS_Face& FOR        = TopoDS::Face(myFace);
  int                iFOR       = BDS.Shape(FOR);
  const TopoDS_Edge& EG         = TopoDS::Edge(BDS.Shape(GI));
#ifdef OCCT_DEBUG
//  int iEG=BDS.Shape(EG);
#endif
  const TopoDS_Face& FS = TopoDS::Face(BDS.Shape(SI));
#ifdef OCCT_DEBUG
//  int iFS=BDS.Shape(FS);
//  bool isclosedFF=BRep_Tool::IsClosed(EG,FOR);
//  bool isclosedFS=BRep_Tool::IsClosed(EG,FS);
//  bool isclosed=(isclosedFF || isclosedFS);
//  bool isrest=BDS.IsSectionEdge(EG);
//  bool issplit=myPB->IsSplit(EG,TopAbs_ON);
//  int rankFS=myPB->GShapeRank(FS);
#endif
  int rankEG = myPB->GShapeRank(EG);
#ifdef OCCT_DEBUG
//  int rankFOR=myPB->GShapeRank(FOR);
#endif

  //  TopAbs_State TBEG = (rankEG == 1) ? TB1 : TB2;
  TopAbs_State TBEG = FUN_build_TB(myPB, rankEG);
#ifdef OCCT_DEBUG
//  TopAbs_State TFEbef = TFE.Before();
//  TopAbs_State TFEaft = TFE.After();
//  bool EGboundFOR =
//                   occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(I)->GBound();
#endif
  bool eghassd = HDS->HasSameDomain(EG);

#ifdef OCCT_DEBUG
  bool tFOR = myPB->GtraceSPS(iFOR);
  //  bool tE=myPB->GtraceSPS(GI);
  //  bool tEFOR = (tE && tFOR);
  if (tFOR)
  {
    debON2d(iFOR);
  }
#endif

  int  iFCX     = SI;
  bool FFinSDSO = true;
#ifdef OCCT_DEBUG
//  bool FFinSDDO = false;
#endif
  bool        FFinSD = true;
  TopoDS_Face FCX    = FS;

#ifdef OCCT_DEBUG
//  TopAbs_Orientation oFOR = BDS.Shape(iFOR).Orientation();
//  TopAbs_Orientation oFS  = BDS.Shape(iFS).Orientation();
//  TopAbs_Orientation oFCX = BDS.Shape(iFCX).Orientation();
//  int irefFOR = BDS.SameDomainRef(FOR);
//  int irefFCX = BDS.SameDomainRef(FCX);
//  bool FORisref = (irefFOR == iFOR);
//  bool FCXisref = (irefFCX == iFCX);
#endif

  TopAbs_Orientation oegFCXF;
  bool               EGBoundFCX = FUN_tool_orientEinFFORWARD(EG, FCX, oegFCXF);
  TopAbs_Orientation oegFCX;
#ifdef OCCT_DEBUG
//  bool ok2 =
#endif
  FUN_tool_orientEinF(EG, FCX, oegFCX);

  bool opeFus = myPB->Opefus();
  bool opeCut = myPB->Opec12() || myPB->Opec21();
  bool opeCom = myPB->Opecom();

  bool yap6 = true;
  yap6      = yap6 && FFinSD;
  //  yap6 = yap6 && (!EGBoundFOR);
  //  yap6 = yap6 && EGBoundFCX;
  yap6 = yap6 && (EGBoundFOR || EGBoundFCX);
  yap6 = yap6 && eghassd;

  //=========================================
  if (yap6)
  {
#ifdef OCCT_DEBUG
    if (tFOR)
      std::cout << "* yap6 = 1" << std::endl;
#endif
    TopAbs_Orientation neworiE = TopAbs_FORWARD;
    // FF est samedomain avec FCX
    // on evalue la transition de FOR par rapport a la matiere 2d de la face FCX
    // au lieu de la transition par rapport a la matiere 3d de la face FS
    // EG est une arete de FCX, oegFCXF=O.T. de EG dans FCX orientee FORWARD

    bool b = false;

    bool SO = FFinSDSO; //(FFinSDSO && (oFOR == oFCX)) || (FFinSDDO && (oFOR != oFCX));
#ifdef OCCT_DEBUG
//    int rkToFill = BDS.AncestorRank(myFace); //DEB
#endif
    bool rk1 = (rankEG == 1);
    if (!rk1)
      return;

    TopAbs_Orientation oegFOR;
    bool               shareG = false;
    bool               ok     = false;
    if (EGBoundFCX)
      ok = FUN_ds_shareG(myPB->DataStructure(), iFOR, iFCX, GI, TopoDS::Edge(EspON), shareG);
    else if (EGBoundFOR)
      ok = FUN_ds_shareG(myPB->DataStructure(), iFCX, iFOR, GI, TopoDS::Edge(EspON), shareG);
    if (!ok)
      return; // nyiFUNRAISE
#ifdef OCCT_DEBUG
    if (tFOR)
      std::cout << " shareG=" << shareG << std::endl;
#endif

    if (SO)
    {
      // FOR and FCX share geometric domain.
      if (opeFus)
        b = shareG;
      else if (opeCut)
        b = (!shareG) && M_OUT(TBEG);
      else if (opeCom)
        b = shareG;
    }
    if (!b)
      return;

    if (!EGBoundFOR)
    {
      TopAbs_Orientation oegFCXF1;
      FUN_tool_orientEinFFORWARD(EG, FCX, oegFCXF1);
      neworiE = oegFCXF;
    }
    else if (EGBoundFOR)
    {
      FUN_tool_orientEinFFORWARD(EG, TopoDS::Face(FOR), oegFOR);
      neworiE = oegFOR;
    }

    //    double f,l; FUN_tool_bounds(EG,f,l); double parON = (f+l)*.4352;
    // clang-format off
    double f,l; FUN_tool_bounds(TopoDS::Edge(EspON),f,l); double parON = (f+l)*.4352; // xpu120698
    // clang-format on
    bool ESO;
    ok = FUN_tool_curvesSO(TopoDS::Edge(EspON), parON, EG, ESO);
    if (!ok)
      return; // xpu120698
    if (!ESO)
      neworiE = TopAbs::Complement(neworiE);

    TopAbs_Orientation oFOR = BDS.Shape(iFOR).Orientation();
    TopAbs_Orientation oFCX = BDS.Shape(iFCX).Orientation();
    if (oFOR != oFCX)
      neworiE = TopAbs::Complement(neworiE);

    TopoDS_Shape newE = EspON;
    newE.Orientation(neworiE);
    myPWES->AddStartElement(newE);
#ifdef OCCT_DEBUG
    if (tFOR)
      std::cout << "  add spON e" << GI << std::endl;
#endif
    return;
  } // yap6

} // GFillONPartsWES2
