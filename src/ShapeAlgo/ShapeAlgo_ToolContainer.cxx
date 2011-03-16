// File:	ShapeAlgo_ToolContainer.cxx
// Created:	Mon Feb  7 12:26:09 2000
// Author:	data exchange team
//		<det@nnov>


#include <ShapeAlgo_ToolContainer.ixx>

//=======================================================================
//function : ShapeAlgo_ToolContainer
//purpose  : 
//=======================================================================

ShapeAlgo_ToolContainer::ShapeAlgo_ToolContainer()
{
}

//=======================================================================
//function : FixShape
//purpose  : 
//=======================================================================

Handle(ShapeFix_Shape) ShapeAlgo_ToolContainer::FixShape() const
{
  return new ShapeFix_Shape;
}

//=======================================================================
//function : EdgeProjAux
//purpose  : 
//=======================================================================

Handle(ShapeFix_EdgeProjAux) ShapeAlgo_ToolContainer::EdgeProjAux() const
{
  return new ShapeFix_EdgeProjAux;
}
