// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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


#include <TopOpeBRepBuild_EdgeBuilder.ixx>
#include <TopOpeBRepBuild_Pave.hxx>

//=======================================================================
//function : TopOpeBRepBuild_EdgeBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_EdgeBuilder::TopOpeBRepBuild_EdgeBuilder()
{
}

//=======================================================================
//function : TopOpeBRepBuild_EdgeBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_EdgeBuilder::TopOpeBRepBuild_EdgeBuilder
(TopOpeBRepBuild_PaveSet& LS, TopOpeBRepBuild_PaveClassifier& LC,
 const Standard_Boolean ForceClass)
{
  InitEdgeBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : InitEdgeBuilder
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_EdgeBuilder::InitEdgeBuilder
(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass)
{
  InitAreaBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : InitEdge
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_EdgeBuilder::InitEdge()
{
  InitArea();
}

//=======================================================================
//function : MoreEdge
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_EdgeBuilder::MoreEdge() const
{
  Standard_Boolean b = MoreArea();
  return b;
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_EdgeBuilder::NextEdge()
{
  NextArea();
}

//=======================================================================
//function : InitVertex
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_EdgeBuilder::InitVertex()
{
  InitLoop();
}

//=======================================================================
//function : MoreVertex
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_EdgeBuilder::MoreVertex() const
{
  Standard_Boolean b = MoreLoop();
  return b;
}

//=======================================================================
//function : NextVertex
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_EdgeBuilder::NextVertex()
{
  NextLoop();
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_EdgeBuilder::Vertex() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = Loop();
  const Handle(TopOpeBRepBuild_Pave)& PV = *((Handle(TopOpeBRepBuild_Pave)*)&L);
  const TopoDS_Shape& V = PV->Vertex();
  return V;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepBuild_EdgeBuilder::Parameter() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = Loop();
  const Handle(TopOpeBRepBuild_Pave)& PV = *((Handle(TopOpeBRepBuild_Pave)*)&L);
  Standard_Real parV = PV->Parameter();
  return parV;
}
