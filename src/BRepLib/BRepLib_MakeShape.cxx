// File:	BRepLib_MakeShape.cxx
// Created:	Fri Jul 23 15:51:51 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepLib_MakeShape.ixx>

//=======================================================================
//function : BRepLib_MakeShape
//purpose  : 
//=======================================================================

BRepLib_MakeShape::BRepLib_MakeShape()
{
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepLib_MakeShape::Build()
{
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepLib_MakeShape::Shape() const
{
  if (!IsDone()) {
    // the following is const cast away
    ((BRepLib_MakeShape*) (void*) this)->Build();
    Check();
  }
  return myShape;
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakeShape::operator TopoDS_Shape() const
{
  return Shape();
}



//=======================================================================
//function : HasDescendants
//purpose  : 
//=======================================================================

Standard_Boolean BRepLib_MakeShape::HasDescendants(const TopoDS_Face&)const
{
  return (Standard_True);
}



//=======================================================================
//function : FaceStatus
//purpose  : 
//=======================================================================

BRepLib_ShapeModification BRepLib_MakeShape::FaceStatus
  (const TopoDS_Face&) const
{
  BRepLib_ShapeModification myStatus = BRepLib_Trimmed;
  return myStatus;
}


//=======================================================================
//function : GeneratedFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::DescendantFaces
  (const TopoDS_Face&)
{
  return myGenFaces;
}

//=======================================================================
//function : NbSurfaces
//purpose  : 
//=======================================================================

Standard_Integer BRepLib_MakeShape::NbSurfaces() const
{
  return (0);
}

//=======================================================================
//function : NewFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::NewFaces(const Standard_Integer)
{
  return myNewFaces;
}


//=======================================================================
//function : FacesFromEdges
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::FacesFromEdges
  (const TopoDS_Edge&)
{
  return myEdgFaces;
}
