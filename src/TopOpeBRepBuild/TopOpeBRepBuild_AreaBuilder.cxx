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

#include <Standard_DomainError.hxx>
#include <TopAbs.hxx>
#include <TopOpeBRepBuild_AreaBuilder.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_LoopClassifier.hxx>
#include <TopOpeBRepBuild_LoopSet.hxx>

//=======================================================================
//function : TopOpeBRepBuild_AreaBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_AreaBuilder::TopOpeBRepBuild_AreaBuilder() :
myUNKNOWNRaise(Standard_False) // no raise if UNKNOWN state found
{
}

//=======================================================================
//function : TopOpeBRepBuild_AreaBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_AreaBuilder::TopOpeBRepBuild_AreaBuilder
(TopOpeBRepBuild_LoopSet&        LS,
 TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass) :
myUNKNOWNRaise(Standard_False) // no raise if UNKNOWN state found
{
  InitAreaBuilder(LS,LC,ForceClass);
}

TopOpeBRepBuild_AreaBuilder::~TopOpeBRepBuild_AreaBuilder()
{}

//=======================================================================
//function : CompareLoopWithListOfLoop
//purpose  : Compare position of the Loop <L> with the Area <LOL>
//           using the Loop Classifier <LC>.
//           According to <whattotest>, Loops of <LOL> are selected or not
//           during <LOL> exploration.
//result   : TopAbs_OUT if <LOL> is empty
//           TopAbs_UNKNOWN if position undefined
//           TopAbs_IN  if <L> is inside all the selected Loops of <LOL>
//           TopAbs_OUT if <L> is outside one of the selected Loops of <LOL>
//           TopAbs_ON  if <L> is on one of the selected Loops of <LOL>
//=======================================================================
TopAbs_State TopOpeBRepBuild_AreaBuilder::CompareLoopWithListOfLoop
(TopOpeBRepBuild_LoopClassifier   &LC,
 const Handle(TopOpeBRepBuild_Loop)& L,
 const TopOpeBRepBuild_ListOfLoop &LOL,
 const TopOpeBRepBuild_LoopEnum   what) const
{
  TopAbs_State                 state = TopAbs_UNKNOWN;
  Standard_Boolean             totest; // L must or not be tested
  TopOpeBRepBuild_ListIteratorOfListOfLoop LoopIter;  
  
  if ( LOL.IsEmpty() ) return TopAbs_OUT;
  
  for ( LoopIter.Initialize(LOL); LoopIter.More(); LoopIter.Next() ) {
    const Handle(TopOpeBRepBuild_Loop)& curL = LoopIter.Value();
    switch ( what ) { 
    case TopOpeBRepBuild_ANYLOOP  : totest = Standard_True;break;
    case TopOpeBRepBuild_BOUNDARY : totest =  curL->IsShape();break;
    case TopOpeBRepBuild_BLOCK    : totest = !curL->IsShape();break;
    default                       : totest = Standard_False;
    }
    if ( totest ) {
      state = LC.Compare(L,curL);
      if (state == TopAbs_OUT) 
	// <L> is out of at least one Loop of <LOL> : stop to explore
	break;  
    }
  }
  
  return state;
}

//=======================================================================
//function : Atomize
//purpose  : myUNKNOWNRaise = True --> raise if <state> = UNKNOWN
//           myUNKNOWNRaise = False--> assign a new value <newstate> to <state>
//=======================================================================

void TopOpeBRepBuild_AreaBuilder::Atomize(TopAbs_State& state, 
					  const TopAbs_State newstate) const
{
  if (myUNKNOWNRaise) {
    Standard_DomainError_Raise_if((state == TopAbs_UNKNOWN),
				  "AreaBuilder : Position Unknown");
  }
  else {
    state = newstate;
  }
}


//=======================================================================
//function : InitAreaBuilder
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_AreaBuilder::InitAreaBuilder
(TopOpeBRepBuild_LoopSet&        LS,
 TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass)
{
  TopAbs_State     state;
  Standard_Boolean Loopinside;
  Standard_Boolean loopoutside;
  
  TopOpeBRepBuild_ListIteratorOfListOfListOfLoop AreaIter;
  TopOpeBRepBuild_ListIteratorOfListOfLoop       LoopIter;
  // boundaryloops : list of boundary loops out of the areas.
  TopOpeBRepBuild_ListOfLoop                     boundaryloops; 
  
  myArea.Clear();          // Clear the list of Area to be built
  
  for (LS.InitLoop(); LS.MoreLoop(); LS.NextLoop()) {
    
    // process a new loop : L is the new current Loop
    const Handle(TopOpeBRepBuild_Loop)& L = LS.Loop();
    Standard_Boolean boundaryL = L->IsShape();
    
    // L = Shape et ForceClass  : on traite L comme un block
    // L = Shape et !ForceClass : on traite L comme un pur Shape
    // L = !Shape               : on traite L comme un block
    Standard_Boolean traitercommeblock = (!boundaryL) || ForceClass;
    if ( ! traitercommeblock ) {

      // the loop L is a boundary loop : 
      // - try to insert it in an existing area, such as L is inside all 
      //   the block loops. Only block loops of the area are compared. 
      // - if L could not be inserted, store it in list of boundary loops.

      Loopinside = Standard_False; 
      for (AreaIter.Initialize(myArea); AreaIter.More(); AreaIter.Next()) {
	TopOpeBRepBuild_ListOfLoop& aArea = AreaIter.Value();
	if ( aArea.IsEmpty() ) continue;
	state = CompareLoopWithListOfLoop(LC,L,aArea,TopOpeBRepBuild_BLOCK );
	if (state == TopAbs_UNKNOWN) Atomize(state,TopAbs_IN);
	Loopinside = ( state == TopAbs_IN);
	if ( Loopinside ) break;
      } // end of Area scan

      if ( Loopinside ) {
	TopOpeBRepBuild_ListOfLoop& aArea = AreaIter.Value();
	ADD_Loop_TO_LISTOFLoop(L,aArea,(void*)("IN, to current area"));
      }
      else if ( ! Loopinside ) {
	ADD_Loop_TO_LISTOFLoop(L,boundaryloops,(void*)("! IN, to boundaryloops"));
      }

    } // end of boundary loop
    
    else { 
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
      for (AreaIter.Initialize(myArea); AreaIter.More(); AreaIter.Next() ) {
	TopOpeBRepBuild_ListOfLoop& aArea = AreaIter.Value();
	if ( aArea.IsEmpty() ) continue;
 	state = CompareLoopWithListOfLoop(LC,L,aArea,TopOpeBRepBuild_ANYLOOP);
	if (state == TopAbs_UNKNOWN) Atomize(state,TopAbs_IN);
 	Loopinside = (state == TopAbs_IN);
	if ( Loopinside ) break;
      } // end of Area scan
      
      if ( Loopinside) {
	TopOpeBRepBuild_ListOfLoop& aArea = AreaIter.Value();
	Standard_Boolean allShape = Standard_True;
	TopOpeBRepBuild_ListOfLoop removedLoops;
	LoopIter.Initialize(aArea);
	while (LoopIter.More()) {
	  state = LC.Compare(LoopIter.Value(),L);
	  if (state == TopAbs_UNKNOWN) Atomize(state,TopAbs_IN); // not OUT
	  loopoutside = ( state == TopAbs_OUT );
	  if ( loopoutside ) {
	    const Handle(TopOpeBRepBuild_Loop)& curL = LoopIter.Value();
	    // remove the loop from the area
	    ADD_Loop_TO_LISTOFLoop
	      (curL,removedLoops,(void*)("loopoutside = 1, area = removedLoops"));
	    
	    allShape = allShape && curL->IsShape();
	    REM_Loop_FROM_LISTOFLoop
	      (LoopIter,AreaIter.Value(),(void*)("loop of cur. area, cur. area"));
	  }
	  else {
	    LoopIter.Next();
	  }
	}
	// insert the loop in the area
	ADD_Loop_TO_LISTOFLoop(L,aArea,(void*)("area = current"));
	if ( ! removedLoops.IsEmpty() ) {
	  if ( allShape ) {
	    ADD_LISTOFLoop_TO_LISTOFLoop
	      (removedLoops,boundaryloops,
	       (void*)("allShape = 1"),(void*)("removedLoops"),(void*)("boundaryloops"));
	  }
	  else {
	    // make a new area with the removed loops
            TopOpeBRepBuild_ListOfLoop thelist;
	    myArea.Append(thelist);
	    ADD_LISTOFLoop_TO_LISTOFLoop
	      (removedLoops,myArea.Last(),
	       (void*)("allShape = 0"),(void*)("removedLoops"),(void*)("new area"));
	  }
	}
      } // Loopinside == True
      
      else {
        Standard_Integer ashapeinside,ablockinside;
	TopOpeBRepBuild_ListOfLoop thelist1;
	myArea.Append(thelist1);
	TopOpeBRepBuild_ListOfLoop& newArea0 = myArea.Last();
	ADD_Loop_TO_LISTOFLoop(L,newArea0,(void*)("new area"));
	
        LoopIter.Initialize(boundaryloops);
        while ( LoopIter.More() ) {
          ashapeinside = ablockinside = Standard_False;
	  state = LC.Compare(LoopIter.Value(),L);
	  if (state == TopAbs_UNKNOWN) Atomize(state,TopAbs_IN);
          ashapeinside = (state == TopAbs_IN);
          if (ashapeinside) {
	    state = LC.Compare(L,LoopIter.Value());
	    if (state == TopAbs_UNKNOWN) Atomize(state,TopAbs_IN);
	    ablockinside = (state == TopAbs_IN);
	  }
	  if ( ashapeinside && ablockinside ) {
	    const Handle(TopOpeBRepBuild_Loop)& curL = LoopIter.Value();
	    ADD_Loop_TO_LISTOFLoop
	      (curL,newArea0,(void*)("ashapeinside && ablockinside, new area"));

	    REM_Loop_FROM_LISTOFLoop
	      (LoopIter,boundaryloops,(void*)("loop of boundaryloops, boundaryloops"));
	  }
          else { 
	    LoopIter.Next();
	  }
	} // end of boundaryloops scan
      } // Loopinside == False
    } // end of block loop
  } // end of LoopSet LS scan
  
  InitArea();
}

//=======================================================================
//function : InitArea
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_AreaBuilder::InitArea()
{
  myAreaIterator.Initialize(myArea);
  InitLoop();
  Standard_Integer n = myArea.Extent();
  return n;
}

//=======================================================================
//function : MoreArea
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_AreaBuilder::MoreArea() const
{
  Standard_Boolean b = myAreaIterator.More();
  return b;
}

//=======================================================================
//Function : NextArea
//Purpose  : 
//=======================================================================

void TopOpeBRepBuild_AreaBuilder::NextArea()
{
  myAreaIterator.Next();
  InitLoop();
}

//=======================================================================
//function : InitLoop
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_AreaBuilder::InitLoop()
{
  Standard_Integer n = 0;
  if (myAreaIterator.More()) {
    const TopOpeBRepBuild_ListOfLoop& LAL = myAreaIterator.Value();
    myLoopIterator.Initialize(LAL);
    n = LAL.Extent();
  }
  else { // Create an empty ListIteratorOfListOfLoop
    myLoopIterator = TopOpeBRepBuild_ListIteratorOfListOfLoop();  
  }
  return n;
}

//=======================================================================
//function : MoreLoop
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_AreaBuilder::MoreLoop() const
{
  Standard_Boolean b = myLoopIterator.More();
  return b;
}

//=======================================================================
//function : NextLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_AreaBuilder::NextLoop()
{
  myLoopIterator.Next();
}

//=======================================================================
//function : Loop
//purpose  : 
//=======================================================================

const Handle(TopOpeBRepBuild_Loop)& TopOpeBRepBuild_AreaBuilder::Loop() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = myLoopIterator.Value();
  return L;
}

//=======================================================================
//function : ADD_Loop_TO_LISTOFLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_AreaBuilder::ADD_Loop_TO_LISTOFLoop
  (const Handle(TopOpeBRepBuild_Loop)& L,
   TopOpeBRepBuild_ListOfLoop& LOL,
   const Standard_Address /*ss*/) const
{
  LOL.Append(L);
}

//=======================================================================
//function : REM_Loop_FROM_LISTOFLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_AreaBuilder::REM_Loop_FROM_LISTOFLoop
  (TopOpeBRepBuild_ListIteratorOfListOfLoop& ITA,
   TopOpeBRepBuild_ListOfLoop& A,
   const Standard_Address /*ss*/) const
{
  A.Remove(ITA);
}

//=======================================================================
//function : ADD_LISTOFLoop_TO_LISTOFLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_AreaBuilder::ADD_LISTOFLoop_TO_LISTOFLoop
  (TopOpeBRepBuild_ListOfLoop& A1,
   TopOpeBRepBuild_ListOfLoop& A2,
   const Standard_Address /*ss*/,
   const Standard_Address /*ss1*/,
   const Standard_Address /*ss2*/) const
{
  A2.Append(A1);
}
