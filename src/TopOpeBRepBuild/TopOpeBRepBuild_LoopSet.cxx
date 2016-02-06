// Created on: 1993-03-23
// Created by: Jean Yves LEBEY
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


#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_LoopSet.hxx>

//=======================================================================
//function : TopOpeBRepBuild_LoopSet
//purpose  : 
//=======================================================================
TopOpeBRepBuild_LoopSet::TopOpeBRepBuild_LoopSet() : 
myLoopIndex(1), myNbLoop(0)
{
}

TopOpeBRepBuild_LoopSet::~TopOpeBRepBuild_LoopSet()
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

Handle(TopOpeBRepBuild_Loop) TopOpeBRepBuild_LoopSet::Loop() const
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
