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

//: q0 abv 12.03.99: mat-a.stp: protection against no pcurves in SwapSeam()
//    abv 28.04.99  S4137: added method Add(WireData), method SetLast fixed
//    abv 05.05.99  S4174: protection against INTERNAL/EXTERNAL edges

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <ShapeExtend_WireData.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeExtend_WireData, Standard_Transient)

//=================================================================================================

ShapeExtend_WireData::ShapeExtend_WireData()
{
  Clear();
}

//=================================================================================================

ShapeExtend_WireData::ShapeExtend_WireData(const TopoDS_Wire& wire,
                                           const bool         chained,
                                           const bool         theManifold)
{
  Init(wire, chained, theManifold);
}

//=================================================================================================

void ShapeExtend_WireData::Init(const occ::handle<ShapeExtend_WireData>& other)
{
  Clear();
  int i, nb = other->NbEdges();
  for (i = 1; i <= nb; i++)
    Add(other->Edge(i));
  nb = other->NbNonManifoldEdges();
  for (i = 1; i <= nb; i++)
    Add(other->NonmanifoldEdge(i));
  myManifoldMode = other->ManifoldMode();
}

//=================================================================================================

bool ShapeExtend_WireData::Init(const TopoDS_Wire& wire, const bool chained, const bool theManifold)
{
  Clear();
  myManifoldMode   = theManifold;
  bool          OK = true;
  TopoDS_Vertex Vlast;
  for (TopoDS_Iterator it(wire); it.More(); it.Next())
  {
    TopoDS_Edge E = TopoDS::Edge(it.Value());

    // protect against INTERNAL/EXTERNAL edges
    if ((E.Orientation() != TopAbs_REVERSED && E.Orientation() != TopAbs_FORWARD))
    {
      myNonmanifoldEdges->Append(E);
      continue;
    }

    TopoDS_Vertex V1, V2;
    for (TopoDS_Iterator itv(E); itv.More(); itv.Next())
    {
      TopoDS_Vertex V = TopoDS::Vertex(itv.Value());
      if (V.Orientation() == TopAbs_FORWARD)
        V1 = V;
      else if (V.Orientation() == TopAbs_REVERSED)
        V2 = V;
    }

    // chainage? Si pas bon et chained False on repart sur WireExplorer
    if (!Vlast.IsNull() && !Vlast.IsSame(V1) && theManifold)
    {
      OK = false;
      if (!chained)
        break;
    }
    Vlast = V2;
    if (wire.Orientation() == TopAbs_REVERSED)
      myEdges->Prepend(E);
    else
      myEdges->Append(E);
  }

  if (!myManifoldMode)
  {
    int nb = myNonmanifoldEdges->Length();
    int i  = 1;
    for (; i <= nb; i++)
      myEdges->Append(myNonmanifoldEdges->Value(i));
    myNonmanifoldEdges->Clear();
  }
  //    refaire chainage ?  Par WireExplorer
  if (OK || chained)
    return OK;

  Clear();
  for (BRepTools_WireExplorer we(wire); we.More(); we.Next())
    myEdges->Append(TopoDS::Edge(we.Current()));

  return OK;
}

//=================================================================================================

void ShapeExtend_WireData::Clear()
{
  myEdges            = new NCollection_HSequence<TopoDS_Shape>();
  myNonmanifoldEdges = new NCollection_HSequence<TopoDS_Shape>;
  mySeamF = mySeamR = -1;
  mySeams.Nullify();
  myManifoldMode = true;
}

//=================================================================================================

void ShapeExtend_WireData::ComputeSeams(const bool enforce)
{
  if (mySeamF >= 0 && !enforce)
    return;

  mySeams = new NCollection_HSequence<int>();
  mySeamF = mySeamR = 0;
  TopoDS_Shape                                                  S;
  int                                                           i, nb = NbEdges();
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> ME;
  int*                                                          SE = new int[nb + 1];

  //  deux passes : d abord on mappe les Edges REVERSED
  //  Pour chacune, on note aussi son RANG dans la liste
  for (i = 1; i <= nb; i++)
  {
    S = Edge(i);
    if (S.Orientation() == TopAbs_REVERSED)
    {
      int num = ME.Add(S);
      SE[num] = i;
    }
  }

  //  ensuite on voit les Edges FORWARD qui y seraient deja -> on note leur n0
  //  c-a-d le n0 de la directe ET de la reverse
  for (i = 1; i <= nb; i++)
  {
    S = Edge(i);
    if (S.Orientation() == TopAbs_REVERSED)
      continue;
    int num = ME.FindIndex(S);
    if (num <= 0)
      continue;
    if (mySeamF == 0)
    {
      mySeamF = i;
      mySeamR = SE[num];
    }
    else
    {
      mySeams->Append(i);
      mySeams->Append(SE[num]);
    }
  }

  mySeamsCache.Clear();
  for (i = 1; i <= mySeams->Length(); i++)
  {
    mySeamsCache.Add(mySeams->Value(i));
  }

  delete[] SE; // ne pas oublier !!
}

//=================================================================================================

void ShapeExtend_WireData::SetLast(const int num)
{
  if (num == 0)
    return;
  int i, nb = NbEdges();
  for (i = nb; i > num; i--)
  {
    TopoDS_Edge edge = TopoDS::Edge(myEdges->Value(nb));
    myEdges->Remove(nb);
    myEdges->InsertBefore(1, edge);
  }
  mySeamF = -1;
}

//=================================================================================================

void ShapeExtend_WireData::SetDegeneratedLast()
{
  int i, nb = NbEdges();
  for (i = 1; i <= nb; i++)
  {
    if (BRep_Tool::Degenerated(Edge(i)))
    {
      SetLast(i);
      return;
    }
  }
}

//=================================================================================================

void ShapeExtend_WireData::Add(const TopoDS_Edge& edge, const int atnum)
{
  if (edge.Orientation() != TopAbs_REVERSED && edge.Orientation() != TopAbs_FORWARD
      && myManifoldMode)
  {
    myNonmanifoldEdges->Append(edge);
    return;
  }

  if (edge.IsNull())
    return;
  if (atnum == 0)
  {
    myEdges->Append(edge);
  }
  else
  {
    myEdges->InsertBefore(atnum, edge);
  }
  mySeamF = -1;
}

//=================================================================================================

void ShapeExtend_WireData::Add(const TopoDS_Wire& wire, const int atnum)
{
  if (wire.IsNull())
    return;
  int                                n = atnum;
  NCollection_Sequence<TopoDS_Shape> aNMEdges;
  for (TopoDS_Iterator it(wire); it.More(); it.Next())
  {
    TopoDS_Edge edge = TopoDS::Edge(it.Value());
    if (edge.Orientation() != TopAbs_REVERSED && edge.Orientation() != TopAbs_FORWARD)
    {
      if (myManifoldMode)
        myNonmanifoldEdges->Append(edge);
      else
        aNMEdges.Append(edge);
      continue;
    }
    if (n == 0)
    {
      myEdges->Append(edge);
    }
    else
    {
      myEdges->InsertBefore(n, edge);
      n++;
    }
  }
  int i = 1, nb = aNMEdges.Length();
  for (; i <= nb; i++)
    myEdges->Append(aNMEdges.Value(i));
  mySeamF = -1;
}

//=================================================================================================

void ShapeExtend_WireData::Add(const occ::handle<ShapeExtend_WireData>& wire, const int atnum)
{
  if (wire.IsNull())
    return;
  NCollection_Sequence<TopoDS_Shape> aNMEdges;
  int                                n = atnum;
  int                                i = 1;
  for (; i <= wire->NbEdges(); i++)
  {
    TopoDS_Edge aE = wire->Edge(i);
    if (aE.Orientation() == TopAbs_INTERNAL || aE.Orientation() == TopAbs_EXTERNAL)
    {
      aNMEdges.Append(aE);
      continue;
    }

    if (n == 0)
    {
      myEdges->Append(wire->Edge(i));
    }
    else
    {
      myEdges->InsertBefore(n, wire->Edge(i));
      n++;
    }
  }

  // non-manifold edges for non-manifold wire should be added at end
  for (i = 1; i <= aNMEdges.Length(); i++)
    myEdges->Append(aNMEdges.Value(i));

  for (i = 1; i <= wire->NbNonManifoldEdges(); i++)
  {
    if (myManifoldMode)
      myNonmanifoldEdges->Append(wire->NonmanifoldEdge(i));
    else
    {
      if (n == 0)
        myEdges->Append(wire->Edge(i));

      else
      {
        myEdges->InsertBefore(n, wire->Edge(i));
        n++;
      }
    }
  }

  mySeamF = -1;
}

//=================================================================================================

void ShapeExtend_WireData::Add(const TopoDS_Shape& shape, const int atnum)
{
  if (shape.ShapeType() == TopAbs_EDGE)
    Add(TopoDS::Edge(shape), atnum);
  else if (shape.ShapeType() == TopAbs_WIRE)
    Add(TopoDS::Wire(shape), atnum);
}

//=================================================================================================

void ShapeExtend_WireData::AddOriented(const TopoDS_Edge& edge, const int mode)
{
  if (edge.IsNull() || mode < 0)
    return;
  TopoDS_Edge E = edge;
  if (mode == 1 || mode == 3)
    E.Reverse();
  Add(E, mode / 2); // mode = 0,1 -> 0  mode = 2,3 -> 1
}

//=================================================================================================

void ShapeExtend_WireData::AddOriented(const TopoDS_Wire& wire, const int mode)
{
  if (wire.IsNull() || mode < 0)
    return;
  TopoDS_Wire W = wire;
  if (mode == 1 || mode == 3)
    W.Reverse();
  Add(W, mode / 2); // mode = 0,1 -> 0  mode = 2,3 -> 1
}

void ShapeExtend_WireData::AddOriented(const TopoDS_Shape& shape, const int mode)
{
  if (shape.ShapeType() == TopAbs_EDGE)
    AddOriented(TopoDS::Edge(shape), mode);
  else if (shape.ShapeType() == TopAbs_WIRE)
    AddOriented(TopoDS::Wire(shape), mode);
}

//=================================================================================================

void ShapeExtend_WireData::Remove(const int num)
{

  myEdges->Remove(num > 0 ? num : NbEdges());

  mySeamF = -1;
}

//=================================================================================================

void ShapeExtend_WireData::Set(const TopoDS_Edge& edge, const int num)
{
  if (edge.Orientation() != TopAbs_REVERSED && edge.Orientation() != TopAbs_FORWARD
      && myManifoldMode)
  {
    if (num <= myNonmanifoldEdges->Length())
      myNonmanifoldEdges->SetValue(num, edge);
    else
      myNonmanifoldEdges->Append(edge);
  }

  else
    myEdges->SetValue((num > 0 ? num : NbEdges()), edge);
  mySeamF = -1;
}

//=======================================================================
// function : Reverse
// purpose  : reverse order of edges in the wire
//=======================================================================

void ShapeExtend_WireData::Reverse()
{
  int i, nb = NbEdges();

  // inverser les edges + les permuter pour inverser le wire
  for (i = 1; i <= nb / 2; i++)
  {
    TopoDS_Shape S1 = myEdges->Value(i);
    S1.Reverse();
    TopoDS_Shape S2 = myEdges->Value(nb + 1 - i);
    S2.Reverse();
    myEdges->SetValue(i, S2);
    myEdges->SetValue(nb + 1 - i, S1);
  }
  //  nb d edges impair : inverser aussi l edge du milieu (rang inchange)
  if (nb % 2)
  { //  test impair
    i               = (nb + 1) / 2;
    TopoDS_Shape SI = myEdges->Value(i);
    SI.Reverse();
    myEdges->SetValue(i, SI);
  }
  mySeamF = -1;
}

//=================================================================================================

//  Fonction auxiliaire SwapSeam pour inverser
static void SwapSeam(const TopoDS_Shape& S, const TopoDS_Face& F)
{
  TopoDS_Edge E = TopoDS::Edge(S);
  if (E.IsNull() || F.IsNull())
    return;
  if (E.Orientation() == TopAbs_REVERSED)
    return; // ne le faire qu une fois !

  TopoDS_Face theface = F;
  theface.Orientation(TopAbs_FORWARD);

  //: S4136  double Tol = BRep_Tool::Tolerance(theface);
  occ::handle<Geom2d_Curve> c2df, c2dr;
  double                    uff, ulf, ufr, ulr;

  // d abord FWD puis REV
  c2df = BRep_Tool::CurveOnSurface(E, theface, uff, ulf);
  E.Orientation(TopAbs_REVERSED);
  c2dr = BRep_Tool::CurveOnSurface(E, theface, ufr, ulr);
  if (c2df.IsNull() || c2dr.IsNull())
    return; //: q0
  // On permute
  E.Orientation(TopAbs_FORWARD);
  BRep_Builder B;
  B.UpdateEdge(E, c2dr, c2df, theface, 0.); //: S4136: Tol
  B.Range(E, theface, uff, ulf);
}

void ShapeExtend_WireData::Reverse(const TopoDS_Face& face)
{
  Reverse();
  if (face.IsNull())
    return;

  //  ATTENTION aux coutures
  //  Une edge de couture est presente deux fois, FWD et REV
  //  Les inverser revient a permuter leur role ... donc ne rien faire
  //  Il faut donc aussi permuter leurs pcurves
  ComputeSeams(true);
  if (mySeamF > 0)
    SwapSeam(myEdges->Value(mySeamF), face);
  if (mySeamR > 0)
    SwapSeam(myEdges->Value(mySeamR), face);
  int nb = (mySeams.IsNull() ? 0 : mySeams->Length());
  for (int i = 1; i <= nb; i++)
  {
    SwapSeam(myEdges->Value(mySeams->Value(i)), face);
  }
  mySeamF = -1;
}

//=================================================================================================

int ShapeExtend_WireData::NbEdges() const
{
  return myEdges->Length();
}

//=================================================================================================

TopoDS_Edge ShapeExtend_WireData::Edge(const int num) const
{
  if (num < 0)
  {
    TopoDS_Edge E = Edge(-num);
    E.Reverse();
    return E;
  }
  return TopoDS::Edge(myEdges->Value(num));
}

//=================================================================================================

int ShapeExtend_WireData::NbNonManifoldEdges() const
{
  return myNonmanifoldEdges->Length();
}

//=================================================================================================

TopoDS_Edge ShapeExtend_WireData::NonmanifoldEdge(const int num) const
{
  TopoDS_Edge E;
  if (num < 0)
    return E;

  return TopoDS::Edge(myNonmanifoldEdges->Value(num));
}

//=================================================================================================

int ShapeExtend_WireData::Index(const TopoDS_Edge& edge)
{
  for (int i = 1; i <= NbEdges(); i++)
    if (Edge(i).IsSame(edge) && (Edge(i).Orientation() == edge.Orientation() || !IsSeam(i)))
      return i;
  return 0;
}

//=================================================================================================

bool ShapeExtend_WireData::IsSeam(const int num)
{
  if (mySeamF < 0)
    ComputeSeams();
  if (mySeamF == 0)
    return false;

  if (num == mySeamF || num == mySeamR)
    return true;
  // Use hash set for O(1) lookup instead of O(n) linear search
  return mySeamsCache.Contains(num);
}

//=================================================================================================

TopoDS_Wire ShapeExtend_WireData::Wire() const
{
  TopoDS_Wire  W;
  BRep_Builder B;
  B.MakeWire(W);
  int  i, nb = NbEdges();
  bool ismanifold = true;
  for (i = 1; i <= nb; i++)
  {
    TopoDS_Edge aE = Edge(i);
    if (aE.Orientation() != TopAbs_FORWARD && aE.Orientation() != TopAbs_REVERSED)
      ismanifold = false;
    B.Add(W, aE);
  }
  if (ismanifold)
  {
    TopoDS_Vertex vf, vl;
    TopExp::Vertices(W, vf, vl);
    if (!vf.IsNull() && !vl.IsNull() && vf.IsSame(vl))
      W.Closed(true);
  }
  if (myManifoldMode)
  {
    nb = NbNonManifoldEdges();
    for (i = 1; i <= nb; i++)
      B.Add(W, NonmanifoldEdge(i));
  }
  return W;
}

//=================================================================================================

TopoDS_Wire ShapeExtend_WireData::WireAPIMake() const
{
  TopoDS_Wire             W;
  BRepBuilderAPI_MakeWire MW;
  int                     i, nb = NbEdges();
  for (i = 1; i <= nb; i++)
    MW.Add(Edge(i));
  if (myManifoldMode)
  {
    nb = NbNonManifoldEdges();
    for (i = 1; i <= nb; i++)
      MW.Add(NonmanifoldEdge(i));
  }
  if (MW.IsDone())
    W = MW.Wire();
  return W;
}

//=================================================================================================

occ::handle<NCollection_HSequence<TopoDS_Shape>> ShapeExtend_WireData::NonmanifoldEdges() const
{
  return myNonmanifoldEdges;
}

//=================================================================================================

bool& ShapeExtend_WireData::ManifoldMode()
{
  return myManifoldMode;
}
