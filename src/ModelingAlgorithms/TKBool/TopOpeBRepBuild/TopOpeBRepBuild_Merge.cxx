// Created on: 1994-08-30
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <BRepCheck_Result.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

// #include <DBRep.hxx>
#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceSPS();
#endif
//------------
// static int ifvNbFace = 0;
// static char *name = "                 ";
//-------------
static void CorrectEdgeOrientation(TopoDS_Shape& aWire)
{

  NCollection_List<TopoDS_Shape> anEdgeList, anAuxList, aTrueEdgeList;
  BRep_Builder         BB;
  TopoDS_Vertex        vf, vl, v1f, v1l;
  bool     append = true;

  TopoDS_Iterator tdi(aWire, false, false);
  for (; tdi.More(); tdi.Next())
    anEdgeList.Append(tdi.Value());

  int n = anEdgeList.Extent();
  if (n <= 1)
    return;

  NCollection_List<TopoDS_Shape>::Iterator anIt(anEdgeList);

  TopoDS_Shape anCurEdge = anIt.Value();
  TopExp::Vertices(TopoDS::Edge(anCurEdge), vf, vl, true);
  aTrueEdgeList.Append(anCurEdge);
  anIt.Next();

  while (n > 0 && append)
  {

    for (; anIt.More(); anIt.Next())
    {
      append    = false;
      anCurEdge = anIt.Value();
      TopExp::Vertices(TopoDS::Edge(anCurEdge), v1f, v1l, true);
      if (v1f.IsSame(vl))
      {
        aTrueEdgeList.Append(anCurEdge);
        vl     = v1l;
        append = true;
        continue;
      }
      if (v1l.IsSame(vf))
      {
        aTrueEdgeList.Append(anCurEdge);
        vf     = v1f;
        append = true;
        continue;
      }

      if (v1l.IsSame(vl))
      {
        TopoDS_Shape anRevEdge = anCurEdge.Reversed();
        aTrueEdgeList.Append(anRevEdge);
        vl     = v1f;
        append = true;
        continue;
      }
      if (v1f.IsSame(vf))
      {
        TopoDS_Shape anRevEdge = anCurEdge.Reversed();
        aTrueEdgeList.Append(anRevEdge);
        vf     = v1l;
        append = true;
        continue;
      }

      anAuxList.Append(anCurEdge);
    }

    anEdgeList.Assign(anAuxList);
    anAuxList.Clear(); // something wrong in Assign when list contains 1 element.
    n = anEdgeList.Extent();
    anIt.Initialize(anEdgeList);
  }

  if (n > 0)
    aTrueEdgeList.Append(anEdgeList);

  aWire.Nullify();
  BB.MakeWire(TopoDS::Wire(aWire));
  anIt.Initialize(aTrueEdgeList);
  for (; anIt.More(); anIt.Next())
    BB.Add(aWire, anIt.Value());
}

static void CorrectUnclosedWire(TopoDS_Shape& aWire)
{
  //  std::cout << "-------CorrectUnclosedWire" << std::endl;
  BRep_Builder    BB;
  TopoDS_Iterator tdi(aWire, false, false);
  for (; tdi.More(); tdi.Next())
  {
    const TopoDS_Shape& ed  = tdi.Value();
    int    nbv = ed.NbChildren();
    if (nbv <= 1)
    {
      //      std::cout << "Remove bad edge" << std::endl;
      BB.Remove(aWire, ed);
    }
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> VElists;
  VElists.Clear();
  TopExp::MapShapesAndAncestors(aWire, TopAbs_VERTEX, TopAbs_EDGE, VElists);
  int nbVer = VElists.Extent(), i;

  for (i = 1; i <= nbVer; i++)
  {
    const NCollection_List<TopoDS_Shape>& Elist = VElists.FindFromIndex(i);
    if (Elist.Extent() == 1)
    {
      const TopoDS_Shape& anEdge = Elist.First();
      //      std::cout << "Remove redundant edge" << std::endl;
      BB.Remove(aWire, anEdge);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeShapes(const TopoDS_Shape& S1,
                                          const TopAbs_State  ToBuild1,
                                          const TopoDS_Shape& S2,
                                          const TopAbs_State  ToBuild2)
{
  bool lesmemes = S1.IsEqual(S2);
  if (lesmemes)
  {
#ifdef OCCT_DEBUG
    std::cout << "TopOpeBRepBuild : S1 == S2" << std::endl;
#endif
    return;
  }

#ifdef OCCT_DEBUG
  GdumpSHASETreset();
#endif

  myState1                = ToBuild1;
  myState2                = ToBuild2;
  myShape1                = S1;
  myShape2                = S2;
  bool S1null = S1.IsNull();
  bool S2null = S2.IsNull();

  MapShapes(S1, S2);
  SplitSectionEdges();

  //======================== debut KPart
  if (IsKPart())
  {
    MergeKPart();
    ClearMaps();
    return;
  }
  //======================== fin KPart

  bool RevOri1 = Reverse(ToBuild1, ToBuild2);
  bool RevOri2 = Reverse(ToBuild2, ToBuild1);

  // Create a face set <SFS> connected by edges
  // -----------------------------------------
  TopOpeBRepBuild_ShellFaceSet SFS;

#ifdef OCCT_DEBUG
  SFS.DEBNumber(GdumpSHASETindex());
#endif

  // NYI : SplitCompound appele par SplitShape

  TopOpeBRepTool_ShapeExplorer ex1;

  TopAbs_ShapeEnum t1 = TopAbs_COMPOUND, tex1 = TopAbs_COMPOUND;

  if (!S1null)
  {
    t1 = tex1 = TopType(S1);
    if (t1 == TopAbs_COMPOUND)
    {
      tex1 = TopAbs_SOLID;
      ex1.Init(S1, tex1);
      if (!ex1.More())
      {
        tex1 = TopAbs_SHELL;
        ex1.Init(S1, tex1);
        if (!ex1.More())
        {
          tex1 = TopAbs_FACE;
          ex1.Init(S1, tex1);
          if (!ex1.More())
          {
            tex1 = TopAbs_EDGE;
            ex1.Init(S1, tex1);
          }
        }
      }
    }
    else if (t1 == TopAbs_WIRE)
    {
      tex1 = TopAbs_EDGE;
      ex1.Init(S1, tex1);
    }
    else
      ex1.Init(S1, tex1);
    SplitShapes(ex1, ToBuild1, ToBuild2, SFS, RevOri1);
  }

  TopOpeBRepTool_ShapeExplorer ex2;

  TopAbs_ShapeEnum t2 = TopAbs_COMPOUND, tex2 = TopAbs_COMPOUND;

  if (!S2null)
  {
    t2 = tex2 = TopType(S2);
    if (t2 == TopAbs_COMPOUND)
    {
      tex2 = TopAbs_SOLID;
      ex2.Init(S2, tex2);
      if (!ex2.More())
      {
        tex2 = TopAbs_SHELL;
        ex2.Init(S2, tex2);
        if (!ex2.More())
        {
          tex2 = TopAbs_FACE;
          ex2.Init(S2, tex2);
          if (!ex2.More())
          {
            tex2 = TopAbs_EDGE;
            ex2.Init(S2, tex2);
          }
        }
      }
    }
    else if (t2 == TopAbs_WIRE)
    {
      tex2 = TopAbs_EDGE;
      ex2.Init(S2, tex2);
    }
    else
      ex2.Init(S2, tex2);
    SplitShapes(ex2, ToBuild2, ToBuild1, SFS, RevOri2);
  }

  // S1 or S2 = COMPOUND : connect them the list of merged shapes build
  // on the first subshape.

  if (!S1null)
  {
    if (t1 == TopAbs_COMPOUND)
    {
      NCollection_List<TopoDS_Shape>& L1 = ChangeMerged(S1, ToBuild1);
      ex1.Init(S1, tex1);
      if (ex1.More())
      {
        const TopoDS_Shape&   SS1  = ex1.Current();
        NCollection_List<TopoDS_Shape>& LSS1 = ChangeMerged(SS1, ToBuild1);
        L1                         = LSS1;
      }
    }
  }

  if (!S2null)
  {
    if (t2 == TopAbs_COMPOUND)
    {
      NCollection_List<TopoDS_Shape>& L2 = ChangeMerged(S2, ToBuild2);
      ex2.Init(S2, tex2);
      if (ex2.More())
      {
        const TopoDS_Shape&   SS2  = ex2.Current();
        NCollection_List<TopoDS_Shape>& LSS2 = ChangeMerged(SS2, ToBuild2);
        L2                         = LSS2;
      }
    }
  }

  ClearMaps();

} // MergeShapes

//=================================================================================================

bool TopOpeBRepBuild_Builder::Classify() const
{
  return myClassifyVal;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::ChangeClassify(const bool classify)
{
  myClassifyDef = true;
  myClassifyVal = classify;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeSolids(const TopoDS_Shape& S1,
                                          const TopAbs_State  ToBuild1,
                                          const TopoDS_Shape& S2,
                                          const TopAbs_State  ToBuild2)
{
  MergeShapes(S1, ToBuild1, S2, ToBuild2);
} // MergeSolids

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeSolid(const TopoDS_Shape& S, const TopAbs_State ToBuild)
{
  TopoDS_Shape Snull;
  MergeShapes(S, ToBuild, Snull, ToBuild);
} // MergeSolid

//=================================================================================================

void TopOpeBRepBuild_Builder::MakeSolids(TopOpeBRepBuild_SolidBuilder& SOBU,
                                         NCollection_List<TopoDS_Shape>&         L)
{
  TopoDS_Shape newSolid;
  TopoDS_Shape newShell;
  for (SOBU.InitSolid(); SOBU.MoreSolid(); SOBU.NextSolid())
  {
    myBuildTool.MakeSolid(newSolid);
    for (SOBU.InitShell(); SOBU.MoreShell(); SOBU.NextShell())
    {
      bool isold = SOBU.IsOldShell();
      if (isold)
        newShell = SOBU.OldShell();
      else
      {
        myBuildTool.MakeShell(newShell);
        for (SOBU.InitFace(); SOBU.MoreFace(); SOBU.NextFace())
        {
          const TopoDS_Shape& F = SOBU.Face();
          myBuildTool.AddShellFace(newShell, F);
        }
      }
      // clang-format off
      myBuildTool.Closed(newShell,true); // NYI : check exact du caractere closed du shell
      // clang-format on
      myBuildTool.AddSolidShell(newSolid, newShell);
    }
    L.Append(newSolid);
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::MakeShells(TopOpeBRepBuild_SolidBuilder& SOBU,
                                         NCollection_List<TopoDS_Shape>&         L)
{
  TopoDS_Shape newShell;
  for (SOBU.InitShell(); SOBU.MoreShell(); SOBU.NextShell())
  {
    bool isold = SOBU.IsOldShell();
    if (isold)
      newShell = SOBU.OldShell();
    else
    {
      myBuildTool.MakeShell(newShell);
      for (SOBU.InitFace(); SOBU.MoreFace(); SOBU.NextFace())
      {
        const TopoDS_Shape& F = SOBU.Face();
        myBuildTool.AddShellFace(newShell, F);
      }
    }
    L.Append(newShell);
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::MakeFaces(const TopoDS_Shape&          aFace,
                                        TopOpeBRepBuild_FaceBuilder& FABU,
                                        NCollection_List<TopoDS_Shape>&        L)
{
#ifdef OCCT_DEBUG
  int iF   = 0;
  bool tSPS = GtraceSPS(aFace, iF);
  if (tSPS)
  {
    GdumpFABU(FABU);
  }
#endif
  bool hashds = (!myDataStructure.IsNull());
  TopoDS_Shape     newFace;
  TopoDS_Shape     newWire;

  for (FABU.InitFace(); FABU.MoreFace(); FABU.NextFace())
  {
    myBuildTool.CopyFace(aFace, newFace);
    bool hns = false;
    if (hashds)
    {
      const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
      hns                                   = BDS.HasNewSurface(aFace);
      if (hns)
      {
        const occ::handle<Geom_Surface>& SU = BDS.NewSurface(aFace);
        myBuildTool.UpdateSurface(newFace, SU);
      }
    }

    for (FABU.InitWire(); FABU.MoreWire(); FABU.NextWire())
    {
      bool isold = FABU.IsOldWire();
      if (isold)
        newWire = FABU.OldWire();
      else
      {
        myBuildTool.MakeWire(newWire);
        for (FABU.InitEdge(); FABU.MoreEdge(); FABU.NextEdge())
        {
          const TopoDS_Shape& E = FABU.Edge();
          if (hns)
            myBuildTool.UpdateSurface(E, aFace, newFace);
          myBuildTool.AddWireEdge(newWire, E);
        }
      }
      //----------- IFV
      if (!isold)
      {
        BRepCheck_Analyzer bca(newWire, false);
        if (!bca.IsValid())
        {
          newWire.Free(true);
          CorrectUnclosedWire(newWire);
          const occ::handle<BRepCheck_Result>&      bcr = bca.Result(newWire);
          NCollection_List<BRepCheck_Status>::Iterator itl(bcr->Status());
          for (; itl.More(); itl.Next())
          {
            if (itl.Value() == BRepCheck_BadOrientationOfSubshape)
            {
              CorrectEdgeOrientation(newWire);
              break;
            }
          }
        }
      }
      myBuildTool.Closed(newWire, true); // NYI : check exact du caractere closed du wire
      myBuildTool.AddFaceWire(newFace, newWire);
    }

    L.Append(newFace);
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::MakeEdges(const TopoDS_Shape&          anEdge,
                                        TopOpeBRepBuild_EdgeBuilder& EDBU,
                                        NCollection_List<TopoDS_Shape>&        L)
{
#ifdef OCCT_DEBUG
  int iE;
  bool tSPS = GtraceSPS(anEdge, iE);
  int ne   = 0;
#endif

  TopoDS_Shape newEdge;
  for (EDBU.InitEdge(); EDBU.MoreEdge(); EDBU.NextEdge())
  {

    // 1 vertex sur edge courante => suppression edge
    int nloop = 0;
    for (EDBU.InitVertex(); EDBU.MoreVertex(); EDBU.NextVertex())
      nloop++;
    if (nloop <= 1)
      continue;

    myBuildTool.CopyEdge(anEdge, newEdge);

    bool hasvertex = false;
    for (EDBU.InitVertex(); EDBU.MoreVertex(); EDBU.NextVertex())
    {
      TopoDS_Shape       V    = EDBU.Vertex();
      TopAbs_Orientation Vori = V.Orientation();

      bool hassd = myDataStructure->HasSameDomain(V);
      if (hassd)
      {
        // on prend le vertex reference de V
        int iref = myDataStructure->SameDomainReference(V);
        V                     = myDataStructure->Shape(iref);
        V.Orientation(Vori);
      }

      TopAbs_Orientation oriV = V.Orientation();
      if (oriV != TopAbs_EXTERNAL)
      {
        // betonnage
        bool equafound = false;
        TopExp_Explorer  exE(newEdge, TopAbs_VERTEX);
        for (; exE.More(); exE.Next())
        {
          const TopoDS_Shape& VE    = exE.Current();
          TopAbs_Orientation  oriVE = VE.Orientation();
          if (V.IsEqual(VE))
          {
            equafound = true;
            break;
          }
          else if (oriVE == TopAbs_FORWARD || oriVE == TopAbs_REVERSED)
          {
            if (oriV == oriVE)
            {
              equafound = true;
              break;
            }
          }
          else if (oriVE == TopAbs_INTERNAL || oriVE == TopAbs_EXTERNAL)
          {
            double parV  = EDBU.Parameter();
            double parVE = BRep_Tool::Parameter(TopoDS::Vertex(VE), TopoDS::Edge(newEdge));
            if (parV == parVE)
            {
              equafound = true;
              break;
            }
          }
        }
        if (!equafound)
        {
          hasvertex          = true;
          double parV = EDBU.Parameter();
          myBuildTool.AddEdgeVertex(newEdge, V);
          myBuildTool.Parameter(newEdge, V, parV);
        }
      }
    } // loop on vertices of new edge newEdge

#ifdef OCCT_DEBUG
    if (tSPS)
    {
      std::cout << std::endl;
    }
    if (tSPS)
    {
      std::cout << "V of new edge " << ++ne << std::endl;
    }
    if (tSPS)
    {
      GdumpEDG(newEdge);
    }
#endif

    if (hasvertex)
      L.Append(newEdge);
  } // loop on EDBU edges
} // MakeEdges

//=================================================================================================

bool TopOpeBRepBuild_Builder::IsMerged(const TopoDS_Shape& S,
                                                   const TopAbs_State  ToBuild) const
{
  const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>* p = NULL;
  if (ToBuild == TopAbs_OUT)
    p = &myMergedOUT;
  else if (ToBuild == TopAbs_IN)
    p = &myMergedIN;
  else if (ToBuild == TopAbs_ON)
    p = &myMergedON;
  if (p == NULL)
    return false;

  bool notbound = !(*p).IsBound(S);
  if (notbound)
  {
    return false;
  }
  else
  {
    const NCollection_List<TopoDS_Shape>& L       = Merged(S, ToBuild);
    bool            isempty = L.IsEmpty();
    return (!isempty);
  }
} // IsMerged

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::Merged(const TopoDS_Shape& S,
                                                            const TopAbs_State  ToBuild) const
{
  const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>* p = NULL;
  if (ToBuild == TopAbs_OUT)
    p = &myMergedOUT;
  else if (ToBuild == TopAbs_IN)
    p = &myMergedIN;
  else if (ToBuild == TopAbs_ON)
    p = &myMergedON;
  if (p == NULL)
    return myEmptyShapeList;

  if (!(*p).IsBound(S))
  {
    return myEmptyShapeList;
  }
  else
  {
    const NCollection_List<TopoDS_Shape>& L = (*p)(S).ListOnState();
    return L;
  }
} // Merged

//=================================================================================================

NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::ChangeMerged(const TopoDS_Shape& S,
                                                            const TopAbs_State  ToBuild)
{
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>* p = NULL;
  if (ToBuild == TopAbs_OUT)
    p = &myMergedOUT;
  else if (ToBuild == TopAbs_IN)
    p = &myMergedIN;
  else if (ToBuild == TopAbs_ON)
    p = &myMergedON;
  if (p == NULL)
    return myEmptyShapeList;

  if (!(*p).IsBound(S))
  {
    TopOpeBRepDS_ListOfShapeOn1State thelist;
    (*p).Bind(S, thelist);
  }
  NCollection_List<TopoDS_Shape>& L = (*p)(S).ChangeListOnState();
  return L;
} // ChangeMerged

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeEdges(const NCollection_List<TopoDS_Shape>&, // L1,
                                         const TopAbs_State,          // ToBuild1,
                                         const NCollection_List<TopoDS_Shape>&, // L2,
                                         const TopAbs_State,          // ToBuild2,
                                         const bool,      // Keepon1,
                                         const bool,      // Keepon2,
                                         const bool)      // Keepon12)
{
} // MergeEdges

//=================================================================================================

void TopOpeBRepBuild_Builder::MergeFaces(const NCollection_List<TopoDS_Shape>&, // S1,
                                         const TopAbs_State,          // ToBuild1,
                                         const NCollection_List<TopoDS_Shape>&, // S2,
                                         const TopAbs_State,          // ToBuild2
                                         const bool,      // onA,
                                         const bool,      // onB,
                                         const bool)      // onAB)
{
}
