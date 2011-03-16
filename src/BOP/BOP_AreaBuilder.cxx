// File:	BOP_AreaBuilder.cxx
// Created:	Thu Dec 21 17:07:40 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>
//Copyright:	 Matra Datavision 1995

// " Voyager, c'est bien utile, ca fait travailler l'imagination.
//   Tout le reste n'est que deceptions et fatigues. Notre voyage 
//   a nous est entierement imaginaire. Voila sa force. "
//                         Celine
//                         Voyage au bout de la nuit

#include <BOP_AreaBuilder.ixx>

#include <Standard_DomainError.hxx>
#include <TopAbs.hxx>

//=======================================================================
//function : BOP_AreaBuilder::BOP_AreaBuilder
//purpose  : 
//=======================================================================
  BOP_AreaBuilder::BOP_AreaBuilder() 
:
  myUNKNOWNRaise(Standard_False) 
{
}

//=======================================================================
//function : TopOpeBRepBuild_AreaBuilder
//purpose  : 
//=======================================================================
  BOP_AreaBuilder::BOP_AreaBuilder (BOP_LoopSet&        LS,
				    BOP_LoopClassifier& LC,
				    const Standard_Boolean ForceClass)
:
  myUNKNOWNRaise(Standard_False) 
{
  InitAreaBuilder(LS, LC, ForceClass);
}
//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
  void BOP_AreaBuilder::Delete()
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
  TopAbs_State BOP_AreaBuilder::CompareLoopWithListOfLoop (BOP_LoopClassifier &LC,
							   const Handle(BOP_Loop)& L,
							   const BOP_ListOfLoop &LOL,
							   const BOP_LoopEnum   what) const
{
  TopAbs_State                 state = TopAbs_UNKNOWN;
  Standard_Boolean             totest; // L must or not be tested
  BOP_ListIteratorOfListOfLoop LoopIter;  
  
  if ( LOL.IsEmpty() ) {
    return TopAbs_OUT;
  }

  LoopIter.Initialize(LOL);
  for (; LoopIter.More(); LoopIter.Next() ) {
    const Handle(BOP_Loop)& curL = LoopIter.Value();
    switch ( what ) { 
    case BOP_ANYLOOP  : 
      totest = Standard_True;
      break;
    case BOP_BOUNDARY : 
      totest =  curL->IsShape();
      break;
    case BOP_BLOCK    : 
      totest = !curL->IsShape();
      break;
    default:
      totest = Standard_False;
      break;
    }
    if ( totest ) {
      state = LC.Compare(L,curL);
      if (state == TopAbs_OUT) 
	// <L> is out of at least one Loop 
	//of <LOL> : stop to explore
	break;  
    }
  }
  return state;
}

//=======================================================================
//function : Atomize
//purpose  : 
//=======================================================================
  void BOP_AreaBuilder::Atomize(TopAbs_State& state, 
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
  void BOP_AreaBuilder::InitAreaBuilder(BOP_LoopSet& LS,
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
	BOP_ListOfLoop& aArea = AreaIter.Value();
	if ( aArea.IsEmpty() ) continue;
	state = CompareLoopWithListOfLoop(LC,L,aArea,BOP_BLOCK );
	if (state == TopAbs_UNKNOWN) {
	  Atomize(state,TopAbs_IN);
	}	
	Loopinside = ( state == TopAbs_IN);
	if ( Loopinside ) {
	  break;
	}
      } // end of Area scan

      if ( Loopinside ) {
	BOP_ListOfLoop& aArea = AreaIter.Value();
	ADD_Loop_TO_LISTOFLoop(L,aArea);
      }
      else if ( ! Loopinside ) {
	ADD_Loop_TO_LISTOFLoop(L,boundaryloops);
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
	BOP_ListOfLoop& aArea = AreaIter.Value();
	if ( aArea.IsEmpty() ) {
	  continue;
	} 	
	state = CompareLoopWithListOfLoop(LC,L,aArea,BOP_ANYLOOP);
	if (state == TopAbs_UNKNOWN) Atomize(state,TopAbs_IN);
 	Loopinside = (state == TopAbs_IN);
	if ( Loopinside ) {
	  break;
	}
      } // end of Area scan
      
      if ( Loopinside) {
	BOP_ListOfLoop& aArea = AreaIter.Value();
	Standard_Boolean allShape = Standard_True;
	BOP_ListOfLoop removedLoops;
	
	LoopIter.Initialize(aArea);
	while (LoopIter.More()) {
	  state = LC.Compare(LoopIter.Value(),L);
	  if (state == TopAbs_UNKNOWN){
	    Atomize(state,TopAbs_IN); // not OUT
	  }
	  
	  loopoutside = ( state == TopAbs_OUT );
	  
	  if ( loopoutside ) {
	    const Handle(BOP_Loop)& curL = LoopIter.Value();
	    // remove the loop from the area
	    ADD_Loop_TO_LISTOFLoop (curL,removedLoops);
	    
	    allShape = allShape && curL->IsShape();
	    REM_Loop_FROM_LISTOFLoop(LoopIter,AreaIter.Value());
	  }
	  else {
	    LoopIter.Next();
	  }
	}
	// insert the loop in the area
	ADD_Loop_TO_LISTOFLoop(L,aArea);
	if ( ! removedLoops.IsEmpty() ) {
	  if ( allShape ) {
	    ADD_LISTOFLoop_TO_LISTOFLoop(removedLoops,boundaryloops);
	  }
	  else {
	    // make a new area with the removed loops
            BOP_ListOfLoop thelistofloop;
	    myArea.Append(thelistofloop);
	    ADD_LISTOFLoop_TO_LISTOFLoop (removedLoops,myArea.Last());
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
	  state = LC.Compare(LoopIter.Value(),L);
	  if (state == TopAbs_UNKNOWN) {
	    Atomize(state,TopAbs_IN);
          }
	 
	  ashapeinside = (state == TopAbs_IN);
          if (ashapeinside) {
	    state = LC.Compare(L,LoopIter.Value());
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
  
  InitArea();
}

//=======================================================================
//function : InitArea
//purpose  : 
//=======================================================================
  Standard_Integer BOP_AreaBuilder::InitArea()
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
  Standard_Boolean BOP_AreaBuilder::MoreArea() const
{
  Standard_Boolean b = myAreaIterator.More();
  return b;
}

//=======================================================================
//Function : NextArea
//Purpose  : 
//=======================================================================
  void BOP_AreaBuilder::NextArea()
{
  myAreaIterator.Next();
  InitLoop();
}

//=======================================================================
//function : InitLoop
//purpose  : 
//=======================================================================
  Standard_Integer BOP_AreaBuilder::InitLoop()
{
  Standard_Integer n = 0;
  if (myAreaIterator.More()) {
    const BOP_ListOfLoop& LAL = myAreaIterator.Value();
    myLoopIterator.Initialize(LAL);
    n = LAL.Extent();
  }
  else { // Create an empty ListIteratorOfListOfLoop
    myLoopIterator = BOP_ListIteratorOfListOfLoop();  
  }
  return n;
}

//=======================================================================
//function : MoreLoop
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_AreaBuilder::MoreLoop() const
{
  Standard_Boolean b = myLoopIterator.More();
  return b;
}

//=======================================================================
//function : NextLoop
//purpose  : 
//=======================================================================
  void BOP_AreaBuilder::NextLoop()
{
  myLoopIterator.Next();
}

//=======================================================================
//function : Loop
//purpose  : 
//=======================================================================
  const Handle(BOP_Loop)& BOP_AreaBuilder::Loop() const
{
  const Handle(BOP_Loop)& L = myLoopIterator.Value();
  return L;
}

//=======================================================================
//function : ADD_Loop_TO_LISTOFLoop
//purpose  : 
//=======================================================================
  void BOP_AreaBuilder::ADD_Loop_TO_LISTOFLoop(const Handle(BOP_Loop)& L,
					       BOP_ListOfLoop& LOL) const
{
  LOL.Append(L);
}

//=======================================================================
//function : REM_Loop_FROM_LISTOFLoop
//purpose  : 
//=======================================================================
  void BOP_AreaBuilder::REM_Loop_FROM_LISTOFLoop(BOP_ListIteratorOfListOfLoop& ITA,
						 BOP_ListOfLoop& A) const
{
  A.Remove(ITA);
}

//=======================================================================
//function : ADD_LISTOFLoop_TO_LISTOFLoop
//purpose  : 
//=======================================================================
  void BOP_AreaBuilder::ADD_LISTOFLoop_TO_LISTOFLoop(BOP_ListOfLoop& A1,
						     BOP_ListOfLoop& A2) const
						     
{
  A2.Append(A1);
}
