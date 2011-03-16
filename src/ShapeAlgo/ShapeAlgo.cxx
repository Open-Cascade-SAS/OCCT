// File:	ShapeAlgo.cxx
// Created:	Mon Feb  7 12:23:37 2000
// Author:	data exchange team
//		<det@nnov>


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
