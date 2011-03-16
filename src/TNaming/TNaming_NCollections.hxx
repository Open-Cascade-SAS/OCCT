// File:	TNaming_NCollections.hxx
// Created:	Thu Mar 25 11:07:22 2010
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
//Copyright:     Open CasCade SA 2010


#ifndef TNaming_NCollections_HeaderFile
#define TNaming_NCollections_HeaderFile
#include <NCollection_Map.hxx>   
#include <NCollection_DataMap.hxx>
typedef NCollection_Map<TopoDS_Shape> TNaming_MapOfShape; 
typedef TNaming_MapOfShape::Iterator TNaming_MapIteratorOfMapOfShape;
typedef NCollection_DataMap<TopoDS_Shape, TNaming_MapOfShape> TNaming_DataMapOfShapeMapOfShape; 
typedef TNaming_DataMapOfShapeMapOfShape::Iterator TNaming_DataMapIteratorOfDataMapOfShapeMapOfShape; 
//=======================================================================
//function : NCollection => IsEqual
//=======================================================================
Standard_Boolean IsEqual (const TopoDS_Shape& S1, const TopoDS_Shape& S2) 
{
  return S1.IsEqual(S2);
}

#endif
