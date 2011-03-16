// File:	BRepFeat_LocalOperation.cxx
// Created:	Tue Jun 13 11:50:02 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <BRepFeat_LocalOperation.ixx>

#include <TopExp_Explorer.hxx>

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepFeat_LocalOperation::Build () 
{
//  try {
    myBuilder.PerformResult();
    if (myBuilder.IsDone()) {
      Done();
      myShape = myBuilder.ResultingShape();

    // creation of the Map.
      TopExp_Explorer ex;
      myMap.Clear();
      for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
	myMap.Add(ex.Current());
      }
    }
    else {
      NotDone();
    }
//  }
//  catch (StdFail_NotDone) {
//    NotDone();
//  }
}
