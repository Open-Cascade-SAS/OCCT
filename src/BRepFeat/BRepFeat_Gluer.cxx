// File:	BRepFeat_Gluer.cxx
// Created:	Fri Mar  8 09:50:16 1996
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <BRepFeat_Gluer.ixx>
#include <TopoDS.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepFeat_Gluer::Build()
{
  myGluer.Perform();
  if (myGluer.IsDone()) {
    Done();
    myShape = myGluer.ResultingShape();
  }
  else {
    NotDone();
  }
}



//=======================================================================
//function : isdeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepFeat_Gluer::IsDeleted
   (const TopoDS_Shape& F) 
{
  return (myGluer.DescendantFaces(TopoDS::Face(F)).IsEmpty());
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Gluer::Modified
   (const TopoDS_Shape& F)
{
  if (F.ShapeType() == TopAbs_FACE) {
    const TopTools_ListOfShape& LS = myGluer.DescendantFaces(TopoDS::Face(F));
    if (!LS.IsEmpty()) {
      if (!LS.First().IsSame(F))
	return myGluer.DescendantFaces(TopoDS::Face(F));
    }
  }
  static TopTools_ListOfShape LIM;
  return  LIM;
}

