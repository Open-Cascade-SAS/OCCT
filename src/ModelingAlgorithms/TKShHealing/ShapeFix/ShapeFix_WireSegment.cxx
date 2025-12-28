// Created on: 1999-04-27
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
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

#include <ShapeAnalysis_Edge.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_WireSegment.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

ShapeFix_WireSegment::ShapeFix_WireSegment()
{
  Clear();
  myOrient = TopAbs_FORWARD;
}

//=================================================================================================

ShapeFix_WireSegment::ShapeFix_WireSegment(const occ::handle<ShapeExtend_WireData>& wire,
                                           const TopAbs_Orientation            ori)
{
  Load(wire);
  myOrient = ori;
}

//=================================================================================================

void ShapeFix_WireSegment::Clear()
{
  myWire                 = new ShapeExtend_WireData;
  myWire->ManifoldMode() = false;
  myIUMin                = new NCollection_HSequence<int>;
  myIUMax                = new NCollection_HSequence<int>;
  myIVMin                = new NCollection_HSequence<int>;
  myIVMax                = new NCollection_HSequence<int>;
  myVertex               = TopoDS_Vertex();
}

//=================================================================================================

void ShapeFix_WireSegment::Load(const occ::handle<ShapeExtend_WireData>& wire)
{
  //  myWire = wire;
  Clear();
  myWire->ManifoldMode() = wire->ManifoldMode();
  for (int i = 1; i <= wire->NbEdges(); i++)
    AddEdge(i, wire->Edge(i));
}

//=================================================================================================

const occ::handle<ShapeExtend_WireData>& ShapeFix_WireSegment::WireData() const
{
  return myWire;
}

//=================================================================================================

void ShapeFix_WireSegment::Orientation(const TopAbs_Orientation ori)
{
  myOrient = ori;
}

//=================================================================================================

TopAbs_Orientation ShapeFix_WireSegment::Orientation() const
{
  return myOrient;
}

//=================================================================================================

TopoDS_Vertex ShapeFix_WireSegment::FirstVertex() const
{
  ShapeAnalysis_Edge sae;
  return sae.FirstVertex(myWire->Edge(1));
}

//=================================================================================================

TopoDS_Vertex ShapeFix_WireSegment::LastVertex() const
{
  ShapeAnalysis_Edge sae;
  return sae.LastVertex(myWire->Edge(myWire->NbEdges()));
}

//=================================================================================================

bool ShapeFix_WireSegment::IsClosed() const
{
  TopoDS_Vertex v;
  v = FirstVertex();
  return v.IsSame(LastVertex());
}

//=======================================================================
// WORK with EDGES and PATCH INDICES
//=======================================================================

#define MININD -32000
#define MAXIND 32000

//=================================================================================================

int ShapeFix_WireSegment::NbEdges() const
{
  return myWire->NbEdges();
}

//=================================================================================================

TopoDS_Edge ShapeFix_WireSegment::Edge(const int i) const
{

  return myWire->Edge(i);
}

//=================================================================================================

void ShapeFix_WireSegment::SetEdge(const int i, const TopoDS_Edge& edge)
{
  myWire->Set(edge, i);
}

//=================================================================================================

void ShapeFix_WireSegment::AddEdge(const int i, const TopoDS_Edge& edge)
{
  AddEdge(i, edge, MININD, MAXIND, MININD, MAXIND);
}

//=================================================================================================

void ShapeFix_WireSegment::AddEdge(const int i,
                                   const TopoDS_Edge&     edge,
                                   const int iumin,
                                   const int iumax,
                                   const int ivmin,
                                   const int ivmax)
{
  myWire->Add(edge, i);
  if (i == 0)
  {
    myIUMin->Append(iumin);
    myIUMax->Append(iumax);
    myIVMin->Append(ivmin);
    myIVMax->Append(ivmax);
  }
  else
  {
    myIUMin->InsertBefore(i, iumin);
    myIUMax->InsertBefore(i, iumax);
    myIVMin->InsertBefore(i, ivmin);
    myIVMax->InsertBefore(i, ivmax);
  }
}

//=================================================================================================

void ShapeFix_WireSegment::SetPatchIndex(const int i,
                                         const int iumin,
                                         const int iumax,
                                         const int ivmin,
                                         const int ivmax)
{
  myIUMin->SetValue(i, iumin);
  myIUMax->SetValue(i, iumax);
  myIVMin->SetValue(i, ivmin);
  myIVMax->SetValue(i, ivmax);
}

//=================================================================================================

void ShapeFix_WireSegment::DefineIUMin(const int i, const int iumin)
{
  if (myIUMin->Value(i) < iumin)
    myIUMin->SetValue(i, iumin);
#ifdef OCCT_DEBUG
  if (myIUMin->Value(i) > myIUMax->Value(i))
    std::cout << "Warning: ShapeFix_WireSegment::DefineIUMin: indexation error" << std::endl;
#endif
}

//=================================================================================================

void ShapeFix_WireSegment::DefineIUMax(const int i, const int iumax)
{
  if (myIUMax->Value(i) > iumax)
    myIUMax->SetValue(i, iumax);
#ifdef OCCT_DEBUG
  int iun = myIUMin->Value(i), iux = myIUMax->Value(i);
  if (iun > iux)
    std::cout << "Warning: ShapeFix_WireSegment::DefineIUMax: indexation error" << std::endl;
#endif
}

//=================================================================================================

void ShapeFix_WireSegment::DefineIVMin(const int i, const int ivmin)
{
  if (myIVMin->Value(i) < ivmin)
    myIVMin->SetValue(i, ivmin);
#ifdef OCCT_DEBUG
  int ivn = myIVMin->Value(i), ivx = myIVMax->Value(i);
  if (ivn > ivx)
    std::cout << "Warning: ShapeFix_WireSegment::DefineIVMin: indexation error" << std::endl;
#endif
}

//=================================================================================================

void ShapeFix_WireSegment::DefineIVMax(const int i, const int ivmax)
{
  if (myIVMax->Value(i) > ivmax)
    myIVMax->SetValue(i, ivmax);
#ifdef OCCT_DEBUG
  int ivn = myIVMin->Value(i), ivx = myIVMax->Value(i);
  if (ivn > ivx)
    std::cout << "Warning: ShapeFix_WireSegment::DefineIVMax: indexation error" << std::endl;
#endif
}

//=================================================================================================

void ShapeFix_WireSegment::GetPatchIndex(const int i,
                                         int&      iumin,
                                         int&      iumax,
                                         int&      ivmin,
                                         int&      ivmax) const
{
  iumin = myIUMin->Value(i);
  iumax = myIUMax->Value(i);
  ivmin = myIVMin->Value(i);
  ivmax = myIVMax->Value(i);
}

//=================================================================================================

bool ShapeFix_WireSegment::CheckPatchIndex(const int i) const
{
  int dU = myIUMax->Value(i) - myIUMin->Value(i);
  int dV = myIVMax->Value(i) - myIVMin->Value(i);
  bool ok = (dU == 0 || dU == 1) && (dV == 0 || dV == 1);
#ifdef OCCT_DEBUG
  if (!ok)
    std::cout << "Warning: ShapeFix_WireSegment::CheckPatchIndex: incomplete indexation"
              << std::endl;
#endif
  return ok;
}

//=================================================================================================

void ShapeFix_WireSegment::SetVertex(const TopoDS_Vertex& theVertex)
{
  myVertex = theVertex;
  // SetVertex(theVertex, MININD, MAXIND, MININD, MAXIND);
}

/*//=================================================================================================

void ShapeFix_WireSegment::SetVertex(const TopoDS_Vertex& theVertex,
                     int iumin,
                     int iumax,
                     int ivmin,
                     int ivmax)
  myVertex = theVertex;
  myIUMin->Append ( iumin );
  myIUMax->Append ( iumax );
  myIVMin->Append ( ivmin );
  myIVMax->Append ( ivmax );
}
*/
//=================================================================================================

TopoDS_Vertex ShapeFix_WireSegment::GetVertex() const
{
  return myVertex;
}

//=================================================================================================

bool ShapeFix_WireSegment::IsVertex() const
{
  return !myVertex.IsNull();
}
