// File:	TopOpeBRepBuild_WireToFace.cxx
// Created:	Mon Jan 29 16:42:26 1996
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <TopOpeBRepBuild_WireToFace.ixx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>

//=======================================================================
//function : TopOpeBRepBuild_WireToFace
//purpose  : 
//=======================================================================

TopOpeBRepBuild_WireToFace::TopOpeBRepBuild_WireToFace()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_WireToFace::Init()
{
  myLW.Clear();
}


//=======================================================================
//function : AddWire
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_WireToFace::AddWire(const TopoDS_Wire& W)
{
  myLW.Append(W);
}

//=======================================================================
//function : MakeFaces
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_WireToFace::MakeFaces(const TopoDS_Face& F,
					   TopTools_ListOfShape& LF)
{
  LF.Clear();
  
  TopOpeBRepBuild_WireEdgeSet wes(F);
  for (TopTools_ListIteratorOfListOfShape it(myLW);it.More();it.Next())
    wes.AddShape(it.Value());
  
  Standard_Boolean ForceClass = Standard_True;
  TopOpeBRepBuild_FaceBuilder FB;
  FB.InitFaceBuilder(wes,F,ForceClass);
  
  TopOpeBRepDS_BuildTool BT(TopOpeBRepTool_APPROX);
  TopOpeBRepBuild_Builder B(BT);
  B.MakeFaces(F,FB,LF);
}
