// File:	ShapeUpgrade_ShapeDivideClosedEdges.cxx
// Created:	Thu May 25 10:01:06 2000
// Author:	data exchange team
//		<det@friendox>


#include <ShapeUpgrade_ShapeDivideClosedEdges.ixx>
#include <ShapeUpgrade_ClosedEdgeDivide.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>

//=======================================================================
//function : ShapeUpgrade_ShapeDivideClosedEdges
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideClosedEdges::ShapeUpgrade_ShapeDivideClosedEdges(const TopoDS_Shape& S):
       ShapeUpgrade_ShapeDivide(S)  
{
  SetNbSplitPoints(1);
}

//=======================================================================
//function : SetNbSplitPoints
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivideClosedEdges::SetNbSplitPoints(const Standard_Integer /*num*/)
{
  Handle(ShapeUpgrade_ClosedEdgeDivide) tool = new ShapeUpgrade_ClosedEdgeDivide;
  Handle(ShapeUpgrade_WireDivide) wtool = new ShapeUpgrade_WireDivide;
  wtool->SetEdgeDivideTool(tool);
  Handle(ShapeUpgrade_FaceDivide) ftool = new ShapeUpgrade_FaceDivide;
  ftool->SetWireDivideTool(wtool);
  ftool->SetSplitSurfaceTool( 0 );
  SetSplitFaceTool(ftool);
}
