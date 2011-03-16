#include <ShapeUpgrade_ShapeDivideArea.ixx>
#include <ShapeUpgrade_FaceDivideArea.hxx>
#include <Precision.hxx>
  
//=======================================================================
//function : ShapeUpgrade_ShapeDivideArea
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideArea::ShapeUpgrade_ShapeDivideArea():
       ShapeUpgrade_ShapeDivide()
{
  myMaxArea = Precision::Infinite();
}

//=======================================================================
//function : ShapeUpgrade_ShapeDivideArea
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideArea::ShapeUpgrade_ShapeDivideArea(const TopoDS_Shape& S):
       ShapeUpgrade_ShapeDivide(S)
       
{
  myMaxArea = Precision::Infinite();
}

//=======================================================================
//function : GetSplitFaceTool
//purpose  : 
//=======================================================================

 Handle(ShapeUpgrade_FaceDivide) ShapeUpgrade_ShapeDivideArea::GetSplitFaceTool() const
{
  Handle(ShapeUpgrade_FaceDivideArea) aFaceTool = new ShapeUpgrade_FaceDivideArea;
  aFaceTool->MaxArea() = myMaxArea;
  return aFaceTool;
}

