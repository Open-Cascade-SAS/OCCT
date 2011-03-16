// File:	TopOpeBRepBuild_FaceAreaBuilder.cxx
// Created:	Thu Dec 21 17:07:40 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>
//Copyright:	 Matra Datavision 1995

#include <TopOpeBRepBuild_FaceAreaBuilder.ixx>

//=======================================================================
//function : TopOpeBRepBuild_FaceAreaBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_FaceAreaBuilder::TopOpeBRepBuild_FaceAreaBuilder()
{
}

//=======================================================================
//function : TopOpeBRepBuild_FaceAreaBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_FaceAreaBuilder::TopOpeBRepBuild_FaceAreaBuilder
(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass)
{
  InitFaceAreaBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : TopOpeBRepBuild_FaceAreaBuilder
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_FaceAreaBuilder::InitFaceAreaBuilder
(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass)
{
  InitAreaBuilder(LS,LC,ForceClass);
}
