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

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_WireToFace.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GetcontextNOREGUFA();
extern bool TopOpeBRepBuild_GetcontextREGUXPU();
extern bool TopOpeBRepBuild_GettraceSAVFREGU();

void debregufa(const int /*iF*/) {}
#endif


#define M_FORWARD(O) (O == TopAbs_FORWARD)
#define M_REVERSED(O) (O == TopAbs_REVERSED)

//=================================================================================================

void TopOpeBRepBuild_Builder::RegularizeFaces(const TopoDS_Shape&                   FF,
                                              const NCollection_List<TopoDS_Shape>& lnewFace,
                                              NCollection_List<TopoDS_Shape>&       LOF)
{
  LOF.Clear();
  myMemoSplit.Clear();

  NCollection_List<TopoDS_Shape>::Iterator itl(lnewFace);
  for (; itl.More(); itl.Next())
  {
    const TopoDS_Shape&            newFace = itl.Value();
    NCollection_List<TopoDS_Shape> newFaceLOF;
    RegularizeFace(FF, newFace, newFaceLOF);
#ifdef OCCT_DEBUG
//    int nnewFaceLOF = newFaceLOF.Extent(); // DEB
#endif
    LOF.Append(newFaceLOF);
  }
#ifdef OCCT_DEBUG
//  int nLOF = LOF.Extent(); // DEB
#endif

  int nr = myMemoSplit.Extent();
  if (nr == 0)
    return;

  // lfsdFF = faces SameDomain de FF
  NCollection_List<TopoDS_Shape> lfsdFF, lfsdFF1, lfsdFF2;
  GFindSamDom(FF, lfsdFF1, lfsdFF2);
  lfsdFF.Append(lfsdFF1);
  lfsdFF.Append(lfsdFF2);

  NCollection_List<TopoDS_Shape>::Iterator itlfsdFF(lfsdFF);
  for (; itlfsdFF.More(); itlfsdFF.Next())
  {
    const TopoDS_Shape& fsdFF = itlfsdFF.Value();
    // au moins une arete de FF dont le Split() est lui meme Split()
    TopExp_Explorer x;
    for (x.Init(fsdFF, TopAbs_EDGE); x.More(); x.Next())
    {
      //    for (TopExp_Explorer x(fsdFF,TopAbs_EDGE);x.More();x.Next()) {
      const TopoDS_Shape& e = x.Current();
#ifdef OCCT_DEBUG
//      int ie = myDataStructure->Shape(e); //DEB
//      bool issect = myDataStructure->DS().IsSectionEdge(TopoDS::Edge(e));
#endif

      int          ranke   = GShapeRank(e);
      TopAbs_State staeope = (ranke == 1) ? myState1 : myState2;

      for (int iiista = 1; iiista <= 2; iiista++)
      {

        TopAbs_State stae = staeope;
        if (iiista == 2)
          stae = TopAbs_ON;

        bool issplite = IsSplit(e, stae);
        if (!issplite)
          continue;

        NCollection_List<TopoDS_Shape>& lspe = ChangeSplit(e, stae);
#ifdef OCCT_DEBUG
//	int nlspe = lspe.Extent(); // DEB
#endif
        NCollection_List<TopoDS_Shape> newlspe;
        for (NCollection_List<TopoDS_Shape>::Iterator itl1(lspe); itl1.More(); itl1.Next())
        {
          const TopoDS_Shape& esp     = itl1.Value();
          bool                espmemo = myMemoSplit.Contains(esp);
          if (!espmemo)
            newlspe.Append(esp);
          else
          {
            const NCollection_List<TopoDS_Shape>& lspesp = Splits(esp, stae);
            GCopyList(lspesp, newlspe);
          }
        }
        lspe.Clear();
        GCopyList(newlspe, lspe);

      } // iiista
    } // explorer (fsdFF,TopAbs_EDGE)
  } // itlfsdFF.More()
} // RegularizeFaces

/*static void FUN_setInternal(TopoDS_Face& F)
{
  TopExp_Explorer ex(F,TopAbs_EDGE);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> meF,meR,meI;
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    TopAbs_Orientation oE = E.Orientation();
    bool isclo = BRep_Tool::IsClosed(E,F); // E has 2d rep on F
    if (isclo) continue;
    bool isb = false; // the edge is FOR + REV in F
    if      (M_FORWARD(oE))  {meF.Add(E); isb = meR.Contains(E);}
    else if (M_REVERSED(oE)) {meR.Add(E); isb = meF.Contains(E);}
    if (isb) meI.Add(E.Oriented(TopAbs_INTERNAL));
  }

  BRep_Builder BB;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator it(meI);
  for (; it.More(); it.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(it.Key());
    BB.Remove(F,E.Oriented(TopAbs_FORWARD));
    BB.Remove(F,E.Oriented(TopAbs_REVERSED));
    BB.Add(F,E);
  }
}*/

//=================================================================================================

void TopOpeBRepBuild_Builder::RegularizeFace(const TopoDS_Shape&             FF,
                                             const TopoDS_Shape&             anewFace,
                                             NCollection_List<TopoDS_Shape>& LOF)
{
  LOF.Clear();
  const TopoDS_Face& newFace = TopoDS::Face(anewFace);
  bool               toregu  = true;
  bool               usewtof = true;

#ifdef OCCT_DEBUG
  int  iF;
  bool tSPSFF = GtraceSPS(FF, iF);
  //  bool savfregu = TopOpeBRepBuild_GettraceSAVFREGU();
  if (TopOpeBRepBuild_GetcontextNOREGUFA())
    toregu = false;
  if (TopOpeBRepBuild_GetcontextREGUXPU())
    usewtof = false;
  if (tSPSFF)
    debregufa(iF);
#endif

  // If the same edge appears FOR+REV in the resulting face and
  // whereas it's not a closing edge, set it as INTERNAL instead.
  // FRA60275(iF=4) + PRO16297
  //  FUN_setInternal(newFace);

  if (!toregu)
  {
    LOF.Append(newFace);
    return;
  }

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
       ownw; // OldWires --> NewWires;
  bool rw = false;
  bool rf = false;
  myESplits.Clear();

  rw = TopOpeBRepTool::RegularizeWires(newFace, ownw, myESplits);

  if (!rw)
  {
    LOF.Append(newFace);
    return;
  }

  NCollection_List<TopoDS_Shape> newfaces;
  if (usewtof)
  {
    TopOpeBRepBuild_WireToFace wtof;
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
      Iterator itownw(ownw);
    for (; itownw.More(); itownw.Next())
    {
      const NCollection_List<TopoDS_Shape>& lw = itownw.Value();

      // xpu200798 : cto902D4 f14ou
      // recall ownw = {(ow = old wire, lnw = {list of new wires descendant of old wire}}
      // if lnw is empty, then ow is kept unchanged.
      bool kept = lw.IsEmpty();
      if (kept)
      {
        const TopoDS_Wire& ow = TopoDS::Wire(itownw.Key());
        wtof.AddWire(ow);
      }
      for (NCollection_List<TopoDS_Shape>::Iterator iw(lw); iw.More(); iw.Next())
      {
        const TopoDS_Wire& w = TopoDS::Wire(iw.Value());
        wtof.AddWire(w);
      }
    }
    wtof.MakeFaces(newFace, newfaces);
#ifdef OCCT_DEBUG
//    int nnewfaces = newfaces.Extent(); // DEB
#endif
    rf = (newfaces.Extent() != 0);
  }
  else
  {
    rf = TopOpeBRepTool::RegularizeFace(newFace, ownw, newfaces);
  }

  if (!rf)
  {
    LOF.Append(newFace);
    return;
  }

#ifdef OCCT_DEBUG
  if (tSPSFF)
  {
    std::cout << "RegularizeFace " << iF << std::endl;
    debregufa(iF);
  }
#endif

  // LOF = nouvelles faces regularisees de newFace
  NCollection_List<TopoDS_Shape>::Iterator itlnf(newfaces);
  for (; itlnf.More(); itlnf.Next())
    LOF.Append(TopoDS::Face(itlnf.Value()));

  // mise a jour des aretes decoupees
  // Edge(FF) = {E}, E-->Split(E) = {E'}, E'-->myESplits(E') = {E''}

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> menf; // menf = aretes de newFace
  TopExp_Explorer                                        x;
  for (x.Init(newFace, TopAbs_EDGE); x.More(); x.Next())
  {
    const TopoDS_Shape& E = x.Current();
    menf.Add(E);
  }

  // lfsdFF = faces SameDomain de FF
  NCollection_List<TopoDS_Shape> lfsdFF, lfsdFF1, lfsdFF2;
  GFindSamDom(FF, lfsdFF1, lfsdFF2);
  lfsdFF.Append(lfsdFF1);
  lfsdFF.Append(lfsdFF2);

  NCollection_List<TopoDS_Shape>::Iterator itlfsdFF(lfsdFF);
  for (; itlfsdFF.More(); itlfsdFF.Next())
  {
    const TopoDS_Shape& fsdFF = itlfsdFF.Value();

#ifdef OCCT_DEBUG
    int  ifsdFF    = 0;
    bool tSPSfsdFF = GtraceSPS(fsdFF, ifsdFF);
    if (tSPSfsdFF)
      debregufa(ifsdFF);
#endif

    int          rankfsdFF = GShapeRank(fsdFF);
    TopAbs_State stafsdFF  = (rankfsdFF == 1) ? myState1 : myState2;
#ifdef OCCT_DEBUG
//    bool issplitfsdFF = IsSplit(fsdFF,stafsdFF);
#endif

    /*#ifdef OCCT_DEBUG
        const NCollection_List<TopoDS_Shape>& lspfsdFF = Splits(fsdFF,stafsdFF);
        int nlspfsdFF = lspfsdFF.Extent();
    #endif*/

    // iteration sur les aretes de fsdFF
    for (x.Init(fsdFF, TopAbs_EDGE); x.More(); x.Next())
    {

      // fsdFFe : 1 edge de fsdFF = 1 face SameDomain de FF
      const TopoDS_Shape& fsdFFe = x.Current();

#ifdef OCCT_DEBUG
      int  ifsdFFe    = 0;
      bool tSPSfsdFFe = GtraceSPS(fsdFFe, ifsdFFe);
      if (tSPSfsdFFe)
        debregufa(ifsdFFe);
#endif

      // a priori, on ne peut avoir plus de deux etats splites
      // sur l'arete , soit (IN + ON) , soit (OUT + ON)
      for (int iiista = 1; iiista <= 2; iiista++)
      {
        TopAbs_State stafsdFFe = stafsdFF;
        if (iiista == 2)
          stafsdFFe = TopAbs_ON;

        NCollection_List<TopoDS_Shape>& lspfsdFFe = ChangeSplit(fsdFFe, stafsdFFe);
#ifdef OCCT_DEBUG
//	bool issplitfsdFFe = IsSplit(fsdFFe,stafsdFFe);
//	int nlspfsdFFe = lspfsdFFe.Extent();
#endif

        for (NCollection_List<TopoDS_Shape>::Iterator it(lspfsdFFe); it.More(); it.Next())
        {

          // fsdFFe (Cf supra E) a ete splittee, espfdsFFe = arete splittee de fsdFFe

          const TopoDS_Shape& espfsdFFe = it.Value();
          bool                inmenf    = menf.Contains(espfsdFFe);
          if (!inmenf)
            continue;

          // fsdFFe (Cf supra E) a ete splittee, espfdsFFe = arete splittee de fsdFFe
          // espfsdFFe est une arete de Split(fsdFFe) ET figure dans newFace (Cf supra E')

          bool resplitloc = myESplits.IsBound(espfsdFFe);
          if (resplitloc)
          {

            // fsdFFe (Cf supra E) a ete splittee, espfdsFFe = arete splittee de fsdFFe
            // espfsdFFe est une arete de Split(fsdFFe) ET figure dans newFace (Cf supra E')
            // espfsdFFe de newFace a ete redecoupee par RegularizeWires

            // son decoupage lresplit est stocke dans la DS du Builder
            const NCollection_List<TopoDS_Shape>& lresplit =
              myESplits.Find(espfsdFFe); // Cf supra E''

            // on memorise que espfsdFFe est redecoupee ...
            myMemoSplit.Add(espfsdFFe);

            // on stocke le nouveau decoupage de espfsdFFe dans la DS du builder ...
            NCollection_List<TopoDS_Shape>& lsp = ChangeSplit(espfsdFFe, stafsdFFe);
            GCopyList(lresplit, lsp);
          }
        } // it.More
      } // iiista
    } // explore(fsdFF,TopAbs_EDGE)
  } // itlfsdFF.More()


} // RegularizeFace
