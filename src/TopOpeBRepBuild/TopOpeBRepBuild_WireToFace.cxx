// Created on: 1996-01-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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
