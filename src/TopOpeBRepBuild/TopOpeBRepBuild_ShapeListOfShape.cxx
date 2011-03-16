// File:	TopOpeBRepBuild_ShapeListOfShape.cxx
// Created:	Thu Jun 26 17:22:23 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#include <TopOpeBRepBuild_ShapeListOfShape.ixx>

#define MTTdmiomoslos TopTools_DataMapIteratorOfDataMapOfShapeListOfShape
#define MTTdmoslos TopTools_DataMapOfShapeListOfShape
#define MTTmiomos TopTools_MapIteratorOfMapOfShape
#define MTTmos TopTools_MapOfShape
#define MTTliolos TopTools_ListIteratorOfListOfShape
#define MTTlos TopTools_ListOfShape
#define MTs TopoDS_Shape
#define MTf TopoDS_Face
#define MTe TopoDS_Edge
#define MTv TopoDS_Vertex
#define MDSlioloi TopOpeBRepDS_ListIteratorOfListOfInterference
#define MDSloi TopOpeBRepDS_ListOfInterference
#define MDShi Handle(TopOpeBRepDS_Interference)
#define MDSi TopOpeBRepDS_Interference
#define MDShssi Handle(TopOpeBRepDS_ShapeShapeInterference)
#define MDSssi TopOpeBRepDS_ShapeShapeInterference
#define MDSii TopOpeBRepDS_InterferenceIterator
#define MDShds Handle(TopOpeBRepDS_HDataStructure)
#define MDSds TopOpeBRepDS_DataStructure
#define MDSk TopOpeBRepDS_Kind
#define MDSsd TopOpeBRepDS_ShapeData
#define MDSmosd TopOpeBRepDS_MapOfShapeData
#define Msr Standard_Real
#define Msi Standard_Integer
#define Msb Standard_Boolean
#define Mso Standard_Ostream
#define Mtcas TCollection_AsciiString

#ifdef DEB
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GettraceSPS();
#endif

//=======================================================================
//function : TopOpeBRepBuild_ShapeListOfShape
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape()
{
}

//=======================================================================
//function : TopOpeBRepBuild_ShapeListOfShape
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape
(const TopoDS_Shape& S)
{
  myShape = S;
}

//=======================================================================
//function : TopOpeBRepBuild_ShapeListOfShape
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape
(const TopoDS_Shape& S, const TopTools_ListOfShape& L)
{
  myShape = S;
  myList = L;
}

//=======================================================================
//function : List
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& TopOpeBRepBuild_ShapeListOfShape::List() const
{
  return myList;
}

//=======================================================================
//function : ChangeList
//purpose  : 
//=======================================================================

TopTools_ListOfShape& TopOpeBRepBuild_ShapeListOfShape::ChangeList()
{
  return myList;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_ShapeListOfShape::Shape() const
{
  return myShape;
}

//=======================================================================
//function : ChangeShape
//purpose  : 
//=======================================================================

TopoDS_Shape& TopOpeBRepBuild_ShapeListOfShape::ChangeShape()
{
  return myShape;
}

