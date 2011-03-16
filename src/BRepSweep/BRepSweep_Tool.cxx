// File:	BRepSweep_Tool.cxx
// Created:	Wed Jun  9 18:18:22 1993
// Author:	Laurent BOURESCHE
//		<lbo@phobox>


#include <BRepSweep_Tool.ixx>
#include <TopExp.hxx>

//=======================================================================
//function : BRepSweep_Tool
//purpose  : 
//=======================================================================

BRepSweep_Tool::BRepSweep_Tool(const TopoDS_Shape& aShape)
{
 TopExp::MapShapes(aShape,myMap);
}


//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================

Standard_Integer  BRepSweep_Tool::NbShapes()const 
{
  return myMap.Extent();
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  BRepSweep_Tool::Index(const TopoDS_Shape& aShape)const 
{
  if(!myMap.Contains(aShape)) return 0;
  return myMap.FindIndex(aShape);
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepSweep_Tool::Shape(const Standard_Integer anIndex)const 
{
  return myMap.FindKey(anIndex);
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum  BRepSweep_Tool::Type(const TopoDS_Shape& aShape)const 
{
  return aShape.ShapeType();
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  BRepSweep_Tool::Orientation
                                     (const TopoDS_Shape& aShape)const 
{
  return aShape.Orientation();
}


//=======================================================================
//function : SetOrientation
//purpose  : 
//=======================================================================

void BRepSweep_Tool::SetOrientation (TopoDS_Shape& aShape, 
				     const TopAbs_Orientation Or)const 
{
  aShape.Orientation(Or);
}
