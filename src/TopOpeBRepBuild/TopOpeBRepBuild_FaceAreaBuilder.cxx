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


#include <TopOpeBRepBuild_FaceAreaBuilder.ixx>

//=======================================================================
//function : TopOpeBRepBuild_FaceAreaBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_FaceAreaBuilder::TopOpeBRepBuild_FaceAreaBuilder()
{
}

//=======================================================================
//function : TopOpeBRepBuild_FaceAreaBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_FaceAreaBuilder::TopOpeBRepBuild_FaceAreaBuilder
(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass)
{
  InitFaceAreaBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : TopOpeBRepBuild_FaceAreaBuilder
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_FaceAreaBuilder::InitFaceAreaBuilder
(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC,
 const Standard_Boolean ForceClass)
{
  InitAreaBuilder(LS,LC,ForceClass);
}
