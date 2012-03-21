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

//modified by NIZNHY-PKV Tue Apr  3 15:50:23 2001

#include <BOP_Area2dBuilder.ixx>

#include <BOP_Loop.hxx>
#include <BOP_ListIteratorOfListOfLoop.hxx>

//=======================================================================
//function : BOP_Area2dBuilder::BOP_Area2dBuilder
//purpose  : 
//=======================================================================
BOP_Area2dBuilder::BOP_Area2dBuilder()
{
}

//=======================================================================
//function : BOP_Area2dBuilder::BOP_Area2dBuilder
//purpose  : 
//=======================================================================
  BOP_Area2dBuilder::BOP_Area2dBuilder(BOP_LoopSet& LS, 
				       BOP_LoopClassifier& LC,
				       const Standard_Boolean ForceClass)
{
  InitAreaBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : InitAreaBuilder
//purpose  : 
//=======================================================================
  void BOP_Area2dBuilder::InitAreaBuilder(BOP_LoopSet&        LS,
					  BOP_LoopClassifier& LC,
					  const Standard_Boolean ForceClass)
{
  TopAbs_State     state;
  Standard_Boolean Loopinside;
  Standard_Boolean loopoutside;
  
  BOP_ListIteratorOfListOfListOfLoop AreaIter;
  BOP_ListIteratorOfListOfLoop       LoopIter;
  // boundaryloops : list of boundary loops out of the areas.
  BOP_ListOfLoop                     boundaryloops; 
  
  myArea.Clear();          // Clear the list of Area to be built
  
  for (LS.InitLoop(); LS.MoreLoop(); LS.NextLoop()) {
    
    // process a new loop : L is the new current Loop
    const Handle(BOP_Loop)& L = LS.Loop();
    Standard_Boolean boundaryL = L->IsShape();
    //
    //const TopoDS_Shape& aSL=L->Shape();
    //
    // L = shape et ForceClass  : on traite L comme un block
    // L = shape et !ForceClass : on traite L comme un pur shape
    // L = !shape               : on traite L comme un block
    Standard_Boolean traitercommeblock = (!boundaryL) || ForceClass;
    if ( !traitercommeblock ) {
      // the loop L is a boundary loop : 
      // - try to insert it in an existing area, such as L is inside all 
      //   the block loops. Only block loops of the area are compared. 
      // - if L could not be inserted, store it in list of boundary loops.
      Loopinside = Standard_False; 
      AreaIter.Initialize(myArea);
      for (; AreaIter.More(); AreaIter.Next()) {
	BOP_ListOfLoop& aArea = AreaIter.Value();
	if ( aArea.IsEmpty() ) {
	  continue;
	}	
	state = CompareLoopWithListOfLoop(LC,L,aArea,BOP_BLOCK );
	if (state == TopAbs_UNKNOWN){
	  Atomize(state,TopAbs_IN);
	}	
	Loopinside = ( state == TopAbs_IN);
	if ( Loopinside ) {
	  break;
	}
      } // end of Area scan

      if ( Loopinside ) {
	BOP_ListOfLoop& aArea = AreaIter.Value();
	ADD_Loop_TO_LISTOFLoop(L, aArea);
      }
      else if ( ! Loopinside ) {
	ADD_Loop_TO_LISTOFLoop(L, boundaryloops);
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
      //
      Standard_Integer aNbAreas, aNbLoops;
      Loopinside = Standard_False;
      //
      aNbAreas=myArea.Extent();
      if (aNbAreas) {
	AreaIter.Initialize(myArea);
	for (; AreaIter.More(); AreaIter.Next()) {
	  BOP_ListOfLoop& aArea = AreaIter.Value();
	  aNbLoops=aArea.Extent();
	  if (!aNbLoops){
	    continue;
	  }
	  //
	  state = CompareLoopWithListOfLoop(LC, L, aArea, BOP_ANYLOOP);
	  //
	  if (state == TopAbs_UNKNOWN){
	    Atomize(state,TopAbs_IN);
	  }
	  //
	  Loopinside = (state==TopAbs_IN);
	  if ( Loopinside ){
	    break;
	  }
	} // end of Area scan
      }
      //
      if ( Loopinside) {
	BOP_ListOfLoop& aArea = AreaIter.Value();
	Standard_Boolean allShape = Standard_True;
	BOP_ListOfLoop removedLoops;
	LoopIter.Initialize(aArea);
	while (LoopIter.More()) {
	  const Handle(BOP_Loop)& curL = LoopIter.Value();
	  state = LC.Compare(curL, L);
	  if (state == TopAbs_UNKNOWN) {
	    Atomize(state,TopAbs_IN); // not OUT
	  }	  
	  //
	  loopoutside = ( state == TopAbs_OUT );
	  //
	  if ( loopoutside ) {
	    // remove the loop from the area
	    ADD_Loop_TO_LISTOFLoop(curL, removedLoops);
	    
	    allShape = allShape && curL->IsShape();
	    REM_Loop_FROM_LISTOFLoop(LoopIter, aArea);
	  }
	  else {
	    LoopIter.Next();
	  }
	}// while (LoopIter.More()) {
	//
	// insert the loop in the area
	ADD_Loop_TO_LISTOFLoop(L, aArea);
	if ( ! removedLoops.IsEmpty() ) {
	  if ( allShape ) {
	    ADD_LISTOFLoop_TO_LISTOFLoop (removedLoops,boundaryloops);
	  }
	  else {
	    // make a new area with the removed loops
            BOP_ListOfLoop thelistofloop;
	    myArea.Append(thelistofloop);
	    ADD_LISTOFLoop_TO_LISTOFLoop(removedLoops, myArea.Last());
	  }
	}
      } // Loopinside == True
      
      else {
        Standard_Integer ashapeinside,ablockinside;
	BOP_ListOfLoop thelistofloop1;
	myArea.Append(thelistofloop1);
	BOP_ListOfLoop& newArea0 = myArea.Last();
	ADD_Loop_TO_LISTOFLoop(L, newArea0);
	
        LoopIter.Initialize(boundaryloops);
        while ( LoopIter.More() ) {
          ashapeinside = ablockinside = Standard_False;
	  const Handle(BOP_Loop)& lb = LoopIter.Value();
	  state = LC.Compare(lb,L);
	  if (state == TopAbs_UNKNOWN){
	    Atomize(state,TopAbs_IN);
	  }          
	  ashapeinside = (state == TopAbs_IN);
          if (ashapeinside) {
	    state = LC.Compare(L,lb);
	    if (state == TopAbs_UNKNOWN){
	      Atomize(state,TopAbs_IN);
	    }	    
	    ablockinside = (state == TopAbs_IN);
	  }
	  if ( ashapeinside && ablockinside ) {
	    const Handle(BOP_Loop)& curL = LoopIter.Value();
	    ADD_Loop_TO_LISTOFLoop(curL, newArea0);

	    REM_Loop_FROM_LISTOFLoop(LoopIter,boundaryloops);
	  }
          else { 
	    LoopIter.Next();
	  }
	} // end of boundaryloops scan
      } // Loopinside == False
    } // end of block loop
  } // end of LoopSet LS scan
  
  if ( ! boundaryloops.IsEmpty() ) {
    if ( myArea.IsEmpty() )  {
      BOP_ListOfLoop newArea3;
      newArea3.Append(boundaryloops);
      myArea.Append(newArea3);
    }
  }

  InitArea();
}
