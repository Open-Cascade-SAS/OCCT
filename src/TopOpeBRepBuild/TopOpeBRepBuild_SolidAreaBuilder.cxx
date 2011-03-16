// File:	TopOpeBRepBuild_SolidAreaBuilder.cxx
// Created:	Thu Dec 21 17:07:40 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>
//Copyright:	 Matra Datavision 1995

#include <TopOpeBRepBuild_SolidAreaBuilder.ixx>

//=======================================================================
//function : TopOpeBRepBuild_SolidAreaBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_SolidAreaBuilder::TopOpeBRepBuild_SolidAreaBuilder()
{
}

//=======================================================================
//function : TopOpeBRepBuild_SolidAreaBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_SolidAreaBuilder::TopOpeBRepBuild_SolidAreaBuilder
(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass)
{
  InitSolidAreaBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : TopOpeBRepBuild_SolidAreaBuilder
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_SolidAreaBuilder::InitSolidAreaBuilder
(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass)
{
  InitAreaBuilder(LS,LC,ForceClass);
}
