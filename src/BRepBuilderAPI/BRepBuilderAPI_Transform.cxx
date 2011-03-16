// File:	BRepBuilderAPI_Transform.cxx
// Created:	Fri Dec  9 09:14:55 1994
// Author:	Jacques GOUSSARD
//		<jag@topsn2>


#include <BRepBuilderAPI_Transform.ixx>

#include <BRepTools_TrsfModification.hxx>
#include <gp.hxx>


//=======================================================================
//function : BRepBuilderAPI_Transform
//purpose  : 
//=======================================================================

BRepBuilderAPI_Transform::BRepBuilderAPI_Transform (const gp_Trsf& T) :
  myTrsf(T)
{
  myModification = new BRepTools_TrsfModification(T);
}


//=======================================================================
//function : BRepBuilderAPI_Transform
//purpose  : 
//=======================================================================

BRepBuilderAPI_Transform::BRepBuilderAPI_Transform (const TopoDS_Shape& S,
				      const gp_Trsf& T,
				      const Standard_Boolean Copy) :
  myTrsf(T)
{
  myModification = new BRepTools_TrsfModification(T);
  Perform(S,Copy);
}



//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_Transform::Perform(const TopoDS_Shape& S,
				const Standard_Boolean Copy)
{
//  myUseModif = Copy || myTrsf.IsNegative(); bug gp_Trsf.
  myUseModif = Copy || 
    myTrsf.ScaleFactor()*myTrsf.HVectorialPart().Determinant() < 0. ||
      Abs(Abs(myTrsf.ScaleFactor()) - 1) > gp::Resolution();
  if (myUseModif) {
    Handle(BRepTools_TrsfModification) theModif = 
      Handle(BRepTools_TrsfModification)::DownCast(myModification);
    theModif->Trsf() = myTrsf;
    DoModif(S,myModification);
  }
  else {
    myLocation = myTrsf;
    myShape = S.Moved(myLocation);
    Done();
  }

}

//=======================================================================
//function : ModifiedShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepBuilderAPI_Transform::ModifiedShape
  (const TopoDS_Shape& S) const
{  
  if (myUseModif) {
    return myModifier.ModifiedShape(S);
  }
  static TopoDS_Shape SM;
  SM = S.Moved (myLocation);
  return SM;
}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_Transform::Modified
  (const TopoDS_Shape& F)
{
  if (!myUseModif) {
    myGenerated.Clear();
    myGenerated.Append(F.Moved(myLocation));
    return myGenerated;
  }
  return BRepBuilderAPI_ModifyShape::Modified(F);
}

