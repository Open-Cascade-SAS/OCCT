// File:	QANewBRepNaming_LoaderParent.cxx
// Created:	Mon Oct 25 12:01:04 1999
// Author:	Sergey ZARITCHNY
//		<s-zaritchny@opencascade.com>
// Copyright:	Open CASCADE 2003

#include <QANewBRepNaming_LoaderParent.ixx>

#include <TNaming.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TDataStd_Name.hxx>
#include <TNaming_NamedShape.hxx>


//=======================================================================
//function : GetDangleShapes
//purpose  : Returns dangle sub shapes Generator - Dangle.
//=======================================================================

Standard_Boolean QANewBRepNaming_LoaderParent::GetDangleShapes(const TopoDS_Shape& ShapeIn,
						    const TopAbs_ShapeEnum GeneratedFrom,
						    TopTools_DataMapOfShapeShape& Dangles) 
{
  Dangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (GeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (GeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(ShapeIn, GeneratedTo, GeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) Dangles.Bind(ancestors.First(), mayBeDangle);
  }
  return Dangles.Extent();
}

//=======================================================================
//function : GetDangleShapes
//purpose  : Returns dangle sub shapes.
//=======================================================================
/*
Standard_Boolean QANewBRepNaming_LoaderParent::GetDangleShapes(const TopoDS_Shape& ShapeIn,
						    const TopAbs_ShapeEnum GeneratedFrom,
						    TopTools_MapOfShape& Dangles) 
{
  Dangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (GeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (GeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(ShapeIn, GeneratedTo, GeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) Dangles.Add(mayBeDangle);
  }
  return Dangles.Extent();
}
*/
//=======================================================================
//function : LoadGeneratedDangleShapes
//purpose  : 
//=======================================================================

void QANewBRepNaming_LoaderParent::LoadGeneratedDangleShapes(const TopoDS_Shape&          ShapeIn,
						  const TopAbs_ShapeEnum       GeneratedFrom,
						  TNaming_Builder&             Builder)
{
  TopTools_DataMapOfShapeShape dangles;
  if (!QANewBRepNaming_LoaderParent::GetDangleShapes(ShapeIn, GeneratedFrom, dangles)) return;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itr(dangles);
  for (; itr.More(); itr.Next()) Builder.Generated(itr.Key(), itr.Value());
}

