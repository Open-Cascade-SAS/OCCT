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
#include <BRep_Tool.hxx>
#include <ShapeAnalysis_Shell.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

ShapeAnalysis_Shell::ShapeAnalysis_Shell()
    : myConex(false)
{
}

//=================================================================================================

void ShapeAnalysis_Shell::Clear()
{
  myShells.Clear();
  myBad.Clear();
  myFree.Clear();
  myConex = false;
}

//=================================================================================================

void ShapeAnalysis_Shell::LoadShells(const TopoDS_Shape& shape)
{
  if (shape.IsNull())
    return;

  if (shape.ShapeType() == TopAbs_SHELL)
    myShells.Add(shape); // szv#4:S4163:12Mar99 i =
  else
  {
    for (TopExp_Explorer exs(shape, TopAbs_SHELL); exs.More(); exs.Next())
    {
      const TopoDS_Shape& sh = exs.Current();
      myShells.Add(sh); // szv#4:S4163:12Mar99 i =
    }
  }
}

//  CheckOrientedShells : alimente BadEdges et FreeEdges
//  BadEdges : edges presentes plus d une fois dans une meme orientation
//  FreeEdges : edges presentes une seule fois
//  On utilise pour cela une fonction auxiliaire : CheckEdges
//    Qui alimente 2 maps auxiliaires : les edges directes et les inverses

static bool CheckEdges(const TopoDS_Shape&                                            shape,
                       NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& bads,
                       NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& dirs,
                       NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& revs,
                       NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& ints)
{
  bool res = false;

  if (shape.ShapeType() != TopAbs_EDGE)
  {
    for (TopoDS_Iterator it(shape); it.More(); it.Next())
    {
      if (CheckEdges(it.Value(), bads, dirs, revs, ints))
        res = true;
    }
  }
  else
  {
    TopoDS_Edge E = TopoDS::Edge(shape);
    if (BRep_Tool::Degenerated(E))
      return false;

    if (shape.Orientation() == TopAbs_FORWARD)
    {
      // Check if shape already exists - Add returns index <= previous extent if duplicate
      const int aPrevExtent = dirs.Extent();
      if (dirs.Add(shape) <= aPrevExtent)
      {
        bads.Add(shape);
        res = true;
      }
    }
    if (shape.Orientation() == TopAbs_REVERSED)
    {
      // Check if shape already exists - Add returns index <= previous extent if duplicate
      const int aPrevExtent = revs.Extent();
      if (revs.Add(shape) <= aPrevExtent)
      {
        bads.Add(shape);
        res = true;
      }
    }
    if (shape.Orientation() == TopAbs_INTERNAL)
    {
      ints.Add(shape);
    }
  }

  return res;
}

//=================================================================================================

bool ShapeAnalysis_Shell::CheckOrientedShells(const TopoDS_Shape& shape,
                                              const bool          alsofree,
                                              const bool          checkinternaledges)
{
  myConex = false;
  if (shape.IsNull())
    return false;
  bool res = false;

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> dirs, revs, ints;
  for (TopExp_Explorer exs(shape, TopAbs_SHELL); exs.More(); exs.Next())
  {
    const TopoDS_Shape& sh = exs.Current();
    // szv#4:S4163:12Mar99 optimized
    if (CheckEdges(sh, myBad, dirs, revs, ints))
      if (myShells.Add(sh))
        res = true;
  }

  //  Resteraient a faire les FreeEdges
  if (!alsofree)
    return res;

  //  Free Edges. Ce sont les edges d une map pas dans l autre
  //  et lycee de Versailles  (les maps dirs et revs)
  int nb = dirs.Extent();
  int i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i++)
  {
    const TopoDS_Shape& sh = dirs.FindKey(i);
    if (!myBad.Contains(sh))
    {
      if (!revs.Contains(sh))
      {
        if (checkinternaledges)
        {
          if (!ints.Contains(sh))
          {
            myFree.Add(sh);
          }
          else
            myConex = true;
        }
        else
        {
          myFree.Add(sh);
        }
      }
      else
        myConex = true;
    }
    else
      myConex = true;
  }

  nb = revs.Extent();
  for (i = 1; i <= nb; i++)
  {
    const TopoDS_Shape& sh = revs.FindKey(i);
    if (!myBad.Contains(sh))
    {
      if (!dirs.Contains(sh))
      {
        if (checkinternaledges)
        {
          if (!ints.Contains(sh))
          {
            myFree.Add(sh);
          }
          else
            myConex = true;
        }
        else
        {
          myFree.Add(sh);
        }
      }
      else
        myConex = true;
    }
    else
      myConex = true;
  }

  return res;
}

//=================================================================================================

bool ShapeAnalysis_Shell::IsLoaded(const TopoDS_Shape& shape) const
{
  if (shape.IsNull())
    return false;
  return myShells.Contains(shape);
}

//=================================================================================================

int ShapeAnalysis_Shell::NbLoaded() const
{
  return myShells.Extent();
}

//=================================================================================================

TopoDS_Shape ShapeAnalysis_Shell::Loaded(const int num) const
{
  return myShells.FindKey(num);
}

//=================================================================================================

bool ShapeAnalysis_Shell::HasBadEdges() const
{
  return (myBad.Extent() > 0);
}

//=================================================================================================

TopoDS_Compound ShapeAnalysis_Shell::BadEdges() const
{
  TopoDS_Compound C;
  BRep_Builder    B;
  B.MakeCompound(C);
  int n = myBad.Extent();
  for (int i = 1; i <= n; i++)
    B.Add(C, myBad.FindKey(i));
  return C;
}

//=================================================================================================

bool ShapeAnalysis_Shell::HasFreeEdges() const
{
  return (myFree.Extent() > 0);
}

//=================================================================================================

TopoDS_Compound ShapeAnalysis_Shell::FreeEdges() const
{
  TopoDS_Compound C;
  BRep_Builder    B;
  B.MakeCompound(C);
  int n = myFree.Extent();
  for (int i = 1; i <= n; i++)
    B.Add(C, myFree.FindKey(i));
  return C;
}

//=================================================================================================

bool ShapeAnalysis_Shell::HasConnectedEdges() const
{
  return myConex;
}
