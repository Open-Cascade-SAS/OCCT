// File:	BOP_LoopSet.cxx
// Created:	Tue Mar 23 15:01:08 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

#include <BOP_LoopSet.ixx>

//=======================================================================
//function : BOP_LoopSet::BOP_LoopSet
//purpose  : 
//=======================================================================
  BOP_LoopSet::BOP_LoopSet() 
: 
  myLoopIndex(1), 
  myNbLoop(0)
{}
//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
  void BOP_LoopSet::Delete()
{}
//=======================================================================
//function : InitLoop
//purpose  : 
//=======================================================================
  void BOP_LoopSet::InitLoop()
{
  myLoopIterator.Initialize(myListOfLoop);
  myLoopIndex = 1;
  myNbLoop = myListOfLoop.Extent();
}
//=======================================================================
//function : MoreLoop
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_LoopSet::MoreLoop() const
{
  Standard_Boolean b = myLoopIterator.More();
  return b;
}
//=======================================================================
//function : NextLoop
//purpose  : 
//=======================================================================
  void BOP_LoopSet::NextLoop()
{
  myLoopIndex++;
  myLoopIterator.Next();
}
//=======================================================================
//function : Loop
//purpose  : 
//=======================================================================
  const Handle(BOP_Loop)& BOP_LoopSet::Loop() const
{
  const Handle(BOP_Loop)& L = myLoopIterator.Value();
  return L;
}
//=======================================================================
//function : ChangeListOfLoop
//purpose  : 
//=======================================================================
  BOP_ListOfLoop& BOP_LoopSet::ChangeListOfLoop()
{
  return myListOfLoop;
}
