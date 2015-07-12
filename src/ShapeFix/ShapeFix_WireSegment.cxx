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
#include <TopoDS_Wire.hxx>

//=======================================================================
//function : ShapeFix_WireSegment
//purpose  : 
//=======================================================================
ShapeFix_WireSegment::ShapeFix_WireSegment()
{
  Clear();
  myOrient = TopAbs_FORWARD;
}

//=======================================================================
//function : ShapeFix_WireSegment
//purpose  : 
//=======================================================================

ShapeFix_WireSegment::ShapeFix_WireSegment(const Handle(ShapeExtend_WireData)& wire,
					   const TopAbs_Orientation ori)
{
  Load ( wire );
  myOrient = ori;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::Clear()
{
  myWire = new ShapeExtend_WireData;
  myWire->ManifoldMode() = Standard_False;
  myIUMin = new TColStd_HSequenceOfInteger;
  myIUMax = new TColStd_HSequenceOfInteger;
  myIVMin = new TColStd_HSequenceOfInteger;
  myIVMax = new TColStd_HSequenceOfInteger;
  myVertex = TopoDS_Vertex();
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::Load (const Handle(ShapeExtend_WireData)& wire)
{
//  myWire = wire;
  Clear();
  myWire->ManifoldMode() = wire->ManifoldMode();
  for ( Standard_Integer i=1; i <= wire->NbEdges(); i++ ) 
    AddEdge ( i, wire->Edge(i) );
}

//=======================================================================
//function : WireData
//purpose  : 
//=======================================================================

const Handle(ShapeExtend_WireData)& ShapeFix_WireSegment::WireData() const
{
  return myWire;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::Orientation(const TopAbs_Orientation ori)
{
  myOrient = ori;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation ShapeFix_WireSegment::Orientation() const
{
  return myOrient;
}

//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeFix_WireSegment::FirstVertex() const
{
  ShapeAnalysis_Edge sae;
  return sae.FirstVertex (myWire->Edge(1));
}

//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeFix_WireSegment::LastVertex() const
{
  ShapeAnalysis_Edge sae;
  return sae.LastVertex (myWire->Edge(myWire->NbEdges()));
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_WireSegment::IsClosed() const
{
  TopoDS_Vertex v;
  v = FirstVertex();
  return v.IsSame(LastVertex());
}

//=======================================================================
// WORK with EDGES and PATCH INDICES
//=======================================================================

#define MININD        -32000
#define MAXIND         32000

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer ShapeFix_WireSegment::NbEdges() const
{
  return  myWire->NbEdges();
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

TopoDS_Edge ShapeFix_WireSegment::Edge (const Standard_Integer i) const
{
 
  return  myWire->Edge(i);
}

//=======================================================================
//function : SetEdge
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::SetEdge (const Standard_Integer i, 
				    const TopoDS_Edge &edge) 
{
  myWire->Set ( edge, i );
}

//=======================================================================
//function : AddEdge
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::AddEdge (const Standard_Integer i, 
				    const TopoDS_Edge &edge) 
{
  AddEdge ( i, edge, MININD, MAXIND, MININD, MAXIND );
}

//=======================================================================
//function : AddEdge
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::AddEdge (const Standard_Integer i, 
				    const TopoDS_Edge &edge,
				    const Standard_Integer iumin,
				    const Standard_Integer iumax,
				    const Standard_Integer ivmin,
				    const Standard_Integer ivmax) 
{
  myWire->Add ( edge, i );
  if ( i ==0 ) {
    myIUMin->Append ( iumin );
    myIUMax->Append ( iumax );
    myIVMin->Append ( ivmin );
    myIVMax->Append ( ivmax );
  }
  else {
    myIUMin->InsertBefore ( i, iumin );
    myIUMax->InsertBefore ( i, iumax );
    myIVMin->InsertBefore ( i, ivmin );
    myIVMax->InsertBefore ( i, ivmax );
  }
}

//=======================================================================
//function : SetPatchIndex
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::SetPatchIndex (const Standard_Integer i, 
					  const Standard_Integer iumin,
					  const Standard_Integer iumax,
					  const Standard_Integer ivmin,
					  const Standard_Integer ivmax) 
{
  myIUMin->SetValue ( i, iumin );
  myIUMax->SetValue ( i, iumax );
  myIVMin->SetValue ( i, ivmin );
  myIVMax->SetValue ( i, ivmax );
}

//=======================================================================
//function : DefineIUMin
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::DefineIUMin (const Standard_Integer i, 
					const Standard_Integer iumin) 
{
  if ( myIUMin->Value(i) < iumin ) myIUMin->SetValue ( i, iumin );
#ifdef OCCT_DEBUG
  if ( myIUMin->Value(i) > myIUMax->Value(i) )
    cout << "Warning: ShapeFix_WireSegment::DefineIUMin: indexation error" << endl;
#endif
}

//=======================================================================
//function : DefineIUMax
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::DefineIUMax (const Standard_Integer i, 
					const Standard_Integer iumax) 
{
  if ( myIUMax->Value(i) > iumax ) myIUMax->SetValue ( i, iumax );
#ifdef OCCT_DEBUG
  Standard_Integer iun = myIUMin->Value(i), iux = myIUMax->Value(i);
  if ( iun > iux )
    cout << "Warning: ShapeFix_WireSegment::DefineIUMax: indexation error" << endl;
#endif
}

//=======================================================================
//function : DefineIVMin
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::DefineIVMin (const Standard_Integer i, 
					const Standard_Integer ivmin) 
{
  if ( myIVMin->Value(i) < ivmin ) myIVMin->SetValue ( i, ivmin );
#ifdef OCCT_DEBUG
  Standard_Integer ivn = myIVMin->Value(i), ivx = myIVMax->Value(i);
  if ( ivn > ivx )
    cout << "Warning: ShapeFix_WireSegment::DefineIVMin: indexation error" << endl;
#endif
}

//=======================================================================
//function : DefineIVMax
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::DefineIVMax (const Standard_Integer i, 
					const Standard_Integer ivmax) 
{
  if ( myIVMax->Value(i) > ivmax ) myIVMax->SetValue ( i, ivmax );
#ifdef OCCT_DEBUG
  Standard_Integer ivn = myIVMin->Value(i), ivx = myIVMax->Value(i);
  if ( ivn > ivx )
    cout << "Warning: ShapeFix_WireSegment::DefineIVMax: indexation error" << endl;
#endif
}

//=======================================================================
//function : GetPatchIndex
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::GetPatchIndex (const Standard_Integer i, 
					  Standard_Integer &iumin,
					  Standard_Integer &iumax,
					  Standard_Integer &ivmin,
					  Standard_Integer &ivmax) const
{
  iumin = myIUMin->Value(i);
  iumax = myIUMax->Value(i);
  ivmin = myIVMin->Value(i);
  ivmax = myIVMax->Value(i);
}

//=======================================================================
//function : CheckPatchIndex
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_WireSegment::CheckPatchIndex (const Standard_Integer i) const
{
  Standard_Integer dU = myIUMax->Value(i) - myIUMin->Value(i);
  Standard_Integer dV = myIVMax->Value(i) - myIVMin->Value(i);
  Standard_Boolean ok = ( dU ==0 || dU ==1 ) && ( dV ==0 || dV ==1 );
#ifdef OCCT_DEBUG
  if ( ! ok )
    cout << "Warning: ShapeFix_WireSegment::CheckPatchIndex: incomplete indexation" << endl;
#endif
  return ok;
}

//=======================================================================
//function : SetVertex
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::SetVertex(const TopoDS_Vertex& theVertex)
{
  myVertex = theVertex;
  //SetVertex(theVertex, MININD, MAXIND, MININD, MAXIND);
}

/*//=======================================================================
//function : SetVertex
//purpose  : 
//=======================================================================

void ShapeFix_WireSegment::SetVertex(const TopoDS_Vertex& theVertex,
				     Standard_Integer iumin,
				     Standard_Integer iumax,
				     Standard_Integer ivmin,
				     Standard_Integer ivmax)
  myVertex = theVertex;
  myIUMin->Append ( iumin );
  myIUMax->Append ( iumax );
  myIVMin->Append ( ivmin );
  myIVMax->Append ( ivmax );
}
*/
//=======================================================================
//function : GetVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeFix_WireSegment::GetVertex() const
{
  return myVertex;
}

//=======================================================================
//function : IsVertex
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_WireSegment::IsVertex() const
{
  return !myVertex.IsNull();
}
