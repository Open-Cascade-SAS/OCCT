// Created on: 1993-06-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_ProgramError.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_FuseEdges.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

// #include <BRepAdaptor_Curve2d.hxx>
#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GetcontextNOFE();
#endif

//=================================================================================================

void TopOpeBRepBuild_Builder::End()
{
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  {
    // recodage de la continuite (edge,(f1,f2)) != C0 perdue lors
    // du changement de surface-support d'une arete non decoupee
    // d'une face tangente.
    for (int i = 1; i <= 2; i++)
    {
      TopoDS_Shape S;
      TopAbs_State sta = TopAbs_UNKNOWN;
      if (i == 1)
      {
        S   = myShape1;
        sta = myState1;
      }
      else if (i == 2)
      {
        S   = myShape2;
        sta = myState2;
      }

      TopExp_Explorer exs;
      for (exs.Init(S, TopAbs_SHELL); exs.More(); exs.Next())
      {
        //      for (TopExp_Explorer exs(S,TopAbs_SHELL);exs.More();exs.Next()) {
        const TopoDS_Shape& SH       = exs.Current();
        bool    SHhassha = BDS.HasShape(SH);
        if (!SHhassha)
          continue;

        bool Fhassam = false;
        TopExp_Explorer  exf;
        for (exf.Init(SH, TopAbs_FACE); exf.More(); exf.Next())
        {
          //	for (TopExp_Explorer exf(SH,TopAbs_FACE);exf.More(); exf.Next()) {
          Fhassam = myDataStructure->HasSameDomain(exf.Current());
          if (Fhassam)
            break;
        }
        if (!Fhassam)
          continue;

        NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> M;
        TopExp::MapShapesAndAncestors(S, TopAbs_EDGE, TopAbs_FACE, M);
        int nE = M.Extent();
        for (int iE = 1; iE <= nE; iE++)
        {
          const TopoDS_Edge& E = TopoDS::Edge(M.FindKey(iE));
          if (IsSplit(E, sta))
            continue;
          const NCollection_List<TopoDS_Shape>& LF = M.FindFromIndex(iE);
          if (LF.Extent() < 2)
            continue;

          // NYI : > 2 faces connexes par E : iterer sur tous les couples
          NCollection_List<TopoDS_Shape>::Iterator itLF(LF);
          const TopoDS_Face&                 F1 = TopoDS::Face(itLF.Value());
          itLF.Next();
          const TopoDS_Face& F2 = TopoDS::Face(itLF.Value());
          GeomAbs_Shape      C  = BRep_Tool::Continuity(E, F1, F2);
          if (C == GeomAbs_C0)
            continue;

          bool F1hassam = myDataStructure->HasSameDomain(F1);
          bool F2hassam = myDataStructure->HasSameDomain(F2);
          if (!F1hassam && !F2hassam)
            continue;

          bool F1issplit = IsSplit(F1, sta);
          bool F2issplit = IsSplit(F2, sta);
          F1issplit &= (Splits(F1, sta).Extent() != 0);
          F2issplit &= (Splits(F2, sta).Extent() != 0);
          if (!F1issplit && !F2issplit)
            continue;

          TopoDS_Face FF1 = F1, FF2 = F2;
          for (int ii = 1; ii <= 2; ii++)
          {
            if ((ii == 1 && !F1issplit) || (ii == 2 && !F2issplit))
              continue;
            TopoDS_Face F;
            if (ii == 1)
              F = F1;
            else
              F = F2;
            bool                   f = false;
            NCollection_List<TopoDS_Shape>::Iterator it;
            for (it.Initialize(Splits(F, sta)); it.More(); it.Next())
            {
              const TopoDS_Shape& SF = it.Value();
              if (SF.ShapeType() != TopAbs_FACE)
                continue;
              TopExp_Explorer ex;
              for (ex.Init(SF, TopAbs_EDGE); ex.More(); ex.Next())
              {
                if (ex.Current().IsSame(E))
                {
                  if (ii == 1)
                    FF1 = TopoDS::Face(it.Value());
                  else
                    FF2 = TopoDS::Face(it.Value());
                  f = true;
                  break;
                }
              }
              if (f)
                break;
            }
          }
          BRep_Builder B;
          B.Continuity(E, FF1, FF2, C);
        }
      }
    }
  }

  // M.A.J de la tolerance des vertex
  {
    // modified by NIZHNY-MKK  Fri Oct  6 16:13:33 2000.BEGIN
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                aMapOfNewEdges, aMapOfNewVertices;
    NCollection_List<TopoDS_Shape>::Iterator anIt;
    int                   iteratorofnewshape = 0;
    for (iteratorofnewshape = 1; iteratorofnewshape <= myDataStructure->NbCurves();
         iteratorofnewshape++)
    {
      for (anIt.Initialize(NewEdges(iteratorofnewshape)); anIt.More(); anIt.Next())
      {
        aMapOfNewEdges.Add(anIt.Value());
      }
    }
    for (iteratorofnewshape = 1; iteratorofnewshape <= myDataStructure->NbPoints();
         iteratorofnewshape++)
    {
      aMapOfNewVertices.Add(NewVertex(iteratorofnewshape));
    }
    // modified by NIZHNY-MKK  Fri Oct  6 16:13:36 2000.END

    TopoDS_Compound R;
    BRep_Builder    B;
    B.MakeCompound(R);
    const NCollection_List<TopoDS_Shape>&        lmergesha1 = Merged(myShape1, myState1);
    NCollection_List<TopoDS_Shape>::Iterator it(lmergesha1);
    for (; it.More(); it.Next())
      B.Add(R, it.Value());
    const NCollection_List<TopoDS_Shape>& LOES = Section();
#ifdef OCCT_DEBUG
//    int nLOES = LOES.Extent();
#endif

    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> idmoelof;
    TopExp::MapShapesAndAncestors(R, TopAbs_EDGE, TopAbs_FACE, idmoelof);
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> idmovloe;
    TopExp::MapShapesAndUniqueAncestors(R, TopAbs_VERTEX, TopAbs_EDGE, idmovloe);
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> idmovloes;
    for (NCollection_List<TopoDS_Shape>::Iterator I(LOES); I.More(); I.Next())
      TopExp::MapShapesAndAncestors(I.Value(), TopAbs_VERTEX, TopAbs_EDGE, idmovloes);
    int iv, nv = idmovloe.Extent();
    for (iv = 1; iv <= nv; iv++)
    {
      int     nP1  = 0;
      const TopoDS_Vertex& V    = TopoDS::Vertex(idmovloe.FindKey(iv));
      bool     isbe = idmovloes.Contains(V);
      if (!isbe)
        continue;

      const NCollection_List<TopoDS_Shape>& loe = idmovloe.FindFromIndex(iv);

#ifdef OCCT_DEBUG
//      int nloe = loe.Extent();
#endif
      NCollection_List<TopoDS_Shape>::Iterator iloe;
      for (iloe.Initialize(loe); iloe.More(); iloe.Next())
      {
        const TopoDS_Edge&          E    = TopoDS::Edge(iloe.Value());
        const NCollection_List<TopoDS_Shape>& lof  = idmoelof.FindFromKey(E);
        int            nlof = lof.Extent();
        nP1 += nlof + 1;
      }

      NCollection_Array1<gp_Pnt> TP(1, nP1);
      int   nP2 = 0;
      for (iloe.Initialize(loe); iloe.More(); iloe.Next())
      {
        const TopoDS_Edge& E  = TopoDS::Edge(iloe.Value());
        double      pv = BRep_Tool::Parameter(V, E);
        gp_Pnt             Pv;
        double      f, l;
        occ::handle<Geom_Curve> C3D = BRep_Tool::Curve(E, f, l);
        if (!C3D.IsNull())
        {
          Pv        = C3D->Value(pv);
          TP(++nP2) = Pv;
        }
        const NCollection_List<TopoDS_Shape>& lof = idmoelof.FindFromKey(E);
#ifdef OCCT_DEBUG
//	int nlof = lof.Extent();
#endif
        for (NCollection_List<TopoDS_Shape>::Iterator ilof(lof); ilof.More(); ilof.Next())
        {
          const TopoDS_Face&   F = TopoDS::Face(ilof.Value());
          double        tolpc;
          bool     pcf = FC2D_HasCurveOnSurface(E, F);
          occ::handle<Geom2d_Curve> C2D;
          if (!pcf)
          {
            C2D = FC2D_CurveOnSurface(E, F, f, l, tolpc);
            if (C2D.IsNull())
              throw Standard_ProgramError("TopOpeBRepBuild_Builder::End 1");
            double tolE = BRep_Tool::Tolerance(E);
            double tol  = std::max(tolE, tolpc);
            B.UpdateEdge(E, C2D, F, tol);
          }
          C2D                    = FC2D_CurveOnSurface(E, F, f, l, tolpc);
          gp_Pnt2d            P2 = C2D->Value(pv);
          BRepAdaptor_Surface BAS(F, false);
          Pv        = BAS.Value(P2.X(), P2.Y());
          TP(++nP2) = Pv;
          // modified by NIZHNY-MKK  Fri Sep 29 16:08:28 2000.BEGIN
          if (aMapOfNewEdges.Contains(E))
          {
            double anEdgeTol = BRep_Tool::Tolerance(E);
            double aFaceTol  = BRep_Tool::Tolerance(F);
            if (anEdgeTol < aFaceTol)
              B.UpdateEdge(E, aFaceTol);
          }
          // modified by NIZHNY-MKK  Fri Sep 29 16:08:32 2000.END
        }
        // modified by NIZHNY-MKK  Fri Sep 29 16:54:08 2000.BEGIN
        if (aMapOfNewVertices.Contains(V))
        {
          double aVertexTol = BRep_Tool::Tolerance(V);
          double anEdgeTol  = BRep_Tool::Tolerance(E);
          if (aVertexTol < anEdgeTol)
            B.UpdateVertex(V, anEdgeTol);
        }
        // modified by NIZHNY-MKK  Fri Sep 29 16:54:12 2000.END
      }

      double newtol = BRep_Tool::Tolerance(V);
      Bnd_Box       BOX;
      gp_Pnt        Pv = BRep_Tool::Pnt(V);
      BOX.Set(Pv);
      for (int i = 1; i <= nP2; i++)
      {
        const gp_Pnt& Pi = TP(i);
        BOX.Update(Pi.X(), Pi.Y(), Pi.Z());
      }
      double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      BOX.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      gp_Pnt        P1(aXmin, aYmin, aZmin);
      gp_Pnt        P2(aXmax, aYmax, aZmax);
      double d = P1.Distance(P2);
      if (d > newtol)
      {
#ifdef OCCT_DEBUG
        std::cout << "\npoint P" << iv << " " << Pv.X() << " " << Pv.Y() << " " << Pv.Z()
                  << std::endl;
        std::cout << "TopOpeBRepBuild_Builder::End BOX newtol " << newtol << " -> " << d
                  << std::endl;
#endif
        newtol = d;
        B.UpdateVertex(V, newtol);
      }
    }
  }

  bool makeFE = true;
#ifdef OCCT_DEBUG
  makeFE = !TopOpeBRepBuild_GetcontextNOFE();
#endif

  if (makeFE)
  {
    //    TopAbs_State state = myState1;
    NCollection_List<TopoDS_Shape>& ls = ChangeMerged(myShape1, myState1);
    for (NCollection_List<TopoDS_Shape>::Iterator itls(ls); itls.More(); itls.Next())
    {
      TopoDS_Shape&            SFE = itls.ChangeValue();
      TopOpeBRepTool_FuseEdges FE(SFE);

      // avoid fusing old edges
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mapOldEdges;
      TopExp::MapShapes(myShape1, TopAbs_EDGE, mapOldEdges);
      TopExp::MapShapes(myShape2, TopAbs_EDGE, mapOldEdges);
      FE.AvoidEdges(mapOldEdges);

      // Get List of edges that have been fused
      NCollection_DataMap<int, NCollection_List<TopoDS_Shape>> mle;
      FE.Edges(mle);

      int nle = mle.Extent();
      if (nle != 0)
      {
        FE.Perform();
        SFE = FE.Shape();

        NCollection_DataMap<int, TopoDS_Shape> mre;
        NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>   mlf;
        FE.ResultEdges(mre);
        FE.Faces(mlf);

        // edit the split to remove to edges to be fused and put them into the Merged
        //

        UpdateSplitAndMerged(mle, mre, mlf, TopAbs_IN);
        UpdateSplitAndMerged(mle, mre, mlf, TopAbs_OUT);
        UpdateSplitAndMerged(mle, mre, mlf, TopAbs_ON);
      }

    } // makeFE
  }
}

//=======================================================================
// function : UpdateSplitAndMerged
// purpose  :  edit the split to remove to edges to be fused and put them into the Merged
//=======================================================================

void TopOpeBRepBuild_Builder::UpdateSplitAndMerged(const NCollection_DataMap<int, NCollection_List<TopoDS_Shape>>& mle,
                                                   const NCollection_DataMap<int, TopoDS_Shape>&       mre,
                                                   const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&         mlf,
                                                   const TopAbs_State state)
{
  const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>&           MapSplit = MSplit(state);
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>::Iterator it;
  for (it.Initialize(MapSplit); it.More(); it.Next())
  {
    const TopoDS_Shape& e = it.Key();

    // For each edge of the MapSplit
    if (e.ShapeType() == TopAbs_EDGE)
    {

      // get the list of split edges.
      NCollection_List<TopoDS_Shape>& LstSplit = ChangeSplit(e, state);

      // for each edge of the list of split edges
      NCollection_List<TopoDS_Shape>::Iterator itSplitEdg;
      itSplitEdg.Initialize(LstSplit);
      while (itSplitEdg.More())
      {
        const TopoDS_Shape& edgecur = itSplitEdg.Value();

        // for each "packet" of edges to be fused
        NCollection_DataMap<int, NCollection_List<TopoDS_Shape>>::Iterator itLstEdg;
        itLstEdg.Initialize(mle);
        bool Found = false;
        while (itLstEdg.More() && !Found)
        {
          const int&     iLst    = itLstEdg.Key();
          const NCollection_List<TopoDS_Shape>& LmapEdg = mle.Find(iLst);

          // look for each edge of the list if it is in the map Split
          NCollection_List<TopoDS_Shape>::Iterator itEdg;
          itEdg.Initialize(LmapEdg);
          while (itEdg.More() && !Found)
          {
            const TopoDS_Shape& edgefuse = itEdg.Value();
            if (edgecur.IsSame(edgefuse))
            {
              Found = true;

              LstSplit.Remove(itSplitEdg);

              // edit the list of merged
              TopAbs_State stateMerged;
              if (ShapeRank(e) == 1)
                stateMerged = myState1;
              else
                stateMerged = myState2;

              NCollection_List<TopoDS_Shape> LstMerged;
              LstMerged.Append(mre(iLst));
              ChangeMerged(e, stateMerged) = LstMerged;
            }
            itEdg.Next();
          }

          itLstEdg.Next();
        }

        if (!Found)
        {
          itSplitEdg.Next();
        }
      }
    }
    // For each face of the MapSplit
    else if (e.ShapeType() == TopAbs_FACE)
    {
      // get the list of split faces.
      NCollection_List<TopoDS_Shape>& LstSplit = ChangeSplit(e, state);

      // for each face of the list of split faces
      NCollection_List<TopoDS_Shape>::Iterator itSplitFac;
      itSplitFac.Initialize(LstSplit);
      while (itSplitFac.More())
      {
        const TopoDS_Shape& facecur = itSplitFac.Value();

        if (mlf.IsBound(facecur))
        {
          LstSplit.InsertBefore(mlf(facecur), itSplitFac);
          LstSplit.Remove(itSplitFac);
        }
        else
        {
          itSplitFac.Next();
        }
      }
    }
  }
}
