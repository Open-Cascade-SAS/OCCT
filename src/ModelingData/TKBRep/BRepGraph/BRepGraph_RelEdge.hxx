// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _BRepGraph_RelEdge_HeaderFile
#define _BRepGraph_RelEdge_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <TCollection_AsciiString.hxx>

//! Semantic kind of a directed relationship edge in the graph.
enum class BRepGraph_RelKind : int
{
  Contains        = 0,   //!< Parent -> child in TopoDS hierarchy
  OuterWire       = 1,   //!< FaceNode -> its outer WireNode
  InnerWire       = 2,   //!< FaceNode -> a hole WireNode
  RealizedBy      = 3,   //!< TopoNode -> geometry node (Face->Surf, Edge->Curve)
  ParameterizedBy = 4,   //!< EdgeNode -> PCurveNode (with FaceNode context)
  SameDomain      = 5,   //!< FaceNode <-> FaceNode sharing identical Surface Handle
  DerivedFrom     = 6,   //!< History: new node -> original node, with op label
  UserDefined     = 100  //!< Algorithm-specific edges (sewing candidates, etc.)
};

//! A single directed relationship between two graph nodes.
//!
//! @param ContextNode valid only for ParameterizedBy (holds the FaceNode
//!   that scopes the pcurve).
//! @param Label valid only for DerivedFrom (operation name string).
struct BRepGraph_RelEdge
{
  BRepGraph_RelKind       Kind = BRepGraph_RelKind::Contains;
  BRepGraph_NodeId        Source;
  BRepGraph_NodeId        Target;
  BRepGraph_NodeId        ContextNode;  //!< ParameterizedBy only
  TCollection_AsciiString Label;        //!< DerivedFrom only
};

#endif // _BRepGraph_RelEdge_HeaderFile
