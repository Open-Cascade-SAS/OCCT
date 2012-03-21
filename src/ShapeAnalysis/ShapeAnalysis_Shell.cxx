// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//szv#4 S4163
#include <ShapeAnalysis_Shell.ixx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void ShapeAnalysis_Shell::Clear() 
{
  myShells.Clear();
  myBad.Clear();
  myFree.Clear();
  myConex = Standard_False;
}

//=======================================================================
//function : LoadShells
//purpose  : 
//=======================================================================

 void ShapeAnalysis_Shell::LoadShells(const TopoDS_Shape& shape) 
{
  if (shape.IsNull()) return;

  if (shape.ShapeType() == TopAbs_SHELL) myShells.Add (shape); //szv#4:S4163:12Mar99 i =
  else {
    for (TopExp_Explorer exs (shape,TopAbs_SHELL); exs.More(); exs.Next()) {
      TopoDS_Shape sh = exs.Current();
      myShells.Add (sh); //szv#4:S4163:12Mar99 i =
    }
  }
}


//  CheckOrientedShells : alimente BadEdges et FreeEdges
//  BadEdges : edges presentes plus d une fois dans une meme orientation
//  FreeEdges : edges presentes une seule fois
//  On utilise pour cela une fonction auxiliaire : CheckEdges
//    Qui alimente 2 maps auxiliaires : les edges directes et les inverses

static  Standard_Boolean CheckEdges(const TopoDS_Shape& shape,
                                    TopTools_IndexedMapOfShape& bads,
                                    TopTools_IndexedMapOfShape& dirs,
                                    TopTools_IndexedMapOfShape& revs,
                                    TopTools_IndexedMapOfShape& ints)
{
  Standard_Boolean res = Standard_False;

  if (shape.ShapeType() != TopAbs_EDGE) {
    for (TopoDS_Iterator it(shape); it.More(); it.Next()) {
      if (CheckEdges (it.Value(),bads,dirs,revs,ints)) res = Standard_True;
    }
  }
  else {
    TopoDS_Edge E = TopoDS::Edge(shape);
    if (BRep_Tool::Degenerated(E)) return Standard_False;

    if (shape.Orientation() == TopAbs_FORWARD) {
      //szv#4:S4163:12Mar99 optimized
      if (dirs.FindIndex (shape) == 0) dirs.Add (shape);
      else { bads.Add (shape); res = Standard_True; }
    }
    if (shape.Orientation() == TopAbs_REVERSED) {
      //szv#4:S4163:12Mar99 optimized
      if (revs.FindIndex (shape) == 0) revs.Add (shape);
      else { bads.Add (shape); res = Standard_True; }
    }
    if (shape.Orientation() == TopAbs_INTERNAL) {
      if (ints.FindIndex (shape) == 0) ints.Add (shape);
      //else { bads.Add (shape); res = Standard_True; }
    }
  }

  return res;
}

//=======================================================================
//function : CheckOrientedShells
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Shell::CheckOrientedShells(const TopoDS_Shape& shape,
							  const Standard_Boolean alsofree,
                                                          const Standard_Boolean checkinternaledges)
{
  myConex = Standard_False;
  if (shape.IsNull()) return Standard_False;
  Standard_Boolean res = Standard_False;

  TopTools_IndexedMapOfShape dirs, revs, ints;
  for (TopExp_Explorer exs(shape,TopAbs_SHELL); exs.More(); exs.Next()) {
    TopoDS_Shape sh = exs.Current();
    //szv#4:S4163:12Mar99 optimized
    if (CheckEdges (sh,myBad,dirs,revs,ints))
      if (myShells.Add (sh)) res = Standard_True;
  }

  //  Resteraient a faire les FreeEdges
  if (!alsofree) return res;

  //  Free Edges . Ce sont les edges d une map pas dans l autre
  //  et lycee de Versailles  (les maps dirs et revs)
  Standard_Integer nb = dirs.Extent();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i ++) {
    TopoDS_Shape sh = dirs.FindKey (i);
    if (!myBad.Contains(sh)) {
      if (!revs.Contains(sh)) {
        if(checkinternaledges) {
          if (!ints.Contains(sh)) {
            myFree.Add (sh);
          }
          else myConex = Standard_True;
        }
        else {
          myFree.Add (sh);
        }
      }
      else myConex = Standard_True;
    }
    else myConex = Standard_True;
  }

  nb = revs.Extent();
  for (i = 1; i <= nb; i ++) {
    TopoDS_Shape sh = revs.FindKey (i);
    if (!myBad.Contains(sh)) {
      if (!dirs.Contains(sh)) {
        if(checkinternaledges) {
          if (!ints.Contains(sh)) {
            myFree.Add (sh);
          }
          else myConex = Standard_True;
        }
        else {
          myFree.Add (sh);
        }
      }
      else myConex = Standard_True;
    }
    else myConex = Standard_True;
  }

  return res;
}

//=======================================================================
//function : IsLoaded
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAnalysis_Shell::IsLoaded(const TopoDS_Shape& shape) const
{
  if (shape.IsNull()) return Standard_False;
  return myShells.Contains (shape);
}

//=======================================================================
//function : NbLoaded
//purpose  : 
//=======================================================================

 Standard_Integer ShapeAnalysis_Shell::NbLoaded() const
{
  return myShells.Extent();
}

//=======================================================================
//function : Loaded
//purpose  : 
//=======================================================================

 TopoDS_Shape ShapeAnalysis_Shell::Loaded(const Standard_Integer num) const
{
  return myShells.FindKey (num);
}

//=======================================================================
//function : HasBadEdges
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAnalysis_Shell::HasBadEdges() const
{
  return (myBad.Extent() > 0);
}

//=======================================================================
//function : BadEdges
//purpose  : 
//=======================================================================

 TopoDS_Compound ShapeAnalysis_Shell::BadEdges() const
{
  TopoDS_Compound C;
  BRep_Builder B;
  B.MakeCompound (C);
  Standard_Integer n = myBad.Extent();
  for (Standard_Integer i = 1; i <= n; i ++)  B.Add (C,myBad.FindKey(i));
  return C;
}

//=======================================================================
//function : HasFreeEdges
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAnalysis_Shell::HasFreeEdges() const
{
  return (myFree.Extent() > 0);
}

//=======================================================================
//function : FreeEdges
//purpose  : 
//=======================================================================

 TopoDS_Compound ShapeAnalysis_Shell::FreeEdges() const
{
  TopoDS_Compound C;
  BRep_Builder B;
  B.MakeCompound (C);
  Standard_Integer n = myFree.Extent();
  for (Standard_Integer i = 1; i <= n; i ++)  B.Add (C,myFree.FindKey(i));
  return C;
}

//=======================================================================
//function : HasConnectedEdges
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAnalysis_Shell::HasConnectedEdges() const
{
  return myConex;
}
