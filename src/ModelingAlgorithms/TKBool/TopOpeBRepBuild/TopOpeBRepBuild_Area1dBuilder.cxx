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

#include <TopOpeBRepBuild_Area1dBuilder.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_LoopEnum.hxx>
#include <TopOpeBRepBuild_PaveClassifier.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceAREA();
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Area1dBuilder::DumpList(const TopOpeBRepBuild_ListOfLoop& LOL)
{
  Standard_Integer                         iLOL;
  TopOpeBRepBuild_ListIteratorOfListOfLoop itLOL;
  for (iLOL = 0, itLOL.Initialize(LOL); itLOL.More(); iLOL++, itLOL.Next())
  {
    if (iLOL)
      std::cout << "               ";
    else
      std::cout << "DUMP_AREA    : ";
    const Handle(TopOpeBRepBuild_Loop)& L = itLOL.Value();
    L->Dump();
    std::cout << std::endl;
  }
#else
void TopOpeBRepBuild_Area1dBuilder::DumpList(const TopOpeBRepBuild_ListOfLoop&)
{
#endif
}

//=================================================================================================

TopOpeBRepBuild_Area1dBuilder::TopOpeBRepBuild_Area1dBuilder() {}

//=================================================================================================

TopOpeBRepBuild_Area1dBuilder::TopOpeBRepBuild_Area1dBuilder(TopOpeBRepBuild_PaveSet&        LS,
                                                             TopOpeBRepBuild_PaveClassifier& LC,
                                                             const Standard_Boolean ForceClass)
{
  InitAreaBuilder(LS, LC, ForceClass);
}

//=================================================================================================

void TopOpeBRepBuild_Area1dBuilder::InitAreaBuilder(TopOpeBRepBuild_LoopSet&        LS,
                                                    TopOpeBRepBuild_LoopClassifier& LC,
                                                    const Standard_Boolean          ForceClass)
{
  TopAbs_State     state;
  Standard_Boolean Loopinside;
  Standard_Boolean loopoutside;

  TopOpeBRepBuild_ListIteratorOfListOfListOfLoop AreaIter;
  TopOpeBRepBuild_ListIteratorOfListOfLoop       LoopIter;
  // boundaryloops : list of boundary loops out of the areas.
  TopOpeBRepBuild_ListOfLoop boundaryloops;

  myArea.Clear(); // Clear the list of Area to be built

  for (LS.InitLoop(); LS.MoreLoop(); LS.NextLoop())
  {

    // process a new loop : L is the new current Loop
    const Handle(TopOpeBRepBuild_Loop)& L         = LS.Loop();
    Standard_Boolean                    boundaryL = L->IsShape();

#ifdef OCCT_DEBUG
    if (TopOpeBRepBuild_GettraceAREA())
    {
      std::cout << "++++ new loop : ";
      L->Dump();
      if (boundaryL)
        std::cout << " is bound";
      else
        std::cout << " is not bound";
      std::cout << std::endl;
    }
#endif

    // L = shape et ForceClass  : on traite L comme un block
    // L = shape et !ForceClass : on traite L comme un pur shape
    // L = !shape               : on traite L comme un block
    Standard_Boolean traitercommeblock = !boundaryL || ForceClass;
    if (!traitercommeblock)
    {

      // the loop L is a boundary loop :
      // - try to insert it in an existing area, such as L is inside all
      //   the block loops. Only block loops of the area are compared.
      // - if L could not be inserted, store it in list of boundary loops.

      Loopinside = Standard_False;
      for (AreaIter.Initialize(myArea); AreaIter.More(); AreaIter.Next())
      {
        const TopOpeBRepBuild_ListOfLoop& aArea = AreaIter.Value();
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
        TopOpeBRepBuild_ListOfLoop& aArea = AreaIter.ChangeValue();
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

      Loopinside = Standard_False;
      for (AreaIter.Initialize(myArea); AreaIter.More(); AreaIter.Next())
      {
        TopOpeBRepBuild_ListOfLoop& aArea = AreaIter.ChangeValue();
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
        TopOpeBRepBuild_ListOfLoop& aArea    = AreaIter.ChangeValue();
        Standard_Boolean            allShape = Standard_True;
        TopOpeBRepBuild_ListOfLoop  removedLoops;
        LoopIter.Initialize(aArea);
        while (LoopIter.More())
        {
          state = LC.Compare(LoopIter.Value(), L);
          if (state == TopAbs_UNKNOWN)
            Atomize(state, TopAbs_IN); // not OUT
          loopoutside = (state == TopAbs_OUT);
          if (loopoutside)
          {
            const Handle(TopOpeBRepBuild_Loop)& curL = LoopIter.Value();
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
            TopOpeBRepBuild_ListOfLoop thelist;
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
        Standard_Integer           ashapeinside, ablockinside;
        TopOpeBRepBuild_ListOfLoop thelist1;
        myArea.Append(thelist1);
        TopOpeBRepBuild_ListOfLoop& newArea0 = myArea.Last();
        ADD_Loop_TO_LISTOFLoop(L, newArea0, (void*)("new area"));

        LoopIter.Initialize(boundaryloops);
        while (LoopIter.More())
        {
          ashapeinside = ablockinside = Standard_False;
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
            const Handle(TopOpeBRepBuild_Loop)& curL = LoopIter.Value();
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

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceAREA())
  {
    if (!myArea.IsEmpty())
    {
      std::cout << "------- Areas -------" << std::endl;
      for (AreaIter.Initialize(myArea); AreaIter.More(); AreaIter.Next())
        DumpList(AreaIter.Value());
      std::cout << "---------------------" << std::endl;
    }
  }
#endif

  InitArea();
}

//=================================================================================================

void TopOpeBRepBuild_Area1dBuilder::ADD_Loop_TO_LISTOFLoop(const Handle(TopOpeBRepBuild_Loop)& L,
                                                           TopOpeBRepBuild_ListOfLoop&         LOL,
                                                           const Standard_Address
#ifdef OCCT_DEBUG
                                                             ss
#endif
) const
{
  LOL.Append(L);

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceAREA())
  {
    std::cout << "--------------------- add area loop to area : ";
    L->Dump();
    std::cout << std::endl;
    if (ss != NULL)
      std::cout << (char*)ss << std::endl;
    DumpList(LOL);
    std::cout << "---------------------" << std::endl;
  }
#endif
}

//=================================================================================================

void TopOpeBRepBuild_Area1dBuilder::REM_Loop_FROM_LISTOFLoop(
  TopOpeBRepBuild_ListIteratorOfListOfLoop& ITA,
  TopOpeBRepBuild_ListOfLoop&               A,
#ifdef OCCT_DEBUG
  const Standard_Address ss) const
{
  char* s = (char*)ss;
#else
  const Standard_Address) const
{
#endif

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceAREA())
  {
    if (ITA.More())
    {
      std::cout << "--------------------- remove area loop from area : ";
      ITA.Value()->Dump();
      std::cout << std::endl;
      if (s != NULL)
        std::cout << s << std::endl;
    }
  }
#endif

  A.Remove(ITA);

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceAREA())
  {
    DumpList(A);
    std::cout << "---------------------" << std::endl;
  }
#endif
}

//=================================================================================================

void TopOpeBRepBuild_Area1dBuilder::ADD_LISTOFLoop_TO_LISTOFLoop(TopOpeBRepBuild_ListOfLoop& A1,
                                                                 TopOpeBRepBuild_ListOfLoop& A2,
#ifdef OCCT_DEBUG
                                                                 const Standard_Address ss,
                                                                 const Standard_Address ss1,
                                                                 const Standard_Address ss2) const
#else
                                                                 const Standard_Address,
                                                                 const Standard_Address,
                                                                 const Standard_Address) const
#endif
{
#ifdef OCCT_DEBUG
  char* s  = (char*)ss;
  char* s1 = (char*)ss1;
  char* s2 = (char*)ss2;

  if (TopOpeBRepBuild_GettraceAREA())
  {
    std::cout << "--------------------- add area 1 to area 2 : ";
    if (s != NULL)
      std::cout << s;
    std::cout << std::endl;
    std::cout << "1 : ";
    if (s1 != NULL)
      std::cout << s1;
    std::cout << std::endl;
    DumpList(A1);
  }
#endif

  A2.Append(A1);

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceAREA())
  {
    std::cout << "2 : ";
    if (s2 != NULL)
      std::cout << s2;
    std::cout << std::endl;
    DumpList(A2);
    std::cout << "---------------------" << std::endl;
  }
#endif
}
