// Created on: 1993-09-30
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


