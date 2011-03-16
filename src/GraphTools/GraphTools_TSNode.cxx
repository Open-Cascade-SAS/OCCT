
// Copyright: 	Matra-Datavision 1991
// File:	GraphTools_TSNode.cxx
// Created:	Thu Jun 20 15:46:35 1991
// Author:	Denis PASCAL
//		<dp>

#include <GraphTools_TSNode.ixx>

//=======================================================================
//function : GraphTools_TSNode
//purpose  : 
//=======================================================================
GraphTools_TSNode::GraphTools_TSNode () 
{
  referenceCount = 0;
}


//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void GraphTools_TSNode::Reset () 
{
  referenceCount = 0;
  mySuccessors.Clear();
}



//=======================================================================
//function : IncreaseRef
//purpose  : 
//=======================================================================

void GraphTools_TSNode::IncreaseRef() { referenceCount++; }

//=======================================================================
//function : DecreaseRef
//purpose  : 
//=======================================================================

void GraphTools_TSNode::DecreaseRef() { referenceCount--; }


//=======================================================================
//function : NbRef
//purpose  : 
//=======================================================================

Standard_Integer   GraphTools_TSNode::NbRef() const 
{
  return referenceCount;
}


//=======================================================================
//function : AddSuccessor
//purpose  : 
//=======================================================================

void  GraphTools_TSNode::AddSuccessor(const Standard_Integer s) 
{
  mySuccessors.Append(s);
}


//=======================================================================
//function : NbSuccessors
//purpose  : 
//=======================================================================

Standard_Integer GraphTools_TSNode::NbSuccessors() const
{
  return mySuccessors.Length();
}

//=======================================================================
//function : GetSuccessor
//purpose  : 
//=======================================================================

Standard_Integer  GraphTools_TSNode::GetSuccessor
  (const Standard_Integer index) const 
{
  return mySuccessors(index);
}




