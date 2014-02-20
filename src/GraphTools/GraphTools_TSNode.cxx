// Created on: 1991-06-20
// Created by: Denis PASCAL
// Copyright (c) 1991-1999 Matra Datavision
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




