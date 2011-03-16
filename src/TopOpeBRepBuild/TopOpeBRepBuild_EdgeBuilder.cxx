// File:	TopOpeBRepBuild_EdgeBuilder.cxx
// Created:	Thu Dec 21 17:07:40 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>
// Copyright:	 Matra Datavision 1995

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
