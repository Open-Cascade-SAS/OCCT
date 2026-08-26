// Created on: 1997-11-25
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

#include <TopOpeBRepDS_connex.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

namespace
{
const NCollection_List<TopoDS_Shape>& EmptyConnexList()
{
  static const NCollection_List<TopoDS_Shape> anEmptyConnexList;
  return anEmptyConnexList;
}
} // namespace

// modified by NIZNHY-PKV Sun Dec 15 17:41:40 2002 t

Standard_EXPORT const NCollection_List<TopoDS_Shape>& FDSCNX_EdgeConnexityShapeIndex(
  const TopoDS_Shape&                             E,
  const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
  const int                                       SI)
{
  if (HDS.IsNull())
  {
    return EmptyConnexList();
  }
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  if (!BDS.myConnexPrepared)
  {
    return EmptyConnexList();
  }
  if (SI != 1 && SI != 2)
  {
    return EmptyConnexList();
  }
  TopAbs_ShapeEnum                  t   = E.ShapeType();
  if (t != TopAbs_EDGE)
  {
    return EmptyConnexList();
  }
  bool has = FDSCNX_HasConnexFace(E, HDS);
  if (!has)
  {
    return EmptyConnexList();
  }
  int re = BDS.AncestorRank(E);
  if (re == 0)
  {
    return EmptyConnexList();
  }
  const NCollection_DataMap<TopoDS_Shape,
                            NCollection_List<TopoDS_Shape>,
                            TopTools_ShapeMapHasher>& elf =
    (SI == 1) ? BDS.myConnexEdges1 : BDS.myConnexEdges2;
  if (!elf.IsBound(E))
  {
    return EmptyConnexList();
  }
  const NCollection_List<TopoDS_Shape>& lof = elf.Find(E);
  return lof;
}

// S = edge --> liste de faces connexes par S
// S = face --> liste d'edges E de S qui ont au moins une autre face connexe
Standard_EXPORT const NCollection_List<TopoDS_Shape>& FDSCNX_EdgeConnexitySameShape(
  const TopoDS_Shape&                             S,
  const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  if (HDS.IsNull())
  {
    return EmptyConnexList();
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  TopAbs_ShapeEnum t = S.ShapeType();
  if (t == TopAbs_EDGE)
  {
    int                                   si = BDS.AncestorRank(S);
    const NCollection_List<TopoDS_Shape>& lf = FDSCNX_EdgeConnexityShapeIndex(S, HDS, si);
    return lf;
  }
  else if (t == TopAbs_FACE)
  {
    if (BDS.myConnexFaceEdges.IsBound(S))
    {
      const NCollection_List<TopoDS_Shape>& le = BDS.myConnexFaceEdges.Find(S);
      return le;
    }
  }
  return EmptyConnexList();
}

Standard_EXPORT void FDSCNX_Prepare(const TopoDS_Shape& /*S1*/,
                                    const TopoDS_Shape& /*S2*/,
                                    const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  if (HDS.IsNull())
  {
    return;
  }
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  BDS.myConnexEdges1.Clear();
  BDS.myConnexEdges2.Clear();
  BDS.myConnexFaceEdges.Clear();
  BDS.myConnexPrepared = false;

  int i = 0, n = BDS.NbShapes();
  for (i = 1; i <= n; i++)
  {
    const TopoDS_Shape& f = BDS.Shape(i);
    if (f.ShapeType() != TopAbs_FACE)
    {
      continue;
    }
    int rf = BDS.AncestorRank(f);
    if (rf == 0)
    {
      continue;
    }
    //    BDS.Shape(f);
    NCollection_DataMap<TopoDS_Shape,
                        NCollection_List<TopoDS_Shape>,
                        TopTools_ShapeMapHasher>& fle = BDS.myConnexFaceEdges;
    NCollection_DataMap<TopoDS_Shape,
                        NCollection_List<TopoDS_Shape>,
                        TopTools_ShapeMapHasher>& elf =
      (rf == 1) ? BDS.myConnexEdges1 : BDS.myConnexEdges2;
    TopExp_Explorer exe;
    for (exe.Init(f, TopAbs_EDGE); exe.More(); exe.Next())
    {
      //    for (TopExp_Explorer exe(f,TopAbs_EDGE);exe.More();exe.Next()) {
      const TopoDS_Shape& e = exe.Current();
      //               BDS.Shape(e);
      //      bool se = BDS.IsSectionEdge(TopoDS::Edge(e)); if (!se) continue;
      bool hs = BDS.HasShape(TopoDS::Edge(e));
      if (!hs)
      {
        continue;
      }

      NCollection_List<TopoDS_Shape>* aListFle = fle.ChangeSeek(f);
      if (aListFle == nullptr)
      {
        aListFle = fle.Bound(f, NCollection_List<TopoDS_Shape>());
      }
      aListFle->Append(e);

      NCollection_List<TopoDS_Shape>* aListElf = elf.ChangeSeek(e);
      if (aListElf == nullptr)
      {
        aListElf = elf.Bound(e, NCollection_List<TopoDS_Shape>());
      }
      aListElf->Append(f);
    }
  }
  BDS.myConnexPrepared = true;
}

Standard_EXPORT bool FDSCNX_HasConnexFace(const TopoDS_Shape&                             S,
                                          const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  if (HDS.IsNull())
  {
    return false;
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  const TopAbs_ShapeEnum            t   = S.ShapeType();
  if (t != TopAbs_FACE && t != TopAbs_EDGE)
  {
    return false;
  }
  const int rs = BDS.AncestorRank(S);
  if (rs == 0)
  {
    return false;
  }

  const NCollection_DataMap<TopoDS_Shape,
                            NCollection_List<TopoDS_Shape>,
                            TopTools_ShapeMapHasher>* pelf =
    (rs == 1) ? &BDS.myConnexEdges1 : &BDS.myConnexEdges2;
  bool has = (t == TopAbs_EDGE ? pelf : &BDS.myConnexFaceEdges)->IsBound(S);
  return has;
}

Standard_EXPORT void FDSCNX_FaceEdgeConnexFaces(const TopoDS_Shape&                             F,
                                                const TopoDS_Shape&                             E,
                                                const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                                NCollection_List<TopoDS_Shape>&                 LF)
{
  LF.Clear();
  // verifier que E est une arete de connexite de F
  bool                                  EofF = false;
  const NCollection_List<TopoDS_Shape>& loe  = FDSCNX_EdgeConnexitySameShape(F, HDS);
  if (loe.IsEmpty())
  {
    return;
  }
  for (NCollection_List<TopoDS_Shape>::Iterator i(loe); i.More(); i.Next())
  {
    const TopoDS_Shape& e = i.Value();
    //             HDS->Shape(e);
    bool b = e.IsSame(E);
    if (b)
    {
      EofF = true;
      break;
    }
  }
  if (!EofF)
  {
    return;
  }

  const NCollection_List<TopoDS_Shape>& lof = FDSCNX_EdgeConnexitySameShape(E, HDS);
  if (lof.IsEmpty())
  {
    return;
  }
  for (NCollection_List<TopoDS_Shape>::Iterator it(lof); it.More(); it.Next())
  {
    const TopoDS_Shape& f = it.Value();
    bool                b = f.IsSame(F);
    if (!b)
    {
      LF.Append(f);
    }
  }
}

Standard_EXPORT void FDSCNX_DumpIndex(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                      const int                                       I)
{
  if (HDS.IsNull())
  {
    return;
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  int                               ns  = BDS.NbShapes();
  if (I < 1 || I > ns)
  {
    return;
  }

  int                                   i  = I;
  const TopoDS_Shape&                   s  = BDS.Shape(i);
  TopAbs_ShapeEnum                      ts = s.ShapeType();
  const NCollection_List<TopoDS_Shape>& ls = FDSCNX_EdgeConnexitySameShape(s, HDS);
  if (ts == TopAbs_EDGE)
  {
    NCollection_List<TopoDS_Shape>::Iterator ils(ls);
    if (!ils.More())
    {
      return;
    }

    for (; ils.More(); ils.Next())
    {
      std::cout << BDS.Shape(ils.Value()) << " ";
    }
  }
  else if (ts == TopAbs_FACE)
  {
    NCollection_List<TopoDS_Shape>::Iterator ils(ls);
    if (!ils.More())
    {
      return;
    }

    for (; ils.More(); ils.Next())
    {
      const TopoDS_Shape&            e = ils.Value();
      NCollection_List<TopoDS_Shape> lf;
      FDSCNX_FaceEdgeConnexFaces(s, e, HDS, lf);
      NCollection_List<TopoDS_Shape>::Iterator ilf(lf);
      if (!ilf.More())
      {
        continue;
      }
      for (; ilf.More(); ilf.Next())
      {
        std::cout << BDS.Shape(ilf.Value()) << " ";
      }
    }
  }
}

Standard_EXPORT void FDSCNX_Dump(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS, const int I)
{
  if (HDS.IsNull())
  {
    return;
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  int                               ns  = BDS.NbShapes();
  if (I < 1 || I > ns)
  {
    return;
  }

  int                                   i  = I;
  const TopoDS_Shape&                   s  = BDS.Shape(i);
  int                                   is = BDS.Shape(s);
  TopAbs_ShapeEnum                      ts = s.ShapeType();
  const NCollection_List<TopoDS_Shape>& ls = FDSCNX_EdgeConnexitySameShape(s, HDS);
  if (ts == TopAbs_EDGE)
  {
    NCollection_List<TopoDS_Shape>::Iterator ils(ls);
    if (!ils.More())
    {
      return;
    }

    std::cout << "clear;";
    for (; ils.More(); ils.Next())
    {
      std::cout << "tsee f " << BDS.Shape(ils.Value()) << ";";
    }
    std::cout << "tsee e " << is << ";### edge " << is << " connexity" << '\n';
  }
  else if (ts == TopAbs_FACE)
  {
    NCollection_List<TopoDS_Shape>::Iterator ils(ls);
    if (!ils.More())
    {
      return;
    }
    for (; ils.More(); ils.Next())
    {
      const TopoDS_Shape&            e  = ils.Value();
      int                            ie = BDS.Shape(e);
      NCollection_List<TopoDS_Shape> lf;
      FDSCNX_FaceEdgeConnexFaces(s, e, HDS, lf);
      NCollection_List<TopoDS_Shape>::Iterator ilf(lf);
      if (!ilf.More())
      {
        continue;
      }

      std::cout << "clear;";
      std::cout << "tsee f " << is << ";";
      for (; ilf.More(); ilf.Next())
      {
        std::cout << "tsee f " << BDS.Shape(ilf.Value()) << ";";
      }
      std::cout << "tsee e " << ie << ";### face " << is << " connexity" << '\n';
    }
  }
}

Standard_EXPORT void FDSCNX_Dump(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  if (HDS.IsNull())
  {
    return;
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  int                               ns  = BDS.NbShapes();
  for (int i = 1; i <= ns; i++)
  {
    FDSCNX_Dump(HDS, i);
  }
}
