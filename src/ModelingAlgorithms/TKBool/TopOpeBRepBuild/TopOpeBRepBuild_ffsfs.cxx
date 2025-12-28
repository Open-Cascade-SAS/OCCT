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
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_FuseFace.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_SC.hxx>

#ifdef OCCT_DEBUG
  #define DEBSHASET(sarg, meth, shaset, str)                                                       \
    TCollection_AsciiString sarg((meth));                                                          \
    (sarg) = (sarg) + (shaset).DEBNumber() + (str);
Standard_EXPORT bool TopOpeBRepBuild_GetcontextNOFUFA();

Standard_EXPORT void debffsfs(const int i)
{
  std::cout << "+++ debffsfs " << i << std::endl;
}

Standard_EXPORT void debffflo(const int i)
{
  std::cout << "+++ debffflo " << i << std::endl;
}
#endif

static bool                  STATIC_motheropedef = false;
static TopOpeBRepBuild_GTopo STATIC_Gmotherope;

Standard_EXPORT void FUN_setmotherope(const TopOpeBRepBuild_GTopo& G)
{
  STATIC_Gmotherope   = G;
  STATIC_motheropedef = true;
}

Standard_EXPORT void FUN_unsetmotherope()
{
  STATIC_motheropedef = false;
}

Standard_EXPORT bool FUN_ismotheropedef()
{
  return STATIC_motheropedef;
}

Standard_EXPORT const TopOpeBRepBuild_GTopo& FUN_motherope()
{
  return STATIC_Gmotherope;
}

// Standard_IMPORT extern bool GLOBAL_classifysplitedge;
Standard_EXPORTEXTERN bool GLOBAL_classifysplitedge;
// Standard_IMPORT extern bool GLOBAL_revownsplfacori;
Standard_EXPORTEXTERN bool GLOBAL_revownsplfacori;
Standard_EXPORT void       FUNBUILD_ANCESTORRANKPREPARE(TopOpeBRepBuild_Builder&              B,
                                                        const NCollection_List<TopoDS_Shape>& LF1,
                                                        const NCollection_List<TopoDS_Shape>& LF2,
                                                        const TopOpeBRepDS_Config             c1,
                                                        const TopOpeBRepDS_Config             c2);
Standard_EXPORT void       FUNBUILD_ANCESTORRANKGET(TopOpeBRepBuild_Builder& B,
                                                    const TopoDS_Shape&      f,
                                                    bool&                    of1,
                                                    bool&                    of2);

static int FUN_getAncestorFsp(TopOpeBRepBuild_Builder&              B,
                              TopOpeBRepTool_ShapeClassifier&       SC,
                              const NCollection_List<TopoDS_Shape>& LF,
                              const TopoDS_Shape&                   fsp,
                              bool&                                 p3ddef,
                              gp_Pnt&                               p3d);
static int FUN_getAncestorFsp(TopOpeBRepBuild_Builder&              B,
                              TopOpeBRepTool_ShapeClassifier&       SC,
                              const NCollection_List<TopoDS_Shape>& LF1,
                              const NCollection_List<TopoDS_Shape>& LF2,
                              const TopoDS_Shape&                   fsp);
#ifdef OCCT_DEBUG
// static void FUN_getAncestorFsp(const occ::handle<TopOpeBRepDS_HDataStructure>&
// HDS,TopOpeBRepTool_ShapeClassifier& SC,const NCollection_List<TopoDS_Shape>& LF1,const
// NCollection_List<TopoDS_Shape>& LF2,const NCollection_List<TopoDS_Shape>& spFOR,
// NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>*
// SplitAnc);
#endif

static int FUN_getAncestorFsp(TopOpeBRepBuild_Builder&              B,
                              TopOpeBRepTool_ShapeClassifier&       SC,
                              const NCollection_List<TopoDS_Shape>& LF,
                              const TopoDS_Shape&                   fsp,
                              bool&                                 p3ddef,
                              gp_Pnt&                               p3d)
{
  const TopOpeBRepDS_DataStructure& BDS = B.DataStructure()->DS(); // How to do static <--> const

  NCollection_List<TopoDS_Shape>::Iterator itf(LF);
  for (; itf.More(); itf.Next())
  {
    const TopoDS_Face& f  = TopoDS::Face(itf.Value());
    TopAbs_State       st = SC.StateShapeShape(fsp, f, 1);
    if ((st == TopAbs_UNKNOWN) || (st == TopAbs_OUT))
      continue;
    if (st == TopAbs_ON)
    {
      if (!p3ddef)
      {
        bool ok = BRepClass3d_SolidExplorer::FindAPointInTheFace(TopoDS::Face(fsp), p3d);
        if (!ok)
          return 0;
        p3ddef = true;
      }
      gp_Pnt2d p2d;
      double   dd = 0.;
      bool     ok = FUN_tool_projPonF(p3d, f, p2d, dd);
      if (!ok)
        return 0;
      double tolf = BRep_Tool::Tolerance(f) * 1.e1;
      if (dd > tolf)
        return 0;
      double                  TolClass = 1e-8;
      BRepTopAdaptor_FClass2d FClass2d(f, TolClass);
      st = FClass2d.Perform(p2d);
    }
    if (st == TopAbs_IN)
    {
      int ianc = BDS.Shape(f);
      return ianc;
    }
  } // itf(LF)
  return 0;
}

static int FUN_getAncestorFsp(TopOpeBRepBuild_Builder&              B,
                              TopOpeBRepTool_ShapeClassifier&       SC,
                              const NCollection_List<TopoDS_Shape>& LF1,
                              const NCollection_List<TopoDS_Shape>& LF2,
                              const TopoDS_Shape&                   fsp)
{
  // IMPORTANT : fsp is split IN/OUT of F1,so it has only one ancestor face
  // LF1 faces sdm with LF2
  const TopOpeBRepDS_DataStructure& BDS = B.DataStructure()->DS();
  bool                              of1, of2;
  FUNBUILD_ANCESTORRANKGET(B, fsp, of1, of2);
  int rkfsp = 0;
  if (of1 && !of2)
    rkfsp = 1;
  else if (of2 && !of1)
    rkfsp = 2;
  bool unk = (rkfsp == 0);

  int    rkf1   = BDS.AncestorRank(LF1.First());
  int    rkf2   = BDS.AncestorRank(LF2.First());
  bool   p3ddef = false;
  gp_Pnt p3d;

  bool ison1 = (rkf1 == rkfsp);
  bool ison2 = (rkf2 == rkfsp);

  int ianc1 = 0, ianc2 = 0;
  if (ison1 || unk)
    ianc1 = FUN_getAncestorFsp(B, SC, LF1, fsp, p3ddef, p3d);
  if (ison1)
    return ianc1;

  if (ison2 || unk)
    ianc2 = FUN_getAncestorFsp(B, SC, LF2, fsp, p3ddef, p3d);
  if (ison2)
    return ianc2;

  if (ianc1 + ianc2 > 0)
  {
    if (ianc1 == 0)
      return ianc2;
    else if (ianc2 == 0)
      return ianc1;
    else
      return 0; // fsp has 2 ancestor faces
  }
  return 0;
}

Standard_EXPORT NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>* GLOBAL_SplitAnc =
  NULL; // xpu260598

static void FUN_getAncestorFsp(
  TopOpeBRepBuild_Builder&                                         B,
  TopOpeBRepTool_ShapeClassifier&                                  SC,
  const NCollection_List<TopoDS_Shape>&                            LF1,
  const NCollection_List<TopoDS_Shape>&                            LF2,
  const TopoDS_Shape&                                              FOR,
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>* SplitAnc)
{
  if (SplitAnc == NULL)
    return;

  bool issplitIN = B.IsSplit(FOR, TopAbs_IN);
  bool issplitOU = B.IsSplit(FOR, TopAbs_OUT);
  if (!issplitIN && !issplitOU)
    return;

  NCollection_List<TopoDS_Shape> spFOR;
  if (issplitIN)
    FDS_copy(B.Splits(FOR, TopAbs_IN), spFOR);
  if (issplitOU)
    FDS_copy(B.Splits(FOR, TopAbs_OUT), spFOR);

  for (NCollection_List<TopoDS_Shape>::Iterator itsp(spFOR); itsp.More(); itsp.Next())
  {
    const TopoDS_Shape& fsp = itsp.Value();

    bool isbound = SplitAnc->IsBound(fsp);
    if (isbound)
      continue;

    int ianc = FUN_getAncestorFsp(B, SC, LF1, LF2, fsp);
    if (ianc != 0)
    {
      SplitAnc->Bind(fsp, ianc);
    }
  } // itsp
}

Standard_EXPORT NCollection_List<TopoDS_Shape>* GLOBAL_lfr1         = NULL;
Standard_EXPORT bool                            GLOBAL_lfrtoprocess = false;

// Standard_IMPORT extern NCollection_List<TopoDS_Shape>* GLOBAL_lfr1;
// Standard_IMPORT NCollection_List<TopoDS_Shape>* GLOBAL_lfr1;
// Standard_IMPORT extern bool GLOBAL_lfrtoprocess;
// Standard_IMPORT bool GLOBAL_lfrtoprocess;

//=================================================================================================

void TopOpeBRepBuild_Builder::GFillFaceSFS(const TopoDS_Shape&                   FOR,
                                           const NCollection_List<TopoDS_Shape>& LSO2,
                                           const TopOpeBRepBuild_GTopo&          Gin,
                                           TopOpeBRepBuild_ShellFaceSet&         SFS)
{
  TopAbs_State TB1, TB2;
  Gin.StatesON(TB1, TB2);

#ifdef OCCT_DEBUG
  int  iF;
  bool tSPS = GtraceSPS(FOR, iF);
  if (tSPS)
    std::cout << std::endl;
#endif
  const TopOpeBRepDS_DataStructure& BDS     = myDataStructure->DS();
  bool                              tosplit = GToSplit(FOR, TB1);
  bool                              tomerge = GToMerge(FOR);
#ifdef OCCT_DEBUG
//  int iFOR = BDS.Shape(FOR);
#endif
  int rkFOR = BDS.AncestorRank(FOR);

#ifdef OCCT_DEBUG
  if (tSPS)
  {
    GdumpSHASTA(FOR, TB1, "--- GFillFaceSFS START ");
    std::cout << " tosplit " << tosplit << " tomerge " << tomerge << std::endl;
    debffsfs(iF);
  }
#endif

  TopoDS_Shape FF = FOR;
  FF.Orientation(TopAbs_FORWARD);
  bool hsd            = myDataStructure->HasSameDomain(FOR); // xpu280598
  GLOBAL_lfrtoprocess = false;

  if (tosplit && tomerge)
  {

    // merge des faces SameDomain

    // on effectue le merge ssi FOR est la reference de ses faces SameDomain
    int                 iref  = myDataStructure->SameDomainReference(FOR);
    const TopoDS_Shape& fref  = myDataStructure->Shape(iref);
    bool                isref = FOR.IsSame(fref);

    bool makemerge = isref;
    // xpu280199 PRO16958 : f10=ref(f6), GToMerge(f10)=false
    bool makemergeref = GToMerge(fref);
    makemerge         = makemerge || (!makemergeref && (rkFOR == 1));

    if (makemerge)
    {

#ifdef OCCT_DEBUG
      if (tSPS)
      {
        GdumpSHASTA(FOR, TB1, "[[[[[[[[[[[[[[[[[[[[[[[[[[ GFillFaceSFS makemerge START ");
        std::cout << std::endl;
      }
#endif

      bool performfufa = true;
#ifdef OCCT_DEBUG
      performfufa = !TopOpeBRepBuild_GetcontextNOFUFA();
#endif
      if (performfufa)
      {
        GLOBAL_lfrtoprocess = true;
        if (GLOBAL_lfrtoprocess)
        {
          if (GLOBAL_lfr1 == NULL)
            GLOBAL_lfr1 = (NCollection_List<TopoDS_Shape>*)new NCollection_List<TopoDS_Shape>();
          GLOBAL_lfr1->Clear();
        }
      }

      // xpu280598 : Filling up GLOBAL_SplitAnc = {(fsp,ifanc)}
      //              . fsp = spIN/OU(fanc),
      //              . fanc hsdm is the unique ancestor face
      if (GLOBAL_SplitAnc == NULL)
        GLOBAL_SplitAnc =
          (NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>*)new NCollection_DataMap<
            TopoDS_Shape,
            int,
            TopTools_ShapeMapHasher>();
      GLOBAL_SplitAnc->Clear();

      NCollection_List<TopoDS_Shape> LFSO, LFDO, LFSO1, LFDO1, LFSO2, LFDO2;
      GFindSamDomSODO(FF, LFSO, LFDO); // -980617
                                       //      FDSSDM_sordor(FF,LFSO,LFDO);
      int rankF = GShapeRank(FF), rankX = (rankF) ? ((rankF == 1) ? 2 : 1) : 0;
      GFindSameRank(LFSO, rankF, LFSO1);
      GFindSameRank(LFDO, rankF, LFDO1);
      GFindSameRank(LFSO, rankX, LFSO2);
      GFindSameRank(LFDO, rankX, LFDO2);
      NCollection_List<TopoDS_Shape> LF1, LF2;

      TopOpeBRepBuild_GTopo GM;
      TopAbs_State          TB, NTB;
      bool                  dodo;
      int                   l1, l2;

      // WES : toutes les faces de meme orientation topologique
      LF1  = LFSO1; // NYI pointeurs
      LF2  = LFSO2; // NYI pointeurs
      l1   = LF1.Extent();
      l2   = LF2.Extent();
      dodo = (l1 != 0) && (l2 != 0);

      FUN_unsetmotherope(); // +12/07

      GM = Gin;
      GM.ChangeConfig(TopOpeBRepDS_SAMEORIENTED, TopOpeBRepDS_SAMEORIENTED);
      if (dodo)
      {
#ifdef OCCT_DEBUG
        if (tSPS)
        {
          GdumpSAMDOM(LF1, (char*)"LF1 (LFSO1) : ");
          GdumpSAMDOM(LF2, (char*)"LF2 (LFSO2) : ");
        }
#endif
        GLOBAL_classifysplitedge = true;
        GFillFacesWESMakeFaces(LF1, LF2, LSO2, GM);
        GLOBAL_classifysplitedge = false;

        GLOBAL_revownsplfacori = true;
        FUNBUILD_ANCESTORRANKPREPARE(*this,
                                     LF1,
                                     LF2,
                                     TopOpeBRepDS_SAMEORIENTED,
                                     TopOpeBRepDS_SAMEORIENTED);
        if (hsd)
          FUN_getAncestorFsp((*this),
                             myShapeClassifier,
                             LF1,
                             LF2,
                             FOR,
                             GLOBAL_SplitAnc); // xpu280598

        // GLOBAL_lfrtoprocess = t
        // ==> on ne stocke PAS les faces 'startelement' dans le SFS
        //     mais dans GLOBAL_lfr1,
        // GLOBAL_lfrtoprocess = f
        // ==> on stocke normalement dans le SFS et pas dans GLOBAL_lfr1
        // NYI : + argument a la methode GSplitFaceSFS ?? a voir

        // ici : GLOBAL_lfrtoprocess = t
        // clang-format off
	if (GLOBAL_lfr1==NULL) GLOBAL_lfr1=(NCollection_List<TopoDS_Shape>*)new NCollection_List<TopoDS_Shape>(); //flo150998
        // clang-format on
        GLOBAL_lfr1->Clear();
        GSplitFaceSFS(FOR, LSO2, GM, SFS);
        GLOBAL_lfrtoprocess    = false;
        GLOBAL_revownsplfacori = false;
      }

      // WES : FOR + faces d'orientation topologique opposee
      LF1  = LFSO1; // NYI pointeurs
      LF2  = LFDO2; // NYI pointeurs
      l1   = LF1.Extent();
      l2   = LF2.Extent();
      dodo = (l1 != 0) && (l2 != 0);

      GM  = Gin;       // OUT,OUT-> OUT,IN + IN OUT
      TB  = TB2;       //           ------
      NTB = TopAbs_IN; // NTB = (TB == TopAbs_OUT) ? TopAbs_IN : TopAbs_OUT;
      GM.ChangeValue(TB, TopAbs_ON, false);
      GM.ChangeValue(NTB, TopAbs_ON, true);
      GM.ChangeConfig(TopOpeBRepDS_SAMEORIENTED, TopOpeBRepDS_DIFFORIENTED);
      FUN_setmotherope(GM); // +12/07
      if (dodo)
      {
#ifdef OCCT_DEBUG
        if (tSPS)
        {
          TopAbs_State TB11, TB21;
          GM.StatesON(TB11, TB21);
          std::cout << std::endl;
          std::cout << "@@@@" << std::endl;
          std::cout << "@@@@@@@@ partie 1 : ";
          TopAbs::Print(TB11, std::cout);
          std::cout << " ";
          TopAbs::Print(TB21, std::cout);
          std::cout << std::endl;
          std::cout << "@@@@" << std::endl;
          GdumpSAMDOM(LF1, (char*)"LF1 (LFSO1) : ");
          GdumpSAMDOM(LF2, (char*)"LF2 (LFDO2) : ");
          std::cout << std::endl;
        }
#endif
        GLOBAL_classifysplitedge = true;
        GFillFacesWESMakeFaces(LF1, LF2, LSO2, GM);
        GLOBAL_classifysplitedge = false;

        GLOBAL_revownsplfacori = true;
        FUNBUILD_ANCESTORRANKPREPARE(*this,
                                     LF1,
                                     LF2,
                                     TopOpeBRepDS_SAMEORIENTED,
                                     TopOpeBRepDS_DIFFORIENTED);
        if (hsd)
          FUN_getAncestorFsp((*this),
                             myShapeClassifier,
                             LF1,
                             LF2,
                             FOR,
                             GLOBAL_SplitAnc); // xpu280598

        if (Opecom())
        {
          bool issplitIN = IsSplit(FOR, TopAbs_IN);
          if (issplitIN)
          {
            const NCollection_List<TopoDS_Shape>& spFOR = Splits(FOR, TopAbs_IN);
            NCollection_List<TopoDS_Shape>        spFORcopy;
            FDS_copy(spFOR, spFORcopy);

            NCollection_List<TopoDS_Shape>::Iterator it(LF1);
            for (; it.More(); it.Next())
            {
              const TopoDS_Shape& f       = it.Value();
              bool                issplit = IsSplit(f, TopAbs_IN);
              if (issplit)
                ChangeSplit(f, TopAbs_IN).Clear();
            }
            it.Initialize(LF2);
            for (; it.More(); it.Next())
            {
              const TopoDS_Shape& f       = it.Value();
              bool                issplit = IsSplit(f, TopAbs_IN);
              if (issplit)
                ChangeSplit(f, TopAbs_IN).Clear();
            }
            ChangeSplit(FOR, TopAbs_IN).Append(spFORcopy); // keep split for reference
          } // issplitIN
        } // OpeCom

        GSplitFaceSFS(FOR, LSO2, GM, SFS);
        GLOBAL_revownsplfacori = false;

      } // dodo

      if (!Opecom())
      {
        GM = Gin; // OUT,OUT-> OUT,IN + IN OUT
        TB = TB1; //                    ------
        //      NTB = (TB == TopAbs_OUT) ? TopAbs_IN : TopAbs_OUT;
        NTB = TopAbs_IN;
        GM.ChangeValue(TopAbs_ON, TB, false);
        GM.ChangeValue(TopAbs_ON, NTB, true);
        GM.ChangeConfig(TopOpeBRepDS_SAMEORIENTED, TopOpeBRepDS_DIFFORIENTED);
        FUN_setmotherope(GM); // +12/07
        if (dodo)
        {
#ifdef OCCT_DEBUG
          if (tSPS)
          {
            TopAbs_State TB12, TB22;
            GM.StatesON(TB12, TB22);
            std::cout << std::endl;
            std::cout << "@@@@" << std::endl;
            std::cout << "@@@@@@@@ partie 2 : ";
            TopAbs::Print(TB12, std::cout);
            std::cout << " ";
            TopAbs::Print(TB22, std::cout);
            std::cout << std::endl;
            std::cout << "@@@@" << std::endl;
            GdumpSAMDOM(LF1, (char*)"LF1 (LFSO1) : ");
            GdumpSAMDOM(LF2, (char*)"LF2 (LFDO2) : ");
            std::cout << std::endl;
          }
#endif
          GLOBAL_classifysplitedge = true;
          GFillFacesWESMakeFaces(LF1, LF2, LSO2, GM);
          GLOBAL_classifysplitedge = false;

          GLOBAL_revownsplfacori = true;
          FUNBUILD_ANCESTORRANKPREPARE(*this,
                                       LF1,
                                       LF2,
                                       TopOpeBRepDS_SAMEORIENTED,
                                       TopOpeBRepDS_DIFFORIENTED);
          if (hsd)
            FUN_getAncestorFsp((*this),
                               myShapeClassifier,
                               LF1,
                               LF2,
                               FOR,
                               GLOBAL_SplitAnc); // xpu280598
          GSplitFaceSFS(FOR, LSO2, GM, SFS);
          GLOBAL_revownsplfacori = false;
        }
      } // !Opecom

      FUN_unsetmotherope(); // +12/07

#ifdef OCCT_DEBUG
      if (tSPS)
      {
        GdumpSHASTA(FOR, TB1, "]]]]]]]]]]]]]]]]]]]]]]]]]] GFillFaceSFS makemerge END ");
        std::cout << std::endl;
      }
#endif

      GLOBAL_SplitAnc->Clear(); // xpu280598

      // FuseFace
      SFS.ChangeStartShapes().Extent();
      if (performfufa)
      {
#ifdef OCCT_DEBUG
        if (tSPS)
          debffflo(iF);
#endif
//	const NCollection_List<TopoDS_Shape>& lou = Splits(FF,TopAbs_OUT); int nou = lou.Extent();
//	const NCollection_List<TopoDS_Shape>& lin = Splits(FF,TopAbs_IN);  int nin = lin.Extent();
//	GCopyList(lou,*GLOBAL_lfr1);
//	GCopyList(lin,*GLOBAL_lfr1);
#ifdef OCCT_DEBUG
//	int nlfr1 = GLOBAL_lfr1->Extent();
#endif

        // NYI : Builder += methode pour le process fufa
        // clang-format off
	TopOpeBRepBuild_FuseFace fufa; NCollection_List<TopoDS_Shape> ldum; int addinternal = 1; // disparition
        // clang-format on
        fufa.Init(ldum, *GLOBAL_lfr1, addinternal);
        fufa.PerformFace();
        bool isdone = fufa.IsDone();
        if (!isdone)
          return;
#ifdef OCCT_DEBUG
//	bool ismodified = fufa.IsModified();
#endif
        const NCollection_List<TopoDS_Shape>& lfr2 = fufa.LFuseFace();
        //
        //	const NCollection_List<TopoDS_Shape>& lfr2 = *GLOBAL_lfr1
        // les faces remplacantes
        for (NCollection_List<TopoDS_Shape>::Iterator itlfr2(lfr2); itlfr2.More(); itlfr2.Next())
        {
          const TopoDS_Shape& flfr2 = itlfr2.Value();

#ifdef OCCT_DEBUG
          if (tSPS)
          {
            DEBSHASET(ss, "--- FillFaceSFS apres fufa", SFS, " AddStartElement SFS+ face ");
            GdumpSHA(flfr2, (void*)ss.ToCString());
            std::cout << " ";
            TopAbs::Print(TB1, std::cout) << " : 1 face ";
            TopAbs::Print(flfr2.Orientation(), std::cout);
            std::cout << std::endl;
          }
#endif
          SFS.AddStartElement(flfr2);
        }
      } // performfufa (context)

    } // makemerge
  } // tosplit && tomerge

  else if (tosplit && !tomerge)
  {
    GSplitFace(FOR, Gin, LSO2);
    GSplitFaceSFS(FOR, LSO2, Gin, SFS);
  }
  else if (!tosplit && !tomerge)
  {
    GSplitFaceSFS(FOR, LSO2, Gin, SFS);
  }

  myEdgeAvoid.Clear();

#ifdef OCCT_DEBUG
  if (tSPS)
  {
    GdumpSHASTA(FOR, TB1, "--- GFillFaceSFS END ");
    std::cout << std::endl;
  }
#endif

} // GFillFaceSFS
