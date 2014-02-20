// Created on: 1993-09-29
// Created by: Denis PASCAL
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

#include <GraphTools_RGNode.ixx>


//=======================================================================
//function : GraphTools_RGNode
//purpose  : 
//=======================================================================

GraphTools_RGNode::GraphTools_RGNode()
{
  visited = 0;
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void  GraphTools_RGNode::Reset()
{
  visited = 0;
  myAdj.Clear();
  mySC.Nullify();
}

//=======================================================================
//function : SetVisited
//purpose  : 
//=======================================================================

void  GraphTools_RGNode::SetVisited(const Standard_Integer v)
{
  visited = v;
}


//=======================================================================
//function : GetVisited
//purpose  : 
//=======================================================================

Standard_Integer  GraphTools_RGNode::GetVisited() const
{
  return visited;
}


//=======================================================================
//function : AddAdj
//purpose  : 
//=======================================================================

void  GraphTools_RGNode::AddAdj (const Standard_Integer adj)
{
  myAdj.Append(adj);
}


//=======================================================================
//function : NbAdj
//purpose  : 
//=======================================================================

Standard_Integer  GraphTools_RGNode::NbAdj() const 
{
  return myAdj.Length();
}


//=======================================================================
//function : GetAdj
//purpose  : 
//=======================================================================

Standard_Integer  GraphTools_RGNode::GetAdj
  (const Standard_Integer index) const
{
  return myAdj(index);
}


//=======================================================================
//function : SetSC
//purpose  : 
//=======================================================================

void  GraphTools_RGNode::SetSC (const Handle(GraphTools_SC)& SC)
{
  mySC = SC;
}


//=======================================================================
//function : GetSC
//purpose  : 
//=======================================================================

Handle(GraphTools_SC)  GraphTools_RGNode::GetSC () const
{
  return mySC;
}
