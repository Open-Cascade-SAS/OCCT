// Created on: 1993-09-29
// Created by: Denis PASCAL
// Copyright (c) 1993-1999 Matra Datavision
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
