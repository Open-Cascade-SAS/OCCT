// File:      BRepMesh_PairOfPolygon.cxx
// Created:   26.01.09 11:08:29
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2009

#include <BRepMesh_PairOfPolygon.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

BRepMesh_PairOfPolygon::BRepMesh_PairOfPolygon()
{
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepMesh_PairOfPolygon::Clear()
{
  myFirst.Nullify();
  myLast.Nullify();
}

//=======================================================================
//function : Prepend
//purpose  : 
//=======================================================================

void BRepMesh_PairOfPolygon::Prepend
 ( const Handle(Poly_PolygonOnTriangulation)& theItem )
{
  myFirst = theItem;
  if (myLast.IsNull())
    myLast = theItem;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

void BRepMesh_PairOfPolygon::Append
 ( const Handle(Poly_PolygonOnTriangulation)& theItem )
{
  if (myFirst.IsNull())
    myFirst = theItem;
  myLast = theItem;
}
