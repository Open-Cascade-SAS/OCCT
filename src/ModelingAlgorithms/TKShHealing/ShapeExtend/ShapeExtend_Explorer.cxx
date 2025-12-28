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

// szv#4 S4163

#include <BRep_Builder.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>

//=================================================================================================

ShapeExtend_Explorer::ShapeExtend_Explorer() {}

//=================================================================================================

TopoDS_Shape ShapeExtend_Explorer::CompoundFromSeq(
  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& seqval) const
{
  BRep_Builder    B;
  TopoDS_Compound C;
  B.MakeCompound(C);
  int i, n = seqval->Length();
  for (i = 1; i <= n; i++)
    B.Add(C, seqval->Value(i));
  return C;
}

//=================================================================================================

static void FillList(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& list,
                     const TopoDS_Shape&                                     comp,
                     const bool                                              expcomp)
{
  for (TopoDS_Iterator it(comp); it.More(); it.Next())
  {
    const TopoDS_Shape& sub = it.Value();
    if (sub.ShapeType() != TopAbs_COMPOUND)
      list->Append(sub);
    else if (!expcomp)
      list->Append(sub);
    else
      FillList(list, sub, expcomp);
  }
}

occ::handle<NCollection_HSequence<TopoDS_Shape>> ShapeExtend_Explorer::SeqFromCompound(
  const TopoDS_Shape& comp,
  const bool          expcomp) const
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> list = new NCollection_HSequence<TopoDS_Shape>();
  if (comp.IsNull())
    return list;
  if (comp.ShapeType() != TopAbs_COMPOUND)
  {
    list->Append(comp);
    return list;
  }
  FillList(list, comp, expcomp);
  return list;
}

//=================================================================================================

void ShapeExtend_Explorer::ListFromSeq(
  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& seqval,
  NCollection_List<TopoDS_Shape>&                         lisval,
  const bool                                              clear) const
{
  if (clear)
    lisval.Clear();
  if (seqval.IsNull())
    return;
  int i, nb = seqval->Length();
  for (i = 1; i <= nb; i++)
    lisval.Append(seqval->Value(i));
}

//=================================================================================================

occ::handle<NCollection_HSequence<TopoDS_Shape>> ShapeExtend_Explorer::SeqFromList(
  const NCollection_List<TopoDS_Shape>& lisval) const
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> seqval =
    new NCollection_HSequence<TopoDS_Shape>();
  NCollection_List<TopoDS_Shape>::Iterator it;
  for (it.Initialize(lisval); it.More(); it.Next())
    seqval->Append(it.Value());
  return seqval;
}

//=================================================================================================

TopAbs_ShapeEnum ShapeExtend_Explorer::ShapeType(const TopoDS_Shape& shape,
                                                 const bool          compound) const
{
  if (shape.IsNull())
    return TopAbs_SHAPE;
  TopAbs_ShapeEnum res = shape.ShapeType();
  if (!compound || res != TopAbs_COMPOUND)
    return res;
  res = TopAbs_SHAPE;
  for (TopoDS_Iterator iter(shape); iter.More(); iter.Next())
  {
    const TopoDS_Shape& sh = iter.Value();
    if (sh.IsNull())
      continue;
    TopAbs_ShapeEnum typ = sh.ShapeType();
    if (typ == TopAbs_COMPOUND)
      typ = ShapeType(sh, compound);
    if (res == TopAbs_SHAPE)
      res = typ;
    //   Egalite : OK;  Pseudo-Egalite : EDGE/WIRE ou FACE/SHELL
    else if (res == TopAbs_EDGE && typ == TopAbs_WIRE)
      res = typ;
    else if (res == TopAbs_WIRE && typ == TopAbs_EDGE)
      continue;
    else if (res == TopAbs_FACE && typ == TopAbs_SHELL)
      res = typ;
    else if (res == TopAbs_SHELL && typ == TopAbs_FACE)
      continue;
    else if (res != typ)
      return TopAbs_COMPOUND;
  }
  return res;
}

//=================================================================================================

TopoDS_Shape ShapeExtend_Explorer::SortedCompound(const TopoDS_Shape&    shape,
                                                  const TopAbs_ShapeEnum type,
                                                  const bool             explore,
                                                  const bool             compound) const
{
  if (shape.IsNull())
    return shape;
  TopAbs_ShapeEnum typ = shape.ShapeType();
  TopoDS_Shape     sh, sh0;
  int              nb = 0;

  //  Compound : on le prend, soit tel quel, soit son contenu
  if (typ == TopAbs_COMPOUND || typ == TopAbs_COMPSOLID)
  {
    TopoDS_Compound C;
    BRep_Builder    B;
    B.MakeCompound(C);
    for (TopoDS_Iterator it(shape); it.More(); it.Next())
    {
      sh0 = SortedCompound(it.Value(), type, explore, compound);
      if (sh0.IsNull())
        continue;
      sh  = sh0;
      typ = sh.ShapeType();
      if (typ == TopAbs_COMPOUND && !compound)
      {
        for (TopoDS_Iterator it2(sh); it2.More(); it2.Next())
        {
          nb++;
          sh = it2.Value();
          B.Add(C, sh);
        }
      }
      else
      {
        nb++;
        B.Add(C, sh);
      }
    }
    if (nb == 0)
      C.Nullify();
    else if (nb == 1)
      return sh;
    return C;
  }

  //   Egalite : OK;  Pseudo-Egalite : EDGE/WIRE ou FACE/SHELL
  if (typ == type)
    return shape;
  if (typ == TopAbs_EDGE && type == TopAbs_WIRE)
  {
    BRep_Builder B;
    TopoDS_Wire  W;
    B.MakeWire(W);
    B.Add(W, shape);
    return W;
  }
  if (typ == TopAbs_FACE && type == TopAbs_SHELL)
  {
    BRep_Builder B;
    TopoDS_Shell S;
    B.MakeShell(S);
    B.Add(S, shape);
    return S;
  }

  //   Le reste : selon exploration
  if (!explore)
  {
    TopoDS_Shape nulsh;
    return nulsh;
  }

  //  Ici, on doit explorer
  //  SOLID + mode COMPOUND : reconduire les SHELLs
  if (typ == TopAbs_SOLID && compound)
  {
    TopoDS_Compound C;
    BRep_Builder    B;
    B.MakeCompound(C);
    for (TopoDS_Iterator it(shape); it.More(); it.Next())
    {
      sh0 = SortedCompound(it.Value(), type, explore, compound);
      if (sh0.IsNull())
        continue;
      sh = sh0;
      nb++;
      B.Add(C, sh);
    }
    if (nb == 0)
      C.Nullify();
    else if (nb == 1)
      return sh;
    return C;
  }

  //  Exploration classique
  TopoDS_Compound CC;
  BRep_Builder    BB;
  BB.MakeCompound(CC);
  // int iena = false; //szv#4:S4163:12Mar99 unused
  for (TopExp_Explorer aExp(shape, type); aExp.More(); aExp.Next())
  {
    nb++;
    sh = aExp.Current();
    BB.Add(CC, sh);
  }
  if (nb == 0)
    CC.Nullify();
  else if (nb == 1)
    return sh;
  return CC;
}

//=================================================================================================

void ShapeExtend_Explorer::DispatchList(
  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& list,
  occ::handle<NCollection_HSequence<TopoDS_Shape>>&       vertices,
  occ::handle<NCollection_HSequence<TopoDS_Shape>>&       edges,
  occ::handle<NCollection_HSequence<TopoDS_Shape>>&       wires,
  occ::handle<NCollection_HSequence<TopoDS_Shape>>&       faces,
  occ::handle<NCollection_HSequence<TopoDS_Shape>>&       shells,
  occ::handle<NCollection_HSequence<TopoDS_Shape>>&       solids,
  occ::handle<NCollection_HSequence<TopoDS_Shape>>&       compsols,
  occ::handle<NCollection_HSequence<TopoDS_Shape>>&       compounds) const
{
  if (list.IsNull())
    return;
  if (vertices.IsNull())
    vertices = new NCollection_HSequence<TopoDS_Shape>();
  if (edges.IsNull())
    edges = new NCollection_HSequence<TopoDS_Shape>();
  if (wires.IsNull())
    wires = new NCollection_HSequence<TopoDS_Shape>();
  if (faces.IsNull())
    faces = new NCollection_HSequence<TopoDS_Shape>();
  if (shells.IsNull())
    shells = new NCollection_HSequence<TopoDS_Shape>();
  if (solids.IsNull())
    solids = new NCollection_HSequence<TopoDS_Shape>();
  if (compsols.IsNull())
    compsols = new NCollection_HSequence<TopoDS_Shape>();
  if (compounds.IsNull())
    compounds = new NCollection_HSequence<TopoDS_Shape>();

  int i, nb = list->Length();
  for (i = 1; i <= nb; i++)
  {
    TopoDS_Shape sh = list->Value(i);
    if (sh.IsNull())
      continue;
    switch (sh.ShapeType())
    {
      case TopAbs_VERTEX:
        vertices->Append(sh);
        break;
      case TopAbs_EDGE:
        edges->Append(sh);
        break;
      case TopAbs_WIRE:
        wires->Append(sh);
        break;
      case TopAbs_FACE:
        faces->Append(sh);
        break;
      case TopAbs_SHELL:
        shells->Append(sh);
        break;
      case TopAbs_SOLID:
        solids->Append(sh);
        break;
      case TopAbs_COMPSOLID:
        compsols->Append(sh);
        break;
      case TopAbs_COMPOUND:
        compounds->Append(sh);
        break;
      default:
        break;
    }
  }
}
