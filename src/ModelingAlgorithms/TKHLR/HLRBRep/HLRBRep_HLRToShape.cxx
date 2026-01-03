// Created on: 1993-10-11
// Created by: Christophe MARION
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

#include <BRep_Builder.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRBRep.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

//=================================================================================================

HLRBRep_HLRToShape::HLRBRep_HLRToShape(const occ::handle<HLRBRep_Algo>& A)
    : myAlgo(A)
{
}

//=================================================================================================

TopoDS_Shape HLRBRep_HLRToShape::InternalCompound(const int           typ,
                                                  const bool          visible,
                                                  const TopoDS_Shape& S,
                                                  const bool          In3d)
{
  bool                      added = false;
  TopoDS_Shape              Result;
  occ::handle<HLRBRep_Data> DS = myAlgo->DataStructure();

  if (!DS.IsNull())
  {
    DS->Projector().Scaled(true);
    int  e1     = 1;
    int  e2     = DS->NbEdges();
    int  f1     = 1;
    int  f2     = DS->NbFaces();
    bool explor = false;
    //    bool todraw;
    if (!S.IsNull())
    {
      int v1, v2;
      int index = myAlgo->Index(S);
      if (index == 0)
        explor = true;
      else
        myAlgo->ShapeBounds(index).Bounds(v1, v2, e1, e2, f1, f2);
    }
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Result));
    HLRBRep_EdgeData* ed = &(DS->EDataArray().ChangeValue(e1 - 1));

    for (int ie = e1; ie <= e2; ie++)
    {
      ed++;
      if (ed->Selected() && !ed->Vertical())
      {
        ed->Used(false);
        ed->HideCount(0);
      }
      else
        ed->Used(true);
    }
    if (explor)
    {
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Edges = DS->EdgeMap();
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Faces = DS->FaceMap();
      TopExp_Explorer                                                Exp;

      for (Exp.Init(S, TopAbs_FACE); Exp.More(); Exp.Next())
      {
        int iface = Faces.FindIndex(Exp.Current());
        if (iface != 0)
          DrawFace(visible, typ, iface, DS, Result, added, In3d);
      }
      if (typ >= 3)
      {

        for (Exp.Init(S, TopAbs_EDGE, TopAbs_FACE); Exp.More(); Exp.Next())
        {
          int ie = Edges.FindIndex(Exp.Current());
          if (ie != 0)
          {
            HLRBRep_EdgeData& EDataIE = DS->EDataArray().ChangeValue(ie);
            if (!EDataIE.Used())
            {
              DrawEdge(visible, false, typ, EDataIE, Result, added, In3d);
              EDataIE.Used(true);
            }
          }
        }
      }
    }
    else
    {

      for (int iface = f1; iface <= f2; iface++)
        DrawFace(visible, typ, iface, DS, Result, added, In3d);

      if (typ >= 3)
      {
        HLRBRep_EdgeData* EDataE11 = &(DS->EDataArray().ChangeValue(e1 - 1));

        for (int ie = e1; ie <= e2; ie++)
        {
          EDataE11++;
          if (!EDataE11->Used())
          {
            DrawEdge(visible, false, typ, *EDataE11, Result, added, In3d);
            EDataE11->Used(true);
          }
        }
      }
    }
    DS->Projector().Scaled(false);
  }
  if (!added)
    Result = TopoDS_Shape();
  return Result;
}

//=================================================================================================

void HLRBRep_HLRToShape::DrawFace(const bool                 visible,
                                  const int                  typ,
                                  const int                  iface,
                                  occ::handle<HLRBRep_Data>& DS,
                                  TopoDS_Shape&              Result,
                                  bool&                      added,
                                  const bool                 In3d) const
{
  HLRBRep_FaceIterator Itf;

  for (Itf.InitEdge(DS->FDataArray().ChangeValue(iface)); Itf.MoreEdge(); Itf.NextEdge())
  {
    int               ie  = Itf.Edge();
    HLRBRep_EdgeData& edf = DS->EDataArray().ChangeValue(ie);
    if (!edf.Used())
    {
      bool todraw;
      if (typ == 1)
        todraw = Itf.IsoLine();
      else if (typ == 2) // outlines
      {
        if (In3d)
          todraw = Itf.Internal() || Itf.OutLine();
        else
          todraw = Itf.Internal();
      }
      else if (typ == 3)
        todraw = edf.Rg1Line() && !edf.RgNLine() && !Itf.OutLine();
      else if (typ == 4)
        todraw = edf.RgNLine() && !Itf.OutLine();
      else
        todraw = !Itf.IsoLine() && !Itf.Internal() && (!edf.Rg1Line() || Itf.OutLine());

      if (todraw)
      {
        DrawEdge(visible, true, typ, edf, Result, added, In3d);
        edf.Used(true);
      }
      else
      {
        if ((typ > 4 || typ == 2) && // sharp or outlines
            (edf.Rg1Line() && !Itf.OutLine()))
        {
          int hc = edf.HideCount();
          if (hc > 0)
          {
            edf.Used(true);
          }
          else
          {
            ++hc;
            edf.HideCount(hc); // to try with another face
          }
        }
        else
        {
          edf.Used(true);
        }
      }
    }
  }
}

//=================================================================================================

void HLRBRep_HLRToShape::DrawEdge(const bool        visible,
                                  const bool        inFace,
                                  const int         typ,
                                  HLRBRep_EdgeData& ed,
                                  TopoDS_Shape&     Result,
                                  bool&             added,
                                  const bool        In3d) const
{
  bool todraw = false;
  if (inFace)
    todraw = true;
  else if (typ == 3)
    todraw = ed.Rg1Line() && !ed.RgNLine();
  else if (typ == 4)
    todraw = ed.RgNLine();
  else
    todraw = !ed.Rg1Line();

  if (todraw)
  {
    double               sta, end;
    float                tolsta, tolend;
    BRep_Builder         B;
    TopoDS_Edge          E;
    HLRAlgo_EdgeIterator It;
    if (visible)
    {
      for (It.InitVisible(ed.Status()); It.MoreVisible(); It.NextVisible())
      {
        It.Visible(sta, tolsta, end, tolend);
        if (!In3d)
          E = HLRBRep::MakeEdge(ed.Geometry(), sta, end);
        else
          E = HLRBRep::MakeEdge3d(ed.Geometry(), sta, end);
        if (!E.IsNull())
        {
          B.Add(Result, E);
          added = true;
        }
      }
    }
    else
    {
      for (It.InitHidden(ed.Status()); It.MoreHidden(); It.NextHidden())
      {
        It.Hidden(sta, tolsta, end, tolend);
        if (!In3d)
          E = HLRBRep::MakeEdge(ed.Geometry(), sta, end);
        else
          E = HLRBRep::MakeEdge3d(ed.Geometry(), sta, end);
        if (!E.IsNull())
        {
          B.Add(Result, E);
          added = true;
        }
      }
    }
  }
}
