// File:	TopOpeBRepBuild_LoopSet.cxx
// Created:	Tue Mar 23 15:01:08 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

#include <TopOpeBRepBuild_LoopSet.ixx>

//=======================================================================
//function : TopOpeBRepBuild_LoopSet
//purpose  : 
//=======================================================================

TopOpeBRepBuild_LoopSet::TopOpeBRepBuild_LoopSet() : 
myLoopIndex(1), myNbLoop(0)
{
}

void TopOpeBRepBuild_LoopSet::Delete()
{}

//=======================================================================
//function : InitLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_LoopSet::InitLoop()
{
  myLoopIterator.Initialize(myListOfLoop);
  myLoopIndex = 1; myNbLoop = myListOfLoop.Extent();
}

//=======================================================================
//function : MoreLoop
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_LoopSet::MoreLoop() const
{
  Standard_Boolean b = myLoopIterator.More();
  return b;
}

//=======================================================================
//function : NextLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_LoopSet::NextLoop()
{
  myLoopIndex++;
  myLoopIterator.Next();
}

//=======================================================================
//function : Loop
//purpose  : 
//=======================================================================

const Handle(TopOpeBRepBuild_Loop)& TopOpeBRepBuild_LoopSet::Loop() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = myLoopIterator.Value();
  return L;
}


//=======================================================================
//function : ChangeListOfLoop
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ListOfLoop& TopOpeBRepBuild_LoopSet::ChangeListOfLoop()
{
  return myListOfLoop;
}
