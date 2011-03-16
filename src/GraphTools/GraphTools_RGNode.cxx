
// File:	GraphTools_RGNode.cxx
// Created:	Wed Sep 29 15:15:45 1993
// Author:	Denis PASCAL
//		<dp@bravox>


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
