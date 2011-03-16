// File:	BRepFeat_Builder.cxx
// Created:	Tue Jun 13 14:30:58 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <BRepFeat_Builder.ixx>

#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BOP_HistoryCollector.hxx>


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Builder::Modified
  (const TopoDS_Shape& F)
{
  myGenerated.Clear();
  TopTools_ListIteratorOfListOfShape it;
  const Handle(BOP_HistoryCollector)& B = myBuilder.History();
  const TopTools_ListOfShape& aLM=B->Modified(F);
  it.Initialize(aLM);
  for (; it.More(); it.Next()) {
    myGenerated.Append(it.Value());
  }
  return myGenerated;
}

