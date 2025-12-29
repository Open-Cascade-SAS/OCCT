// Created on: 1996-09-05
// Created by: Yves FRICAUD
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Loop.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_MakeLoops.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

#include <cstdio>

BRepOffset_MakeLoops::BRepOffset_MakeLoops() = default;

//=================================================================================================

void BRepOffset_MakeLoops::Build(const NCollection_List<TopoDS_Shape>& LF,
                                 const occ::handle<BRepAlgo_AsDes>&    AsDes,
                                 BRepAlgo_Image&                       Image,
                                 BRepAlgo_Image&                       theImageVV,
                                 const Message_ProgressRange&          theRange)
{
  NCollection_List<TopoDS_Shape>::Iterator it(LF);
  NCollection_List<TopoDS_Shape>::Iterator itl, itLCE;
  BRepAlgo_Loop                            Loops;
  Loops.VerticesForSubstitute(myVerVerMap);
  Loops.SetImageVV(theImageVV);
  Message_ProgressScope aPSOuter(theRange, nullptr, 2);
  Message_ProgressScope aPS1(aPSOuter.Next(), "Init loops", LF.Size());
  for (; it.More(); it.Next(), aPS1.Next())
  {
    if (!aPS1.More())
    {
      return;
    }
    const TopoDS_Face& F = TopoDS::Face(it.Value());
    //---------------------------
    // Initialization of Loops.
    //---------------------------
    Loops.Init(F);
    //-----------------------------
    // return edges of F.
    //-----------------------------
    const NCollection_List<TopoDS_Shape>& LE = AsDes->Descendant(F);
    NCollection_List<TopoDS_Shape>        AddedEdges;

    for (itl.Initialize(LE); itl.More(); itl.Next())
    {
      TopoDS_Edge E = TopoDS::Edge(itl.Value());
      if (Image.HasImage(E))
      {
        //-------------------------------------------
        // E was already cut in another face.
        // Return the cut edges reorientate them as E.
        // See pb for the edges that have disappeared?
        //-------------------------------------------
        const NCollection_List<TopoDS_Shape>& LCE = Image.Image(E);
        for (itLCE.Initialize(LCE); itLCE.More(); itLCE.Next())
        {
          TopoDS_Shape CE = itLCE.Value().Oriented(E.Orientation());
          Loops.AddConstEdge(TopoDS::Edge(CE));
        }
      }
      else
      {
        Loops.AddEdge(E, AsDes->Descendant(E));
        AddedEdges.Append(E);
      }
    }
    //------------------------
    // Unwind.
    //------------------------
    Loops.Perform();
    Loops.WiresToFaces();
    //------------------------
    // MAJ SD.
    //------------------------
    const NCollection_List<TopoDS_Shape>& NF = Loops.NewFaces();
    //-----------------------
    // F => New faces;
    //-----------------------
    Image.Bind(F, NF);

    NCollection_List<TopoDS_Shape>::Iterator itAdded;
    for (itAdded.Initialize(AddedEdges); itAdded.More(); itAdded.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(itAdded.Value());
      //-----------------------
      //  E => New edges;
      //-----------------------
      const NCollection_List<TopoDS_Shape>& LoopNE = Loops.NewEdges(E);
      if (Image.HasImage(E))
      {
        Image.Add(E, LoopNE);
      }
      else
      {
        Image.Bind(E, LoopNE);
      }
    }
  }
  Loops.GetVerticesForSubstitute(myVerVerMap);
  if (myVerVerMap.IsEmpty())
    return;
  BRep_Builder          BB;
  Message_ProgressScope aPS2(aPSOuter.Next(), "Building loops", LF.Size());
  for (it.Initialize(LF); it.More(); it.Next(), aPS2.Next())
  {
    if (!aPS2.More())
    {
      return;
    }
    TopoDS_Shape                   F = it.Value();
    NCollection_List<TopoDS_Shape> LIF;
    Image.LastImage(F, LIF);
    for (itl.Initialize(LIF); itl.More(); itl.Next())
    {
      const TopoDS_Shape& IF = itl.Value();
      TopExp_Explorer     EdExp(IF, TopAbs_EDGE);
      for (; EdExp.More(); EdExp.Next())
      {
        TopoDS_Shape                   E = EdExp.Current();
        NCollection_List<TopoDS_Shape> VList;
        TopoDS_Iterator                VerExp(E);
        for (; VerExp.More(); VerExp.Next())
          VList.Append(VerExp.Value());
        NCollection_List<TopoDS_Shape>::Iterator itlv(VList);
        for (; itlv.More(); itlv.Next())
        {
          const TopoDS_Shape& V = itlv.Value();
          if (myVerVerMap.IsBound(V))
          {
            TopoDS_Shape NewV = myVerVerMap(V);
            E.Free(true);
            NewV.Orientation(V.Orientation());
            occ::handle<BRep_TVertex>& TV    = *((occ::handle<BRep_TVertex>*)&V.TShape());
            occ::handle<BRep_TVertex>& NewTV = *((occ::handle<BRep_TVertex>*)&NewV.TShape());
            if (TV->Tolerance() > NewTV->Tolerance())
              NewTV->Tolerance(TV->Tolerance());
            NewTV->ChangePoints().Append(TV->ChangePoints());
            AsDes->Replace(V, NewV);
            BB.Remove(E, V);
            BB.Add(E, NewV);
          }
        }
      }
    }
  }
}

//=================================================================================================

static bool IsBetweenCorks(const TopoDS_Shape&                   E,
                           const occ::handle<BRepAlgo_AsDes>&    AsDes,
                           const NCollection_List<TopoDS_Shape>& LContext)
{
  if (!AsDes->HasAscendant(E))
    return true;
  const NCollection_List<TopoDS_Shape>&    LF = AsDes->Ascendant(E);
  NCollection_List<TopoDS_Shape>::Iterator it;
  for (it.Initialize(LF); it.More(); it.Next())
  {
    const TopoDS_Shape&                      S     = it.Value();
    bool                                     found = false;
    NCollection_List<TopoDS_Shape>::Iterator it2;
    for (it2.Initialize(LContext); it2.More(); it2.Next())
    {
      if (S.IsSame(it2.Value()))
      {
        found = true;
        break;
      }
    }
    if (!found)
      return false;
  }
  return true;
}

//=================================================================================================

void BRepOffset_MakeLoops::BuildOnContext(const NCollection_List<TopoDS_Shape>& LContext,
                                          const BRepOffset_Analyse&             Analyse,
                                          const occ::handle<BRepAlgo_AsDes>&    AsDes,
                                          BRepAlgo_Image&                       Image,
                                          const bool                            InSide,
                                          const Message_ProgressRange&          theRange)
{
  //-----------------------------------------
  // unwinding of caps.
  //-----------------------------------------
  NCollection_List<TopoDS_Shape>::Iterator it(LContext);
  NCollection_List<TopoDS_Shape>::Iterator itl, itLCE;
  BRepAlgo_Loop                            Loops;
  Loops.VerticesForSubstitute(myVerVerMap);
  TopExp_Explorer                                        exp;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MapExtent;

  Message_ProgressScope aPS(theRange, "Building deepening faces", LContext.Extent());
  for (; it.More(); it.Next(), aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Face&                                     F = TopoDS::Face(it.Value());
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MBound;
    //-----------------------------------------------
    // Initialisation of Loops.
    // F is reversed it will be added in myOffC.
    // and myOffC will be reversed in the final result.
    //-----------------------------------------------
    TopoDS_Shape aReversedF = F.Reversed();
    if (InSide)
      Loops.Init(TopoDS::Face(aReversedF));
    //    if (InSide) Loops.Init(TopoDS::Face(F.Reversed()));
    else
      Loops.Init(F);
    //--------------------------------------------------------
    // return edges of F not modified by definition.
    //--------------------------------------------------------
    for (exp.Init(F.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next())
    {
      TopoDS_Edge CE = TopoDS::Edge(exp.Current());
      MBound.Add(CE);
      if (Analyse.HasAncestor(CE))
      {
        // the stop of cups except for the connectivity stops between caps.
        //      if (!AsDes->HasAscendant(CE)) {
        TopoDS_Shape aReversedE = CE.Reversed();
        if (InSide)
          Loops.AddConstEdge(CE);
        else
          Loops.AddConstEdge(TopoDS::Edge(aReversedE));
        //	else        Loops.AddConstEdge(TopoDS::Edge(CE.Reversed()));
      }
    }
    //------------------------------------------------------
    // Trace of offsets + connectivity edge between caps.
    //------------------------------------------------------
    const NCollection_List<TopoDS_Shape>& LE = AsDes->Descendant(F);
    NCollection_List<TopoDS_Shape>        AddedEdges;

    for (itl.Initialize(LE); itl.More(); itl.Next())
    {
      TopoDS_Edge E = TopoDS::Edge(itl.Value());
      if (Image.HasImage(E))
      {
        //-------------------------------------------
        // E was already cut in another face.
        // Return cut edges and orientate them as E.
        // See pb for the edges that have disappeared?
        //-------------------------------------------
        const NCollection_List<TopoDS_Shape>& LCE = Image.Image(E);
        for (itLCE.Initialize(LCE); itLCE.More(); itLCE.Next())
        {
          TopoDS_Shape CE = itLCE.Value().Oriented(E.Orientation());
          if (MapExtent.Contains(E))
          {
            Loops.AddConstEdge(TopoDS::Edge(CE));
            continue;
          }
          if (!MBound.Contains(E))
            CE.Reverse();
          if (InSide)
            Loops.AddConstEdge(TopoDS::Edge(CE));
          else
          {
            TopoDS_Shape aReversedE = CE.Reversed();
            Loops.AddConstEdge(TopoDS::Edge(aReversedE));
          }
          //	  else        Loops.AddConstEdge(TopoDS::Edge(CE.Reversed()));
        }
      }
      else
      {
        if (IsBetweenCorks(E, AsDes, LContext) && AsDes->HasDescendant(E))
        {
          // connection between 2 caps
          MapExtent.Add(E);
          NCollection_List<TopoDS_Shape> LV;
          if (InSide)
          {
            for (itLCE.Initialize(AsDes->Descendant(E)); itLCE.More(); itLCE.Next())
            {
              LV.Append(itLCE.Value().Reversed());
            }
            Loops.AddEdge(E, LV);
          }
          else
          {
            Loops.AddEdge(E, AsDes->Descendant(E));
          }
          AddedEdges.Append(E);
        }
        else if (IsBetweenCorks(E, AsDes, LContext))
        {
          TopoDS_Shape aLocalShape = E.Reversed();
          if (InSide)
            Loops.AddConstEdge(E);
          else
            Loops.AddConstEdge(TopoDS::Edge(aLocalShape));
          //	  if (InSide) Loops.AddConstEdge(TopoDS::Edge(E));
          //	  else        Loops.AddConstEdge(TopoDS::Edge(E.Reversed()));
        }
        else
        {
          TopoDS_Shape aLocalShape = E.Reversed();
          if (InSide)
            Loops.AddConstEdge(TopoDS::Edge(aLocalShape));
          else
            Loops.AddConstEdge(E);
          //	  if (InSide) Loops.AddConstEdge(TopoDS::Edge(E.Reversed()));
          //	  else        Loops.AddConstEdge(TopoDS::Edge(E));
        }
      }
    }
    //------------------------
    // Unwind.
    //------------------------
    Loops.Perform();
    Loops.WiresToFaces();
    //------------------------
    // MAJ SD.
    //------------------------
    const NCollection_List<TopoDS_Shape>& NF = Loops.NewFaces();
    //-----------------------
    // F => New faces;
    //-----------------------
    Image.Bind(F, NF);

    NCollection_List<TopoDS_Shape>::Iterator itAdded;
    for (itAdded.Initialize(AddedEdges); itAdded.More(); itAdded.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(itAdded.Value());
      //-----------------------
      //  E => New edges;
      //-----------------------
      if (Image.HasImage(E))
      {
        Image.Add(E, Loops.NewEdges(E));
      }
      else
      {
        Image.Bind(E, Loops.NewEdges(E));
      }
    }
  }
  Loops.GetVerticesForSubstitute(myVerVerMap);
  if (myVerVerMap.IsEmpty())
    return;
  BRep_Builder BB;
  for (it.Initialize(LContext); it.More(); it.Next())
  {
    TopoDS_Shape                   F = it.Value();
    NCollection_List<TopoDS_Shape> LIF;
    Image.LastImage(F, LIF);
    for (itl.Initialize(LIF); itl.More(); itl.Next())
    {
      const TopoDS_Shape& IF = itl.Value();
      TopExp_Explorer     EdExp(IF, TopAbs_EDGE);
      for (; EdExp.More(); EdExp.Next())
      {
        TopoDS_Shape                   E = EdExp.Current();
        NCollection_List<TopoDS_Shape> VList;
        TopoDS_Iterator                VerExp(E);
        for (; VerExp.More(); VerExp.Next())
          VList.Append(VerExp.Value());
        NCollection_List<TopoDS_Shape>::Iterator itlv(VList);
        for (; itlv.More(); itlv.Next())
        {
          const TopoDS_Shape& V = itlv.Value();
          if (myVerVerMap.IsBound(V))
          {
            TopoDS_Shape NewV = myVerVerMap(V);
            E.Free(true);
            NewV.Orientation(V.Orientation());
            occ::handle<BRep_TVertex>& TV    = *((occ::handle<BRep_TVertex>*)&V.TShape());
            occ::handle<BRep_TVertex>& NewTV = *((occ::handle<BRep_TVertex>*)&NewV.TShape());
            if (TV->Tolerance() > NewTV->Tolerance())
              NewTV->Tolerance(TV->Tolerance());
            NewTV->ChangePoints().Append(TV->ChangePoints());
            AsDes->Replace(V, NewV);
            BB.Remove(E, V);
            BB.Add(E, NewV);
          }
        }
      }
    }
  }
}

//=================================================================================================

void BRepOffset_MakeLoops::BuildFaces(const NCollection_List<TopoDS_Shape>& LF,
                                      const occ::handle<BRepAlgo_AsDes>&    AsDes,
                                      BRepAlgo_Image&                       Image,
                                      const Message_ProgressRange&          theRange)
{
  NCollection_List<TopoDS_Shape>::Iterator itr, itl, itLCE;
  bool                                     ToRebuild;
  BRepAlgo_Loop                            Loops;
  Loops.VerticesForSubstitute(myVerVerMap);
  BRep_Builder B;

  //----------------------------------
  // Loop on all faces //.
  //----------------------------------
  Message_ProgressScope aPS(theRange, "Building faces", LF.Size());
  for (itr.Initialize(LF); itr.More(); itr.Next(), aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    TopoDS_Face F = TopoDS::Face(itr.Value());
    Loops.Init(F);
    ToRebuild = false;
    NCollection_List<TopoDS_Shape> AddedEdges;

    if (!Image.HasImage(F))
    {
      //----------------------------------
      // Face F not yet reconstructed.
      //----------------------------------
      const NCollection_List<TopoDS_Shape>& LE = AsDes->Descendant(F);
      //----------------------------------------------------------------
      // first loop to find if the edges of the face were reconstructed.
      // - maj on map MONV. Some vertices on reconstructed edges
      // coincide geometrically with old but are not IsSame.
      //----------------------------------------------------------------
      NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> MONV;
      TopoDS_Vertex                                                            OV1, OV2, NV1, NV2;

      for (itl.Initialize(LE); itl.More(); itl.Next())
      {
        TopoDS_Edge E = TopoDS::Edge(itl.Value());
        if (Image.HasImage(E))
        {
          const NCollection_List<TopoDS_Shape>& LCE = Image.Image(E);
          if (LCE.Extent() == 1 && LCE.First().IsSame(E))
          {
            TopoDS_Shape aLocalShape = LCE.First().Oriented(E.Orientation());
            TopoDS_Edge  CE          = TopoDS::Edge(aLocalShape);
            //	    TopoDS_Edge CE = TopoDS::Edge(LCE.First().Oriented(E.Orientation()));
            Loops.AddConstEdge(CE);
            continue;
          }
          //----------------------------------
          // F should be reconstructed.
          //----------------------------------
          ToRebuild = true;
          for (itLCE.Initialize(LCE); itLCE.More(); itLCE.Next())
          {
            TopoDS_Shape aLocalShape = itLCE.Value().Oriented(E.Orientation());
            TopoDS_Edge  CE          = TopoDS::Edge(aLocalShape);
            //	    TopoDS_Edge CE = TopoDS::Edge(itLCE.Value().Oriented(E.Orientation()));
            TopExp::Vertices(E, OV1, OV2);
            TopExp::Vertices(CE, NV1, NV2);
            if (!OV1.IsSame(NV1))
              MONV.Bind(OV1, NV1);
            if (!OV2.IsSame(NV2))
              MONV.Bind(OV2, NV2);
            Loops.AddConstEdge(CE);
          }
        }
      }
      if (ToRebuild)
      {

        //-----------------------------------------------------------
        // Non-reconstructed edges on other faces are added.
        // If their vertices were reconstructed they are reconstructed.
        //-----------------------------------------------------------
        for (itl.Initialize(LE); itl.More(); itl.Next())
        {
          double      f, l;
          TopoDS_Edge E = TopoDS::Edge(itl.Value());
          BRep_Tool::Range(E, f, l);
          if (!Image.HasImage(E))
          {
            TopExp::Vertices(E, OV1, OV2);
            NCollection_List<TopoDS_Shape> LV;
            if (MONV.IsBound(OV1))
            {
              TopoDS_Vertex VV = TopoDS::Vertex(MONV(OV1));
              VV.Orientation(TopAbs_FORWARD);
              LV.Append(VV);
              TopoDS_Shape aLocalShape = VV.Oriented(TopAbs_INTERNAL);
              B.UpdateVertex(TopoDS::Vertex(aLocalShape), f, E, BRep_Tool::Tolerance(VV));
            }
            if (MONV.IsBound(OV2))
            {
              TopoDS_Vertex VV = TopoDS::Vertex(MONV(OV2));
              VV.Orientation(TopAbs_REVERSED);
              LV.Append(VV);
              TopoDS_Shape aLocalShape = VV.Oriented(TopAbs_INTERNAL);
              B.UpdateVertex(TopoDS::Vertex(aLocalShape), l, E, BRep_Tool::Tolerance(VV));
              //	      B.UpdateVertex(TopoDS::Vertex(VV.Oriented(TopAbs_INTERNAL)),
              //			     l,E,BRep_Tool::Tolerance(VV));
            }
            if (LV.IsEmpty())
              Loops.AddConstEdge(E);
            else
            {
              Loops.AddEdge(E, LV);
              AddedEdges.Append(E);
            }
          }
        }
      }
    }
    if (ToRebuild)
    {
      //------------------------
      // Reconstruction.
      //------------------------
      Loops.Perform();
      Loops.WiresToFaces();
      //------------------------
      // MAJ SD.
      //------------------------
      const NCollection_List<TopoDS_Shape>& NF = Loops.NewFaces();
      //-----------------------
      // F => New faces;
      //-----------------------
      Image.Bind(F, NF);

      NCollection_List<TopoDS_Shape>::Iterator itAdded;
      for (itAdded.Initialize(AddedEdges); itAdded.More(); itAdded.Next())
      {
        const TopoDS_Edge& E = TopoDS::Edge(itAdded.Value());
        //-----------------------
        //  E => New edges;
        //-----------------------
        if (Image.HasImage(E))
        {
          Image.Add(E, Loops.NewEdges(E));
        }
        else
        {
          Image.Bind(E, Loops.NewEdges(E));
        }
      }
    }
  }
  Loops.GetVerticesForSubstitute(myVerVerMap);
  if (myVerVerMap.IsEmpty())
    return;
  BRep_Builder BB;
  for (itr.Initialize(LF); itr.More(); itr.Next())
  {
    TopoDS_Shape                   F = itr.Value();
    NCollection_List<TopoDS_Shape> LIF;
    Image.LastImage(F, LIF);
    for (itl.Initialize(LIF); itl.More(); itl.Next())
    {
      const TopoDS_Shape& IF = itl.Value();
      TopExp_Explorer     EdExp(IF, TopAbs_EDGE);
      for (; EdExp.More(); EdExp.Next())
      {
        TopoDS_Shape                   E = EdExp.Current();
        NCollection_List<TopoDS_Shape> VList;
        TopoDS_Iterator                VerExp(E);
        for (; VerExp.More(); VerExp.Next())
          VList.Append(VerExp.Value());
        NCollection_List<TopoDS_Shape>::Iterator itlv(VList);
        for (; itlv.More(); itlv.Next())
        {
          const TopoDS_Shape& V = itlv.Value();
          if (myVerVerMap.IsBound(V))
          {
            TopoDS_Shape NewV = myVerVerMap(V);
            E.Free(true);
            NewV.Orientation(V.Orientation());
            occ::handle<BRep_TVertex>& TV    = *((occ::handle<BRep_TVertex>*)&V.TShape());
            occ::handle<BRep_TVertex>& NewTV = *((occ::handle<BRep_TVertex>*)&NewV.TShape());
            if (TV->Tolerance() > NewTV->Tolerance())
              NewTV->Tolerance(TV->Tolerance());
            NewTV->ChangePoints().Append(TV->ChangePoints());
            AsDes->Replace(V, NewV);
            BB.Remove(E, V);
            BB.Add(E, NewV);
          }
        }
      }
    }
  }
}
