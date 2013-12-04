// Created on: 2000-09-29
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PXCAFDoc_GraphNode.ixx>
#include <Standard_GUID.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_GraphNode::PXCAFDoc_GraphNode()
{
  myFathers  = new PXCAFDoc_GraphNodeSequence;
  myChildren = new PXCAFDoc_GraphNodeSequence;
}

//=======================================================================
//function : SetGraphID
//purpose  : 
//=======================================================================

void PXCAFDoc_GraphNode::SetGraphID (const Standard_GUID& explicitID)
{
  myGraphID = explicitID;
}

//=======================================================================
//function : GetGraphID
//purpose  : 
//=======================================================================

Standard_GUID PXCAFDoc_GraphNode::GetGraphID() const
{
  return myGraphID;
}

//=======================================================================
//function : SetFather
//purpose  : 
//=======================================================================

Standard_Integer PXCAFDoc_GraphNode::SetFather(const Handle(PXCAFDoc_GraphNode)& F) 
{
  Standard_Integer Findex = myFathers->Length();
  myFathers->Append(F);
  return ++Findex;
}

//=======================================================================
//function : SetChild
//purpose  : 
//=======================================================================

Standard_Integer PXCAFDoc_GraphNode::SetChild(const Handle(PXCAFDoc_GraphNode)& Ch) 
{
  Standard_Integer Chindex = myChildren->Length();
  myChildren->Append(Ch);
  return ++Chindex;
}

//=======================================================================
//function : GetFather
//purpose  : 
//=======================================================================

 Handle(PXCAFDoc_GraphNode) PXCAFDoc_GraphNode::GetFather(const Standard_Integer Findex) const
{
  Handle(PXCAFDoc_GraphNode) F = myFathers->Value(Findex);
  return F;
}

//=======================================================================
//function : GetChild
//purpose  : 
//=======================================================================

 Handle(PXCAFDoc_GraphNode) PXCAFDoc_GraphNode::GetChild(const Standard_Integer Chindex) const
{
  Handle(PXCAFDoc_GraphNode) Ch = myChildren->Value(Chindex);
  return Ch;
}

//=======================================================================
//function : FatherIndex
//purpose  : 
//=======================================================================

 Standard_Integer PXCAFDoc_GraphNode::FatherIndex(const Handle(PXCAFDoc_GraphNode)& F) const
{
  Standard_Integer Findex = 0;
  for ( ; Findex <= NbFathers(); Findex++) {
    if ( F == myFathers->Value(Findex)) return Findex;
  }
  return 0;
}

//=======================================================================
//function : ChildIndex
//purpose  : 
//=======================================================================

 Standard_Integer PXCAFDoc_GraphNode::ChildIndex(const Handle(PXCAFDoc_GraphNode)& Ch) const
{
  Standard_Integer Chindex = 0;
  for ( ; Chindex <= NbChildren(); Chindex++) {
    if ( Ch == myChildren->Value(Chindex)) return Chindex;
  }
  return 0;
}

//=======================================================================
//function : NbFathers
//purpose  : 
//=======================================================================

 Standard_Integer PXCAFDoc_GraphNode::NbFathers() const
{
  return myFathers->Length();
}

//=======================================================================
//function : NbChildren
//purpose  : 
//=======================================================================

 Standard_Integer PXCAFDoc_GraphNode::NbChildren() const
{
  return myChildren->Length();
}
