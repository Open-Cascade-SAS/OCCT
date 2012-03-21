// Created on: 1994-12-09
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

