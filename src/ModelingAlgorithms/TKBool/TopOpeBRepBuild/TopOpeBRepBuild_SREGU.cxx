// Created on: 1997-10-21
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

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_ShellToSolid.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GetcontextNOREGUSO();
extern bool TopOpeBRepBuild_GetcontextREGUXPU();
extern bool TopOpeBRepBuild_GettraceSAVSREGU();

Standard_EXPORT void debreguso(const int iS)
{
  std::cout << "++ debreguso " << iS << std::endl;
}
#endif

#ifdef DRAW
  #include <DBRep.hxx>
#endif

//=================================================================================================

void TopOpeBRepBuild_Builder::RegularizeSolids(const TopoDS_Shape&                   SO,
                                               const NCollection_List<TopoDS_Shape>& lnewSolid,
                                               NCollection_List<TopoDS_Shape>&       LOSO)
{
  LOSO.Clear();
  myMemoSplit.Clear();

  NCollection_List<TopoDS_Shape>::Iterator itl(lnewSolid);
  for (; itl.More(); itl.Next())
  {
    const TopoDS_Shape&            newSolid = itl.Value();
    NCollection_List<TopoDS_Shape> newSolidLOSO;
    RegularizeSolid(SO, newSolid, newSolidLOSO);
#ifdef OCCT_DEBUG
//    int nnewSolidLOSO = newSolidLOSO.Extent(); // DEB
#endif
    LOSO.Append(newSolidLOSO);
  }
#ifdef OCCT_DEBUG
//  int nLOSO = LOSO.Extent(); // DEB
#endif
  int nr = myMemoSplit.Extent();
  if (nr == 0)
    return;

  // lsosdSO = solids SameDomain de SO
  NCollection_List<TopoDS_Shape> lsosdSO, lsosdSO1, lsosdSO2;
  GFindSamDom(SO, lsosdSO1, lsosdSO2);
  lsosdSO.Append(lsosdSO1);
  lsosdSO.Append(lsosdSO2);

  NCollection_List<TopoDS_Shape>::Iterator itlsosdSO(lsosdSO);
  for (; itlsosdSO.More(); itlsosdSO.Next())
  {
    const TopoDS_Shape& sosdSO = itlsosdSO.Value();
    // au moins une arete de SO dont le Split() est lui meme Split()
    TopExp_Explorer x;
    for (x.Init(sosdSO, TopAbs_FACE); x.More(); x.Next())
    {
      //    for (TopExp_Explorer x(sosdSO,TopAbs_FACE);x.More();x.Next()) {
      const TopoDS_Shape& f        = x.Current();
      int                 rankf    = GShapeRank(f);
      TopAbs_State        staf     = (rankf == 1) ? myState1 : myState2;
      bool                issplitf = IsSplit(f, staf);
      if (!issplitf)
        continue;

      NCollection_List<TopoDS_Shape>  newlspf;
      NCollection_List<TopoDS_Shape>& lspf = ChangeSplit(f, staf);
#ifdef OCCT_DEBUG
//      int nlspf = lspf.Extent(); // DEB
#endif
      for (NCollection_List<TopoDS_Shape>::Iterator itl1(lspf); itl1.More(); itl1.Next())
      {
        const TopoDS_Shape& fsp     = itl1.Value();
        bool                fspmemo = myMemoSplit.Contains(fsp);
        if (!fspmemo)
          newlspf.Append(fsp);
        else
        {
          NCollection_List<TopoDS_Shape>& lspfsp = ChangeSplit(fsp, staf);
          GCopyList(lspfsp, newlspf);
        }
      }
      lspf.Clear();
      GCopyList(newlspf, lspf);

      //      if (staf == TopAbs_IN) {
      //	// IN Solide <=> ON ??? : M.A.J de Split(TopAbs_ON);
      //	bool issplitON = IsSplit(f,TopAbs_ON);
      //	NCollection_List<TopoDS_Shape>& lONf = ChangeSplit(f,TopAbs_ON);
      //	int nONf = lONf.Extent(); // DEB
      //	lONf.Clear();
      //	GCopyList(newlspf,lONf);
      //      } // TopAbs_IN

    } // (sosdSO,TopAbs_FACE)
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::RegularizeSolid(const TopoDS_Shape&             SS,
                                              const TopoDS_Shape&             anewSolid,
                                              NCollection_List<TopoDS_Shape>& LOSO)
{
  LOSO.Clear();
  const TopoDS_Solid& newSolid = TopoDS::Solid(anewSolid);
  bool                toregu   = true;
  bool                usestos  = true;

#ifdef OCCT_DEBUG
  int  iS;
  bool tSPS = GtraceSPS(SS, iS);
  //  bool savsregu = TopOpeBRepBuild_GettraceSAVSREGU();
  if (TopOpeBRepBuild_GetcontextNOREGUSO())
    toregu = false;
  if (TopOpeBRepBuild_GetcontextREGUXPU())
    usestos = false;
  if (tSPS)
    debreguso(iS);
#endif

  if (!toregu)
  {
    LOSO.Append(newSolid);
    return;
  }

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
       osns; // OldShells --> NewShells;
  bool rw = false;
  bool rf = false;
  myFSplits.Clear();

  rw = TopOpeBRepTool::RegularizeShells(newSolid, osns, myFSplits);

  if (!rw)
  {
    LOSO.Append(newSolid);
    return;
  }

  NCollection_List<TopoDS_Shape> newSolids;
  if (usestos)
  {
    TopOpeBRepBuild_ShellToSolid stos;
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
      Iterator itosns(osns);
    for (; itosns.More(); itosns.Next())
    {

      /* JYL 120499 : a mettre dans l'areabuilder du solidbuilder du shelltosolid

            // solution 1 : alerter le SolidClassifier utilise dans le SolidBuilder
            //              de ShellToSolid qu'il y a de des aretes mutilconnexes (2 faces)
            //              dans oldshe QU'IL NE FAUT PAS UTILISER pour tirer un point
            //              representatif du shell.

            // map des edges -> list of face de oldshe
            NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
         TopTools_ShapeMapHasher> maef;
            TopExp::MapShapesAndAncestors(oldshe,TopAbs_EDGE,TopAbs_FACE,maef);
            int ima=1,nma=maef.Extent();
            for(;ima<=nma;ima++) {
          const TopoDS_Edge& eevit = TopoDS::Edge(maef.FindKey(ima));
          const NCollection_List<TopoDS_Shape>& lfa = maef(ima);
          int nlfa = lfa.Extent();
          if (nlfa > 2) {
          }
            }
            // fin solution1
      */

      const NCollection_List<TopoDS_Shape>& lns = itosns.Value();
      for (NCollection_List<TopoDS_Shape>::Iterator iw(lns); iw.More(); iw.Next())
      {
        stos.AddShell(TopoDS::Shell(iw.Value()));
      }
    }
    stos.MakeSolids(newSolid, newSolids);
    rf = (newSolids.Extent() != 0);
  }
  else
  {
    rf = false;
    //    rf = TopOpeBRepTool::RegularizeSolid(newSolid,osns,newSolids);
  }

  if (!rf)
  {
    LOSO.Append(newSolid);
    return;
  }

#ifdef OCCT_DEBUG
  if (tSPS)
  {
    std::cout << "RegularizeSolid " << iS << std::endl;
    debreguso(iS);
  }
#endif

  // LOSO = nouvelles Solids regularisees de newSolid
  NCollection_List<TopoDS_Shape>::Iterator itlnf(newSolids);
  for (; itlnf.More(); itlnf.Next())
    LOSO.Append(TopoDS::Solid(itlnf.Value()));

  // mise a jour des faces decoupees
  // Face(SS) = {E}, E-->Split(E) = {E'}, E'-->myFSplits(E') = {E''}
  // manc : E'-->E pour pouvoir relier
  // Split(manc(E')) = {myFSplits(E')}
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mfns; // mfns = faces de newSolid
  TopExp_Explorer                                        x;
  for (x.Init(newSolid, TopAbs_FACE); x.More(); x.Next())
  {
    const TopoDS_Shape& F = x.Current();
    mfns.Add(F);
  }

  // lssdSS = Solids SameDomain de SS
  NCollection_List<TopoDS_Shape> lssdSS, lssdSS1, lssdSS2;
  GFindSamDom(SS, lssdSS1, lssdSS2);
  lssdSS.Append(lssdSS1);
  lssdSS.Append(lssdSS2);

  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> manc;

  NCollection_List<TopoDS_Shape>::Iterator itlssdSS(lssdSS);
  for (; itlssdSS.More(); itlssdSS.Next())
  {
    const TopoDS_Shape& ssdSS = itlssdSS.Value();
#ifdef OCCT_DEBUG
//    int issdSS = myDataStructure->Shape(ssdSS); // DEB
#endif

    int          rankssdSS = GShapeRank(ssdSS);
    TopAbs_State stassdSS  = (rankssdSS == 1) ? myState1 : myState2;
#ifdef OCCT_DEBUG
//    bool issplitssdSS = IsSplit(ssdSS,stassdSS);
//    const NCollection_List<TopoDS_Shape>& lspssdSS = Splits(ssdSS,stassdSS);
//    int nlspssdSS = lspssdSS.Extent();
#endif

    // iteration sur les faces de ssdSS
    for (x.Init(ssdSS, TopAbs_FACE); x.More(); x.Next())
    {

      // ssdSSf : 1 face de ssdSS = 1 solid SameDomain de Ss
      const TopoDS_Shape& ssdSSf = x.Current();

#ifdef OCCT_DEBUG
      int  issdSSf    = 0;
      bool tSPSssdSSf = GtraceSPS(ssdSSf, issdSSf);
      if (tSPSssdSSf)
        debreguso(issdSSf);
#endif

      TopAbs_State stassdSSf = stassdSS;

      NCollection_List<TopoDS_Shape>& lspssdSSf = ChangeSplit(ssdSSf, stassdSSf);
#ifdef OCCT_DEBUG
//      bool issplitssdSSf = IsSplit(ssdSSf,stassdSSf);
//      int nlspssdSSf = lspssdSSf.Extent();
#endif

      NCollection_List<TopoDS_Shape> newlspssdSSf; // nouvel ensemble de faces splittees de ssdSSf

      for (NCollection_List<TopoDS_Shape>::Iterator it(lspssdSSf); it.More(); it.Next())
      {
        const TopoDS_Shape& fspssdSSf = it.Value();

        bool inmfns = mfns.Contains(fspssdSSf);
        if (!inmfns)
          continue;

        // ssdSSf est une face de ssdSS (Cf supra E)
        // fspssdSSf de Split(ssdSSf) figure dans newSolid (Cf supra E')

        bool rfsplitloc = myFSplits.IsBound(fspssdSSf);
        if (rfsplitloc)
        {

          // ssdSSf (Cf supra E) a ete splittee, fspssdSSf = arete splittee de ssdSSf
          // fspssdSSf est une arete de Split(ssdSSf) ET figure dans newFace (Cf supra E')
          // fspssdSSf de newFace a ete redecoupee par RegularizeWires

          // son decoupage lrfsplit est stocke dans la DS du Builder
          const NCollection_List<TopoDS_Shape>& lrfsplit =
            myFSplits.Find(fspssdSSf); // Cf supra E''

          // on memorise que fspssdSSf est redecoupee ...
          myMemoSplit.Add(fspssdSSf);

          // on stocke le nouveau decoupage de fspssdSSf dans la DS du builder ...
          NCollection_List<TopoDS_Shape>& lsp = ChangeSplit(fspssdSSf, stassdSSf);
          GCopyList(lrfsplit, lsp);
        }
      } // lspssdSSf.More()
    } // explore(ssdSS,TopAbs_FACE)
  } // itlssdSS.More()

#ifdef DRAW
  if (tSPS)
    debreguso(iS);
  if (tSPS && savsregu)
  {
    TCollection_AsciiString str("sregu");
    str = str + iS;
    DBRep::Set(str.ToCString(), newSolid);
    std::cout << "newSolid " << str << " built on Solid " << iS << " saved" << std::endl;
  }
#endif

} // RegularizeSolid
