// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

// " Voyager, c'est bien utile, ca fait travailler l'imagination.
//   Tout le reste n'est que deceptions et fatigues. Notre voyage
//   a nous est entierement imaginaire. Voila sa force. "
//                         Celine
//                         Voyage au bout de la nuit

#include <TopOpeBRepBuild_AreaBuilder.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_LoopClassifier.hxx>
#include <TopOpeBRepBuild_LoopSet.hxx>

//=================================================================================================

TopOpeBRepBuild_AreaBuilder::TopOpeBRepBuild_AreaBuilder()
    : myUNKNOWNRaise(false) // no raise if UNKNOWN state found
{
}

//=================================================================================================

TopOpeBRepBuild_AreaBuilder::TopOpeBRepBuild_AreaBuilder(TopOpeBRepBuild_LoopSet&        LS,
                                                         TopOpeBRepBuild_LoopClassifier& LC,
                                                         const bool                      ForceClass)
    : myUNKNOWNRaise(false) // no raise if UNKNOWN state found
{
  InitAreaBuilder(LS, LC, ForceClass);
}

TopOpeBRepBuild_AreaBuilder::~TopOpeBRepBuild_AreaBuilder() = default;

//=======================================================================
// function : CompareLoopWithListOfLoop
// purpose  : Compare position of the Loop <L> with the Area <LOL>
//           using the Loop Classifier <LC>.
//           According to <whattotest>, Loops of <LOL> are selected or not
//           during <LOL> exploration.
// result   : TopAbs_OUT if <LOL> is empty
//           TopAbs_UNKNOWN if position undefined
//           TopAbs_IN  if <L> is inside all the selected Loops of <LOL>
//           TopAbs_OUT if <L> is outside one of the selected Loops of <LOL>
//           TopAbs_ON  if <L> is on one of the selected Loops of <LOL>
//=======================================================================
TopAbs_State TopOpeBRepBuild_AreaBuilder::CompareLoopWithListOfLoop(
  TopOpeBRepBuild_LoopClassifier&                            LC,
  const occ::handle<TopOpeBRepBuild_Loop>&                   L,
  const NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& LOL,
  const TopOpeBRepBuild_LoopEnum                             what) const
{
  TopAbs_State                                                  state = TopAbs_UNKNOWN;
  bool                                                          totest; // L must or not be tested
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>::Iterator LoopIter;

  if (LOL.IsEmpty())
    return TopAbs_OUT;

  for (LoopIter.Initialize(LOL); LoopIter.More(); LoopIter.Next())
  {
    const occ::handle<TopOpeBRepBuild_Loop>& curL = LoopIter.Value();
    switch (what)
    {
      case TopOpeBRepBuild_ANYLOOP:
        totest = true;
        break;
      case TopOpeBRepBuild_BOUNDARY:
        totest = curL->IsShape();
        break;
      case TopOpeBRepBuild_BLOCK:
        totest = !curL->IsShape();
        break;
      default:
        totest = false;
    }
    if (totest)
    {
      state = LC.Compare(L, curL);
      if (state == TopAbs_OUT)
        // <L> is out of at least one Loop of <LOL> : stop to explore
        break;
    }
  }

  return state;
}

//=======================================================================
// function : Atomize
// purpose  : myUNKNOWNRaise = True --> raise if <state> = UNKNOWN
//           myUNKNOWNRaise = False--> assign a new value <newstate> to <state>
//=======================================================================

void TopOpeBRepBuild_AreaBuilder::Atomize(TopAbs_State& state, const TopAbs_State newstate) const
{
  if (myUNKNOWNRaise)
  {
    Standard_DomainError_Raise_if((state == TopAbs_UNKNOWN), "AreaBuilder : Position Unknown");
  }
  else
  {
    state = newstate;
  }
}

//=================================================================================================

void TopOpeBRepBuild_AreaBuilder::InitAreaBuilder(TopOpeBRepBuild_LoopSet&        LS,
                                                  TopOpeBRepBuild_LoopClassifier& LC,
                                                  const bool                      ForceClass)
{
  TopAbs_State state;
  bool         Loopinside;
  bool         loopoutside;

  NCollection_List<NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>>::Iterator AreaIter;
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>::Iterator                   LoopIter;
  // boundaryloops : list of boundary loops out of the areas.
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>> boundaryloops;

  myArea.Clear(); // Clear the list of Area to be built

  for (LS.InitLoop(); LS.MoreLoop(); LS.NextLoop())
  {

    // process a new loop : L is the new current Loop
    const occ::handle<TopOpeBRepBuild_Loop>& L         = LS.Loop();
    bool                                     boundaryL = L->IsShape();

    // L = Shape et ForceClass  : on traite L comme un block
    // L = Shape et !ForceClass : on traite L comme un pur Shape
    // L = !Shape               : on traite L comme un block
    bool traitercommeblock = (!boundaryL) || ForceClass;
    if (!traitercommeblock)
    {

      // the loop L is a boundary loop :
      // - try to insert it in an existing area, such as L is inside all
      //   the block loops. Only block loops of the area are compared.
      // - if L could not be inserted, store it in list of boundary loops.

      Loopinside = false;
      for (AreaIter.Initialize(myArea); AreaIter.More(); AreaIter.Next())
      {
        const NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& aArea = AreaIter.Value();
        if (aArea.IsEmpty())
          continue;
        state = CompareLoopWithListOfLoop(LC, L, aArea, TopOpeBRepBuild_BLOCK);
        if (state == TopAbs_UNKNOWN)
          Atomize(state, TopAbs_IN);
        Loopinside = (state == TopAbs_IN);
        if (Loopinside)
          break;
      } // end of Area scan

      if (Loopinside)
      {
        NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& aArea = AreaIter.ChangeValue();
        ADD_Loop_TO_LISTOFLoop(L, aArea, (void*)("IN, to current area"));
      }
      else if (!Loopinside)
      {
        ADD_Loop_TO_LISTOFLoop(L, boundaryloops, (void*)("! IN, to boundaryloops"));
      }

    } // end of boundary loop

    else
    {
      // the loop L is a block loop
      // if L is IN theArea :
      //   - stop area scan, insert L in theArea.
      //   - remove from the area all the loops outside L
      //   - make a new area with them, unless they are all boundary
      //   - if they are all boundary put them back in boundaryLoops
      // else :
      //   - create a new area with L.
      //   - insert boundary loops that are IN the new area
      //     (and remove them from 'boundaryloops')

      Loopinside = false;
      for (AreaIter.Initialize(myArea); AreaIter.More(); AreaIter.Next())
      {
        const NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& aArea = AreaIter.Value();
        if (aArea.IsEmpty())
          continue;
        state = CompareLoopWithListOfLoop(LC, L, aArea, TopOpeBRepBuild_ANYLOOP);
        if (state == TopAbs_UNKNOWN)
          Atomize(state, TopAbs_IN);
        Loopinside = (state == TopAbs_IN);
        if (Loopinside)
          break;
      } // end of Area scan

      if (Loopinside)
      {
        NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& aArea    = AreaIter.ChangeValue();
        bool                                                 allShape = true;
        NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>  removedLoops;
        LoopIter.Initialize(aArea);
        while (LoopIter.More())
        {
          state = LC.Compare(LoopIter.Value(), L);
          if (state == TopAbs_UNKNOWN)
            Atomize(state, TopAbs_IN); // not OUT
          loopoutside = (state == TopAbs_OUT);
          if (loopoutside)
          {
            const occ::handle<TopOpeBRepBuild_Loop>& curL = LoopIter.Value();
            // remove the loop from the area
            ADD_Loop_TO_LISTOFLoop(curL,
                                   removedLoops,
                                   (void*)("loopoutside = 1, area = removedLoops"));

            allShape = allShape && curL->IsShape();
            REM_Loop_FROM_LISTOFLoop(LoopIter,
                                     AreaIter.ChangeValue(),
                                     (void*)("loop of cur. area, cur. area"));
          }
          else
          {
            LoopIter.Next();
          }
        }
        // insert the loop in the area
        ADD_Loop_TO_LISTOFLoop(L, aArea, (void*)("area = current"));
        if (!removedLoops.IsEmpty())
        {
          if (allShape)
          {
            ADD_LISTOFLoop_TO_LISTOFLoop(removedLoops,
                                         boundaryloops,
                                         (void*)("allShape = 1"),
                                         (void*)("removedLoops"),
                                         (void*)("boundaryloops"));
          }
          else
          {
            // make a new area with the removed loops
            NCollection_List<occ::handle<TopOpeBRepBuild_Loop>> thelist;
            myArea.Append(thelist);
            ADD_LISTOFLoop_TO_LISTOFLoop(removedLoops,
                                         myArea.Last(),
                                         (void*)("allShape = 0"),
                                         (void*)("removedLoops"),
                                         (void*)("new area"));
          }
        }
      } // Loopinside == True

      else
      {
        int                                                 ashapeinside, ablockinside;
        NCollection_List<occ::handle<TopOpeBRepBuild_Loop>> thelist1;
        myArea.Append(thelist1);
        NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& newArea0 = myArea.Last();
        ADD_Loop_TO_LISTOFLoop(L, newArea0, (void*)("new area"));

        LoopIter.Initialize(boundaryloops);
        while (LoopIter.More())
        {
          ashapeinside = ablockinside = false;
          state                       = LC.Compare(LoopIter.Value(), L);
          if (state == TopAbs_UNKNOWN)
            Atomize(state, TopAbs_IN);
          ashapeinside = (state == TopAbs_IN);
          if (ashapeinside)
          {
            state = LC.Compare(L, LoopIter.Value());
            if (state == TopAbs_UNKNOWN)
              Atomize(state, TopAbs_IN);
            ablockinside = (state == TopAbs_IN);
          }
          if (ashapeinside && ablockinside)
          {
            const occ::handle<TopOpeBRepBuild_Loop>& curL = LoopIter.Value();
            ADD_Loop_TO_LISTOFLoop(curL,
                                   newArea0,
                                   (void*)("ashapeinside && ablockinside, new area"));

            REM_Loop_FROM_LISTOFLoop(LoopIter,
                                     boundaryloops,
                                     (void*)("loop of boundaryloops, boundaryloops"));
          }
          else
          {
            LoopIter.Next();
          }
        } // end of boundaryloops scan
      } // Loopinside == False
    } // end of block loop
  } // end of LoopSet LS scan

  InitArea();
}

//=================================================================================================

int TopOpeBRepBuild_AreaBuilder::InitArea()
{
  myAreaIterator.Initialize(myArea);
  InitLoop();
  int n = myArea.Extent();
  return n;
}

//=================================================================================================

bool TopOpeBRepBuild_AreaBuilder::MoreArea() const
{
  bool b = myAreaIterator.More();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_AreaBuilder::NextArea()
{
  myAreaIterator.Next();
  InitLoop();
}

//=================================================================================================

int TopOpeBRepBuild_AreaBuilder::InitLoop()
{
  int n = 0;
  if (myAreaIterator.More())
  {
    const NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& LAL = myAreaIterator.Value();
    myLoopIterator.Initialize(LAL);
    n = LAL.Extent();
  }
  else
  { // Create an empty ListIteratorOfListOfLoop
    myLoopIterator = NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>::Iterator();
  }
  return n;
}

//=================================================================================================

bool TopOpeBRepBuild_AreaBuilder::MoreLoop() const
{
  bool b = myLoopIterator.More();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_AreaBuilder::NextLoop()
{
  myLoopIterator.Next();
}

//=================================================================================================

const occ::handle<TopOpeBRepBuild_Loop>& TopOpeBRepBuild_AreaBuilder::Loop() const
{
  const occ::handle<TopOpeBRepBuild_Loop>& L = myLoopIterator.Value();
  return L;
}

//=================================================================================================

void TopOpeBRepBuild_AreaBuilder::ADD_Loop_TO_LISTOFLoop(
  const occ::handle<TopOpeBRepBuild_Loop>&             L,
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& LOL,
  void* const /*ss*/) const
{
  LOL.Append(L);
}

//=================================================================================================

void TopOpeBRepBuild_AreaBuilder::REM_Loop_FROM_LISTOFLoop(
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>::Iterator& ITA,
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>&           A,
  void* const /*ss*/) const
{
  A.Remove(ITA);
}

//=================================================================================================

void TopOpeBRepBuild_AreaBuilder::ADD_LISTOFLoop_TO_LISTOFLoop(
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& A1,
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& A2,
  void* const /*ss*/,
  void* const /*ss1*/,
  void* const /*ss2*/) const
{
  A2.Append(A1);
}
