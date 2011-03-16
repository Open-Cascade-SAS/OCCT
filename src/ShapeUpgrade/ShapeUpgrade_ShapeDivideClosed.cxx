// File:	ShapeUpgrade_ShapeDivideClosed.cxx
// Created:	Thu Jul 22 17:59:58 1999
// Author:	data exchange team
//		<det@friendox>


#include <ShapeUpgrade_ShapeDivideClosed.ixx>
#include <ShapeUpgrade_ClosedFaceDivide.hxx>

//=======================================================================
//function : ShapeUpgrade_ShapeDivideClosed
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideClosed::ShapeUpgrade_ShapeDivideClosed(const TopoDS_Shape& S):
     ShapeUpgrade_ShapeDivide(S)  
{
  SetNbSplitPoints(1);
}

//=======================================================================
//function : SetNbSplitPoints
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivideClosed::SetNbSplitPoints(const Standard_Integer num)
{
  Handle(ShapeUpgrade_ClosedFaceDivide) tool = new ShapeUpgrade_ClosedFaceDivide;
  tool->SetNbSplitPoints(num);
  tool->SetWireDivideTool ( 0 ); // no splitting of wire
  SetSplitFaceTool(tool);
}
