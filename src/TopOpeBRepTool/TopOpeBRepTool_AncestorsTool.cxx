// File:	TopOpeBRepTool_AncestorsTool.cxx
// Created:	Thu Aug 12 16:46:45 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>


#include <TopOpeBRepTool_AncestorsTool.ixx>

#include <TopAbs.hxx>
#include <TopExp.hxx>

//=======================================================================
//function : MakeAncestors
//purpose  : 
//=======================================================================

void TopOpeBRepTool_AncestorsTool::MakeAncestors
  (const TopoDS_Shape& S,
   const TopAbs_ShapeEnum TS,
   const TopAbs_ShapeEnum TA,
   TopTools_IndexedDataMapOfShapeListOfShape& M)
{
  TopExp::MapShapesAndAncestors(S,TS,TA,M);
}
