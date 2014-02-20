// Created on: 1993-09-30
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

#include <GraphTools_SC.ixx>

//=======================================================================
//function : GraphTools_SC
//purpose  : 
//=======================================================================

GraphTools_SC::GraphTools_SC () {}


//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void  GraphTools_SC::Reset()
{
  myBackSC.Clear();
  myVertices.Clear();
  myFrontSC.Clear();
}


//=======================================================================
//function : AddVertex
//purpose  : 
//=======================================================================

void  GraphTools_SC::AddVertex(const Standard_Integer V) 
{
  myVertices.Append (V);
}


//=======================================================================
//function : NbVertices
//purpose  : 
//=======================================================================

Standard_Integer  GraphTools_SC::NbVertices() const
{
  return myVertices.Length();
}



//=======================================================================
//function : GetVertex
//purpose  : 
//=======================================================================

Standard_Integer  GraphTools_SC::GetVertex
  (const Standard_Integer index) const
{
  return myVertices(index);
}

//=======================================================================
//function : AddFrontSC
//purpose  : 
//=======================================================================

void  GraphTools_SC::AddFrontSC(const Handle(GraphTools_SC)& SC)
{
  myFrontSC.Append(SC);
}


//=======================================================================
//function : GetFrontSC
//purpose  : 
//=======================================================================

const GraphTools_SCList&  GraphTools_SC::GetFrontSC() const
{
  return myFrontSC;
}


//=======================================================================
//function : AddBackSC
//purpose  : 
//=======================================================================

void  GraphTools_SC::AddBackSC(const Handle(GraphTools_SC)& SC)
{
  myBackSC.Append(SC);
}

//=======================================================================
//function : GetBackSC
//purpose  : 
//=======================================================================

const GraphTools_SCList&  GraphTools_SC::GetBackSC() const
{
  return myBackSC;
}


