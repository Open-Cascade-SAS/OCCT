// Created on: 2000-02-07
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <ShapeAlgo.ixx>
#include <ShapeExtend.hxx>

static Handle(ShapeAlgo_AlgoContainer) theContainer;

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void ShapeAlgo::Init() 
{
  static Standard_Boolean init = Standard_False;
  if (init) return;
  init = Standard_True;
  theContainer = new ShapeAlgo_AlgoContainer;

  // initialization of Standard Shape Healing
  ShapeExtend::Init();
}

//=======================================================================
//function : SetAlgoContainer
//purpose  : 
//=======================================================================

 void ShapeAlgo::SetAlgoContainer(const Handle(ShapeAlgo_AlgoContainer)& aContainer) 
{
  theContainer = aContainer;
}

//=======================================================================
//function : AlgoContainer
//purpose  : 
//=======================================================================

 Handle(ShapeAlgo_AlgoContainer) ShapeAlgo::AlgoContainer() 
{
  return theContainer;
}
