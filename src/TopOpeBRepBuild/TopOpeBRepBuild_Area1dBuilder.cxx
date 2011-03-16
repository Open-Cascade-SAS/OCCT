// File:	TopOpeBRepBuild_Area1dBuilder.cxx
// Created:	Thu Dec 21 17:07:40 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>
// Copyright:	 Matra Datavision 1995

#include <TopOpeBRepBuild_Area1dBuilder.ixx>

#include <TopOpeBRepBuild_Pave.hxx>
#include <TopOpeBRepBuild_LoopEnum.hxx>
#include <Standard_DomainError.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs.hxx>

#ifdef DEB
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GettraceAREA();
Standard_IMPORT Standard_Boolean TopOpeBRepDS_GettraceSTRANGE();
#endif

//=======================================================================
//function : DumpList
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Area1dBuilder::DumpList(const TopOpeBRepBuild_ListOfLoop& LOL)
{
#ifdef DEB
  Standard_Integer iLOL;
  TopOpeBRepBuild_ListIteratorOfListOfLoop  itLOL;
  for (iLOL = 0, itLOL.Initialize(LOL); itLOL.More(); iLOL++, itLOL.Next()) {
    if (iLOL) cout<<"               ";
    else     cout<<"DUMP_AREA    : ";
    const Handle(TopOpeBRepBuild_Loop)& L = itLOL.Value();
    L->Dump();cout<<endl;
  }
#endif
}

//=======================================================================
//function : TopOpeBRepBuild_Area1dBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_Area1dBuilder::TopOpeBRepBuild_Area1dBuilder()
{
}

//=======================================================================
//function : TopOpeBRepBuild_Area1dBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_Area1dBuilder::TopOpeBRepBuild_Area1dBuilder
(TopOpeBRepBuild_PaveSet&        LS,
 TopOpeBRepBuild_PaveClassifier& LC,
 const Standard_Boolean ForceClass)
{
   InitAreaBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : InitAreaBuilder
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Area1dBuilder::InitAreaBuilder
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
                
#ifdef DEB
    if (TopOpeBRepBuild_GettraceAREA()) {
      cout<<"++++ new loop : "; L->Dump();
      if (boundaryL) cout<<" is bound"; else cout<<" is not bound";
      cout<<endl;
    }
#endif


    // L = shape et ForceClass  : on traite L comme un block
    // L = shape et !ForceClass : on traite L comme un pur shape
    // L = !shape               : on traite L comme un block
    Standard_Boolean traitercommeblock = !boundaryL || ForceClass;
     if ( ! traitercommeblock ) {

      // the loop L is a boundary loop : 
      // - try to insert it in an existing area, such as L is inside all 
      //   the block loops. Only block loops of the area are compared. 
      // - if L could not be inserted, store it in list of boundary loops.

        Loopinside = Standard_False; 
        for (AreaIter.Initialize(myArea); AreaIter.More(); AreaIter.Next()) {
	TopOpeBRepBuild_ListOfLoop& aArea = AreaIter.Value();
	if ( aArea.IsEmpty() ) continue;
	state = CompareLoopWithListOfLoop(LC,L,aArea,TopOpeBRepBuild_BLOCK);
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
  
#ifdef DEB
  if (TopOpeBRepBuild_GettraceAREA()) {
    if ( ! myArea.IsEmpty() ) {
      cout<<"------- Areas -------"<<endl;
      for (AreaIter.Initialize(myArea);AreaIter.More();AreaIter.Next())
	DumpList(AreaIter.Value());
      cout<<"---------------------"<<endl;
    }
  }
#endif
  
 InitArea();
}

//=======================================================================
//function : ADD_Loop_TO_LISTOFLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Area1dBuilder::ADD_Loop_TO_LISTOFLoop
(const Handle(TopOpeBRepBuild_Loop)& L,
 TopOpeBRepBuild_ListOfLoop& LOL,
 const Standard_Address ss) const
{
#ifdef DEB
  char* s = (char*)ss;
#endif

  LOL.Append(L);
  
#ifdef DEB
  if (TopOpeBRepBuild_GettraceAREA()) {
    cout<<"--------------------- add area loop to area : ";
    L->Dump(); cout<<endl;
    if (s != NULL) cout<<s<<endl;
    DumpList(LOL);
    cout<<"---------------------"<<endl;
  }
#endif
 }

//=======================================================================
//function : REM_Loop_FROM_LISTOFLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Area1dBuilder::REM_Loop_FROM_LISTOFLoop
(TopOpeBRepBuild_ListIteratorOfListOfLoop& ITA,
 TopOpeBRepBuild_ListOfLoop& A,
 const Standard_Address ss) const
{
#ifdef DEB
  char* s = (char*)ss;
#endif

#ifdef DEB
  if (TopOpeBRepBuild_GettraceAREA()) {
    if (ITA.More()) {
      cout<<"--------------------- remove area loop from area : ";
      ITA.Value()->Dump(); cout<<endl;
      if (s != NULL) cout<<s<<endl;
    }
  }
#endif
  
  A.Remove(ITA);
  
#ifdef DEB
  if (TopOpeBRepBuild_GettraceAREA()) {
    DumpList(A);
    cout<<"---------------------"<<endl;
  }
#endif
}

//=======================================================================
//function : ADD_LISTOFLoop_TO_LISTOFLoop
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Area1dBuilder::ADD_LISTOFLoop_TO_LISTOFLoop
(TopOpeBRepBuild_ListOfLoop& A1,
 TopOpeBRepBuild_ListOfLoop& A2,
 const Standard_Address ss,
 const Standard_Address ss1,
 const Standard_Address ss2) const
{
#ifdef DEB
  char* s  = (char*)ss;
  char* s1 = (char*)ss1;
  char* s2 = (char*)ss2;

  if (TopOpeBRepBuild_GettraceAREA()) {
    cout<<"--------------------- add area 1 to area 2 : ";
    if (s != NULL) cout<<s;  cout<<endl;
    cout<<"1 : "; if (s1 != NULL) cout<<s1; cout<<endl;
    DumpList(A1);
  }
#endif
  
  A2.Append(A1);
  
#ifdef DEB
  if (TopOpeBRepBuild_GettraceAREA()) {
    cout<<"2 : "; if (s2 != NULL) cout<<s2; cout<<endl;
    DumpList(A2);
    cout<<"---------------------"<<endl;
  }
#endif
}









