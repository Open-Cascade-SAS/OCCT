// File:	BRepOffsetAPI_MakeThickSolid.cxx
// Created:	Tue Feb 13 14:27:56 1996
// Author:	Yves FRICAUD
//		<yfr@stylox>


#include <BRepOffsetAPI_MakeThickSolid.ixx>
#include <BRepOffset_MakeOffset.hxx>
#include <Standard_ConstructionError.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>


//=======================================================================
//function : BRepOffsetAPI_MakeThickSolid
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeThickSolid::BRepOffsetAPI_MakeThickSolid()
{
}


//=======================================================================
//function : BRepOffsetAPI_MakeThickSolid
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeThickSolid::BRepOffsetAPI_MakeThickSolid
(const TopoDS_Shape&         S, 
 const TopTools_ListOfShape& ClosingFaces,
 const Standard_Real         Offset, 
 const Standard_Real         Tol, 
 const BRepOffset_Mode       Mode,
 const Standard_Boolean      Intersection,
 const Standard_Boolean      SelfInter,
 const GeomAbs_JoinType      Join)
{
  myOffsetShape.Initialize (S,Offset,Tol,Mode,Intersection,SelfInter,Join);
  TopTools_ListIteratorOfListOfShape it(ClosingFaces);
  for (; it.More(); it.Next()) {
    myOffsetShape.AddFace(TopoDS::Face(it.Value()));
  }
  Build();
}

//=======================================================================
//function : virtual
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeThickSolid::Build()
{
  if (!IsDone()) {
    myOffsetShape.MakeThickSolid();
    if (!myOffsetShape.IsDone()) return;
    myShape  = myOffsetShape.Shape();
    Done();
  }
}



//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeThickSolid::Modified (const TopoDS_Shape& F) 

{
  myGenerated.Clear();
  if (myOffsetShape.OffsetFacesFromShapes().HasImage(F)) {
    if (myOffsetShape.ClosingFaces().Contains(F)) { 
      myOffsetShape.OffsetFacesFromShapes().LastImage (F, myGenerated); 
      // Les face du resultat sont orientees comme dans la piece initiale.
      //si offset a l interieur.
      TopTools_ListIteratorOfListOfShape it(myGenerated);
      for (; it.More(); it.Next())
	it.Value().Reverse();
   
    }
  }
  return myGenerated;
}
