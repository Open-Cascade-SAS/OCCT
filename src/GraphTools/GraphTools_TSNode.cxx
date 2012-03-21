// Created on: 1991-06-20
// Created by: Denis PASCAL
// Copyright (c) 1991-1999 Matra Datavision
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




